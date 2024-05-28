// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "AudioWebSocketStateData.h"
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/client.hpp>
#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <functional>
#include <format>
#include <iosfwd>
#include <memory>
#include <system_error>

namespace Utility::AudioInput
{
	class AudioWebSocket
	{
	public:
		AudioWebSocket(std::string_view serverIP, int serverPort);
		~AudioWebSocket();
		bool connect();
		void send(const char* buffer, unsigned int nBufferFrames);
		void send(std::string message);
		void close(websocketpp::close::status::value code);

	private:
		websocketpp::client<websocketpp::config::asio_client> m_endpoint;
		websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
		websocketpp::lib::shared_ptr<AudioWebSocketStateData> m_connection;
		std::string m_serverIP;
		int m_serverPort;
	};
}