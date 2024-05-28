// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "AudioWebSocketStateData.h"
#pragma warning(push)
#pragma warning(disable : 4267)
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/close.hpp>
#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/roles/client_endpoint.hpp>
#pragma warning(pop)
#include <string>
#include <iosfwd>

namespace Utility::AudioInput
{
	AudioWebSocketStateData::AudioWebSocketStateData(websocketpp::connection_hdl hdl, std::string_view uri)
		: m_hdl(hdl), m_uri(uri)
	{
	}

	void AudioWebSocketStateData::on_open(websocketpp::client<websocketpp::config::asio_client>* c, websocketpp::connection_hdl hdl)
	{
		m_status = "Open";

		websocketpp::client<websocketpp::config::asio_client>::connection_ptr con = c->get_con_from_hdl(hdl);
		m_server = con->get_response_header("Server");
	}

	void AudioWebSocketStateData::on_fail(websocketpp::client<websocketpp::config::asio_client>* c, websocketpp::connection_hdl hdl)
	{
		m_status = "Failed";

		websocketpp::client<websocketpp::config::asio_client>::connection_ptr con = c->get_con_from_hdl(hdl);
		m_server = con->get_response_header("Server");
		m_error_reason = con->get_ec().message();
	}

	void AudioWebSocketStateData::on_close(websocketpp::client<websocketpp::config::asio_client>* c, websocketpp::connection_hdl hdl)
	{
		m_status = "Closed";
		websocketpp::client<websocketpp::config::asio_client>::connection_ptr con = c->get_con_from_hdl(hdl);
		std::stringstream s;
		s << "close code: " << con->get_remote_close_code() << " ("
			<< websocketpp::close::status::get_string(con->get_remote_close_code())
			<< "), close reason: " << con->get_remote_close_reason();
		m_error_reason = s.str();
	}

	websocketpp::connection_hdl AudioWebSocketStateData::get_hdl()
	{
		return m_hdl;
	}

	std::string AudioWebSocketStateData::get_status()
	{
		return m_status;
	}

	void AudioWebSocketStateData::record_sent_message(const std::string& message)
	{
		m_messages.push_back(">> " + message);
	}

	std::ostream& operator<< (std::ostream& out, AudioWebSocketStateData const& data)
	{
		out << "> URI: " << data.m_uri << "\n"
			<< "> Status: " << data.m_status << "\n"
			<< "> Remote Server: " << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
			<< "> Error/close reason: " << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason);

		return out;
	}
}