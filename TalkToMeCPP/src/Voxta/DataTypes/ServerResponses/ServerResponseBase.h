// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once

namespace Voxta::DataTypes::ServerResponses
{
	/// <summary>
	/// Enum identifier to let other systems know what they are dealing with without having to
	/// cast it.
	/// </summary>
	enum class ServerResponseType
	{
		WELCOME,
		CHARACTER_LIST,
		CHARACTER_LOADED,
		CHAT_STARTED,
		CHAT_MESSAGE,
		CHAT_UPDATE,
		SPEECH_TRANSCRIPTION
	};

	/// <summary>
	/// Abstract data struct that all server response data structs derive from
	/// </summary>
	struct ServerResponseBase
	{
		virtual ServerResponseType GetType() = 0;
		virtual ~ServerResponseBase() = default;
	};
}