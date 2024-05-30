// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "Utility/AudioInput/AudioInputWrapper.h"
#include "Utility/AudioPlayback/ThreadedAudioPlayer.h"
#include "Utility/Logging/LoggerInterface.h"
#include "Voxta/DataTypes/CharData.h"
#include "Voxta/DataTypes/ChatMessage.h"
#include "Voxta/VoxtaClient.h"
#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace JustAnExample
{
	/// <summary>
	/// Simple implementation that uses the Voxta C++ utility.
	/// Currently a console application, but can be swapped with anything.
	/// </summary>
	class ExampleImplementation
	{
	public:
		ExampleImplementation(std::string_view serverIP, int serverPort);

		void Start();
		void ForceStop();

	private:
		std::unique_ptr<Utility::Logging::LoggerInterface> m_logger;
		std::unique_ptr<Voxta::VoxtaClient> m_voxtaClient;
		std::unique_ptr<Utility::AudioPlayback::ThreadedAudioPlayer> m_audioPlayer;
		std::unique_ptr<Utility::AudioInput::AudioInputWrapper> m_audioInput;

		std::mutex m_mutexLock;
		std::condition_variable m_quitCondition;
		std::atomic<bool> m_hasApprovalToQuit;
		size_t transcriptionTextLength;

		void ClientCallback(const Voxta::VoxtaClient::VoxtaClientState& newState) const;

		std::string GetWrittenUserInput() const;
		int AskUserForCharacterSelection(int charAmount) const;
		void ListCharacters(const std::vector<const Voxta::DataTypes::CharData*>& characters) const;
		void CharSpeaking(const Voxta::DataTypes::ChatMessage* message, const Voxta::DataTypes::CharData* charSource) const;
		void DisplaySpeechTranscription(std::string_view currentTranscription, bool finalized);
		void DoAudioPlayback(const Voxta::DataTypes::ChatMessage* message) const;
	};
}