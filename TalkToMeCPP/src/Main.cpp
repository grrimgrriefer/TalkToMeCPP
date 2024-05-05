// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "Voxta/VoxtaClient.h"
#include "Logger/ThreadedLogger.h"
#include <iostream>
#include <filesystem>
#include <memory>

int main()
{
	auto path = std::filesystem::current_path();
	path /= "logfile.txt";
	auto logger(std::make_unique<Logger::ThreadedLogger>(path.string()));
	auto client(std::make_unique<Voxta::VoxtaClient>(*logger, "127.0.0.1", 5384));
	client->Connect();

	std::cin.get();
	return 0;
}