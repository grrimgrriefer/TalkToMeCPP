// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include <string>
#include <memory>
#include <vector>

namespace Voxta::DataTypes
{
	struct ChatMessage
	{
		explicit ChatMessage(std::string_view messageId, std::string_view charID, std::string_view text) :
			m_text(text), m_messageId(messageId), m_charId(charID)
		{
		}

		explicit ChatMessage(std::string_view messageId, std::string_view charID) : m_messageId(messageId),
			m_charId(charID)
		{
		}
		~ChatMessage() = default;

		std::string m_text = "";
		std::vector<std::string> m_audioUrls;
		const std::string m_messageId;
		const std::string m_charId;
	};

	struct ChatMessageIdComparer
	{
		std::string_view m_messageId;
		explicit ChatMessageIdComparer(std::string_view messageId) : m_messageId(messageId) {}

		bool operator()(const std::unique_ptr<DataTypes::ChatMessage>& element) const
		{
			return element->m_messageId == m_messageId;
		}
	};
}