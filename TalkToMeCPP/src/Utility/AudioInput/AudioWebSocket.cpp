// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "AudioWebSocket.h"
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
	AudioWebSocket::AudioWebSocket(std::string_view serverIP, int serverPort) : m_serverIP(serverIP), m_serverPort(serverPort)
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

		if (m_connection->get_status() != "Open")
		{
			return;
		}

		std::cout << "> Closing connection " << std::endl;

		websocketpp::lib::error_code ec;
		m_endpoint.close(m_connection->get_hdl(), websocketpp::close::status::going_away, "", ec);
		if (ec)
		{
			std::cout << "> Error closing connection " << ": "
				<< ec.message() << std::endl;
		}
		m_thread->join();
	}

	bool AudioWebSocket::connect()
	{
		websocketpp::lib::error_code ec;
		const std::string uri = std::format("ws://{}:{}/ws/audio/input/stream", m_serverIP, m_serverPort);

		websocketpp::client<websocketpp::config::asio_client>::connection_ptr con = m_endpoint.get_connection(uri, ec);
		if (ec)
		{
			std::cout << "> Connect initialization error: " << ec.message() << std::endl;
			return false;
		}

		websocketpp::lib::shared_ptr<AudioWebSocketStateData> metadata_ptr(std::make_shared<AudioWebSocketStateData>(con->get_handle(), uri));
		m_connection = metadata_ptr;

		con->set_open_handler(websocketpp::lib::bind(
			&AudioWebSocketStateData::on_open,
			metadata_ptr,
			&m_endpoint,
			websocketpp::lib::placeholders::_1
		));
		con->set_fail_handler(websocketpp::lib::bind(
			&AudioWebSocketStateData::on_fail,
			metadata_ptr,
			&m_endpoint,
			websocketpp::lib::placeholders::_1
		));

		m_endpoint.connect(con);

		return true;
	}

	void AudioWebSocket::send(const char* buffer, unsigned int nBufferFrames)
	{
		websocketpp::lib::error_code ec;
		m_endpoint.send(m_connection->get_hdl(), buffer, size_t(nBufferFrames), websocketpp::frame::opcode::binary, ec);
		if (ec)
		{
			std::cout << "> Error sending message: " << ec.message() << std::endl;
			return;
		}

		m_connection->record_sent_message(std::format("binary data bytes: {}", nBufferFrames));
	}

	void AudioWebSocket::send(std::string message)
	{
		websocketpp::lib::error_code ec;
		m_endpoint.send(m_connection->get_hdl(), message, websocketpp::frame::opcode::text, ec);
		if (ec)
		{
			std::cout << "> Error sending message: " << ec.message() << std::endl;
			return;
		}

		m_connection->record_sent_message(message);
	}

	void AudioWebSocket::close(websocketpp::close::status::value code)
	{
		websocketpp::lib::error_code ec;

		m_endpoint.close(m_connection->get_hdl(), code, "", ec);
		if (ec)
		{
			std::cout << "> Error initiating close: " << ec.message() << std::endl;
		}
	}
}