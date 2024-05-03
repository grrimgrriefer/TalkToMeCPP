// 2024 - Creative Commons Zero v1.0 Universal

#pragma once
#include "Logger/ThreadedLogger.h"
#include <cstdlib>
#include <iostream>
#include <filesystem>

void exiting();

Logger::ThreadedLogger* logger;

int main()
{
	auto path = std::filesystem::current_path();
	path /= "logfile.txt";
	logger = new Logger::ThreadedLogger(path.string());

	std::atexit(exiting);

	std::cin.get();
	return 0;
}

void exiting()
{
	delete client;
}