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
	/// <summary>
	/// Class that handles the downloading and playback of STT audio.
	/// This is all done in background threads in a queued manner.
	/// </summary>
	class ThreadedAudioPlayer
	{
	public:
		explicit ThreadedAudioPlayer(Logging::LoggerInterface& logger,
			std::string_view serverIP,
			int serverPort);

		~ThreadedAudioPlayer();

		bool RequestQueuedPlayback(std::string_view pathToFile);
		void RegisterFinishedPlaybackTrigger(const std::function<void()>& onPlaybackFinished);

	private:
		WavTools m_wavTools;
		HttpClient m_httpClient;
		Logging::LoggerInterface& m_logger;

		std::mutex m_queueMutex;
		std::condition_variable m_cv;
		std::function<void()> m_playbackFinished;
		std::jthread m_playbackThread;
		std::queue<std::vector<char>> m_audioQueue;

		std::string m_serverIP;
		int m_serverPort;

		void StartPlayback();
		void StopPlayback();
		void PlaybackLoop(std::stop_token stopToken);
		std::wstring ConvertToWideString(std::string_view narrow) const;
	};
}