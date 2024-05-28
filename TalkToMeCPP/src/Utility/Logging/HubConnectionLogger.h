// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "LoggerInterface.h"
#include <signalrclient/log_writer.h>
#include <string>

namespace Utility::Logging
{
	/// <summary>
	/// Custom logger to redirect signalr messages to our Threadsafe logger.
	/// Without this we can only see messages in the console but not in logfile.
	/// Which is very fucking useful when debuggin complex nested jsons. Source: trust me bro.
	/// </summary>
	class HubConnectionLogger : public signalr::log_writer
	{
	public:
		explicit HubConnectionLogger(LoggerInterface& logger);

		void __cdecl write(const std::string& entry) override;

	private:
		Logging::LoggerInterface& m_logger;
	};
}