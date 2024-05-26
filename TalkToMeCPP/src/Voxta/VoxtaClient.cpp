// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "VoxtaClient.h"
#include "VoxtaApiHandler.h"
#include "../Utility/Logging/LoggerInterface.h"
#include "DataTypes/CharData.h"
#include "DataTypes/ChatSession.h"
#include "DataTypes/ChatMessage.h"
#include "DataTypes/ServerResponses/ServerResponseBase.h"
#include "DataTypes/ServerResponses/ServerResponseWelcome.h"
#include "DataTypes/ServerResponses/ServerResponseCharacterList.h"
#include "DataTypes/ServerResponses/ServerResponseCharacterLoaded.h"
#include "DataTypes/ServerResponses/ServerResponseChatStarted.h"
#include "DataTypes/ServerResponses/ServerResponseChatMessage.h"
#include "DataTypes/ServerResponses/ServerResponseChatUpdate.h"
#include "DataTypes/ServerResponses/ServerResponseSpeechTranscription.h"
#include <format>
#include <future>
#include <exception>
#include <string>
#include <vector>
#include <signalrclient/signalr_value.h>
#include <memory>
#include <iostream>
#include <functional>
#include <map>
#include <type_traits>

namespace Voxta
{
	VoxtaClient::VoxtaClient(std::unique_ptr<Utility::SignalR::SignalRWrapperInterface> connectionBuilder,
			Utility::Logging::LoggerInterface& logger,
			const std::function<void(VoxtaClientState newState)>& stateChange,
			const std::function<std::string()>& requestingUserInputEvent,
			const std::function<void(std::string_view currentTranscription, bool finalized)>& transcribedSpeechUpdate,
			const std::function<void(const DataTypes::ChatMessage*, const DataTypes::CharData*)>& charSpeakingEvent) :
		m_hubConnection(std::move(connectionBuilder)),
		m_stateChange(stateChange),
		m_requestingUserInputEvent(requestingUserInputEvent),
		m_transcribedSpeechUpdate(transcribedSpeechUpdate),
		m_charSpeakingEvent(charSpeakingEvent),
		m_logger(logger)
	{
	}

	std::string_view VoxtaClient::GetUsername() const
	{
		if (m_userData)
		{
			return m_userData->m_name;
		}
		return "";
	}

	const std::vector<std::unique_ptr<const DataTypes::CharData>>& VoxtaClient::GetCharacters() const
	{
		return m_characterList;
	}

	const DataTypes::ChatSession* VoxtaClient::GetChatSession() const
	{
		return m_chatSession.get();
	}

	void VoxtaClient::Connect()
	{
		if (m_currentState != VoxtaClientState::DISCONNECTED)
		{
			m_logger.LogMessage(Utility::Logging::LoggerInterface::LogLevel::Warning, "VoxtaClient is already connected, ignoring new connection attempt");
			return;
		}

		StartListeningToServer();

		std::promise<void> startTask;
		m_hubConnection->Start([this, &startTask] (std::exception_ptr exception)
			{
				SafeInvoke([this] ()
					{
						m_logger.LogMessage(Utility::Logging::LoggerInterface::LogLevel::Info, "VoxtaClient connected successfully");
						SendMessageToServer(m_voxtaApi.GetAuthenticateRequestData());
					}, exception);
				startTask.set_value();
			});

		m_currentState = VoxtaClientState::CONNECTING;
		startTask.get_future().get();
	}

	void VoxtaClient::Disconnect()
	{
		if (m_currentState == VoxtaClientState::DISCONNECTED)
		{
			m_logger.LogMessage(Utility::Logging::LoggerInterface::LogLevel::Warning, "VoxtaClient was already disconnected, ignoring new disconnect attempt");
			return;
		}

		std::promise<void> stopTask;
		m_hubConnection->Stop([this, &stopTask] (std::exception_ptr exception)
			{
				SafeInvoke([this] ()
					{
						m_logger.LogMessage(Utility::Logging::LoggerInterface::LogLevel::Info, "VoxtaClient stopped successfully");
					}, exception);
				stopTask.set_value();
			});

		stopTask.get_future().get();
	}

	void VoxtaClient::StartListeningToServer()
	{
		m_hubConnection->On("ReceiveMessage", [this] (const std::vector<signalr::value>& messageContainer)
			{
				if (messageContainer[0].type() != signalr::value_type::map)
				{
					HandleBadResponse(messageContainer[0]);
				}
				else try
				{
					if (!HandleResponse(messageContainer[0].as_map()))
					{
						m_logger.LogMessage(Utility::Logging::LoggerInterface::LogLevel::Error,
							"Received server response that is not (yet) supported.");
					}
				}
				catch (const std::exception& ex)
				{
					std::string error("Something went wrong while parsing the server response:  ");
					error += ex.what();
					m_logger.LogMessage(Utility::Logging::LoggerInterface::LogLevel::Error, error);
				}
			});
	}

	void VoxtaClient::NotifyAudioPlaybackStart(std::string_view messageId, int startIndex, int endIndex, double duration)
	{
		SendMessageToServer(m_voxtaApi.GetNotifyAudioPlaybackStartData(m_chatSession->m_sessionId, messageId, startIndex, endIndex, duration));
	}

	void VoxtaClient::NotifyAudioPlaybackComplete(std::string_view messageId)
	{
		SendMessageToServer(m_voxtaApi.GetNotifyAudioPlaybackCompleteData(m_chatSession->m_sessionId, messageId));
	}

	void VoxtaClient::LoadCharacter(std::string_view characterId)
	{
		SendMessageToServer(m_voxtaApi.GetLoadCharacterRequestData(characterId));
	}

	void VoxtaClient::SendMessageToServer(const signalr::value& message)
	{
		m_hubConnection->Invoke("SendMessage", std::vector<signalr::value> { message },
			[this] (const signalr::value& value, std::exception_ptr exception)
			{
				SafeInvoke([this, value] ()
					{
						if (value.is_string())
						{
							std::string logMsg = "Received: ";
							logMsg.append(value.as_string());
							m_logger.LogMessage(Utility::Logging::LoggerInterface::LogLevel::Info, logMsg);
						}
					}, exception);
			});
	}

	bool VoxtaClient::HandleResponse(const std::map<std::string, signalr::value>& map)
	{
		if (m_voxtaApi.c_ignoredMessageTypes.contains(map.at("$type").as_string()))
		{
			return true;
		}

		auto response = m_voxtaApi.GetResponseData(map);
		if (!response)
		{
			return false;
		}

		switch (response->GetType())
		{
			using enum DataTypes::ServerResponses::ServerResponseType;
			using enum Utility::Logging::LoggerInterface::LogLevel;
			case WELCOME:
				m_logger.LogMessage(Info, "Logged in sucessfully");
				HandleWelcomeResponse(*response);
				return true;
			case CHARACTER_LIST:
				m_logger.LogMessage(Info, "Fetched character list sucessfully");
				HandleCharacterListResponse(*response);
				return true;
			case CHARACTER_LOADED:
				m_logger.LogMessage(Info, "Loaded selected character sucessfully");
				return HandleCharacterLoadedResponse(*response);
			case CHAT_STARTED:
				m_logger.LogMessage(Info, "Started chat session sucessfully");
				HandleChatStartedResponse(*response);
				return true;
			case CHAT_MESSAGE:
				m_logger.LogMessage(Info, "Received chat message update sucessfully");
				HandleChatMessageResponse(*response);
				return true;
			case CHAT_UPDATE:
				m_logger.LogMessage(Info, "Chat has been updated (i.e. user message added)");
				HandleChatUpdateResponse(*response);
				return true;
			case SPEECH_TRANSCRIPTION:
				m_logger.LogMessage(Info, "Server has recognised speech from the audioWebsocket");
				HandleSpeechTranscriptionResponse(*response);
				return true;
			default:
				return false;
		}
	}

	void VoxtaClient::HandleWelcomeResponse(const DataTypes::ServerResponses::ServerResponseBase& response)
	{
		auto derivedResponse = dynamic_cast<const DataTypes::ServerResponses::ServerResponseWelcome*>(&response);
		m_userData = std::make_unique<DataTypes::CharData>(derivedResponse->m_user);
		m_stateChange(VoxtaClientState::AUTHENTICATED);
		SendMessageToServer(m_voxtaApi.GetLoadCharactersListData());
	}

	void VoxtaClient::HandleCharacterListResponse(const DataTypes::ServerResponses::ServerResponseBase& response)
	{
		auto derivedResponse = dynamic_cast<const DataTypes::ServerResponses::ServerResponseCharacterList*>(&response);
		m_characterList.clear();
		for (auto& charElement : derivedResponse->m_characters)
		{
			m_characterList.emplace_back(std::make_unique<DataTypes::CharData>(charElement));
		}
		m_stateChange(VoxtaClientState::CHARACTER_LOBBY);
	}

	bool VoxtaClient::HandleCharacterLoadedResponse(const DataTypes::ServerResponses::ServerResponseBase& response)
	{
		auto derivedResponse = dynamic_cast<const DataTypes::ServerResponses::ServerResponseCharacterLoaded*>(&response);
		if (auto characterIt = std::ranges::find_if(m_characterList.begin(), m_characterList.end(),
			DataTypes::CharDataIdComparer(derivedResponse->m_characterId)); characterIt != std::end(m_characterList))
		{
			SendMessageToServer(m_voxtaApi.GetStartChatRequestData((*characterIt).get()));
			return true;
		}
		else
		{
			m_logger.LogMessage(Utility::Logging::LoggerInterface::LogLevel::Error,
				"Loaded a character that doesn't exist in the list? This should never happen.");
			return false;
		}
	}

	void VoxtaClient::HandleChatStartedResponse(const DataTypes::ServerResponses::ServerResponseBase& response)
	{
		auto derivedResponse = dynamic_cast<const DataTypes::ServerResponses::ServerResponseChatStarted*>(&response);

		std::vector<const DataTypes::CharData*> characters;
		auto& charIds = derivedResponse->m_characterIds;
		for (int i = 0; i < charIds.size(); i++)
		{
			if (auto characterIt = std::ranges::find_if(m_characterList.begin(), m_characterList.end(),
				DataTypes::CharDataIdComparer(charIds[i])); characterIt != std::end(m_characterList))
			{
				characters.emplace_back((*characterIt).get());
			}
		}

		m_chatSession = std::make_unique<DataTypes::ChatSession>(characters, derivedResponse->m_chatId,
			derivedResponse->m_sessionId, derivedResponse->m_serviceIds);
		m_stateChange(VoxtaClientState::CHATTING);
	}

	void VoxtaClient::HandleChatMessageResponse(const DataTypes::ServerResponses::ServerResponseBase& response)
	{
		auto derivedResponse = dynamic_cast<const DataTypes::ServerResponses::ServerResponseChatMessage*>(&response);

		auto& messages = m_chatSession->m_chatMessages;
		using enum DataTypes::ServerResponses::ServerResponseChatMessage::MessageType;
		switch (derivedResponse->m_messageType)
		{
			case MESSAGE_START:
				m_chatSession->m_chatMessages.emplace(std::make_unique<DataTypes::ChatMessage>(
					derivedResponse->m_messageId, derivedResponse->m_senderId));
				break;
			case MESSAGE_CHUNK:
				if (auto chatMessage = std::ranges::find_if(messages.begin(), messages.end(),
					DataTypes::ChatMessageIdComparer(derivedResponse->m_messageId)); chatMessage != std::end(messages))
				{
					(*chatMessage)->m_text.append((*chatMessage)->m_text.empty() ? derivedResponse->m_messageText
						: std::format(" {}", derivedResponse->m_messageText));
					(*chatMessage)->m_audioUrls.emplace_back(derivedResponse->m_audioUrlPath);
				}
				break;
			case MESSAGE_END:
				if (auto chatMessage = std::ranges::find_if(messages.begin(), messages.end(),
					DataTypes::ChatMessageIdComparer(derivedResponse->m_messageId)); chatMessage != std::end(messages))
				{
					if (auto characterIt = std::ranges::find_if(m_characterList.begin(), m_characterList.end(),
						DataTypes::CharDataIdComparer(derivedResponse->m_senderId)); characterIt != std::end(m_characterList))
					{
						m_charSpeakingEvent((*chatMessage).get(), (*characterIt).get());
					}
				}
				m_sentFinalUserMessage = false; // reset flag for accepting a new finalized microphone sentence
				if (!m_usingMicrophoneInput)
				{
					std::string userInputText = m_requestingUserInputEvent();
					SendMessageToServer(m_voxtaApi.GetSendUserMessageData(m_chatSession->m_sessionId, userInputText));
				}
				break;
		}
	}

	void VoxtaClient::HandleChatUpdateResponse(const DataTypes::ServerResponses::ServerResponseBase& response)
	{
		auto derivedResponse = dynamic_cast<const DataTypes::ServerResponses::ServerResponseChatUpdate*>(&response);

		m_chatSession->m_chatMessages.emplace(std::make_unique<DataTypes::ChatMessage>(derivedResponse->m_messageId,
			derivedResponse->m_senderId, derivedResponse->m_text));
	}

	void VoxtaClient::HandleSpeechTranscriptionResponse(const DataTypes::ServerResponses::ServerResponseBase& response)
	{
		auto derivedResponse = dynamic_cast<const DataTypes::ServerResponses::ServerResponseSpeechTranscription*>(&response);

		using enum DataTypes::ServerResponses::ServerResponseSpeechTranscription::TranscriptionState;
		switch (derivedResponse->m_transcriptionState)
		{
			case PARTIAL:
				m_transcribedSpeechUpdate(derivedResponse->m_transcribedSpeech, false);
				break;
			case END:
				if (!m_sentFinalUserMessage)
				{
					m_sentFinalUserMessage = true;
					m_transcribedSpeechUpdate(derivedResponse->m_transcribedSpeech, true);
					SendMessageToServer(m_voxtaApi.GetSendUserMessageData(m_chatSession->m_sessionId, derivedResponse->m_transcribedSpeech));
				}
				break;
			case CANCELLED:
				// Just ignore cancelled statuses right now,
				// for some reason server says it's cancelled but then it picks back up again, idk why.
				break;
		}
	}

	void VoxtaClient::HandleBadResponse(const signalr::value& response)
	{
		using enum signalr::value_type;
		std::string type;
		switch (response.type())
		{
			case array: type = "array"; break;
			case string: type = "string"; break;
			case float64: type = "float64"; break;
			case null: type = "null"; break;
			case boolean: type = "boolean"; break;
			case binary: type = "binary"; break;
			default:
				type = "unkown?";  break;
		}
		m_logger.LogMessage(Utility::Logging::LoggerInterface::LogLevel::Error, std::format("Recieved a message of type "
			"{} from Voxta server, which is currenlty not supported.", type));
	}

	template<typename Callable>
	void VoxtaClient::SafeInvoke(Callable lambda, std::exception_ptr exception)
	{
		try
		{
			if (exception)
			{
				std::rethrow_exception(exception);
			}
			lambda();
		}
		catch (const std::exception& ex)
		{
			m_logger.LogMessage(Utility::Logging::LoggerInterface::LogLevel::Error, ex.what());
		}
		catch (const std::string& ex)
		{
			m_logger.LogMessage(Utility::Logging::LoggerInterface::LogLevel::Error, ex);
		}
		catch (...)
		{
			m_logger.LogMessage(Utility::Logging::LoggerInterface::LogLevel::Error, "how the hell did you manage to get this error?");
		}
	}
}