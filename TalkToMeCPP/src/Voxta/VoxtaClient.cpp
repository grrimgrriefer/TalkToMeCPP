// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "VoxtaClient.h"
#include "VoxtaApiHandler.h"
#include "../Logger/ThreadedLogger.h"
#include "../Logger/HubConnectionLogger.h"
#include "DataTypes/CharData.h"
#include "DataTypes/VoxtaResponseBase.h"
#include "DataTypes/VoxtaResponseWelcome.h"
#include "DataTypes/VoxtaResponseCharacterList.h"
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

namespace Voxta
{
	VoxtaClient::VoxtaClient(Logger::ThreadedLogger& logger, std::string_view address, int port,
		const std::function<void(VoxtaClientState newState)>& stateChange) :
		m_connection(signalr::hub_connection_builder::create(std::format("http://{}:{}/hub", address, std::to_string(port)))
			.with_logging(std::make_shared<Logger::HubConnectionLogger>(logger), signalr::trace_level::verbose).build()),
		m_stateChange(stateChange),
		m_logger(logger)
	{
		m_connection.on("ReceiveMessage", [this] (const std::vector<signalr::value>& messageContainer)
			{
				if (messageContainer[0].type() != signalr::value_type::map)
				{
					HandleBadResponse(messageContainer[0]);
				}
				else
				{
					try
					{
						if (!HandleResponse(messageContainer[0].as_map()))
						{
							m_logger.Log(Logger::ThreadedLogger::LogLevel::ERROR,
								"Received server response that is not (yet) supported.");
						}
					}
					catch (const std::exception& ex)
					{
						std::string error("Something went wrong while parsing the server response.");
						error += ex.what();
						m_logger.Log(Logger::ThreadedLogger::LogLevel::ERROR, error);
					}
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

	const std::vector<DataTypes::CharData>& VoxtaClient::GetCharacters() const
	{
		return m_characterList;
	}

	void VoxtaClient::Connect()
	{
		std::promise<void> startTask;
		m_connection.start([this, &startTask] (std::exception_ptr exception)
			{
				SafeInvoke([this] ()
					{
						m_logger.Log(Logger::ThreadedLogger::LogLevel::INFO, "VoxtaClient connected successfully");
						SendMessage(m_voxtaCommData.GetRequestData(VoxtaApiHandler::VoxtaRequestType::AUTHENTICATE));
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
		SendMessage(m_voxtaCommData.GetRequestData<std::string_view>(VoxtaApiHandler::VoxtaRequestType::LOAD_CHARACTER, characterId));
	}

	void VoxtaClient::StartChat(std::string_view sessionData, std::string_view contextKey,
		const std::vector<std::string_view>& characterFunctions, std::string_view context)
	{
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
							std::string messagePotato = "Received: ";
							messagePotato.append(value.as_string());
							m_logger.Log(Logger::ThreadedLogger::LogLevel::INFO, messagePotato);
						}
					}, exception);
			});
	}

	bool VoxtaClient::HandleResponse(const std::map<std::string, signalr::value>& map)
	{
		std::unique_ptr<Voxta::DataTypes::VoxtaResponseBase> response = m_voxtaCommData.GetResponseData(map);

		if (response)
		{
			switch (response->GetType())
			{
				case DataTypes::VoxtaResponseType::WELCOME:
				{
					m_logger.Log(Logger::ThreadedLogger::LogLevel::INFO, "Logged in sucessfully");
					auto derivedResponse = dynamic_cast<Voxta::DataTypes::VoxtaResponseWelcome*>(response.get());
					m_userData = std::make_unique<DataTypes::CharData>(derivedResponse->m_user);
					SendMessage(m_voxtaCommData.GetRequestData(VoxtaApiHandler::VoxtaRequestType::LOAD_CHARACTERS_LIST));
					break;
				}
				case DataTypes::VoxtaResponseType::CHARACTER_LIST:
				{
					m_logger.Log(Logger::ThreadedLogger::LogLevel::INFO, "Fetched character list sucessfully");
					auto derivedResponse = dynamic_cast<Voxta::DataTypes::VoxtaResponseCharacterList*>(response.get());
					m_characterList = derivedResponse->m_characters;
					m_stateChange(VoxtaClientState::CHARACTER_LOBBY);
					break;
				}
			}
			return true;
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

	void VoxtaClient::SafeInvoke(const std::function<void()>& lambda, std::exception_ptr exception)
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