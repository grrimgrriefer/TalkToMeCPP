// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "../CharData.h"
#include "ServerResponseBase.h"

namespace Voxta::DataTypes::ServerResponses
{
	/// <summary>
	/// Data struct for Welcome response from the server.
	/// Should be pretty obvious
	/// </summary>
	struct ServerResponseWelcome : ServerResponseBase
	{
		const DataTypes::CharData m_user;

		explicit ServerResponseWelcome(const DataTypes::CharData& userData) :
			m_user(userData)
		{
		}

		ServerResponseType GetType() final
		{
			return ServerResponseType::WELCOME;
		}
	};
}