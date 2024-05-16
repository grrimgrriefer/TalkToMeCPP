// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include <string>

namespace Voxta::DataTypes
{
	/// <summary>
	/// Data struct representing a service.
	///
	/// TODO: check if stuff like serviceName is really needed or not.
	/// Seems a bit wasteful of memory atm
	/// </summary>
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