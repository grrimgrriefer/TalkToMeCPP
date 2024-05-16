// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

namespace Voxta::DataTypes::ServerResponses
{
	/// <summary>
	/// Enum identifier to let other systems know what they are dealing with without having to
	/// cast it.
	///
	/// TODO: Thought this would be more useful, but its kinda meh.
	/// Base class is so useless there's no real use if you don't cast tbh.
	/// </summary>
	enum class ServerResponseType
	{
		WELCOME, CHARACTER_LIST, CHARACTER_LOADED, CHAT_STARTED, CHAT_MESSAGE, CHAT_UPDATE
	};

	/// <summary>
	/// Abstract data struct that all server response data structs derive from
	/// </summary>
	struct ServerResponseBase
	{
		virtual ~ServerResponseBase() = default;
		virtual ServerResponseType GetType() = 0;
	};
}