// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_STL_
#pragma warning(push)
#pragma warning(disable : 4267)
#include <websocketpp/common/connection_hdl.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>
#include <websocketpp/roles/client_endpoint.hpp>
#pragma warning(pop)
#include <string>

namespace Utility::AudioInput
{
	/// <summary>
	/// Contains data regarding the state of the connectionHandle
	/// </summary>
	class AudioWebSocketStateData
	{
	public:
		enum class SocketStateData
		{
			Connecting,
			Open,
			Failed,
			Closed
		};

		AudioWebSocketStateData(websocketpp::connection_hdl hdl, std::string_view uri);

		void OnOpened(websocketpp::client<websocketpp::config::asio_client>* c, websocketpp::connection_hdl hdl);
		void OnFailed(websocketpp::client<websocketpp::config::asio_client>* c, websocketpp::connection_hdl hdl);
		void OnClosed(websocketpp::client<websocketpp::config::asio_client>* c, websocketpp::connection_hdl hdl);

		websocketpp::connection_hdl GetHandle() const;
		SocketStateData GetStatus() const;

	private:
		websocketpp::connection_hdl m_handle;
		SocketStateData m_status = SocketStateData::Connecting;
		std::string m_server = "N/A";
		std::string m_uri;
		std::string m_error_reason;
	};
}