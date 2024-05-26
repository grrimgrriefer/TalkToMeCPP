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

namespace Utility::AudioInput
{
	typedef websocketpp::client<websocketpp::config::asio_client> client;

	class connection_metadata
	{
	public:
		typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;

		connection_metadata(int id, websocketpp::connection_hdl hdl, std::string uri)
			: m_id(id)
			, m_hdl(hdl)
			, m_status("Connecting")
			, m_uri(uri)
			, m_server("N/A")
		{
		}

		void on_open(client* c, websocketpp::connection_hdl hdl)
		{
			m_status = "Open";

			client::connection_ptr con = c->get_con_from_hdl(hdl);
			m_server = con->get_response_header("Server");
		}

		void on_fail(client* c, websocketpp::connection_hdl hdl)
		{
			m_status = "Failed";

			client::connection_ptr con = c->get_con_from_hdl(hdl);
			m_server = con->get_response_header("Server");
			m_error_reason = con->get_ec().message();
		}

		void on_close(client* c, websocketpp::connection_hdl hdl)
		{
			m_status = "Closed";
			client::connection_ptr con = c->get_con_from_hdl(hdl);
			std::stringstream s;
			s << "close code: " << con->get_remote_close_code() << " ("
				<< websocketpp::close::status::get_string(con->get_remote_close_code())
				<< "), close reason: " << con->get_remote_close_reason();
			m_error_reason = s.str();
		}

		websocketpp::connection_hdl get_hdl()
		{
			return m_hdl;
		}

		std::string get_status()
		{
			return m_status;
		}

		int get_id()
		{
			return m_id;
		}

		void record_sent_message(std::string message)
		{
			m_messages.push_back(">> " + message);
		}

		friend std::ostream& operator<< (std::ostream& out, connection_metadata const& data);
	private:
		std::vector<std::string> m_messages;
		int m_id;
		websocketpp::connection_hdl m_hdl;
		std::string m_status;
		std::string m_uri;
		std::string m_server;
		std::string m_error_reason;
	};

	class websocket_endpoint
	{
	public:
		websocket_endpoint() : m_next_id(0)
		{
			m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
			m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

			m_endpoint.init_asio();
			m_endpoint.start_perpetual();

			m_thread.reset(new websocketpp::lib::thread(&client::run, &m_endpoint));
		}

		~websocket_endpoint()
		{
			m_endpoint.stop_perpetual();

			for (con_list::const_iterator it = m_connection_list.begin(); it != m_connection_list.end(); ++it)
			{
				if (it->second->get_status() != "Open")
				{
					// Only close open connections
					continue;
				}

				std::cout << "> Closing connection " << it->second->get_id() << std::endl;

				websocketpp::lib::error_code ec;
				m_endpoint.close(it->second->get_hdl(), websocketpp::close::status::going_away, "", ec);
				if (ec)
				{
					std::cout << "> Error closing connection " << it->second->get_id() << ": "
						<< ec.message() << std::endl;
				}
			}

			m_thread->join();
		}

		int connect(std::string const& uri)
		{
			websocketpp::lib::error_code ec;

			client::connection_ptr con = m_endpoint.get_connection(uri, ec);

			if (ec)
			{
				std::cout << "> Connect initialization error: " << ec.message() << std::endl;
				return -1;
			}

			int new_id = m_next_id++;
			connection_metadata::ptr metadata_ptr(new connection_metadata(new_id, con->get_handle(), uri));
			m_connection_list[new_id] = metadata_ptr;

			con->set_open_handler(websocketpp::lib::bind(
				&connection_metadata::on_open,
				metadata_ptr,
				&m_endpoint,
				websocketpp::lib::placeholders::_1
			));
			con->set_fail_handler(websocketpp::lib::bind(
				&connection_metadata::on_fail,
				metadata_ptr,
				&m_endpoint,
				websocketpp::lib::placeholders::_1
			));

			m_endpoint.connect(con);

			return new_id;
		}

		void send(int id, const char* buffer, unsigned int nBufferFrames)
		{
			websocketpp::lib::error_code ec;

			con_list::iterator metadata_it = m_connection_list.find(id);
			if (metadata_it == m_connection_list.end())
			{
				std::cout << "> No connection found with id " << id << std::endl;
				return;
			}

			m_endpoint.send(metadata_it->second->get_hdl(), buffer, size_t(nBufferFrames), websocketpp::frame::opcode::binary, ec);
			if (ec)
			{
				std::cout << "> Error sending message: " << ec.message() << std::endl;
				return;
			}

			metadata_it->second->record_sent_message(std::format("binary data bytes: {}", nBufferFrames));
		}

		void send(int id, std::string message)
		{
			websocketpp::lib::error_code ec;

			con_list::iterator metadata_it = m_connection_list.find(id);
			if (metadata_it == m_connection_list.end())
			{
				std::cout << "> No connection found with id " << id << std::endl;
				return;
			}

			m_endpoint.send(metadata_it->second->get_hdl(), message, websocketpp::frame::opcode::text, ec);
			if (ec)
			{
				std::cout << "> Error sending message: " << ec.message() << std::endl;
				return;
			}

			metadata_it->second->record_sent_message(message);
		}

		connection_metadata::ptr get_metadata(int id) const
		{
			con_list::const_iterator metadata_it = m_connection_list.find(id);
			if (metadata_it == m_connection_list.end())
			{
				return connection_metadata::ptr();
			}
			else
			{
				return metadata_it->second;
			}
		}

		void close(int id, websocketpp::close::status::value code)
		{
			websocketpp::lib::error_code ec;

			con_list::iterator metadata_it = m_connection_list.find(id);
			if (metadata_it == m_connection_list.end())
			{
				std::cout << "> No connection found with id " << id << std::endl;
				return;
			}

			m_endpoint.close(metadata_it->second->get_hdl(), code, "", ec);
			if (ec)
			{
				std::cout << "> Error initiating close: " << ec.message() << std::endl;
			}
		}

	private:
		typedef std::map<int, connection_metadata::ptr> con_list;

		client m_endpoint;
		websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;

		con_list m_connection_list;
		int m_next_id;
	};
}