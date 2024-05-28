// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ExampleImplementation.h"

int main()
{
	auto implementation = ExampleImplementation("127.0.0.1", 5384);
	implementation.Start();
}