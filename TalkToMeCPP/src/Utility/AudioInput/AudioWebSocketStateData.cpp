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
#include <format>

namespace Utility::AudioInput
{
	AudioWebSocketStateData::AudioWebSocketStateData(websocketpp::connection_hdl hdl, std::string_view uri) :
		m_handle(hdl), m_uri(uri)
	{
	}

	void AudioWebSocketStateData::OnOpened(websocketpp::client<websocketpp::config::asio_client>* c, websocketpp::connection_hdl hdl)
	{
		m_status = AudioWebSocketStateData::SocketStateData::Open;

		websocketpp::client<websocketpp::config::asio_client>::connection_ptr con = c->get_con_from_hdl(hdl);
		m_server = con->get_response_header("Server");
	}

	void AudioWebSocketStateData::OnFailed(websocketpp::client<websocketpp::config::asio_client>* c, websocketpp::connection_hdl hdl)
	{
		m_status = AudioWebSocketStateData::SocketStateData::Failed;

		websocketpp::client<websocketpp::config::asio_client>::connection_ptr con = c->get_con_from_hdl(hdl);
		m_server = con->get_response_header("Server");
		m_error_reason = con->get_ec().message();
	}

	void AudioWebSocketStateData::OnClosed(websocketpp::client<websocketpp::config::asio_client>* c, websocketpp::connection_hdl hdl)
	{
		m_status = AudioWebSocketStateData::SocketStateData::Closed;
		websocketpp::client<websocketpp::config::asio_client>::connection_ptr con = c->get_con_from_hdl(hdl);
		std::stringstream s;
		s << "close code: " << con->get_remote_close_code() << " ("
			<< websocketpp::close::status::get_string(con->get_remote_close_code())
			<< "), close reason: " << con->get_remote_close_reason();
		m_error_reason = s.str();
	}

	websocketpp::connection_hdl AudioWebSocketStateData::GetHandle() const
	{
		return m_handle;
	}

	AudioWebSocketStateData::SocketStateData AudioWebSocketStateData::GetStatus() const
	{
		return m_status;
	}
}