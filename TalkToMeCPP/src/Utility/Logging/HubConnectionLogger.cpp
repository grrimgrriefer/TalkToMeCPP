// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "HubConnectionLogger.h"
#include "ThreadedLogger.h"
#include <string>

namespace Utility::Logging
{
	HubConnectionLogger::HubConnectionLogger(ThreadedLogger& logger)
		: m_logger(logger)
	{
	}

	void __cdecl HubConnectionLogger::write(const std::string& entry)
	{
		m_logger.LogMessage(ThreadedLogger::LogLevel::DEBUG, entry);
	}
}