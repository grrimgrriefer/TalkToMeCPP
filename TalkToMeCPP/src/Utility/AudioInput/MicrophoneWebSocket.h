// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include <SDKDDKVer.h>
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <queue>

namespace Utility::AudioInput
{
	namespace beast = boost::beast;
	namespace http = beast::http;
	namespace websocket = beast::websocket;
	namespace net = boost::asio;
	using tcp = net::ip::tcp;

	class MicrophoneWebSocket
	{
	public:
		MicrophoneWebSocket(std::string_view serverIP, int serverPort)
			: m_serverIP(serverIP), m_serverPort(serverPort),
			m_resolver(m_ioc),
			m_ws(net::make_strand(m_ioc))
		{
		}

		void Start()
		{
			Connect();
			m_work.reset(new net::io_context::work(m_ioc)); // Keep the io_context alive.
			m_ioc_thread = std::thread([this] { m_ioc.run(); });
		}

		void Stop()
		{
			m_work.reset(); // Allow the io_context to run out of work and stop.
			m_ioc.stop();
			if (m_ioc_thread.joinable())
				m_ioc_thread.join();
		}

		~MicrophoneWebSocket()
		{
			Stop();
		}

		void SendData(const std::vector<char> data)
		{
			net::post(m_ws.get_executor(), [this, data] ()->void
				{
					m_writeQueue.push(data);
					if (!m_writeInProgress)
					{
						m_writeInProgress = true;
						DoWrite();
					}
				});
		}

	private:
		void Connect()
		{
			try
			{
				auto const results = m_resolver.resolve(m_serverIP, std::to_string(m_serverPort));
				auto ep = net::connect(m_ws.next_layer(), results);
				m_ws.handshake(m_serverIP + ':' + std::to_string(ep.port()), "/ws/audio/input/stream");

				std::string data = "{\"contentType\":\"audio/wav\", \"sampleRate\" : 16000, \"channels\" : 1, \"bitsPerSample\" : 16, \"bufferMilliseconds\" : 30}";
				m_ws.write(net::buffer(data));
			}
			catch (const std::exception& e)
			{
				std::cerr << "Connection error: " << e.what() << std::endl;
				// Implement reconnection strategy here if necessary
			}
		}

		void DoWrite()
		{
			if (!m_writeQueue.empty())
			{
				std::vector<char> data = m_writeQueue.front();
				m_ws.async_write(net::buffer(data), std::bind(&MicrophoneWebSocket::OnWrite, this, std::placeholders::_1, std::placeholders::_2));
			}
		}

		void OnWrite(boost::system::error_code ec, std::size_t)
		{
			if (!ec)
			{
				m_writeQueue.pop();
				if (!m_writeQueue.empty())
				{
					DoWrite();
				}
				else
				{
					m_writeInProgress = false;
				}
			}
			else
			{
				std::cerr << "Write error: " << ec.message() << " | Error value: " << ec.value() << std::endl;
				m_writeInProgress = false;
			}
		}

		std::string m_serverIP;
		int m_serverPort;
		net::io_context m_ioc;
		tcp::resolver m_resolver;
		websocket::stream<tcp::socket> m_ws;
		std::thread m_ioc_thread;
		std::unique_ptr<net::io_context::work> m_work;
		bool m_writeInProgress = false;
		std::queue<std::vector<char>> m_writeQueue;
	};
}