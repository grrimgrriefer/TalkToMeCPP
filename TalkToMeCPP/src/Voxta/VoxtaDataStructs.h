// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include <string>

namespace Voxta
{
	struct MessageStart
	{
		std::string SessionId;
		std::string MessageId;
		std::string SenderId;
	};

	struct MessageChunk
	{
		std::string SessionId;
		std::string MessageId;
		std::string Text;
		std::string AudioUrl;
		int StartIndex;
		int EndIndex;
		std::string SenderId;
	};

	struct MessageEnd
	{
		std::string SessionId;
		std::string MessageId;
		std::string SenderId;
	};

	struct CharacterItem
	{
		std::string Id;
		std::string Name;
		std::string CreatorNotes;
	};
}