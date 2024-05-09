// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "DataTypes/VoxtaResponseBase.h"
#include "DataTypes/CharData.h"
#include <signalrclient/signalr_value.h>
#include <memory>
#include <string>
#include <map>

namespace Voxta
{
	class VoxtaApiHandler
	{
	public:
		enum class VoxtaGenericRequestType
		{
			AUTHENTICATE, LOAD_CHARACTERS_LIST
		};

		explicit VoxtaApiHandler();
		~VoxtaApiHandler() = default;

		signalr::value GetRequestData(const VoxtaGenericRequestType commData) const;
		signalr::value GetLoadCharacterRequestData(std::string_view characterId) const;
		signalr::value GetStartChatRequestData(DataTypes::CharData charData) const;

		std::unique_ptr<DataTypes::VoxtaResponseBase> GetResponseData(const std::map<std::string, signalr::value>& map) const;

	private:
		const std::unique_ptr<signalr::value> m_authenticateReqData;
		const std::unique_ptr<signalr::value> m_loadCharacterListReqData;

		signalr::value ConstructAuthenticateReqData() const;
		signalr::value ConstructLoadCharacterListReqData() const;
	};
}
