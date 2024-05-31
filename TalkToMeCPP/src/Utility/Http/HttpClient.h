// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "../Logging/LoggerInterface.h"
#include <string>
#include <vector>
#include <Windows.h>
#include <winhttp.h>
#include <stdexcept>
#include <mutex>

namespace Utility::AudioPlayback
{
	/// <summary>
	/// C++ HttpClient implementation using winhttp
	/// This class only exists to download TTS wav audio into memory,
	/// for any other use there's likely gonna need to be an overhaul.
	/// #sorryForFutureMe
	/// </summary>
	class HttpClient
	{
	public:
		/// <summary>
		/// Custom exception so I know there's Http stuff going on when it inevitably crashes
		/// </summary>
		struct HttpClientException : public std::runtime_error
		{
			explicit HttpClientException(std::string_view info) : std::runtime_error(info.data()) {}
		};

		explicit HttpClient(Logging::LoggerInterface& logger);
		~HttpClient();

		std::vector<char> DownloadIntoMemory(const std::wstring& url, const std::wstring& headers);

	private:
		HINTERNET m_hSession;
		Logging::LoggerInterface& m_logger;
		std::mutex m_mutex;

		void InitializeSession();
		void CloseSession();

		bool InitializeConnection(const std::wstring& url,
			HINTERNET& hConnect,
			URL_COMPONENTS& urlComp) const;

		bool CreateRequest(const HINTERNET& hConnect,
			const URL_COMPONENTS& urlComp,
			HINTERNET& hRequest,
			const std::wstring& headers) const;

		void AppendData(std::vector<char>& audioData,
			const LPSTR pszOutBuffer,
			DWORD dwSize) const;

		bool SendRequest(const HINTERNET& hRequest) const;
		bool ReceiveResponse(const HINTERNET& hRequest, std::vector<char>& audioData) const;
	};
}