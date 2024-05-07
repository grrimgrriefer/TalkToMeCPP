// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "HubConnectionLogger.h"
#include "ThreadedLogger.h"
#include <signalrclient/signalr_value.h>
#include <signalrclient/log_writer.h>

namespace Logger
{
	HubConnectionLogger::HubConnectionLogger(ThreadedLogger& logger)
		: m_logger(logger)
	{}

	void __cdecl HubConnectionLogger::write(const std::string& entry)
	{
		m_logger.Log(ThreadedLogger::LogLevel::DEBUG, entry);
	}
}