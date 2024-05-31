// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "CharData.h"
#include "ChatMessage.h"
#include "ServiceData.h"
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

namespace Voxta::DataTypes
{
	/// <summary>
	/// Data struct that contains a chat session (created & managed by VoxtaClient)
	/// </summary>
	struct ChatSession
	{
		std::set<std::unique_ptr<ChatMessage>> m_chatMessages;
		std::vector<const CharData*> m_characters;
		const std::string m_chatId;
		const std::string m_sessionId;
		const std::map<const ServiceData::ServiceType, const ServiceData> m_services; // Assumes we don't allow the user to disable / enable services while chatting

		explicit ChatSession(const std::vector<const CharData*>& characters,
				std::string_view chatId,
				std::string_view sessionId,
				const std::map<const ServiceData::ServiceType, const ServiceData>& services) :
			m_characters(characters),
			m_chatId(chatId),
			m_sessionId(sessionId),
			m_services(services)
		{
		}
	};
}