// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "CharData.h"
#include "ServiceData.h"
#include <string>
#include <map>
#include <memory>
#include <vector>

namespace Voxta::DataTypes
{
	struct ChatSession
	{
		explicit ChatSession(std::vector<std::shared_ptr<CharData>> characters, std::string_view chatId,
			std::string_view sessionId, std::map<ServiceData::ServiceType, ServiceData> services) :
			m_characters(characters), m_chatId(chatId), m_sessionId(sessionId), m_services(services)
		{
		}
		~ChatSession() = default;

		std::vector<std::shared_ptr<CharData>> m_characters;
		const std::string_view m_chatId;
		const std::string_view m_sessionId;
		const std::map<ServiceData::ServiceType, ServiceData> m_services;
	};
}