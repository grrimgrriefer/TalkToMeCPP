// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ThreadedAudioPlayer.h"
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
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/lexical_cast.hpp>
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

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "winhttp.lib")

namespace Utility::Audio
{
	ThreadedAudioPlayer::ThreadedAudioPlayer(Logging::LoggerInterface& logger)
		: m_wavTools(logger), m_logger(logger)
	{
	}

	ThreadedAudioPlayer::~ThreadedAudioPlayer()
	{
		StopPlayback();
	}

	bool ThreadedAudioPlayer::AddToQueue(const std::string& url)
	{
		std::wstring wurl = ConvertToWideString(url);
		boost::uuids::uuid guid = boost::uuids::random_generator()();
		std::string guidString = boost::lexical_cast<std::string>(guid);
		auto tempFile = std::filesystem::temp_directory_path() / std::format("ttmcpp{}.wav", guidString);

		std::wstring headers = L"Accept: audio/x-wav, audio/mpeg\r\n"
			L"Accept-Encoding: gzip, deflate, br, zstd\r\n"
			L"Connection: keep-alive\r\n"
			L"Host: localhost:5384\r\n";

		if (DownloadFileWithHeaders(wurl, tempFile.wstring(), headers))
		{
			std::lock_guard<std::mutex> lock(m_queueMutex);
			m_audioQueue.push(tempFile);
			m_cv.notify_one();
			return true;
		}
		return false;
	}

	std::wstring ThreadedAudioPlayer::ConvertToWideString(const std::string& narrow)
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, narrow.c_str(), -1, nullptr, 0);
		std::wstring wide(len, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, narrow.c_str(), -1, wide.data(), len);
		return wide;
	}

	void ThreadedAudioPlayer::StartPlayback()
	{
		m_playbackThread = std::jthread(std::bind_front(&ThreadedAudioPlayer::PlaybackLoop, this));
	}

	void ThreadedAudioPlayer::StopPlayback()
	{
		if (m_playbackThread.joinable())
		{
			m_playbackThread.request_stop();
			m_cv.notify_one();
			m_playbackThread.join();
		}
	}

	void ThreadedAudioPlayer::PlaybackLoop(std::stop_token stopToken)
	{
		while (!stopToken.stop_requested())
		{
			std::unique_lock<std::mutex> lock(m_queueMutex);
			m_cv.wait(lock, [this, &stopToken]
			{
				return !m_audioQueue.empty() || stopToken.stop_requested();
			});

			if (stopToken.stop_requested())
			{
				break;
			}

			auto tempFile = m_audioQueue.front();
			m_audioQueue.pop();
			lock.unlock();

			PlaySoundA(tempFile.string().c_str(), nullptr, SND_FILENAME | SND_ASYNC);
			std::this_thread::sleep_for(std::chrono::duration<double>(m_wavTools.CalculateDuration(tempFile.string())));

			//std::filesystem::remove(tempFile);
		}
	}

	bool ThreadedAudioPlayer::DownloadFileWithHeaders(const std::wstring& url, const std::wstring& filePath, const std::wstring& headers)
	{
		bool result = false;
		HINTERNET hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
										 WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
										 WINHTTP_NO_PROXY_NAME,
										 WINHTTP_NO_PROXY_BYPASS, 0);

		if (hSession)
		{
			URL_COMPONENTS urlComp;
			ZeroMemory(&urlComp, sizeof(urlComp));
			urlComp.dwStructSize = sizeof(urlComp);

			// Set required components sizes to non-zero so that they are allocated
			urlComp.dwSchemeLength = (DWORD)-1;
			urlComp.dwHostNameLength = (DWORD)-1;
			urlComp.dwUrlPathLength = (DWORD)-1;
			urlComp.dwExtraInfoLength = (DWORD)-1;

			if (WinHttpCrackUrl(url.c_str(), (DWORD)url.length(), 0, &urlComp))
			{
				// Allocate memory for the hostname
				wchar_t* hostName = new wchar_t[urlComp.dwHostNameLength + 1];
				wcsncpy_s(hostName, urlComp.dwHostNameLength + 1, urlComp.lpszHostName, urlComp.dwHostNameLength);
				hostName[urlComp.dwHostNameLength] = L'\0'; // Ensure null-termination

				HINTERNET hConnect = WinHttpConnect(hSession, hostName, urlComp.nPort, 0);
				if (hConnect)
				{
					HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"GET", urlComp.lpszUrlPath,
															NULL, WINHTTP_NO_REFERER,
															WINHTTP_DEFAULT_ACCEPT_TYPES,
															(urlComp.nScheme == INTERNET_SCHEME_HTTPS) ? WINHTTP_FLAG_SECURE : 0);
					if (hRequest)
					{
						// Add headers to the request
						if (WinHttpAddRequestHeaders(hRequest, headers.c_str(), (DWORD)headers.length(), WINHTTP_ADDREQ_FLAG_ADD))
						{
							// Send the request
							if (WinHttpSendRequest(hRequest,
								WINHTTP_NO_ADDITIONAL_HEADERS, 0,
								WINHTTP_NO_REQUEST_DATA, 0,
								0, 0))
							{
								// Write response to file
								if (WinHttpReceiveResponse(hRequest, NULL))
								{
									DWORD dwSize = 0;
									DWORD dwDownloaded = 0;
									LPSTR pszOutBuffer;
									HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_WRITE, FILE_SHARE_READ, NULL,
															  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

									if (hFile != INVALID_HANDLE_VALUE)
									{
										// Keep checking for data until there is nothing left.
										do
										{
											// Check for available data.
											dwSize = 0;
											if (!WinHttpQueryDataAvailable(hRequest, &dwSize))
												printf("Error %u in WinHttpQueryDataAvailable.\n",
														GetLastError());

											// Allocate space for the buffer.
											pszOutBuffer = new char[dwSize + 1];
											if (!pszOutBuffer)
											{
												printf("Out of memory\n");
												dwSize = 0;
											}
											else
											{
												// Read the data.
												ZeroMemory(pszOutBuffer, dwSize + 1);

												if (!WinHttpReadData(hRequest, (LPVOID)pszOutBuffer,
													dwSize, &dwDownloaded))
													printf("Error %u in WinHttpReadData.\n", GetLastError());
												else
													WriteFile(hFile, pszOutBuffer, dwDownloaded, &dwDownloaded, NULL);

												// Free the memory allocated to the buffer.
												delete[] pszOutBuffer;
											}
										}
										while (dwSize > 0);

										CloseHandle(hFile);
										result = true;
									}
								}
							}
						}
						WinHttpCloseHandle(hRequest);
					}
					WinHttpCloseHandle(hConnect);
				}
				else
				{
					// WinHttpConnect failed.
					DWORD dwError = GetLastError();
					std::wcout << L"WinHttpConnect failed with error: " << dwError << std::endl;
				}
			}
			WinHttpCloseHandle(hSession);
		}
		return result;
	}
}