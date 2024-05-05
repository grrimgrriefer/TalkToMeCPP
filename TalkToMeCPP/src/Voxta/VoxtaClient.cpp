// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "VoxtaClient.h"
#include "../Logger/ThreadedLogger.h"
#include <format>
#include <future>
#include <exception>
#include <functional>
#include <string>
#include <system_error>
#include <vector>
#include <signalrclient/hub_connection_builder.h>
#include <signalrclient/signalr_value.h>
#include <memory>
#include <signalrclient/log_writer.h>
#include <iostream>

namespace Voxta
{
	class loggerino : public signalr::log_writer
	{
		void __cdecl write(const std::string& entry) override
		{
			std::cout << entry;
		}
	};

	VoxtaClient::VoxtaClient(Logger::ThreadedLogger& logger, std::string_view address, int port)
		: m_logger(logger),
		connection(signalr::hub_connection_builder::create(std::vformat("http://{0}:{1}/hub", std::make_format_args(address, port))).with_logging(std::make_shared<loggerino>(), signalr::trace_level::verbose).build())
	{
		/*connection.on("ReceiveMessage", [this] (const std::vector<signalr::value>& m)
			{
				m_logger.Log(Logger::ThreadedLogger::LogLevel::INFO, m[0].as_string());
			});*/
	}

	void VoxtaClient::Connect()
	{
		std::promise<void> startTask;
		connection.start([this, &startTask] (std::exception_ptr exception)
		{
			try
			{
				if (exception)
				{
					std::rethrow_exception(exception);
				}

				m_logger.Log(Logger::ThreadedLogger::LogLevel::INFO, "Connection connected successfully");

				Authenticate();
			}
			catch (const std::exception& ex)
			{
				m_logger.Log(Logger::ThreadedLogger::LogLevel::ERROR, ex.what());
			}

			startTask.set_value();
		});

		startTask.get_future().get();
	}

	void VoxtaClient::Disconnect()
	{
		std::promise<void> stopTask;
		connection.stop([this, &stopTask] (std::exception_ptr exception)
		{
			try
			{
				if (exception)
				{
					std::rethrow_exception(exception);
				}

				m_logger.Log(Logger::ThreadedLogger::LogLevel::INFO, "Connection stopped successfully");
			}
			catch (const std::exception& ex)
			{
				m_logger.Log(Logger::ThreadedLogger::LogLevel::ERROR, ex.what());
			}
			stopTask.set_value();
		});

		stopTask.get_future().get();
	}

	void VoxtaClient::LoadCharacter(std::string_view characterId)
	{
	}

	void VoxtaClient::StartChat(std::string_view sessionData, std::string_view contextKey, const std::vector<std::string_view>& characterFunctions, std::string_view context)
	{
	}

	void VoxtaClient::Authenticate()
	{
		std::map<std::string, signalr::value> myMap = {
			{ "$type", "authenticate" },
			{ "client", "TalkToMeCPP" },
			{ "clientVersion", "0.0.1a" },
			{ "scope", std::vector<signalr::value> { "role:app" } },
			{ "capabilities", std::map<std::string, signalr::value> {
				{ "audioInput", "WebSocketStream" },
				{ "audioOutput", "Url" },
				{ "acceptedAudioContentTypes", std::vector<signalr::value> { "audio/x-wav" } }
			} }
		};
		SendMessage(myMap);
	}

	void VoxtaClient::SendMessage(const signalr::value& message)
	{
		std::vector<signalr::value> args{ message };

		connection.invoke("SendMessage", args, [this] (const signalr::value& value, std::exception_ptr exception)
		{
			try
			{
				if (exception)
				{
					std::rethrow_exception(exception);
				}

				if (value.is_string())
				{
					std::string messagePotato = "Received: ";
					messagePotato.append(value.as_string());
					m_logger.Log(Logger::ThreadedLogger::LogLevel::ERROR, messagePotato);
				}
			}
			catch (const std::exception& ex)
			{
				m_logger.Log(Logger::ThreadedLogger::LogLevel::ERROR, ex.what());
			}
		});
	}
}