// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include <string>
#include <exception>
#include <functional>
#include <vector>
#include <signalrclient/signalr_value.h>

namespace Utility::SignalR
{
	class SignalRWrapperInterface
	{
	public:
		virtual ~SignalRWrapperInterface() = default;

		virtual void Start(std::function<void(std::exception_ptr)> callback) noexcept = 0;
		virtual void Stop(std::function<void(std::exception_ptr)> callback) noexcept = 0;

		virtual void On(const std::string& event_name,
			const std::function<void(const std::vector<signalr::value>&)>& handler) = 0;
		virtual void Invoke(const std::string& method_name,
			const std::vector<signalr::value>& arguments = std::vector<signalr::value>(),
			std::function<void(const signalr::value&, std::exception_ptr)> callback =
			[] (const signalr::value&, std::exception_ptr) {}) noexcept = 0;
	};
}
