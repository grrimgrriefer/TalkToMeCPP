// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ServerResponseBase.h"
#include <string>

namespace Voxta::DataTypes::ServerResponses
{
	/// <summary>
	/// Data struct for ChatUpdate response from the server.
	/// Should be pretty obvious
	/// </summary>
	struct ServerResponseChatUpdate : ServerResponseBase
	{
		const std::string m_messageId;
		const std::string m_senderId;
		const std::string m_text;
		const std::string m_sessionId;

		ServerResponseType GetType() final
		{
			return ServerResponseType::CHAT_UPDATE;
		}

		explicit ServerResponseChatUpdate(std::string_view messageId,
				std::string_view senderId,
				std::string_view text,
				std::string_view sessionId) :
			m_messageId(messageId),
			m_senderId(senderId),
			m_text(text),
			m_sessionId(sessionId)
		{
		}
	};
}