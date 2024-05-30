// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "../Utility/Logging/LoggerInterface.h"
#include "../Utility/SignalR/SignalRWrapperInterface.h"
#include "DataTypes/CharData.h"
#include "DataTypes/ChatMessage.h"
#include "Datatypes/ChatSession.h"
#include "DataTypes/ServerResponses/ServerResponseBase.h"
#include "VoxtaApiRequestHandler.h"
#include "VoxtaApiResponseHandler.h"
#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <signalrclient/signalr_value.h>
#include <string>
#include <vector>

namespace Voxta
{
	/// <summary>
	/// Contains the main client-side logic. I.e. decides which response triggers which request.
	/// Also contains the collection of characters, the chat log, etc.
	///
	/// Basically, if the code is a spaghetti, this is the meatball at the center.
	/// </summary>
	class VoxtaClient
	{
	public:
		enum class VoxtaClientState { DISCONNECTED, CONNECTING, AUTHENTICATED, CHARACTER_LOBBY, CHATTING };

		explicit VoxtaClient(std::unique_ptr<Utility::SignalR::SignalRWrapperInterface> connectionBuilder,
			Utility::Logging::LoggerInterface& logger,
			const std::function<void(VoxtaClientState newState)>& stateChange,
			const std::function<std::string()>& requestingUserInputEvent,
			const std::function<void(std::string_view currentTranscription, bool finalized)>& transcribedSpeechUpdate,
			const std::function<void(std::string_view errorMsg)>& fatalErrorTriggered,
			const std::function<void(const DataTypes::ChatMessage*, const DataTypes::CharData*)>& charSpeakingEvent);

		~VoxtaClient() = default;

		void Connect();
		void Disconnect();
		void ForceStopImmediate();

		std::string_view GetUsername() const;
		std::vector<const DataTypes::CharData*> GetCharacters() const;
		const DataTypes::ChatSession* GetChatSession() const;

		void LoadCharacter(std::string_view characterId);
		void NotifyAudioPlaybackStart(std::string_view messageId, int startIndex, int endIndex, double duration);
		void NotifyAudioPlaybackComplete(std::string_view messageId);

	private:
		const VoxtaApiRequestHandler m_voxtaRequestApi = VoxtaApiRequestHandler();
		const VoxtaApiResponseHandler m_voxtaResponseApi = VoxtaApiResponseHandler();
		VoxtaClientState m_currentState = VoxtaClient::VoxtaClientState::DISCONNECTED;
		std::unique_ptr<Utility::SignalR::SignalRWrapperInterface> m_hubConnection;
		std::unique_ptr<DataTypes::CharData> m_userData;
		std::unique_ptr<DataTypes::ChatSession> m_chatSession;
		std::vector<std::unique_ptr<const DataTypes::CharData>> m_characterList;
		Utility::Logging::LoggerInterface& m_logger;
		bool m_usingMicrophoneInput = true;
		bool m_sentFinalUserMessage = false; // very ugly hack but socket returns final like 6 times, and we only wanna send once

		const std::function<void(VoxtaClientState newState)> m_stateChange;
		const std::function<std::string()> m_requestingUserInputEvent;
		const std::function<void(std::string_view currentTranscription, bool finalized)> m_transcribedSpeechUpdate;
		const std::function<void(std::string_view errorMsg)> m_fatalErrorTriggered;
		const std::function<void(const DataTypes::ChatMessage*, const DataTypes::CharData*)> m_charSpeakingEvent;

		void SendMessageToServer(const signalr::value& message);

		void StartListeningToServer();
		void OnReceiveMessage(const std::vector<signalr::value>& messageContainer);
		bool HandleResponse(const std::map<std::string, signalr::value>& map);
		void HandleBadResponse(const signalr::value& response);

		void HandleWelcomeResponse(const DataTypes::ServerResponses::ServerResponseBase& response);
		void HandleCharacterListResponse(const DataTypes::ServerResponses::ServerResponseBase& response);
		bool HandleCharacterLoadedResponse(const DataTypes::ServerResponses::ServerResponseBase& response);
		void HandleChatStartedResponse(const DataTypes::ServerResponses::ServerResponseBase& response);
		void HandleChatMessageResponse(const DataTypes::ServerResponses::ServerResponseBase& response);
		void HandleChatUpdateResponse(const DataTypes::ServerResponses::ServerResponseBase& response);
		void HandleSpeechTranscriptionResponse(const DataTypes::ServerResponses::ServerResponseBase& response);

		template<typename Callable>
		void SafeInvoke(Callable lambda, std::exception_ptr exception);
	};
}