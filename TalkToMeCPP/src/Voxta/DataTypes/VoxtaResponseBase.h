// 2024 - Creative Commons Zero v1.0 Universal

#pragma once

namespace Voxta::DataTypes
{
	enum class VoxtaResponseType { WELCOME, CHARACTER_LIST };

	struct VoxtaResponseBase
	{
		virtual ~VoxtaResponseBase() = default;
		virtual VoxtaResponseType GetType() = 0;
	};
}