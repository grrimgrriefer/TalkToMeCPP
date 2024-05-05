// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "VoxtaClient.h"
#include <format>

namespace Voxta
{
	VoxtaClient::VoxtaClient(const Logger::ThreadedLogger& logger, std::string_view address, int port)
		: m_logger(logger),
		connection(signalr::hub_connection_builder::create(std::vformat("http://{address}:{port}/hub", std::make_format_args(address, port))).build())
	{
	}

	void VoxtaClient::Connect()
	{
	}

	void VoxtaClient::Disconnect()
	{
	}

	void VoxtaClient::LoadCharacter(std::string_view characterId)
	{
	}

	void VoxtaClient::StartChat(std::string_view sessionData, std::string_view contextKey, const std::vector<std::string_view>& characterFunctions, std::string_view context)
	{
	}

	void VoxtaClient::Authenticate()
	{
	}
}