// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

namespace Voxta::DataTypes::ServerResponses
{
	enum class ServerResponseType
	{
		WELCOME, CHARACTER_LIST, CHARACTER_LOADED, CHAT_STARTED, CHAT_MESSAGE, CHAT_UPDATE
	};

	struct ServerResponseBase
	{
		virtual ~ServerResponseBase() = default;
		virtual ServerResponseType GetType() = 0;
	};
}