// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ThreadedAudioPlayer.h"
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

#pragma comment(lib, "urlmon.lib")
#pragma comment(lib, "Winmm.lib")

namespace Utility::Audio
{
	ThreadedAudioPlayer::~ThreadedAudioPlayer()
	{
		StopPlayback();
	}

	bool ThreadedAudioPlayer::AddToQueue(const std::string& url)
	{
		// Download the WAV file to a temporary file
		auto tempFile = std::filesystem::temp_directory_path() / "downloaded.wav";

		if (SUCCEEDED(URLDownloadToFileA(nullptr, url.c_str(), tempFile.string().c_str(), 0, nullptr)))
		{
			std::lock_guard<std::mutex> lock(queueMutex);
			audioQueue.push(tempFile);
			cv.notify_one();
			return true;
		}
		return false;
	}

	void ThreadedAudioPlayer::StartPlayback()
	{
		playbackThread = std::jthread(std::bind_front(&ThreadedAudioPlayer::PlaybackLoop, this));
	}

	void ThreadedAudioPlayer::StopPlayback()
	{
		if (playbackThread.joinable())
		{
			playbackThread.request_stop();
			cv.notify_one();
			playbackThread.join();
		}
	}

	void ThreadedAudioPlayer::PlaybackLoop(std::stop_token stopToken)
	{
		while (!stopToken.stop_requested())
		{
			std::unique_lock<std::mutex> lock(queueMutex);
			cv.wait(lock, [this, &stopToken] { return !audioQueue.empty() || stopToken.stop_requested(); });

			if (stopToken.stop_requested())
			{
				break;
			}

			auto tempFile = audioQueue.front();
			audioQueue.pop();
			lock.unlock();

			PlaySoundA(tempFile.string().c_str(), nullptr, SND_FILENAME | SND_ASYNC);
			std::this_thread::sleep_for(std::chrono::duration<double>(wavTools.CalculateDuration(tempFile.string())));

			std::filesystem::remove(tempFile);
		}
	}
}