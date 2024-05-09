// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "../Logger/ThreadedLogger.h"
#include "DataTypes/CharData.h"
#include "VoxtaApiHandler.h"
#include "VoxtaChatSession.h"
#include <signalrclient/hub_connection.h>
#include <signalrclient/hub_connection_builder.h>
#include <signalrclient/signalr_value.h>
#include <string>
#include <vector>

namespace Voxta
{
	class VoxtaClient
	{
	public:
		enum class VoxtaClientState { DISCONNECTED, LOADING, CHARACTER_LOBBY, CHATTING };

		explicit VoxtaClient(Logger::ThreadedLogger& logger, std::string_view address, int port,
			const std::function<void(VoxtaClientState newState)>& stateChange);
		~VoxtaClient() = default;

		std::string_view GetUsername() const;
		const std::vector<DataTypes::CharData>& GetCharacters() const;

		void Connect();
		void Disconnect();
		void LoadCharacter(std::string_view characterId);

	private:
		signalr::hub_connection m_connection;
		VoxtaApiHandler m_voxtaCommData;
		std::function<void(VoxtaClientState newState)> m_stateChange;
		Logger::ThreadedLogger& m_logger;

		std::unique_ptr<DataTypes::CharData> m_userData;
		std::vector<DataTypes::CharData> m_characterList;

		std::unique_ptr<VoxtaChatSession> m_chatSession;
		VoxtaClientState m_currentState = VoxtaClient::VoxtaClientState::DISCONNECTED;

		void SendMessage(const signalr::value& message);
		bool HandleResponse(const std::map<std::string, signalr::value>& map);
		void HandleBadResponse(const signalr::value& response);
		void SafeInvoke(const std::function<void()>& lambda, std::exception_ptr exception);
	};
}