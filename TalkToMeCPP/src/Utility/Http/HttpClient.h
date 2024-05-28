// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include <string>
#include <vector>
#include <windows.h>
#include <winhttp.h>

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