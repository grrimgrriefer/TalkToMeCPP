// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include <string>
#include <memory>

namespace Voxta::DataTypes
{
	/// <summary>
	/// Data struct representing a character (note: user is also considered a character atm; might change idk yet)
	/// </summary>
	struct CharData
	{
		explicit CharData(std::string_view id, std::string_view name) : m_id(id), m_name(name) {}
		~CharData() = default;

		std::string m_id;
		std::string m_name;
		std::string m_creatorNotes = "";
		bool m_explicitContent = false;
		bool m_favorite = false;
	};

	/// <summary>
	/// Comparer struct for CharData
	/// Used by the std::ranges syntax for improved performance
	///
	/// Yes, two structs in one file. Go ahead, sue me.
	/// </summary>
	struct CharDataIdComparer
	{
		std::string_view m_charId;
		explicit CharDataIdComparer(std::string_view charId) : m_charId(charId) {}

		bool operator()(const std::unique_ptr<const DataTypes::CharData>& element) const
		{
			return element->m_id == m_charId;
		}
	};
}