// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "../Http/HttpClient.h"
#include "../Logging/LoggerInterface.h"
#include "WavTools.h"
#include <functional>
#include <mutex>
#include <queue>
#include <stop_token>
#include <string>
#include <thread>
#include <vector>

namespace Utility::AudioPlayback
{
	class ThreadedAudioPlayer
	{
	public:
		explicit ThreadedAudioPlayer(Logging::LoggerInterface& logger, std::string_view serverIP, int serverPort);
		~ThreadedAudioPlayer();

		bool RequestQueuedPlayback(std::string_view pathToFile);
		void RegisterFinishedPlaybackTrigger(std::function<void()> onPlaybackFinished);

	private:
		std::jthread m_playbackThread;
		std::queue<std::vector<char>> m_audioQueue;
		std::mutex m_queueMutex;
		std::condition_variable m_cv;
		WavTools m_wavTools;
		Logging::LoggerInterface& m_logger;
		HttpClient m_httpClient;
		std::function<void()> m_playbackFinished;
		std::string m_serverIP;
		int m_serverPort;

		void StartPlayback();
		void StopPlayback();
		void PlaybackLoop(std::stop_token stopToken);
		std::wstring ConvertToWideString(std::string_view narrow) const;
	};
}