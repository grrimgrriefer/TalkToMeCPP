// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "SignalRWrapperInterface.h"
#include "../Logging/ThreadedLogger.h"
#include <string>
#include <exception>
#include <functional>
#include <memory>
#include <vector>
#include <signalrclient/signalr_value.h>
#include <signalrclient/hub_connection.h>

namespace Utility::SignalR
{
	class SignalRWrapper : public SignalRWrapperInterface
	{
	public:
		explicit SignalRWrapper(std::string_view address, int port, Logging::ThreadedLogger& logger);
		~SignalRWrapper() override = default;

		void Start(std::function<void(std::exception_ptr)> callback) noexcept override;
		void Stop(std::function<void(std::exception_ptr)> callback) noexcept override;

		void On(const std::string& event_name,
			const std::function<void(const std::vector<signalr::value>&)>& handler) override;
		void Invoke(const std::string& method_name,
			const std::vector<signalr::value>& arguments = std::vector<signalr::value>(),
			std::function<void(const signalr::value&, std::exception_ptr)> callback =
			[] (const signalr::value&, std::exception_ptr) {}) noexcept override;

	private:
		std::unique_ptr<signalr::hub_connection> m_connection;
	};
}