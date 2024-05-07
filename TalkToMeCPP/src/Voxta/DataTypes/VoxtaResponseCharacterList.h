// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "VoxtaResponseBase.h"
#include "CharData.h"
#include <vector>

namespace Voxta::DataTypes
{
	struct VoxtaResponseCharacterList : DataTypes::VoxtaResponseBase
	{
		DataTypes::VoxtaResponseType GetType() final
		{
			return DataTypes::VoxtaResponseType::CHARACTER_LIST;
		}

		explicit VoxtaResponseCharacterList(const std::vector<DataTypes::CharData>& characters) : m_characters(characters)
		{}

		std::vector<DataTypes::CharData> m_characters;
	};
}