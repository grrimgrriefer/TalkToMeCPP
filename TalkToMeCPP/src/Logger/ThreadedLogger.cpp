// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "ThreadedLogger.h"
#include <chrono>
#include <iostream>
#include <mutex>
#include <syncstream>
#include <iterator>
#include <thread>
#include <type_traits>
#include <iosfwd>

namespace Logger
{
	ThreadedLogger::ThreadedLogger(const std::string& path) : m_logFilePath(path)
	{
		std::ofstream newFile(path);
		if (newFile.is_open())
		{
			newFile << "" << std::endl;
			newFile.flush();
			newFile.close();
			std::cout << "Cleared logfile for new session!" << std::endl;
		}
		else
		{
			std::cerr << "Failed to create an empty logfile :(" << std::endl;
		}
	}

	void ThreadedLogger::Log(LogLevel level, const std::string& message)
	{
		std::stringstream formattedMessage;
		switch (level)
		{
			case LogLevel::INFO:
				formattedMessage << "[INFO] ";
				break;
			case LogLevel::WARNING:
				formattedMessage << "[WARNING] ";
				break;
			case LogLevel::ERROR:
				formattedMessage << "[ERROR] ";
				break;
		}

		auto local = std::chrono::zoned_time{ std::chrono::current_zone(), std::chrono::system_clock::now() };
		formattedMessage << "[" << local << "] : ";

		formattedMessage << message;
		auto formattedStr = formattedMessage.str();
		std::osyncstream((level == LogLevel::ERROR) ? std::cerr : std::cout) << formattedStr << std::endl;

		AddToFileWriteQueue(formattedMessage);
	}

	void ThreadedLogger::AddToFileWriteQueue(const std::stringstream& formattedMessage)
	{
		std::scoped_lock<std::mutex> lock(m_mutex);
		m_logQueue.push_back(formattedMessage.str());

		if (!m_writeRequested)
		{
			m_writeRequested = true;
			writeThread = std::jthread(&ThreadedLogger::WriteToFileDelayed, this);
		}
	}

	void ThreadedLogger::WriteToFileDelayed()
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(2000));

		CopyQueueIntoBuffer();

		m_logFile.open(m_logFilePath, std::ofstream::out | std::ofstream::app);
		std::ranges::copy(m_logBuffer, std::ostream_iterator<std::string>(m_logFile, "\n"));
		m_logFile.close();

		m_writeRequested = false;
	}

	void ThreadedLogger::CopyQueueIntoBuffer()
	{
		std::scoped_lock<std::mutex> lock(m_mutex);

		m_logBuffer.resize(m_logQueue.size());
		std::ranges::copy(m_logQueue.begin(), m_logQueue.end(), m_logBuffer.begin());
	}
}