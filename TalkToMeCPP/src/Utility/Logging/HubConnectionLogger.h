// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ThreadedLogger.h"
#include <string>
#include <signalrclient/log_writer.h>

namespace Utility::Logging
{
	class HubConnectionLogger : public signalr::log_writer
	{
	public:
		explicit HubConnectionLogger(ThreadedLogger& logger);

		void __cdecl write(const std::string& entry) override;

	private:
		Logging::ThreadedLogger& m_logger;
	};
}