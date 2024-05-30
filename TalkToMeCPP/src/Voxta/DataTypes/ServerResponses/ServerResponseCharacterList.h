// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "../CharData.h"
#include "ServerResponseBase.h"
#include <vector>

namespace Voxta::DataTypes::ServerResponses
{
	/// <summary>
	/// Data struct for the CharacterList(-loaded) response from the server
	/// Should be pretty obvious.
	/// </summary>
	struct ServerResponseCharacterList : ServerResponseBase
	{
		const std::vector<DataTypes::CharData> m_characters;

		ServerResponseType GetType() final
		{
			return ServerResponseType::CHARACTER_LIST;
		}

		explicit ServerResponseCharacterList(const std::vector<DataTypes::CharData>& characters) :
			m_characters(characters)
		{
		}
	};
}