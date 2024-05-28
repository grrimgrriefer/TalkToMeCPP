// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "HubConnectionLogger.h"
#include "LoggerInterface.h"
#include <string>

namespace Utility::Logging
{
	HubConnectionLogger::HubConnectionLogger(LoggerInterface& logger)
		: m_logger(logger)
	{
	}

	void __cdecl HubConnectionLogger::write(const std::string& entry)
	{
		m_logger.LogMessage(LoggerInterface::LogLevel::Debug, entry);
	}
}