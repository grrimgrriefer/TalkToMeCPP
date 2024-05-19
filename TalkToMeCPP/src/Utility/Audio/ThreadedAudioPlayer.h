// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "WavTools.h"
#include "../Logging/LoggerInterface.h"
#include <string>
#include <filesystem>
#include <thread>
#include <queue>
#include <mutex>
#include <stop_token>

namespace Utility::Audio
{
	class ThreadedAudioPlayer
	{
	public:
		explicit ThreadedAudioPlayer(Logging::LoggerInterface& logger);
		~ThreadedAudioPlayer();

		bool AddToQueue(const std::string& pathToFile);

		void StartPlayback();
		void StopPlayback();

	private:
		std::jthread m_playbackThread;
		std::queue<std::filesystem::path> m_audioQueue;
		std::mutex m_queueMutex;
		std::condition_variable m_cv;
		WavTools m_wavTools;
		Logging::LoggerInterface& m_logger;

		void PlaybackLoop(std::stop_token stopToken);
		bool DownloadFileWithHeaders(const std::wstring& url, const std::wstring& filePath, const std::wstring& headers);
		std::wstring ConvertToWideString(const std::string& narrow);
	};
}