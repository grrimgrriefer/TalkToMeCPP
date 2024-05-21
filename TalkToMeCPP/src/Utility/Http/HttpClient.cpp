// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

//TODO: clean up this include list lmao
#pragma once
#include "HttpClient.h"
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

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "winhttp.lib")

namespace Utility::AudioPlayback
{
	HttpClient::HttpClient()
	{
		InitializeSession();
	}

	HttpClient::~HttpClient()
	{
		CloseSession();
	}

	void HttpClient::InitializeSession()
	{
		// Initialize WinHTTP session here
		m_hSession = WinHttpOpen(L"A Custom User-Agent/1.0",
							   WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
							   WINHTTP_NO_PROXY_NAME,
							   WINHTTP_NO_PROXY_BYPASS, 0);
		if (!m_hSession)
		{
			std::wcerr << L"WinHttpOpen failed with error: " << GetLastError() << std::endl;
		}
	}

	void HttpClient::CloseSession()
	{
		// Close WinHTTP session here
		if (m_hSession)
		{
			WinHttpCloseHandle(m_hSession);
			m_hSession = nullptr;
		}
	}

	std::vector<char> HttpClient::DownloadIntoMemory(const std::wstring& url, const std::wstring& headers) const
	{
		std::vector<char> audioData;
		HINTERNET hConnect = NULL;
		URL_COMPONENTS urlComp = { 0 };

		if (!InitializeConnection(url, hConnect, urlComp))
		{
			return audioData;
		}

		HINTERNET hRequest = NULL;
		if (!CreateRequest(hConnect, urlComp, hRequest, headers))
		{
			WinHttpCloseHandle(hConnect);
			return audioData;
		}

		if (!SendRequest(hRequest))
		{
			WinHttpCloseHandle(hRequest);
			WinHttpCloseHandle(hConnect);
			return audioData;
		}

		if (!ReceiveResponse(hRequest, audioData))
		{
			WinHttpCloseHandle(hRequest);
			WinHttpCloseHandle(hConnect);
			return audioData;
		}

		WinHttpCloseHandle(hRequest);
		WinHttpCloseHandle(hConnect);

		return audioData;
	}

	bool HttpClient::InitializeConnection(const std::wstring& url, HINTERNET& hConnect, URL_COMPONENTS& urlComp) const
	{
		ZeroMemory(&urlComp, sizeof(urlComp));
		urlComp.dwStructSize = sizeof(urlComp);
		urlComp.dwSchemeLength = (DWORD)-1;
		urlComp.dwHostNameLength = (DWORD)-1;
		urlComp.dwUrlPathLength = (DWORD)-1;
		urlComp.dwExtraInfoLength = (DWORD)-1;

		if (!WinHttpCrackUrl(url.c_str(), (DWORD)url.length(), 0, &urlComp))
		{
			return false;
		}

		std::wstring hostName(urlComp.lpszHostName, urlComp.dwHostNameLength);
		hConnect = WinHttpConnect(m_hSession, hostName.c_str(), urlComp.nPort, 0);
		return hConnect != NULL;
	}

	bool HttpClient::CreateRequest(const HINTERNET& hConnect, const URL_COMPONENTS& urlComp, HINTERNET& hRequest, const std::wstring& headers) const
	{
		hRequest = WinHttpOpenRequest(hConnect, L"GET", urlComp.lpszUrlPath,
									  NULL, WINHTTP_NO_REFERER,
									  WINHTTP_DEFAULT_ACCEPT_TYPES,
									  (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0);
		if (!hRequest)
		{
			return false;
		}

		return WinHttpAddRequestHeaders(hRequest, headers.c_str(), (DWORD)headers.length(), WINHTTP_ADDREQ_FLAG_ADD);
	}

	bool HttpClient::SendRequest(const HINTERNET& hRequest) const
	{
		return WinHttpSendRequest(hRequest,
								  WINHTTP_NO_ADDITIONAL_HEADERS, 0,
								  WINHTTP_NO_REQUEST_DATA, 0,
								  0, 0) &&
			WinHttpReceiveResponse(hRequest, NULL);
	}

	bool HttpClient::ReceiveResponse(const HINTERNET& hRequest, std::vector<char>& audioData) const
	{
		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;
		LPSTR pszOutBuffer;

		do
		{
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
			{
				return false;
			}

			if (dwSize > 0)
			{
				pszOutBuffer = new char[dwSize];
				if (!pszOutBuffer)
				{
					return false;
				}

				ZeroMemory(pszOutBuffer, dwSize);

				if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer, dwSize, &dwDownloaded))
				{
					delete[] pszOutBuffer;
					return false;
				}

				AppendData(audioData, pszOutBuffer, dwDownloaded);
				delete[] pszOutBuffer;
			}
		}
		while (dwSize > 0);

		return true;
	}

	void HttpClient::AppendData(std::vector<char>& audioData, const LPSTR pszOutBuffer, DWORD dwSize) const
	{
		audioData.insert(audioData.end(), pszOutBuffer, pszOutBuffer + dwSize);
	}
}