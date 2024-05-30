// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "DataTypes/ServerResponses/ServerResponseBase.h"
#include "DataTypes/ServerResponses/ServerResponseCharacterList.h"
#include "DataTypes/ServerResponses/ServerResponseCharacterLoaded.h"
#include "DataTypes/ServerResponses/ServerResponseChatMessage.h"
#include "DataTypes/ServerResponses/ServerResponseChatStarted.h"
#include "DataTypes/ServerResponses/ServerResponseChatUpdate.h"
#include "DataTypes/ServerResponses/ServerResponseSpeechTranscription.h"
#include "DataTypes/ServerResponses/ServerResponseWelcome.h"
#include <map>
#include <memory>
#include <set>
#include <signalrclient/signalr_value.h>
#include <string>

namespace Voxta
{
	/// <summary>
	/// Class responsable for converting data between the runtime version and the signalr-compatible version.
	/// Note: This class ONLY does the conversion, not actual sending / receiving of anything.
	/// </summary>
	class VoxtaApiResponseHandler
	{
	public:
		// Anything not in here & not implemented will throw an error
		const std::set<std::string> c_ignoredMessageTypes{
			"chatStarting",
			"chatLoadingMessage",
			"chatsSessionsUpdated",
			"contextUpdated",
			"replyGenerating",
			"chatFlow",
			"speechRecognitionStart",
			"recordingRequest",
			"recordingStatus",
			"speechPlaybackComplete" };

		std::unique_ptr<DataTypes::ServerResponses::ServerResponseBase> GetResponseData(
			const std::map<std::string, signalr::value>& map) const;

	private:
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
