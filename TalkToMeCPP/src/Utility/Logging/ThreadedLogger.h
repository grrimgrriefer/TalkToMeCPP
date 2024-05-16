// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "LoggerInterface.h"
#include <fstream>
#include <string>
#include <mutex>
#include <deque>
#include <vector>
#include <thread>
#include <atomic>
#include <iosfwd>

namespace Utility::Logging
{
	/// <summary>
	/// Logger that takes care of receiving messages from main- and detached background threads and
	/// logging stuff into the console. Also buffers messages and writes buffer to disk every 2 seconds.
	/// Note: Writing to disk itself is also run on a background thread so we don't lock up the mainthread
	/// when my old HDD wakes up from standby mode.
	/// </summary>
	class ThreadedLogger : public LoggerInterface
	{
	public:
		explicit ThreadedLogger(const std::string& filename);

		void LogMessage(LogLevel level, const std::string& message) override;

	private:
		std::string m_logFilePath;

		std::mutex m_mutex;
		std::ofstream m_logFile;
		std::deque<std::string> m_logQueue;
		std::vector<std::string> m_logBuffer;
		std::jthread writeThread; // C++20 auto joins jthreads on destruction, so no need for destructor anymore, pretty hype
		std::atomic_bool m_writeRequested;

		void AddToFileWriteQueue(std::string formattedMessage);
		void CopyQueueIntoBuffer();
		void WriteToFileDelayed();
	};
}