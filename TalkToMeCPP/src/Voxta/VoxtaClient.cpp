// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "VoxtaClient.h"
#include "VoxtaApiHandler.h"
#include "../Logger/ThreadedLogger.h"
#include "../Logger/HubConnectionLogger.h"
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
#include <format>
#include <future>
#include <exception>
#include <string>
#include <vector>
#include <signalrclient/hub_connection_builder.h>
#include <signalrclient/signalr_value.h>
#include <memory>
#include <iostream>
#include <functional>
#include <map>
#include <type_traits>

namespace Voxta
{
	VoxtaClient::VoxtaClient(Logger::ThreadedLogger& logger, std::string_view address, int port,
		const std::function<void(VoxtaClientState newState)>& stateChange,
		const std::function<std::string()>& requestingUserInputEvent,
		const std::function<void(const DataTypes::ChatMessage*, const DataTypes::CharData*)>& charSpeakingEvent) :
		m_connection(signalr::hub_connection_builder::create(std::format("http://{}:{}/hub", address, std::to_string(port)))
			.with_logging(std::make_shared<Logger::HubConnectionLogger>(logger), signalr::trace_level::verbose).build()),
		m_stateChange(stateChange), m_logger(logger), m_charSpeakingEvent(charSpeakingEvent), m_requestingUserInputEvent(requestingUserInputEvent)
	{
		m_connection.on("ReceiveMessage", [this] (const std::vector<signalr::value>& messageContainer)
			{
				if (messageContainer[0].type() != signalr::value_type::map)
				{
					HandleBadResponse(messageContainer[0]);
				}
				else try
				{
					if (!HandleResponse(messageContainer[0].as_map()))
					{
						m_logger.Log(Logger::ThreadedLogger::LogLevel::ERROR,
							"Received server response that is not (yet) supported.");
					}
				}
				catch (const std::exception& ex)
				{
					std::string error("Something went wrong while parsing the server response:  ");
					error += ex.what();
					m_logger.Log(Logger::ThreadedLogger::LogLevel::ERROR, error);
				}
			});
	}

	std::string_view VoxtaClient::GetUsername() const
	{
		if (m_userData)
		{
			return m_userData->m_name;
		}
		return "";
	}

	const std::vector<std::unique_ptr<DataTypes::CharData>>& VoxtaClient::GetCharacters() const
	{
		return m_characterList;
	}

	const DataTypes::ChatSession* VoxtaClient::GetChatSession() const
	{
		return m_chatSession.get();
	}

	void VoxtaClient::Connect()
	{
		std::promise<void> startTask;
		m_connection.start([this, &startTask] (std::exception_ptr exception)
			{
				SafeInvoke([this] ()
					{
						m_logger.Log(Logger::ThreadedLogger::LogLevel::INFO, "VoxtaClient connected successfully");
						SendMessage(m_voxtaCommData.GetRequestData(VoxtaApiHandler::VoxtaGenericRequestType::AUTHENTICATE));
					}, exception);
				startTask.set_value();
			});

		startTask.get_future().get();
	}

	void VoxtaClient::Disconnect()
	{
		std::promise<void> stopTask;
		m_connection.stop([this, &stopTask] (std::exception_ptr exception)
			{
				SafeInvoke([this] ()
					{
						m_logger.Log(Logger::ThreadedLogger::LogLevel::INFO, "VoxtaClient stopped successfully");
					}, exception);
				stopTask.set_value();
			});

		stopTask.get_future().get();
	}

	void VoxtaClient::LoadCharacter(std::string_view characterId)
	{
		SendMessage(m_voxtaCommData.GetLoadCharacterRequestData(characterId));
	}

	void VoxtaClient::SendMessage(const signalr::value& message)
	{
		m_connection.invoke("SendMessage", std::vector<signalr::value> { message },
			[this] (const signalr::value& value, std::exception_ptr exception)
			{
				SafeInvoke([this, value] ()
					{
						if (value.is_string())
						{
							std::string logMsg = "Received: ";
							logMsg.append(value.as_string());
							m_logger.Log(Logger::ThreadedLogger::LogLevel::INFO, logMsg);
						}
					}, exception);
			});
	}

	bool VoxtaClient::HandleResponse(const std::map<std::string, signalr::value>& map)
	{
		if (m_voxtaCommData.c_ignoredMessageTypes.contains(map.at("$type").as_string()))
		{
			return true;
		}

		auto response = m_voxtaCommData.GetResponseData(map);
		if (response)
		{
			using enum DataTypes::ServerResponses::ServerResponseType;
			switch (response->GetType())
			{
				case WELCOME:
				{
					m_logger.Log(Logger::ThreadedLogger::LogLevel::INFO, "Logged in sucessfully");
					auto derivedResponse = dynamic_cast<DataTypes::ServerResponses::ServerResponseWelcome*>(response.get());
					m_userData = std::make_unique<DataTypes::CharData>(derivedResponse->m_user);
					SendMessage(m_voxtaCommData.GetRequestData(VoxtaApiHandler::VoxtaGenericRequestType::LOAD_CHARACTERS_LIST));
					return true;
				}
				case CHARACTER_LIST:
				{
					m_logger.Log(Logger::ThreadedLogger::LogLevel::INFO, "Fetched character list sucessfully");
					auto derivedResponse = dynamic_cast<DataTypes::ServerResponses::ServerResponseCharacterList*>(response.get());
					m_characterList.clear();
					for (auto& charElement : derivedResponse->m_characters)
					{
						m_characterList.emplace_back(std::make_unique<DataTypes::CharData>(charElement));
					}
					m_stateChange(VoxtaClientState::CHARACTER_LOBBY);
					return true;
				}
				case CHARACTER_LOADED:
				{
					m_logger.Log(Logger::ThreadedLogger::LogLevel::INFO, "Loaded selected character sucessfully");
					auto derivedResponse = dynamic_cast<DataTypes::ServerResponses::ServerResponseCharacterLoaded*>(response.get());
					if (auto characterIt = std::ranges::find_if(m_characterList.begin(), m_characterList.end(),
						DataTypes::CharDataIdComparer(derivedResponse->m_characterId)); characterIt != std::end(m_characterList))
					{
						SendMessage(m_voxtaCommData.GetStartChatRequestData((*characterIt).get()));
					}
					else
					{
						m_logger.Log(Logger::ThreadedLogger::LogLevel::ERROR,
							"Loaded a character that doesn't exist in the list? This should never happen.");
					}
					return true;
				}
				case CHAT_STARTED:
				{
					m_logger.Log(Logger::ThreadedLogger::LogLevel::INFO, "Started chat session sucessfully");
					auto derivedResponse = dynamic_cast<DataTypes::ServerResponses::ServerResponseChatStarted*>(response.get());

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
					return true;
				}
				case CHAT_MESSAGE:
				{
					m_logger.Log(Logger::ThreadedLogger::LogLevel::INFO, "Received chat message update sucessfully");
					auto derivedResponse = dynamic_cast<DataTypes::ServerResponses::ServerResponseChatMessage*>(response.get());

					auto& messages = m_chatSession->m_chatMessages;
					using enum DataTypes::ServerResponses::ServerResponseChatMessage::MessageType;
					switch (derivedResponse->m_messageType)
					{
						case MESSAGE_START:
							m_chatSession->m_chatMessages.emplace(std::make_unique<DataTypes::ChatMessage>(derivedResponse->m_messageId, derivedResponse->m_senderId));
							break;
						case MESSAGE_CHUNK:
							if (auto chatMessage = std::ranges::find_if(messages.begin(), messages.end(),
								DataTypes::ChatMessageIdComparer(derivedResponse->m_messageId)); chatMessage != std::end(messages))
							{
								(*chatMessage)->m_text.append(std::format(" {}", derivedResponse->m_messageText));
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

							std::string userInputText = m_requestingUserInputEvent();
							SendMessage(m_voxtaCommData.ConstructSendUserMessage(m_chatSession->m_sessionId, userInputText));
							break;
					}
					return true;
				}
				case CHAT_UPDATE:
				{
					m_logger.Log(Logger::ThreadedLogger::LogLevel::INFO, "Chat has been updated (i.e. user message added)");
					auto derivedResponse = dynamic_cast<DataTypes::ServerResponses::ServerResponseChatUpdate*>(response.get());

					m_chatSession->m_chatMessages.emplace(std::make_unique<DataTypes::ChatMessage>(derivedResponse->m_messageId,
						derivedResponse->m_senderId, derivedResponse->m_text));
					return true;
				}
				default:
					return false;
			}
		}
		else
		{
			return false;
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
		m_logger.Log(Logger::ThreadedLogger::LogLevel::ERROR, std::format("Recieved a message of type "
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
			m_logger.Log(Logger::ThreadedLogger::LogLevel::ERROR, ex.what());
		}
	}
}