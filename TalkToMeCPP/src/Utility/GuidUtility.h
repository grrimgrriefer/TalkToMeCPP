// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include <combaseapi.h>
#include <guiddef.h>
#include <iomanip>
#include <ios>
#include <iosfwd>
#include <sstream>
#include <string>
#include <system_error>
#include <winerror.h>
#include <winnt.h>

namespace Utility
{
	class GuidUtility
	{
	public:
		static std::string GenerateGuid()
		{
			GUID guid;
			HRESULT hr = CoCreateGuid(&guid);
			if (SUCCEEDED(hr))
			{
				std::stringstream stream;
				stream << std::hex << std::uppercase
					<< std::setw(8) << std::setfill('0') << guid.Data1
					<< "-" << std::setw(4) << std::setfill('0') << guid.Data2
					<< "-" << std::setw(4) << std::setfill('0') << guid.Data3
					<< "-";
				for (int i = 0; i < sizeof(guid.Data4); ++i)
				{
					if (i == 2)
					{
						stream << "-";
					}
					stream << std::hex << std::setw(2) << std::setfill('0') << (int)guid.Data4[i];
				}
				return stream.str();
			}
			throw std::system_error(-1, std::generic_category(), "Failed to generate guid, no idea how this could happened");
		}
	};
}