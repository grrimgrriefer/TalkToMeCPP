// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
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
	class AudioWebSocketStateData
	{
	public:
		friend std::ostream& operator<< (std::ostream& out, AudioWebSocketStateData const& data);

		AudioWebSocketStateData(websocketpp::connection_hdl hdl, std::string_view uri);

		void on_open(websocketpp::client<websocketpp::config::asio_client>* c, websocketpp::connection_hdl hdl);
		void on_fail(websocketpp::client<websocketpp::config::asio_client>* c, websocketpp::connection_hdl hdl);
		void on_close(websocketpp::client<websocketpp::config::asio_client>* c, websocketpp::connection_hdl hdl);

		websocketpp::connection_hdl get_hdl();
		std::string get_status();
		void record_sent_message(const std::string& message);

	private:
		std::vector<std::string> m_messages;
		websocketpp::connection_hdl m_hdl;
		std::string m_status = "Connecting";
		std::string m_server = "N/A";
		std::string m_uri;
		std::string m_error_reason;
	};
}