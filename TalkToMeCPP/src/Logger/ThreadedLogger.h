// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include <fstream>
#include <string>
#include <mutex>

namespace Logger
{
	class ThreadedLogger
	{
	public:
		enum class LogLevel { INFO, WARNING, ERROR };

		ThreadedLogger(const std::string& filename);
		~ThreadedLogger();

		void log(LogLevel level, const std::string& message);

	private:
		std::ofstream m_logFile;
		std::mutex m_mutex;
	};
}