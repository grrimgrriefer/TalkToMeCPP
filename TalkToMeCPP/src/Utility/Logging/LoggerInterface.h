// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include <string>

namespace Utility::Logging
{
	class LoggerInterface
	{
	public:
		enum class LogLevel { DEBUG, INFO, WARNING, ERROR };
		virtual ~LoggerInterface() = default;

		virtual void LogMessage(LogLevel level, const std::string& message) = 0;
	};
}