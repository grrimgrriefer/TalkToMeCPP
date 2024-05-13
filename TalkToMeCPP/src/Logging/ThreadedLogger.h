// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include <fstream>
#include <string>
#include <mutex>
#include <deque>
#include <vector>
#include <thread>
#include <atomic>
#include <iosfwd>

namespace Logging
{
	class ThreadedLogger
	{
	public:
		enum class LogLevel { DEBUG, INFO, WARNING, ERROR };

		explicit ThreadedLogger(const std::string& filename);
		~ThreadedLogger();

		void Log(LogLevel level, const std::string& message);

	private:
		std::string m_logFilePath;

		std::mutex m_mutex;
		std::ofstream m_logFile;
		std::deque<std::string> m_logQueue;
		std::vector<std::string> m_logBuffer;
		std::jthread writeThread;
		std::atomic_bool m_writeRequested;

		void AddToFileWriteQueue(std::string formattedMessage);
		void CopyQueueIntoBuffer();
		void WriteToFileDelayed();
	};
}