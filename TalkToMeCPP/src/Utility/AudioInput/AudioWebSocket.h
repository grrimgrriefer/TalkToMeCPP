// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_STL_
#include "../Logging/LoggerInterface.h"
#include "AudioWebSocketStateData.h"
#pragma warning(push)
#pragma warning(disable : 4267)
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/close.hpp>
#include <websocketpp/roles/client_endpoint.hpp>
#pragma warning(pop)
#include <string>
#include <memory>
#include <thread>
#include <mutex>

namespace Utility::AudioInput
{
	/// <summary>
	/// Handles the communication through the websocket regarding the audio input.
	/// NOTE: this is not a generic utility, it's currently specific to the /ws/audio/input/stream api
	/// </summary>
	class AudioWebSocket
	{
	public:
		AudioWebSocket(Logging::LoggerInterface& logger,
			std::string_view serverIP,
			int serverPort);

		~AudioWebSocket();

		bool Connect(std::string_view sessionId);
		void Close(websocketpp::close::status::value code);
		void Send(const char* buffer, unsigned int nBufferFrames);
		void Send(const std::string& message);

	private:
		Logging::LoggerInterface& m_logger;
		std::string m_serverIP;
		int m_serverPort;
		std::string m_sessionId;

		std::mutex m_mutex;
		websocketpp::client<websocketpp::config::asio_client> m_endpoint;
		websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
		websocketpp::lib::shared_ptr<AudioWebSocketStateData> m_connection;
	};
}