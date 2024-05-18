// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include <string>
#include <memory>

namespace Voxta::DataTypes
{
	struct VoiceServiceParams
	{
		explicit VoiceServiceParams(std::string_view filename, std::string_view temperature,
			std::string_view topK, std::string_view topP) : m_filename(filename), m_temperature(temperature),
			m_topK(topK), m_topP(topP)
		{
		}
		~VoiceServiceParams() = default;

		const std::string m_filename = "female_03.wav";
		const std::string m_temperature = "0.7";
		const std::string m_topK = nullptr;
		const std::string m_topP = nullptr;
	};

	struct CharVoiceService
	{
		explicit CharVoiceService(VoiceServiceParams params, std::string_view id, std::string_view name) :
			m_parameters(params), m_name(name), m_id(id)
		{
		}
		~CharVoiceService() = default;

		const VoiceServiceParams m_parameters;
		const std::string m_name = "Coqui";
		const std::string m_id = "aef6d791-314f-f1e7-32f1-72b382dc7bd9";
	};

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

		std::shared_ptr<CharVoiceService> m_voiceService = nullptr;
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