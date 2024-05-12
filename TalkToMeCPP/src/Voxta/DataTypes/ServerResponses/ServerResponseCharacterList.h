// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ServerResponseBase.h"
#include "../CharData.h"
#include <vector>

namespace Voxta::DataTypes::ServerResponses
{
	struct ServerResponseCharacterList : ServerResponseBase
	{
		ServerResponseType GetType() final
		{
			return ServerResponseType::CHARACTER_LIST;
		}

		explicit ServerResponseCharacterList(const std::vector<DataTypes::CharData>& characters) : m_characters(characters)
		{
		}

		const std::vector<DataTypes::CharData> m_characters;
	};
}