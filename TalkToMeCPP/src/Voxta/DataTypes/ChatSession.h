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
	///
	/// TODO: verify that we actually need chatMessages for something, only usecase might be
	/// to delete messages but not sure if thats a real usecase and worth the memorycost.
	/// </summary>
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
		// TODO: might have to remove const at some point, cuz user can disable / enable services during chat.
		const std::map<const ServiceData::ServiceType, const ServiceData> m_services;
	};
}