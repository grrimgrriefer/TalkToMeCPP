// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "WavTools.h"
#include "../Logging/LoggerInterface.h"
#include "../Http/HttpClient.h"
#include <string>
#include <filesystem>
#include <thread>
#include <queue>
#include <mutex>
#include <stop_token>
#include <vector>

namespace Utility::AudioPlayback
{
	class ThreadedAudioPlayer
	{
	public:
		explicit ThreadedAudioPlayer(Logging::LoggerInterface& logger, std::string_view serverIP, int serverPort);
		~ThreadedAudioPlayer();

		bool AddToQueue(const std::string& pathToFile);

		void StartPlayback(std::function<void()> onFinishedCallback);
		void StopPlayback();

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

		void PlaybackLoop(std::stop_token stopToken);
		std::wstring ConvertToWideString(const std::string& narrow);
	};
}