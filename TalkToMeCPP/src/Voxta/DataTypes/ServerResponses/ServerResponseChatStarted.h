// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ServerResponseBase.h"
#include <string>
#include <vector>

namespace Voxta::DataTypes::ServerResponses
{
	struct ServerResponseChatStarted : ServerResponseBase
	{
		ServerResponseType GetType() final
		{
			return ServerResponseType::CHAT_STARTED;
		}

		explicit ServerResponseChatStarted(std::string_view userId, const std::vector<std::string>& characterIds,
			const std::map<const DataTypes::ServiceData::ServiceType, const DataTypes::ServiceData>& serviceIds,
			std::string_view chatId, std::string_view sessionId) :
			m_characterIds(characterIds), m_serviceIds(serviceIds), m_userId(userId), m_chatId(chatId), m_sessionId(sessionId)
		{
		}

		std::vector<std::string> m_characterIds;
		std::map<const DataTypes::ServiceData::ServiceType, const DataTypes::ServiceData> m_serviceIds;
		const std::string m_userId;
		const std::string m_chatId;
		const std::string m_sessionId;
	};
}