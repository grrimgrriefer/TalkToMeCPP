// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ServerResponseBase.h"
#include <string>

namespace Voxta::DataTypes::ServerResponses
{
	/// <summary>
	/// Data struct for ChatMessage responses from the server.
	///
	/// Note: We use this same struct for Start, Chunk and End. It's a bit confusing here,
	/// but it reduces confusion in the VoxtaClient, which is more important.
	///
	/// Warning: For the Start and the End version, it uses the first constructor so some fields are on default values.
	/// </summary>
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

		const MessageType m_messageType;
		const std::string m_messageId;
		const std::string m_senderId;
		const std::string m_sessionId;
		const int m_startIndex = 0;
		const int m_endIndex = 0;
		const std::string m_messageText = "";
		const std::string m_audioUrlPath = "";
	};
}