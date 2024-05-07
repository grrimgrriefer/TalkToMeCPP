// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "Voxta/VoxtaClient.h"
#include "Logger/ThreadedLogger.h"
#include <iostream>
#include <filesystem>
#include <memory>
#include <algorithm>
#include <format>
#include <functional>

class SimpleClient
{
public:
	std::unique_ptr<Voxta::VoxtaClient> voxtaClient;

	void DoStuff()
	{
		auto path = std::filesystem::current_path();
		path /= "logfile.txt";
		auto logger(std::make_unique<Logger::ThreadedLogger>(path.string()));
		voxtaClient = std::make_unique<Voxta::VoxtaClient>(*logger, "127.0.0.1", 5384, [this] (Voxta::VoxtaClient::VoxtaClientState newState)
			{
				ClientCallback(newState);
			});
		voxtaClient->Connect();

		std::cin.get();
	}

	void ClientCallback(const Voxta::VoxtaClient::VoxtaClientState& newState) const
	{
		if (newState == Voxta::VoxtaClient::VoxtaClientState::IDLE)
		{
			std::cout << std::endl << std::format("Welcome {} !!", voxtaClient->GetUsername()) << std::endl << std::endl;
			std::cout << "I was able to find your girlfriends listed below :D" << std::endl;

			auto& characters = voxtaClient->GetCharacters();
			for (int i = 0; i < characters.size(); i++)
			{
				std::string output = std::format("   {}  {}", i, characters[i].m_name);
				if (characters[i].m_explicitContent)
				{
					output.append("  [Explicit]");
				}

				if (!characters[i].m_creatorNotes.empty())
				{
					output.append(std::format("  ({})", characters[i].m_creatorNotes));
				}

				std::cout << output << std::endl;
			}
		}
	}
};

int main()
{
	auto client = std::make_unique<SimpleClient>();
	client->DoStuff();
	return 0;
}