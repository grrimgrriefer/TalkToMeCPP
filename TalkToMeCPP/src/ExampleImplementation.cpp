// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ExampleImplementation.h"
#include "Utility/AudioInput/AudioInputWrapper.h"
#include "Utility/AudioPlayback/ThreadedAudioPlayer.h"
#include "Utility/Logging/ThreadedLogger.h"
#include "Utility/SignalR/SignalRWrapper.h"
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
#include <vector>

namespace JustAnExample
{
	ExampleImplementation::ExampleImplementation(std::string_view serverIP, int serverPort)
	{
		auto path = std::filesystem::current_path();
		path /= "logfile.txt";

		m_logger = std::make_unique<Utility::Logging::ThreadedLogger>(path.string());
		m_audioPlayer = std::make_unique<Utility::AudioPlayback::ThreadedAudioPlayer>(*m_logger,
			serverIP,
			serverPort);
		m_audioInput = std::make_unique<Utility::AudioInput::AudioInputWrapper>(*m_logger,
			serverIP,
			serverPort,
			[this] (std::string_view initializeStatus)
			{
				DisplayMicrophoneInitialization(initializeStatus);
			});

		m_voxtaClient = std::make_unique<Voxta::VoxtaClient>(
			std::make_unique<Utility::SignalR::SignalRWrapper>(serverIP, serverPort, *m_logger),
			*m_logger,
			true,
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
			[this] (std::string_view fatalError)
			{
				std::cout << fatalError << std::endl;
				ForceStop();
				std::cin.get();
			},
			[this] (const Voxta::DataTypes::ChatMessage* message, const Voxta::DataTypes::CharData* charSource)
			{
				CharSpeaking(message, charSource);
				DoAudioPlayback(message);
			});
	}

	void ExampleImplementation::Start()
	{
		m_voxtaClient->Connect();
		std::unique_lock lock(m_mutexLock);
		m_quitCondition.wait(lock, [this] { return m_hasApprovalToQuit.load(); });
	}

	void ExampleImplementation::ForceStop()
	{
		m_hasApprovalToQuit.store(true);
		m_quitCondition.notify_one();
		m_voxtaClient->ForceStopImmediate();
	}

	void ExampleImplementation::ClientCallback(const Voxta::VoxtaClient::VoxtaClientState& newState) const
	{
		switch (newState)
		{
			case Voxta::VoxtaClient::VoxtaClientState::CHARACTER_LOBBY:
			{
				auto characters = m_voxtaClient->GetCharacters();
				int charAmount = characters.size() & INT_MAX;

				ListCharacters(characters);
				int index = AskUserForCharacterSelection(charAmount);
				m_voxtaClient->StartChatWithCharacter(characters[index]->m_id);
				break;
			}
			case Voxta::VoxtaClient::VoxtaClientState::CHATTING:
			{
				auto& characters = m_voxtaClient->GetChatSession()->m_characters;
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

	std::string ExampleImplementation::GetWrittenUserInput() const
	{
		std::cout << std::format("{}: ", m_voxtaClient->GetUsername());

		std::string lineInput;
		std::getline(std::cin >> std::ws, lineInput);
		return lineInput;
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
					m_voxtaClient->GetCharacters()[index]->m_name) << std::endl;

				return index;
			}

			std::cout << std::format("Dangit!! I couldn't read that as a valid number :( Sorry... \nPls only enter a number between 0 and {}",
				charAmount - 1) << std::endl;
		}
		while (true);
	}

	void ExampleImplementation::ListCharacters(const std::vector<const Voxta::DataTypes::CharData*>& characters) const
	{
		auto charAmount = characters.size();

		std::cout << std::endl << std::format("Welcome {} to TalkToMeCPP!!", m_voxtaClient->GetUsername()) << std::endl << std::endl;
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

	void ExampleImplementation::CharSpeaking(const Voxta::DataTypes::ChatMessage* message, const Voxta::DataTypes::CharData* charSource) const
	{
		std::cout << std::endl << std::format("{}: {}", charSource->m_name, message->m_text) << std::endl;
	}

	void ExampleImplementation::DisplayMicrophoneInitialization(std::string_view currentTranscription)
	{
		std::cout << currentTranscription << std::endl;
	}

	void ExampleImplementation::DisplaySpeechTranscription(std::string_view currentTranscription, bool finalized)
	{
		std::string outputText = finalized
			? std::format("{}: {}", m_voxtaClient->GetUsername(), currentTranscription)
			: std::format("{} is busy speaking: {}", m_voxtaClient->GetUsername(), currentTranscription);

		if (outputText.length() > transcriptionTextLength)
		{
			transcriptionTextLength = outputText.length();
		}
		else
		{
			// Add empty spaces to overwrite the old line in the CMD
			std::string indent(transcriptionTextLength - outputText.length(), ' ');
			outputText.append(indent);
		}

		if (!finalized)
		{
			std::cout << "\r" << outputText;
		}
		else
		{
			std::cout << "\r" << outputText << std::endl;
			m_audioInput->StopStreaming();
		}
	}

	void ExampleImplementation::DoAudioPlayback(const Voxta::DataTypes::ChatMessage* message) const
	{
		m_audioPlayer->RegisterFinishedPlaybackTrigger([this, message] ()
			{
				m_voxtaClient->NotifyAudioPlaybackComplete(message->m_messageId);
				m_audioInput->StartStreaming(m_voxtaClient->GetChatSession()->m_sessionId);
			});

		for (const auto& url : message->m_audioUrls)
		{
			m_audioPlayer->RequestQueuedPlayback(url);
		}
	}
}