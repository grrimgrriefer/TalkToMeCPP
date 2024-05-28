// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ExampleImplementation.h"
#include "Utility/AudioInput/AudioInputWrapper.h"
#include "Utility/AudioPlayback/ThreadedAudioPlayer.h"
#include "Utility/Logging/ThreadedLogger.h"
#include "Utility/SignalR/SignalRWrapper.h"
#include "Utility/SignalR/SignalRWrapperInterface.h"
#include "Voxta/DataTypes/CharData.h"
#include "Voxta/DataTypes/ChatMessage.h"
#include "Voxta/VoxtaClient.h"
#include <climits>
#include <filesystem>
#include <format>
#include <iostream>
#include <istream>
#include <memory>
#include <ostream>
#include <stdlib.h>
#include <string>
#include <type_traits>
#include <vector>

ExampleImplementation::ExampleImplementation(std::string_view serverIP, int serverPort)
{
	auto path = std::filesystem::current_path();
	path /= "logfile.txt";
	logger = std::make_unique<Utility::Logging::ThreadedLogger>(path.string());
	audioPlayer = std::make_unique<Utility::AudioPlayback::ThreadedAudioPlayer>(*logger, serverIP, serverPort);
	audioInput = std::make_unique<Utility::AudioInput::AudioInputWrapper>(serverIP, serverPort);

	auto wrapper = std::make_unique<Utility::SignalR::SignalRWrapper>(serverIP, serverPort, *logger);
	std::unique_ptr<Utility::SignalR::SignalRWrapperInterface> interfacePtr = std::move(wrapper);
	voxtaClient = std::make_unique<Voxta::VoxtaClient>(std::move(interfacePtr), *logger,
		[this] (Voxta::VoxtaClient::VoxtaClientState newState)
		{
			ClientCallback(newState);
		},
		[this] ()
		{
			return GetWrittenUserInput();
		},
		[this] (std::string_view currentTranscription, bool finalized)
		{
			DisplaySpeechTranscription(currentTranscription, finalized);
		},
		[this] (const Voxta::DataTypes::ChatMessage* message, const Voxta::DataTypes::CharData* charSource)
		{
			CharSpeaking(message, charSource);
			DoAudioPlayback(message);
		});
}

void ExampleImplementation::Start()
{
	voxtaClient->Connect();
	std::unique_lock lock(mutexLock);
	quittinTimeCondition.wait(lock, [this] { return itsQuittingTime; });
}

void ExampleImplementation::ForceStop()
{
	itsQuittingTime = true;
	quittinTimeCondition.notify_one();
}

void ExampleImplementation::ClientCallback(const Voxta::VoxtaClient::VoxtaClientState& newState)
{
	switch (newState)
	{
		case Voxta::VoxtaClient::VoxtaClientState::CHARACTER_LOBBY:
		{
			auto& characters = voxtaClient->GetCharacters();
			int charAmount = characters.size() & INT_MAX;

			ListCharacters(characters);
			int index = AskUserForCharacterSelection(charAmount);
			voxtaClient->LoadCharacter(characters[index]->m_id);
			break;
		}
		case Voxta::VoxtaClient::VoxtaClientState::CHATTING:
		{
			auto& characters = voxtaClient->GetChatSession()->m_characters;
			std::string charNames;
			for (int i = 0; i < characters.size(); i++)
			{
				charNames.append(characters[i]->m_name);
				if (i < characters.size() - 1)
				{
					charNames.append("and ");
				}
			}
			std::cout << std::endl << std::format("Starting conversation with {} ...", charNames) << std::endl << std::endl;
			break;
		}
		default:
			break;
	}
}

void ExampleImplementation::CharSpeaking(const Voxta::DataTypes::ChatMessage* message, const Voxta::DataTypes::CharData* charSource) const
{
	std::cout << std::endl << std::format("{}: {}", charSource->m_name, message->m_text) << std::endl;
}

void ExampleImplementation::DisplaySpeechTranscription(std::string_view currentTranscription, bool finalized)
{
	if (!finalized)
	{
		std::cout << "\r" << std::format("{} is busy speaking: {}", voxtaClient->GetUsername(), currentTranscription);
	}
	else
	{
		std::cout << "\r" << std::format("{}: {}", voxtaClient->GetUsername(), currentTranscription) << std::endl;
		audioInput->StopStreaming();
	}
}

int ExampleImplementation::AskUserForCharacterSelection(int charAmount) const
{
	std::cout << std::endl << "Please enter the number of whoever you want to chat with :3" << std::endl;
	do
	{
		std::string selection;
		std::cin >> selection;

		char* end = nullptr;
		if (long index = std::strtol(selection.c_str(), &end, 10); *end == '\0' && index >= 0 && index < charAmount)
		{
			std::cout << std::endl << std::format("Awesome! Hold on for a bit, I'll go get {} and make sure she's ready for you :D",
				voxtaClient->GetCharacters()[index]->m_name) << std::endl;

			return index;
		}

		std::cout << std::format("Dangit!! I couldn't read that as a valid number :( Sorry... \nPls only enter a number between 0 and {}",
			charAmount - 1) << std::endl;
	}
	while (true);
}

void ExampleImplementation::ListCharacters(const std::vector<std::unique_ptr<const Voxta::DataTypes::CharData>>& characters) const
{
	auto charAmount = characters.size();

	std::cout << std::endl << std::format("Welcome {} to TalkToMeCPP!!", voxtaClient->GetUsername()) << std::endl << std::endl;
	std::cout << "Listed below are the conversation partners I was able to find :D" << std::endl;

	for (int i = 0; i < charAmount; i++)
	{
		std::string output = std::format("   {}  {}", i, characters[i]->m_name);
		if (characters[i]->m_explicitContent)
		{
			output.append("  [Explicit]");
		}

		if (!characters[i]->m_creatorNotes.empty())
		{
			output.append(std::format("  ({})", characters[i]->m_creatorNotes));
		}

		std::cout << output << std::endl;
	}
}

std::string ExampleImplementation::GetWrittenUserInput() const
{
	std::cout << std::format("{}: ", voxtaClient->GetUsername());

	std::string line;
	std::getline(std::cin >> std::ws, line);
	return line;
}

void ExampleImplementation::DoAudioPlayback(const Voxta::DataTypes::ChatMessage* message)
{
	for (int i = 0; i < message->m_audioUrls.size(); i++)
	{
		audioPlayer->AddToQueue(message->m_audioUrls[i]);
	}
	audioPlayer->StartPlayback([this, message] ()
		{
			voxtaClient->NotifyAudioPlaybackComplete(message->m_messageId);
			audioInput->StartStreaming();
		});
}