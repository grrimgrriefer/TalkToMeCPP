// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

//TODO: clean up this include list lmao
#pragma once
#include "../Logging/LoggerInterface.h"
#include <windows.h>
#include <urlmon.h>
#include <string>
#include <filesystem>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <playsoundapi.h>
#include <chrono>
#include <stop_token>
#include <functional>
#include <winhttp.h>
#include <errhandlingapi.h>
#include <fileapi.h>
#include <handleapi.h>
#include <stringapiset.h>
#include <WinNls.h>
#include <cstdio>
#include <cstring>
#include <format>
#include <iostream>
#include <ostream>
#include <boost/uuid/random_generator.hpp>
#include <vector>
#include <type_traits>

namespace Utility::AudioPlayback
{
	class HttpClient
	{
	public:
		HttpClient();
		~HttpClient();

		std::vector<char> DownloadIntoMemory(const std::wstring& url, const std::wstring& headers) const;

	private:
		HINTERNET m_hSession;
		void InitializeSession();
		void CloseSession();

		bool InitializeConnection(const std::wstring& url, HINTERNET& hConnect, URL_COMPONENTS& urlComp) const;
		bool CreateRequest(const HINTERNET& hConnect, const URL_COMPONENTS& urlComp, HINTERNET& hRequest, const std::wstring& headers) const;
		bool SendRequest(const HINTERNET& hRequest) const;
		bool ReceiveResponse(const HINTERNET& hRequest, std::vector<char>& audioData) const;
		void AppendData(std::vector<char>& audioData, const LPSTR pszOutBuffer, DWORD dwSize) const;
	};
}