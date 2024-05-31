// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "DataTypes/CharData.h"
#include <signalrclient/signalr_value.h>
#include <string>

namespace Voxta
{
	/// <summary>
	/// Class responsable for converting data between the runtime version and the signalr-compatible version.
	/// Note: This class ONLY does the conversion, not actual sending / receiving of anything.
	/// </summary>
	class VoxtaApiRequestHandler
	{
	public:
		signalr::value GetAuthenticateRequestData() const;
		signalr::value GetLoadCharactersListData() const;
		signalr::value GetLoadCharacterRequestData(std::string_view characterId) const;
		signalr::value GetStartChatRequestData(const DataTypes::CharData* charData) const;
		signalr::value GetSendUserMessageData(std::string_view sessionId, std::string_view userMessage) const;
		signalr::value GetNotifyAudioPlaybackStartData(std::string_view sessionId,
			std::string_view messageId,
			int startIndex,
			int endIndex,
			double duration) const;

		signalr::value GetNotifyAudioPlaybackCompleteData(std::string_view sessionId, std::string_view messageId) const;
	};
}