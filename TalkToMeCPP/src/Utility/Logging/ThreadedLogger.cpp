// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "LoggerInterface.h"
#include "ThreadedLogger.h"
#include <chrono>
#include <deque>
#include <iosfwd>
#include <iostream>
#include <syncstream>
#include <iterator>
#include <mutex>
#include <string>
#include <thread>
#include <exception>

namespace Utility::Logging
{
	ThreadedLogger::ThreadedLogger(std::string_view path) :
		m_logFilePath(path)
	{
		try
		{
			std::ofstream newFile(m_logFilePath, std::ios::trunc);
			if (newFile.is_open())
			{
				newFile << "Log started" << std::endl;
				newFile.flush();
				std::cout << "Cleared logfile for new session!" << std::endl;
			}
			else
			{
				std::cerr << "Failed to create an empty logfile :(" << std::endl;
			}
		}
		catch (const std::exception& e)
		{
			std::cerr << "Fatal error when trying to create logfile on disk: " << e.what() << std::endl;
		}
	}

	void ThreadedLogger::LogMessage(LogLevel level,
		const std::string& message)
	{
		std::stringstream formattedMessage;
		using enum LogLevel;
		switch (level)
		{
			case Debug:
				formattedMessage << "[DEBUG] ";
				break;
			case Info:
				formattedMessage << "[INFO] ";
				break;
			case Warning:
				formattedMessage << "[WARNING] ";
				break;
			case Error:
				formattedMessage << "[ERROR] ";
				break;
		}

		auto local = std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::system_clock::now() };
		formattedMessage << "[" << local << "] : ";

		formattedMessage << message;
		std::string formattedStr = formattedMessage.str();
		if (level == LogLevel::Warning || level == LogLevel::Error)
		{
			// Only log errors and warnings to console, rest goes in the logfile
			std::osyncstream((level == LogLevel::Error) ? std::cerr : std::cout) << formattedStr << std::endl;
		}

		AddToFileWriteQueue(formattedStr);
	}

	void ThreadedLogger::AddToFileWriteQueue(std::string_view formattedMessage)
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