// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "SignalRWrapper.h"
#include "../Logging/ThreadedLogger.h"
#include "../Logging/HubConnectionLogger.h"
#include <format>
#include <signalrclient/hub_connection_builder.h>
#include <memory>

namespace Utility::SignalR
{
	SignalRWrapper::SignalRWrapper(std::string_view address, int port, Logging::ThreadedLogger& logger) :
		m_connection(std::make_unique<signalr::hub_connection>(signalr::hub_connection_builder::create(std::format("http://{}:{}/hub", address, std::to_string(port)))
			.with_logging(std::make_shared<Logging::HubConnectionLogger>(logger), signalr::trace_level::verbose).build()))
	{}

	void SignalRWrapper::Start(std::function<void(std::exception_ptr)> callback) noexcept
	{
		m_connection->start(callback);
	}

	void SignalRWrapper::Stop(std::function<void(std::exception_ptr)> callback) noexcept
	{
		m_connection->stop(callback);
	}

	void SignalRWrapper::On(const std::string& event_name, const std::function<void(const std::vector<signalr::value>&)>& handler)
	{
		m_connection->on(event_name, handler);
	}

	void SignalRWrapper::Invoke(const std::string& method_name, const std::vector<signalr::value>& arguments,
		std::function<void(const signalr::value&, std::exception_ptr)> callback) noexcept
	{
		m_connection->invoke(method_name, arguments, callback);
	}
}