// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include <exception>
#include <functional>
#include <signalrclient/signalr_value.h>
#include <string>
#include <vector>

namespace Utility::SignalR
{
	/// <summary>
	/// Interface whose only purpose is so we can Mock the wrapper for unit tests.
	/// Syntax is same as microsoft, so don't yell at me for them not utilising c++20 syntax
	/// </summary>
	class SignalRWrapperInterface
	{
	public:
		virtual ~SignalRWrapperInterface() = default;

		virtual void Start(const std::function<void(std::exception_ptr)>& callback) noexcept = 0;
		virtual void Stop(const std::function<void(std::exception_ptr)>& callback) noexcept = 0;

		virtual void On(const std::string& event_name,
			const std::function<void(const std::vector<signalr::value>&)>& handler) = 0;

		virtual void Invoke(const std::string& method_name,
			const std::vector<signalr::value>& arguments = std::vector<signalr::value>(),
			const std::function<void(const signalr::value&, std::exception_ptr)>& callback =
				[] (const signalr::value&, std::exception_ptr) { /*empty cuz optional*/ }) noexcept = 0;
	};
}
