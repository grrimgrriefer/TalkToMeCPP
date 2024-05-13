// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "ThreadedLogger.h"
#include <signalrclient/signalr_value.h>
#include <signalrclient/log_writer.h>

namespace Logging
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