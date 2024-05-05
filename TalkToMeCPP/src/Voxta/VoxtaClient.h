// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "../Logger/ThreadedLogger.h"
#include "signalrclient/hub_connection.h"
#include "signalrclient/hub_connection_builder.h"
#include "signalrclient/signalr_value.h"
#include <string>
#include <vector>

namespace Voxta
{
	class VoxtaClient
	{
	public:
		explicit VoxtaClient(const Logger::ThreadedLogger& logger, std::string_view address, int port);
		~VoxtaClient() = default;

		void Connect();
		void Disconnect();
		void LoadCharacter(std::string_view characterId);
		void StartChat(std::string_view sessionData, std::string_view contextKey, const std::vector<std::string_view>& characterFunctions, std::string_view context);

	private:
		const Logger::ThreadedLogger& m_logger;
		signalr::hub_connection connection;

		void Authenticate();
	};
}