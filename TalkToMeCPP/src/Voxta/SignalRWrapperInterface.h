// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include <exception>
#include <functional>
#include <string>
#include <vector>
#include <signalrclient/connection_state.h>
#include <signalrclient/signalr_client_config.h>
#include <signalrclient/signalr_value.h>

namespace Voxta
{
	class SignalRWrapperInterface
	{
	public:
		virtual ~SignalRWrapperInterface() = default;

		virtual void start(std::function<void(std::exception_ptr)> callback) noexcept = 0;
		virtual void stop(std::function<void(std::exception_ptr)> callback) noexcept = 0;

		virtual void on(const std::string& event_name, const std::function<void(const std::vector<signalr::value>&)>& handler) = 0;
		virtual void invoke(const std::string& method_name, const std::vector<signalr::value>& arguments = std::vector<signalr::value>(), std::function<void(const signalr::value&, std::exception_ptr)> callback = [] (const signalr::value&, std::exception_ptr) {}) noexcept = 0;

		//virtual signalr::connection_state get_connection_state() const = 0;
		//virtual std::string get_connection_id() const = 0;
		//virtual void set_disconnected(const std::function<void __cdecl(std::exception_ptr)>& disconnected_callback) = 0;
		//virtual void set_client_config(const signalr::signalr_client_config& config) = 0;
		//virtual void send(const std::string& method_name, const std::vector<signalr::value>& arguments = std::vector<signalr::value>(), std::function<void(std::exception_ptr)> callback = [] (std::exception_ptr) {}) noexcept = 0;
	};
}
