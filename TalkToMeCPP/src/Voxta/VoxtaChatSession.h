// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "DataTypes/CharData.h"

namespace Voxta
{
	class VoxtaChatSession
	{
	public:
		explicit VoxtaChatSession(const DataTypes::CharData& character);
		~VoxtaChatSession() = default;

	private:
		const DataTypes::CharData& m_character;
	};
}