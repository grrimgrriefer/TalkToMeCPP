// 2024 - Creative Commons Zero v1.0 Universal

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

		std::vector<DataTypes::CharData> m_characters;
	};
}