// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "Voxta/VoxtaClient.h"
#include "Logger/ThreadedLogger.h"
#include "Voxta/DataTypes/CharData.h"
#include <iostream>
#include <filesystem>
#include <memory>
#include <algorithm>
#include <format>
#include <functional>
#include <cctype>
#include <string>
#include <cstdlib>
#include <vector>
#include <climits>

class ExampleClient
{
public:
	void DoStuff()
	{
		auto path = std::filesystem::current_path();
		path /= "logfile.txt";
		auto logger(std::make_unique<Logger::ThreadedLogger>(path.string()));
		voxtaClient = std::make_unique<Voxta::VoxtaClient>(*logger, "127.0.0.1", 5384,
			[this] (Voxta::VoxtaClient::VoxtaClientState newState)
			{
				ClientCallback(newState);
			});
		voxtaClient->Connect();
	}

private:
	std::unique_ptr<Voxta::VoxtaClient> voxtaClient;

	void ClientCallback(const Voxta::VoxtaClient::VoxtaClientState& newState) const
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
				break;
			}
			default:
				break;
		}
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
				return index;
			}

			std::cout << std::format("Dangit, I couldn't read that as a valid number. :( Pls only enter a number between 0 and {}",
				charAmount) << std::endl;
		}
		while (true);
	}

	void ListCharacters(const std::vector<std::shared_ptr<Voxta::DataTypes::CharData>>& characters) const
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
};

int main()
{
	auto client = std::make_unique<ExampleClient>();
	client->DoStuff();
	return 0;
}