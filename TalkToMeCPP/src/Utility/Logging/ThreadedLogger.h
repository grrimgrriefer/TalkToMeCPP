// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "LoggerInterface.h"
#include <deque>
#include <iosfwd>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <fstream>
#include <atomic>

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
		explicit ThreadedLogger(std::string_view filename);

		void LogMessage(LogLevel level, const std::string& message) override;

	private:
		const std::string m_logFilePath;
		std::mutex m_mutex;
		std::ofstream m_logFile;
		std::deque<std::string> m_logQueue;
		std::vector<std::string> m_logBuffer;
		std::jthread writeThread;
		std::atomic_bool m_writeRequested;

		void AddToFileWriteQueue(std::string_view formattedMessage);
		void CopyQueueIntoBuffer();
		void WriteToFileDelayed();
	};
}