// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "VoxtaChatSession.h"
#include "DataTypes/CharData.h"

namespace Voxta
{
	VoxtaChatSession::VoxtaChatSession(const DataTypes::CharData& character) : m_character(character)
	{
	}
}