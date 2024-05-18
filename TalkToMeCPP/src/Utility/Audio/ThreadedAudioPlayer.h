// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "WavTools.h"
#include <string>
#include <filesystem>
#include <thread>
#include <queue>
#include <mutex>

namespace Utility::Audio
{
	class ThreadedAudioPlayer
	{
	public:
		ThreadedAudioPlayer() = default;
		~ThreadedAudioPlayer();

		bool AddToQueue(const std::string& pathToFile);

		void StartPlayback();
		void StopPlayback();

	private:
		std::jthread playbackThread;
		std::queue<std::filesystem::path> audioQueue;
		std::mutex queueMutex;
		std::condition_variable cv;
		WavTools wavTools;

		void PlaybackLoop(std::stop_token stopToken);
	};
}