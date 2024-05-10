// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include <string>

namespace Voxta::DataTypes
{
	struct ServiceData
	{
		enum class ServiceType { TEXT_GEN, SPEECH_TO_TEXT, TEXT_TO_SPEECH };

		explicit ServiceData(ServiceType type, std::string_view name, std::string_view id) :
			m_serviceType(type), m_serviceName(name), m_serviceId(id)
		{
		}
		~ServiceData() = default;

		ServiceType m_serviceType;
		std::string m_serviceName;
		std::string m_serviceId;
	};
}