// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "CharData.h"
#include "ServiceData.h"
#include "ChatMessage.h"
#include <string>
#include <map>
#include <memory>
#include <vector>
#include <set>

namespace Voxta::DataTypes
{
	struct ChatSession
	{
		explicit ChatSession(std::vector<const CharData*> characters, std::string_view chatId,
			std::string_view sessionId, std::map<const ServiceData::ServiceType, const ServiceData> services) :
			m_characters(characters), m_chatId(chatId), m_sessionId(sessionId), m_services(services)
		{
		}
		~ChatSession() = default;

		std::set<std::unique_ptr<ChatMessage>> m_chatMessages;
		std::vector<const CharData*> m_characters;
		const std::string m_chatId;
		const std::string m_sessionId;
		const std::map<const ServiceData::ServiceType, const ServiceData> m_services;
	};
}