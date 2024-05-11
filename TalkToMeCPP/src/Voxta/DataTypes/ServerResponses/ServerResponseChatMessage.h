// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "ServerResponseBase.h"
#include <string>

namespace Voxta::DataTypes::ServerResponses
{
	struct ServerResponseChatMessage : ServerResponseBase
	{
		enum class MessageType
		{
			MESSAGE_START, MESSAGE_CHUNK, MESSAGE_END
		};

		ServerResponseType GetType() final
		{
			return ServerResponseType::CHAT_MESSAGE;
		}

		explicit ServerResponseChatMessage(MessageType type, std::string_view messageId, std::string_view senderId,
			std::string_view sessionId) : m_messageType(type), m_messageId(messageId), m_senderId(senderId), m_sessionId(sessionId)
		{
		}

		explicit ServerResponseChatMessage(MessageType type, std::string_view messageId, std::string_view senderId,
			std::string_view sessionId, int startIndex, int endIndex, std::string_view messageText, std::string_view audioUrlPath) :
			m_messageType(type), m_messageId(messageId), m_senderId(senderId), m_sessionId(sessionId), m_startIndex(startIndex),
			m_endIndex(endIndex), m_messageText(messageText), m_audioUrlPath(audioUrlPath)
		{
		}

		MessageType m_messageType;
		std::string_view m_messageId;
		std::string_view m_senderId;
		std::string_view m_sessionId;
		int m_startIndex = 0;
		int m_endIndex = 0;
		std::string_view m_messageText = "";
		std::string_view m_audioUrlPath = "";
	};
}