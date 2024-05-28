// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "../Logging/LoggerInterface.h"
#include "ThreadedAudioPlayer.h"
#include <chrono>
#include <format>
#include <functional>
#include <iostream>
#include <mutex>
#include <ostream>
#include <playsoundapi.h>
#include <stop_token>
#include <string>
#include <stringapiset.h>
#include <thread>
#include <windows.h>
#include <WinNls.h>

#pragma comment(lib, "Winmm.lib")

namespace Utility::AudioPlayback
{
	ThreadedAudioPlayer::ThreadedAudioPlayer(Logging::LoggerInterface& logger, std::string_view serverIP, int serverPort)
		: m_wavTools(logger), m_logger(logger), m_serverIP(serverIP), m_serverPort(serverPort)
	{
	}

	ThreadedAudioPlayer::~ThreadedAudioPlayer()
	{
		StopPlayback();
	}

	bool Utility::AudioPlayback::ThreadedAudioPlayer::AddToQueue(std::string_view url)
	{
		std::wstring headers = L"Accept: audio/x-wav, audio/mpeg\r\n"
			L"Accept-Encoding: gzip, deflate, br, zstd\r\n"
			L"Connection: keep-alive\r\n"
			L"Host: " + ConvertToWideString(std::format("{}:{}", m_serverIP, m_serverPort)) + L"\r\n";

		auto audioData = m_httpClient.DownloadIntoMemory(ConvertToWideString(std::format("http://{}:{}{}", m_serverIP, m_serverPort, url)), headers);

		if (audioData.size() > 0)
		{
			std::lock_guard<std::mutex> lock(m_queueMutex);
			m_audioQueue.push(audioData);
			m_cv.notify_one();
			return true;
		}
		return false;
	}

	std::wstring ThreadedAudioPlayer::ConvertToWideString(std::string_view narrow) const
	{
		int len = MultiByteToWideChar(CP_UTF8, 0, narrow.data(), -1, nullptr, 0);
		std::wstring wide(len, L'\0');
		MultiByteToWideChar(CP_UTF8, 0, narrow.data(), -1, wide.data(), len);
		return wide;
	}

	void ThreadedAudioPlayer::StartPlayback(std::function<void()> onPlaybackFinished)
	{
		if (m_playbackFinished)
		{
			std::cerr << "Can't start playback while playing" << std::endl;
			return;
		}
		m_playbackFinished = onPlaybackFinished;
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
				bool weStillPlayin = !m_audioQueue.empty() || stopToken.stop_requested();
				if (!weStillPlayin)
				{
					m_playbackFinished();
				}
				return weStillPlayin;
			});

			auto& audioData = m_audioQueue.front();
			PlaySoundA(reinterpret_cast<LPCSTR>(audioData.data()), nullptr, SND_MEMORY | SND_ASYNC);
			std::this_thread::sleep_for(std::chrono::duration<double>(m_wavTools.CalculateDuration(audioData)));
			m_audioQueue.pop();
			lock.unlock();
		}
	}
}