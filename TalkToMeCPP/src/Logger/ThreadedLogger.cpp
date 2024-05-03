// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "ThreadedLogger.h"
#include <chrono>
#include <iostream>

namespace Logger
{
	ThreadedLogger::ThreadedLogger(const std::string& path)
	{
		// TODO: exception handling
		std::ofstream newFile(path);
		m_logFile.open(path, std::ofstream::out | std::ofstream::app);
	}

	ThreadedLogger::~ThreadedLogger()
	{
		// TODO: exception handling
		if (m_logFile.is_open())
		{
			m_logFile.close();
		}
	}

	void ThreadedLogger::log(LogLevel level, const std::string& message)
	{
		std::lock_guard<std::mutex> lock(m_mutex);

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
		std::cout << "[" << local << "] : ";

		formattedMessage << message;
		auto formattedStr = formattedMessage.str();

		// Write to console and log file
		std::cout << formattedStr << std::endl;
		m_logFile << formattedStr << std::endl;
	}
}