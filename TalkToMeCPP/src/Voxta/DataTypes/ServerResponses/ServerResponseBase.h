// 2024 - Creative Commons Zero v1.0 Universal

#pragma once

namespace Voxta::DataTypes::ServerResponses
{
	enum class ServerResponseType
	{
		WELCOME, CHARACTER_LIST, CHARACTER_LOADED, CHAT_STARTED
	};

	struct ServerResponseBase
	{
		virtual ~ServerResponseBase() = default;
		virtual ServerResponseType GetType() = 0;
	};
}