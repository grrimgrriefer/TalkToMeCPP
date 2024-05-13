// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "ThreadedLogger.h"
#include "LoggerInterface.h"
#include <chrono>
#include <iostream>
#include <mutex>
#include <iterator>
#include <thread>
#include <iosfwd>
#include <deque>
#include <string>
#include <syncstream>

namespace Utility::Logging
{
	ThreadedLogger::ThreadedLogger(const std::string& path) : m_logFilePath(path)
	{
		std::ofstream newFile(path, std::ios::trunc);
		if (newFile.is_open())
		{
			newFile << "Log started" << std::endl;
			newFile.flush();
			newFile.close();
			std::cout << "Cleared logfile for new session!" << std::endl;
		}
		else
		{
			std::cerr << "Failed to create an empty logfile :(" << std::endl;
		}
	}

	ThreadedLogger::~ThreadedLogger()
	{
		if (writeThread.joinable())
		{
			writeThread.join();
		}
	}

	void ThreadedLogger::LogMessage(LogLevel level, const std::string& message)
	{
		std::stringstream formattedMessage;
		using enum LogLevel;
		switch (level)
		{
			case DEBUG:
				formattedMessage << "[DEBUG] ";
				break;
			case INFO:
				formattedMessage << "[INFO] ";
				break;
			case WARNING:
				formattedMessage << "[WARNING] ";
				break;
			case ERROR:
				formattedMessage << "[ERROR] ";
				break;
		}

		auto local = std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::system_clock::now() };
		formattedMessage << "[" << local << "] : ";

		formattedMessage << message;
		std::string formattedStr = formattedMessage.str();
		if (level == LogLevel::WARNING || level == LogLevel::ERROR)
		{
			// Only log errors and warnings to console, rest goes in the logfile
			std::osyncstream((level == LogLevel::ERROR) ? std::cerr : std::cout) << formattedStr << std::endl;
		}

		AddToFileWriteQueue(formattedStr);
	}

	void ThreadedLogger::AddToFileWriteQueue(std::string formattedMessage)
	{
		std::scoped_lock<std::mutex> lock(m_mutex);
		m_logQueue.emplace_back(formattedMessage);

		if (!m_writeRequested)
		{
			m_writeRequested = true;
			writeThread = std::jthread(&ThreadedLogger::WriteToFileDelayed, this);
		}
	}

	void ThreadedLogger::CopyQueueIntoBuffer()
	{
		std::scoped_lock<std::mutex> lock(m_mutex);

		m_logBuffer.resize(m_logQueue.size());
		std::ranges::copy(m_logQueue.begin(), m_logQueue.end(), m_logBuffer.begin());
		m_logQueue.clear();
	}

	void ThreadedLogger::WriteToFileDelayed()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));
		CopyQueueIntoBuffer();
		std::scoped_lock<std::mutex> lock(m_mutex);

		m_logFile.open(m_logFilePath, std::ofstream::out | std::ofstream::app);
		std::ranges::copy(m_logBuffer, std::ostream_iterator<std::string>(m_logFile, "\n"));
		m_logFile.flush();
		m_logFile.close();

		m_writeRequested = false;
	}
}