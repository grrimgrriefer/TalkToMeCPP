// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include <string>

namespace Voxta::DataTypes
{
	struct CharData
	{
		explicit CharData(std::string_view id, std::string_view name) : m_id(id), m_name(name) {}

		std::string m_id;
		std::string m_name;
		std::string m_creatorNotes = "";
		bool m_explicitContent = false;
		bool m_favorite = false;
	};
}