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
		enum class VoxtaRequestType { AUTHENTICATE, LOAD_CHARACTERS_LIST };

		explicit VoxtaApiHandler();
		~VoxtaApiHandler() = default;

		const signalr::value& GetRequestData(const VoxtaRequestType commData) const;
		std::unique_ptr<DataTypes::VoxtaResponseBase> GetResponseData(const std::map<std::string, signalr::value>& map) const;

	private:
		std::unique_ptr<signalr::value> m_authenticateReqData;
		std::unique_ptr<signalr::value> m_loadCharacterListReqData;

		signalr::value ConstructAuthenticateReqData() const;
		signalr::value ConstructLoadCharacterListReqData() const;
	};
}