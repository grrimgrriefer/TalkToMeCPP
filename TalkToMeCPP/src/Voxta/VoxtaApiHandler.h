// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "DataTypes/ServerResponses/ServerResponseBase.h"
#include "DataTypes/ServerResponses/ServerResponseWelcome.h"
#include "DataTypes/ServerResponses/ServerResponseCharacterList.h"
#include "DataTypes/ServerResponses/ServerResponseCharacterLoaded.h"
#include "DataTypes/ServerResponses/ServerResponseChatStarted.h"
#include "DataTypes/ServerResponses/ServerResponseChatMessage.h"
#include "DataTypes/ServerResponses/ServerResponseChatUpdate.h"
#include "DataTypes/ServerResponses/ServerResponseSpeechTranscription.h"
#include "DataTypes/CharData.h"
#include <signalrclient/signalr_value.h>
#include <memory>
#include <string>
#include <map>
#include <set>

namespace Voxta
{
	/// <summary>
	/// Class responsable for converting data between the runtime version and the signalr-compatible version.
	/// Note: This class ONLY does the conversion, not actual sending / receiving of anything.
	/// </summary>
	class VoxtaApiHandler // TODO: maybe static helper class instead? idk yet
	{
	public:
		// Anything not in here & not implemented will throw an error
		const std::set<std::string> c_ignoredMessageTypes{ "chatStarting", "chatLoadingMessage", "chatsSessionsUpdated",
			"contextUpdated", "replyGenerating", "replyGenerating", "chatFlow", "speechRecognitionStart", "recordingRequest",
			"recordingStatus", "speechPlaybackComplete" };

		// Used to convert server responses into data we can use.
		std::unique_ptr<DataTypes::ServerResponses::ServerResponseBase> GetResponseData(
			const std::map<std::string, signalr::value>& map) const;

		// All of these are used to generate server-request datastructures.
		signalr::value GetAuthenticateRequestData() const;
		signalr::value GetLoadCharactersListData() const;
		signalr::value GetLoadCharacterRequestData(std::string_view characterId) const;
		signalr::value GetStartChatRequestData(const DataTypes::CharData* charData) const;
		signalr::value GetSendUserMessageData(std::string_view sessionId, std::string_view userMessage) const;

		signalr::value GetNotifyAudioPlaybackStartData(std::string_view sessionId, std::string_view messageId,
			int startIndex, int endIndex, double duration) const;
		signalr::value GetNotifyAudioPlaybackCompleteData(std::string_view sessionId, std::string_view messageId) const;

		// TODO: needed to close app cleanly
		// type: stopChat
		// type: deleteChat

	private:

		// Internal helper functions for the public GetResponseData function.
		std::unique_ptr<DataTypes::ServerResponses::ServerResponseChatUpdate> GetChatUpdateResponse(
			const std::map<std::string, signalr::value>& map) const;
		std::unique_ptr<DataTypes::ServerResponses::ServerResponseChatMessage> GetReplyEndReponseResponse(
			const std::map<std::string, signalr::value>& map) const;
		std::unique_ptr<DataTypes::ServerResponses::ServerResponseChatMessage> GetReplyChunkReponseResponse(
			const std::map<std::string, signalr::value>& map) const;
		std::unique_ptr<DataTypes::ServerResponses::ServerResponseChatMessage> GetReplyStartReponseResponse(
			const std::map<std::string, signalr::value>& map) const;
		std::unique_ptr<DataTypes::ServerResponses::ServerResponseChatStarted> GetChatStartedResponse(
			const std::map<std::string, signalr::value>& map) const;
		std::unique_ptr<DataTypes::ServerResponses::ServerResponseCharacterLoaded> GetCharacterLoadedResponse(
			const std::map<std::string, signalr::value>& map) const;
		std::unique_ptr<DataTypes::ServerResponses::ServerResponseCharacterList> GetCharacterListLoadedResponse(
			const std::map<std::string, signalr::value>& map) const;
		std::unique_ptr<DataTypes::ServerResponses::ServerResponseWelcome> GetWelcomeResponse(
			const std::map<std::string, signalr::value>& map) const;
		std::unique_ptr<DataTypes::ServerResponses::ServerResponseSpeechTranscription> GetSpeechRecognitionPartial(
			const std::map<std::string, signalr::value>& map) const;
		std::unique_ptr<DataTypes::ServerResponses::ServerResponseSpeechTranscription> GetSpeechRecognitionEnd(
			const std::map<std::string, signalr::value>& map) const;
	};
}
