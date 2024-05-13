// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "Voxta/VoxtaClient.h"
#include "Logging/ThreadedLogger.h"
#include "Voxta/DataTypes/CharData.h"
#include "Voxta/DataTypes/ChatMessage.h"
#include "Voxta/SignalRWrapper.h"
#include <iostream>
#include <filesystem>
#include <memory>
#include <format>
#include <string>
#include <vector>
#include <climits>
#include <stdlib.h>
#include <mutex>
#include <istream>
#include <type_traits>

class MainThreadHogger
{
public:
	MainThreadHogger()
	{
		auto path = std::filesystem::current_path();
		path /= "logfile.txt";
		logger = std::make_unique<Logging::ThreadedLogger>(path.string());

		auto wrapper = std::make_unique<Voxta::SignalRWrapper>("127.0.0.1", 5384, *logger);
		std::unique_ptr<Voxta::SignalRWrapperInterface> interfacePtr = std::move(wrapper);
		voxtaClient = std::make_unique<Voxta::VoxtaClient>(std::move(interfacePtr), *logger,
			[this] (Voxta::VoxtaClient::VoxtaClientState newState)
			{
				ClientCallback(newState);
			},
			[this] ()
			{
				return AskUserInput();
			},
			[this] (const Voxta::DataTypes::ChatMessage* message, const Voxta::DataTypes::CharData* charSource)
			{
				CharSpeaking(message, charSource);
			});

		voxtaClient->Connect();
		std::unique_lock lock(mutexLock);
		quittinTimeCondition.wait(lock, [this] { return itsQuittingTime; });
	}

	void ForceStop()
	{
		itsQuittingTime = true;
		quittinTimeCondition.notify_one();
	}

private:
	std::unique_ptr<Logging::ThreadedLogger> logger;
	std::unique_ptr<Voxta::VoxtaClient> voxtaClient;
	std::mutex mutexLock;
	std::condition_variable quittinTimeCondition;
	bool itsQuittingTime;

	void ClientCallback(const Voxta::VoxtaClient::VoxtaClientState& newState)
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

	void CharSpeaking(const Voxta::DataTypes::ChatMessage* message, const Voxta::DataTypes::CharData* charSource) const
	{
		std::cout << std::endl << std::format("{}: {}", charSource->m_name, message->m_text) << std::endl;
	}

	int AskUserForCharacterSelection(int charAmount) const
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

	void ListCharacters(const std::vector<std::unique_ptr<Voxta::DataTypes::CharData>>& characters) const
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

	std::string AskUserInput() const
	{
		std::cout << std::format("{}: ", voxtaClient->GetUsername());

		std::string line;
		std::getline(std::cin >> std::ws, line);
		return line;
	}
};

int main()
{
	auto client = MainThreadHogger();
	return 0;
}