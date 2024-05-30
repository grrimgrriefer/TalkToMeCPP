// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include <memory>
#include <string>
#include <vector>

namespace Voxta::DataTypes
{
	/// <summary>
	/// Data struct for a single chat message (from either a character or the user)
	/// </summary>
	struct ChatMessage
	{
		std::string m_text;
		std::vector<std::string> m_audioUrls;
		const std::string m_messageId;
		const std::string m_charId;

		explicit ChatMessage(std::string_view messageId, std::string_view charID, std::string_view text) :
			m_text(text), m_messageId(messageId), m_charId(charID)
		{
		}

		explicit ChatMessage(std::string_view messageId, std::string_view charID) : m_messageId(messageId),
			m_charId(charID)
		{
		}
	};

	/// <summary>
	/// Comparer struct for ChatMessage
	/// Used by the std::ranges syntax for improved performance
	///
	/// Yes, two structs in one file. Go ahead, sue me.
	/// </summary>
	struct ChatMessageIdComparer
	{
		std::string m_messageId;

		explicit ChatMessageIdComparer(std::string_view messageId) :
			m_messageId(messageId)
		{
		}

		bool operator()(const std::unique_ptr<DataTypes::ChatMessage>& element) const
		{
			return element->m_messageId == m_messageId;
		}
	};
}