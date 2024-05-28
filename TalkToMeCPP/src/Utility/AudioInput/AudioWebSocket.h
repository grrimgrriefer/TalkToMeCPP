// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_STL_
#include "AudioWebSocketStateData.h"
#pragma warning(push)
#pragma warning(disable : 4267)
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/close.hpp>
#include <websocketpp/roles/client_endpoint.hpp>
#pragma warning(pop)
#include <string>
#include <thread>
#include <memory>

namespace Utility::AudioInput
{
	class AudioWebSocket
	{
	public:
		AudioWebSocket(std::string_view serverIP, int serverPort);
		~AudioWebSocket();
		bool connect();
		void send(const char* buffer, unsigned int nBufferFrames);
		void send(const std::string& message);
		void close(websocketpp::close::status::value code);

	private:
		websocketpp::client<websocketpp::config::asio_client> m_endpoint;
		websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;
		websocketpp::lib::shared_ptr<AudioWebSocketStateData> m_connection;
		std::string m_serverIP;
		int m_serverPort;
	};
}