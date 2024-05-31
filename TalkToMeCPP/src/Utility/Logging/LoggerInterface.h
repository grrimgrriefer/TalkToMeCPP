// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include <string>

namespace Utility::Logging
{
	/// <summary>
	/// Interface implemented by the ThreadedLogger.
	/// Main purpose is so we can mock the logger and running Unit tests won't generate
	/// garbage log files on the disk every fkin time.
	/// </summary>
	class LoggerInterface
	{
	public:
		enum class LogLevel
		{
			Debug,
			Info,
			Warning,
			Error
		};

		virtual ~LoggerInterface() = default;

		virtual void LogMessage(LogLevel level, const std::string& message) = 0;
	};
}