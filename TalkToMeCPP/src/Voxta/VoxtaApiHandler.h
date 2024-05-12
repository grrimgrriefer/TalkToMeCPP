// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "DataTypes/ServerResponses/ServerResponseBase.h"
#include "DataTypes/CharData.h"
#include <signalrclient/signalr_value.h>
#include <memory>
#include <string>
#include <map>
#include <set>

namespace Voxta
{
	class VoxtaApiHandler
	{
	public:
		enum class VoxtaGenericRequestType
		{
			AUTHENTICATE, LOAD_CHARACTERS_LIST
		};

		const std::set<std::string> c_ignoredMessageTypes{ "chatStarting", "chatLoadingMessage", "chatsSessionsUpdated",
			"contextUpdated", "replyGenerating", "replyGenerating", "chatFlow" };

		explicit VoxtaApiHandler();
		~VoxtaApiHandler() = default;

		signalr::value GetRequestData(const VoxtaGenericRequestType commData) const;
		signalr::value GetLoadCharacterRequestData(std::string_view characterId) const;
		signalr::value GetStartChatRequestData(const DataTypes::CharData* charData) const;
		signalr::value ConstructSendUserMessage(std::string_view sessionId, std::string_view userMessage) const;

		// TODO: maybe?
		// type: stopChat (to stop chat)
		// type: deleteChat (to delete chat)

		const std::unique_ptr<DataTypes::ServerResponses::ServerResponseBase> GetResponseData(const std::map<std::string, signalr::value>& map) const;

	private:
		const std::unique_ptr<signalr::value> m_authenticateReqData;
		const std::unique_ptr<signalr::value> m_loadCharacterListReqData;

		signalr::value ConstructAuthenticateReqData() const;
		signalr::value ConstructLoadCharacterListReqData() const;
	};
}
