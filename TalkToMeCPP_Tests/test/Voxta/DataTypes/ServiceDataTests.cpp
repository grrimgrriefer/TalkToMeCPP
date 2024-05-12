// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#include "pch.h"
#include "CppUnitTest.h"
#include "../TalkToMeCPP/src/Voxta/DataTypes/ServiceData.h"
#include <string>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace TalkToMeCPPTests
{
	TEST_CLASS(ServiceDataTests)
	{
	public:
		TEST_METHOD(TestServiceDataConstructorServiceType)
		{
			Voxta::DataTypes::ServiceData::ServiceType serviceType = Voxta::DataTypes::ServiceData::ServiceType::TEXT_GEN;
			Voxta::DataTypes::ServiceData serviceData(serviceType, "TextGenerator", "service001");

			Assert::IsTrue(serviceType == serviceData.m_serviceType);
		}

		TEST_METHOD(TestServiceDataConstructorServiceName)
		{
			std::string serviceName = "TextGenerator";
			Voxta::DataTypes::ServiceData serviceData(Voxta::DataTypes::ServiceData::ServiceType::TEXT_GEN, serviceName, "service001");

			Assert::AreEqual(serviceName, serviceData.m_serviceName);
		}

		TEST_METHOD(TestServiceDataConstructorServiceId)
		{
			std::string serviceId = "service001";
			Voxta::DataTypes::ServiceData serviceData(Voxta::DataTypes::ServiceData::ServiceType::TEXT_GEN, "TextGenerator", serviceId);

			Assert::AreEqual(serviceId, serviceData.m_serviceId);
		}
	};
}