// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "../Logging/LoggerInterface.h"
#include "SignalRWrapperInterface.h"
#include <exception>
#include <functional>
#include <memory>
#include <signalrclient/hub_connection.h>
#include <signalrclient/signalr_value.h>
#include <string>
#include <vector>

namespace Utility::SignalR
{
	/// <summary>
	/// Microsoft put their hubconnection behind a locked hubconnectionbuilder so we need this wrapper in order to
	/// not leak their shitty bad practices into our nice dependency-injection style code.
	/// </summary>
	class SignalRWrapper : public SignalRWrapperInterface
	{
	public:
		explicit SignalRWrapper(std::string_view address, int port, Logging::LoggerInterface& logger);
		~SignalRWrapper() override = default;

		void Start(std::function<void(std::exception_ptr)> callback) noexcept override;
		void Stop(std::function<void(std::exception_ptr)> callback) noexcept override;

		void On(const std::string& event_name,
			const std::function<void(const std::vector<signalr::value>&)>& handler) override;
		void Invoke(const std::string& method_name,
			const std::vector<signalr::value>& arguments = std::vector<signalr::value>(),
			std::function<void(const signalr::value&, std::exception_ptr)> callback =
			[] (const signalr::value&, std::exception_ptr) { /*empty cuz optional*/ }) noexcept override;

	private:
		std::unique_ptr<signalr::hub_connection> m_connection;
	};
}
