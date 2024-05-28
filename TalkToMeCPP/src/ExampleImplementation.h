// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "Utility/AudioInput/AudioInputWrapper.h"
#include "Voxta/VoxtaClient.h"
#include "Utility/Logging/ThreadedLogger.h"
#include "Voxta/DataTypes/CharData.h"
#include "Voxta/DataTypes/ChatMessage.h"
#include "Utility/AudioPlayback/ThreadedAudioPlayer.h"
#include <memory>
#include <string>
#include <vector>
#include <mutex>

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
	std::unique_ptr<Utility::Logging::ThreadedLogger> logger;
	std::unique_ptr<Voxta::VoxtaClient> voxtaClient;
	std::mutex mutexLock;
	std::condition_variable quittinTimeCondition;
	std::unique_ptr<Utility::AudioPlayback::ThreadedAudioPlayer> audioPlayer;
	std::unique_ptr<Utility::AudioInput::AudioInputWrapper> audioInput;
	bool itsQuittingTime;

	void ListCharacters(const std::vector<std::unique_ptr<const Voxta::DataTypes::CharData>>& characters) const;
	void ClientCallback(const Voxta::VoxtaClient::VoxtaClientState& newState);
	void CharSpeaking(const Voxta::DataTypes::ChatMessage* message, const Voxta::DataTypes::CharData* charSource) const;
	void DisplaySpeechTranscription(std::string_view currentTranscription, bool finalized);
	void DoAudioPlayback(const Voxta::DataTypes::ChatMessage* message);
	int AskUserForCharacterSelection(int charAmount) const;
	std::string GetWrittenUserInput() const;
};