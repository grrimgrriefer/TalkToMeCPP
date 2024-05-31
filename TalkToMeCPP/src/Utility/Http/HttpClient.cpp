// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "HttpClient.h"
#include "../Logging/LoggerInterface.h"
#include <errhandlingapi.h>
#include <string>
#include <vector>
#include <Windows.h>
#include <winhttp.h>
#include <format>
#include <mutex>

#pragma comment(lib, "winhttp.lib")

namespace Utility::AudioPlayback
{
	HttpClient::HttpClient(Logging::LoggerInterface& logger) :
		m_logger(logger)
	{
		InitializeSession();
	}

	HttpClient::~HttpClient()
	{
		CloseSession();
	}

	std::vector<char> HttpClient::DownloadIntoMemory(const std::wstring& url, const std::wstring& headers)
	{
		std::scoped_lock<std::mutex> lock(m_mutex);
		std::vector<char> audioData;
		HINTERNET hConnect = nullptr;
		URL_COMPONENTS urlComp = { 0 };

		if (!InitializeConnection(url, hConnect, urlComp))
		{
			return audioData;
		}

		HINTERNET hRequest = nullptr;
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

	void HttpClient::InitializeSession()
	{
		// Initialize WinHTTP session here
		m_hSession = WinHttpOpen(L"TalkToMeCppHTTP/0.2a",
							   WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
							   WINHTTP_NO_PROXY_NAME,
							   WINHTTP_NO_PROXY_BYPASS, 0);
		if (!m_hSession)
		{
			auto lastError = GetLastError();
			m_logger.LogMessage(Logging::LoggerInterface::LogLevel::Error,
				std::format("Failed to initialize WinHTTP session. Error: {}", std::to_string(lastError)));
			throw HttpClientException(std::format("Failed to initialize WinHTTP session. Error: {}", std::to_string(lastError)));
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

	bool HttpClient::InitializeConnection(const std::wstring& url,
		HINTERNET& hConnect,
		URL_COMPONENTS& urlComp) const
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
		return hConnect != nullptr;
	}

	bool HttpClient::CreateRequest(const HINTERNET& hConnect,
		const URL_COMPONENTS& urlComp,
		HINTERNET& hRequest,
		const std::wstring& headers) const
	{
		hRequest = WinHttpOpenRequest(hConnect, L"GET", urlComp.lpszUrlPath,
									  nullptr, WINHTTP_NO_REFERER,
									  WINHTTP_DEFAULT_ACCEPT_TYPES,
									  (urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0);
		if (!hRequest)
		{
			return false;
		}

		return WinHttpAddRequestHeaders(hRequest, headers.c_str(), -1, WINHTTP_ADDREQ_FLAG_ADD);
	}

	void HttpClient::AppendData(std::vector<char>& audioData,
		const LPSTR pszOutBuffer,
		DWORD dwSize) const
	{
		audioData.insert(audioData.end(), pszOutBuffer, pszOutBuffer + dwSize);
	}

	bool HttpClient::SendRequest(const HINTERNET& hRequest) const
	{
		return WinHttpSendRequest(hRequest,
								  WINHTTP_NO_ADDITIONAL_HEADERS, 0,
								  WINHTTP_NO_REQUEST_DATA, 0,
								  0, 0) &&
			WinHttpReceiveResponse(hRequest, nullptr);
	}

	bool HttpClient::ReceiveResponse(const HINTERNET& hRequest, std::vector<char>& audioData) const
	{
		DWORD dwSize = 0;
		DWORD dwDownloaded = 0;

		do
		{
			if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
			{
				return false;
			}

			if (dwSize > 0)
			{
				std::vector<char> buffer(dwSize);

				if (!WinHttpReadData(hRequest, (LPVOID)buffer.data(), dwSize, &dwDownloaded))
				{
					return false;
				}

				AppendData(audioData, buffer.data(), dwDownloaded);
			}
		}
		while (dwSize > 0);

		return true;
	}
}