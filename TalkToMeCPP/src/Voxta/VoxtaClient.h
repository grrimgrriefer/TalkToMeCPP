// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "../Utility/Logging/LoggerInterface.h"
#include "DataTypes/CharData.h"
#include "VoxtaApiHandler.h"
#include "Datatypes/ChatSession.h"
#include "DataTypes/ChatMessage.h"
#include "../Utility/SignalR/SignalRWrapperInterface.h"
#include <signalrclient/signalr_value.h>
#include <string>
#include <vector>
#include <exception>
#include <functional>
#include <map>
#include <memory>

namespace Voxta
{
	class VoxtaClient
	{
	public:
		enum class VoxtaClientState { DISCONNECTED, LOADING, CHARACTER_LOBBY, CHATTING };

		explicit VoxtaClient(std::unique_ptr<Utility::SignalR::SignalRWrapperInterface> connectionBuilder,
			Utility::Logging::LoggerInterface& logger,
			const std::function<void(VoxtaClientState newState)>& stateChange,
			const std::function<std::string()>& requestingUserInputEvent,
			const std::function<void(const DataTypes::ChatMessage*, const DataTypes::CharData*)>& charSpeakingEvent);

		~VoxtaClient() = default;

		std::string_view GetUsername() const;
		const std::vector<std::unique_ptr<const DataTypes::CharData>>& GetCharacters() const;
		const DataTypes::ChatSession* GetChatSession() const;

		void Connect();
		void Disconnect();
		void LoadCharacter(std::string_view characterId);

	private:
		std::unique_ptr<Utility::SignalR::SignalRWrapperInterface> m_connection;
		VoxtaApiHandler m_voxtaApi;
		const std::function<void(VoxtaClientState newState)> m_stateChange;
		const std::function<std::string()> m_requestingUserInputEvent;
		const std::function<void(const DataTypes::ChatMessage*, const DataTypes::CharData*)> m_charSpeakingEvent;
		Utility::Logging::LoggerInterface& m_logger;

		std::unique_ptr<DataTypes::CharData> m_userData = nullptr;
		std::unique_ptr<DataTypes::ChatSession> m_chatSession = nullptr;
		std::vector<std::unique_ptr<const DataTypes::CharData>> m_characterList;
		VoxtaClientState m_currentState = VoxtaClient::VoxtaClientState::DISCONNECTED;

		void StartListeningToServer();
		void SendMessage(const signalr::value& message);
		bool HandleResponse(const std::map<std::string, signalr::value>& map);
		void HandleBadResponse(const signalr::value& response);

		template<typename Callable>
		void SafeInvoke(Callable lambda, std::exception_ptr exception);
	};
}