// 2024 - Creative Commons Zero v1.0 Universal

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

		explicit ServerResponseChatStarted(std::string_view userId, const std::vector<std::string_view>& characterIds,
			const std::map<DataTypes::ServiceData::ServiceType, DataTypes::ServiceData>& serviceIds, std::string_view chatId, std::string_view sessionId) :
			m_userId(userId), m_characterIds(characterIds), m_serviceIds(serviceIds), m_chatId(chatId), m_sessionId(sessionId)
		{
		}

		std::string_view m_userId;
		std::vector<std::string_view> m_characterIds;
		std::map<DataTypes::ServiceData::ServiceType, DataTypes::ServiceData> m_serviceIds;
		std::string_view m_chatId;
		std::string_view m_sessionId;
	};
}