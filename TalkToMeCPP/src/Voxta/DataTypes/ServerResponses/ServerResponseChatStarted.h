// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "../ServiceData.h"
#include "ServerResponseBase.h"
#include <map>
#include <string>
#include <vector>

namespace Voxta::DataTypes::ServerResponses
{
	/// <summary>
	/// Data struct for ChatStarted response from the server.
	/// Should be pretty obvious
	/// </summary>
	struct ServerResponseChatStarted : ServerResponseBase
	{
		const std::vector<std::string> m_characterIds;
		const std::map<const DataTypes::ServiceData::ServiceType, const DataTypes::ServiceData> m_services;
		const std::string m_userId;
		const std::string m_chatId;
		const std::string m_sessionId;

		explicit ServerResponseChatStarted(std::string_view userId,
				const std::vector<std::string>& characterIds,
				const std::map<const Voxta::DataTypes::ServiceData::ServiceType, const DataTypes::ServiceData>& services,
				std::string_view chatId,
				std::string_view sessionId) :
			m_characterIds(characterIds),
			m_services(services),
			m_userId(userId),
			m_chatId(chatId),
			m_sessionId(sessionId)
		{
		}

		ServerResponseType GetType() final
		{
			return ServerResponseType::CHAT_STARTED;
		}
	};
}