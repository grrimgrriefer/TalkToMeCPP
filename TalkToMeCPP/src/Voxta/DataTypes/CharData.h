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

	struct CharDataIdComparer
	{
		std::string_view m_charId;
		explicit CharDataIdComparer(std::string_view charId) : m_charId(charId) {}

		bool operator()(const DataTypes::CharData& element) const
		{
			return element.m_id == m_charId;
		}
	};
}