// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "AudioWebSocket.h"
#include "AudioWebSocketStateData.h"
#include "../Logging/LoggerInterface.h"
#pragma warning(push)
#pragma warning(disable : 4267)
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/close.hpp>
#include <websocketpp/frame.hpp>
#include <websocketpp/logger/levels.hpp>
#include <websocketpp/roles/client_endpoint.hpp>
#pragma warning(pop)
#include <string>
#include <functional>
#include <format>
#include <memory>
#include <thread>
#include <system_error>
#include <mutex>

namespace Utility::AudioInput
{
	AudioWebSocket::AudioWebSocket(Logging::LoggerInterface& logger,
			std::string_view serverIP,
			int serverPort) :
		m_logger(logger),
		m_serverIP(serverIP),
		m_serverPort(serverPort)
	{
		m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
		m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

		m_endpoint.init_asio();
		m_endpoint.start_perpetual();

		m_thread.reset(new websocketpp::lib::thread(&websocketpp::client<websocketpp::config::asio_client>::run, &m_endpoint));
	}

	AudioWebSocket::~AudioWebSocket()
	{
		m_endpoint.stop_perpetual();

		if (m_connection->GetStatus() != AudioWebSocketStateData::SocketStateData::Open)
		{
			return;
		}

		m_logger.LogMessage(Logging::LoggerInterface::LogLevel::Info, "Closing AudioWebSocket connection ");

		websocketpp::lib::error_code ec;
		try
		{
			m_endpoint.close(m_connection->GetHandle(), websocketpp::close::status::going_away, "", ec);
		}
		catch (const std::exception& ex)
		{
			m_logger.LogMessage(Logging::LoggerInterface::LogLevel::Error,
				std::format("Error closing connection: {} ", ex.what()));
		}
		if (ec)
		{
			m_logger.LogMessage(Logging::LoggerInterface::LogLevel::Error,
				std::format("Error closing connection: {} ", ec.message()));
		}
		m_thread->join();
	}

	bool AudioWebSocket::Connect()
	{
		std::scoped_lock<std::mutex> lock(m_mutex);
		websocketpp::lib::error_code ec;
		const std::string uri = std::format("ws://{}:{}/ws/audio/input/stream", m_serverIP, m_serverPort);

		websocketpp::client<websocketpp::config::asio_client>::connection_ptr con = m_endpoint.get_connection(uri, ec);
		if (ec)
		{
			m_logger.LogMessage(Logging::LoggerInterface::LogLevel::Error,
				std::format("Connect initialization error: {} ", ec.message()));
			return false;
		}

		auto metadata_ptr(std::make_shared<AudioWebSocketStateData>(con->get_handle(), uri));
		m_connection = metadata_ptr;

		con->set_open_handler(websocketpp::lib::bind(
			&AudioWebSocketStateData::OnOpened,
			metadata_ptr,
			&m_endpoint,
			websocketpp::lib::placeholders::_1
		));
		con->set_fail_handler(websocketpp::lib::bind(
			&AudioWebSocketStateData::OnFailed,
			metadata_ptr,
			&m_endpoint,
			websocketpp::lib::placeholders::_1
		));

		m_endpoint.connect(con);
		return true;
	}

	void AudioWebSocket::Close(websocketpp::close::status::value code)
	{
		std::scoped_lock<std::mutex> lock(m_mutex);
		websocketpp::lib::error_code ec;

		m_endpoint.close(m_connection->GetHandle(), code, "", ec);
		if (ec)
		{
			m_logger.LogMessage(Logging::LoggerInterface::LogLevel::Error,
				std::format("Error initiating close: {} ", ec.message()));
		}
	}

	void AudioWebSocket::Send(const char* buffer, unsigned int nBufferFrames)
	{
		std::scoped_lock<std::mutex> lock(m_mutex);
		websocketpp::lib::error_code ec;

		m_endpoint.send(m_connection->GetHandle(), buffer, size_t(nBufferFrames), websocketpp::frame::opcode::binary, ec);
		if (ec)
		{
			m_logger.LogMessage(Logging::LoggerInterface::LogLevel::Error,
				std::format("Error sending message: {} ", ec.message()));
			return;
		}
	}

	void AudioWebSocket::Send(const std::string& message)
	{
		std::scoped_lock<std::mutex> lock(m_mutex);
		websocketpp::lib::error_code ec;

		m_endpoint.send(m_connection->GetHandle(), message, websocketpp::frame::opcode::text, ec);
		if (ec)
		{
			m_logger.LogMessage(Logging::LoggerInterface::LogLevel::Error,
				std::format("Error sending message: {} ", ec.message()));
			return;
		}
	}
}