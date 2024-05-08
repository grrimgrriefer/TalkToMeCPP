// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "DataTypes/VoxtaResponseBase.h"
#include <signalrclient/signalr_value.h>
#include <memory>

namespace Voxta
{
	class VoxtaApiHandler
	{
	public:
		enum class VoxtaRequestType
		{
			AUTHENTICATE, LOAD_CHARACTERS_LIST, LOAD_CHARACTER
		};

		explicit VoxtaApiHandler();
		~VoxtaApiHandler() = default;

		signalr::value GetRequestData(const VoxtaRequestType commData) const;
		template<typename... Args>
		signalr::value GetRequestData(const VoxtaRequestType commData, Args ...args) const;
		std::unique_ptr<DataTypes::VoxtaResponseBase> GetResponseData(const std::map<std::string, signalr::value>& map) const;

	private:
		const std::unique_ptr<signalr::value> m_authenticateReqData;
		const std::unique_ptr<signalr::value> m_loadCharacterListReqData;

		signalr::value ConstructAuthenticateReqData() const;
		signalr::value ConstructLoadCharacterListReqData() const;
		signalr::value ConstructLoadCharacterReqData(std::string_view characterId) const;
	};
}

template signalr::value Voxta::VoxtaApiHandler::GetRequestData<std::basic_string_view<char, std::char_traits<char>>>(const VoxtaRequestType commData, std::basic_string_view<char, std::char_traits<char>>) const;