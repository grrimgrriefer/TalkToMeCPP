// Copyright(c) 2024 grrimgrriefer & DZnnah, see LICENSE for details.

#pragma once
#include "ExampleImplementation.h"
#include <Windows.h>
#include <consoleapi.h>
#include <processenv.h>

static void DisableAnnoyingQuickEditMode();

int main()
{
	DisableAnnoyingQuickEditMode();

	auto implementation = JustAnExample::ExampleImplementation("127.0.0.1", 5384);
	implementation.Start();
}

/// <summary>
/// Quick Edit is a 'NeW WiNdOwS 10 FeAtUrE' that can buffer cmd output causing the app to stall
/// This is a quick fix to disable it.
/// </summary>
static void DisableAnnoyingQuickEditMode()
{
	HANDLE hInput = GetStdHandle(STD_INPUT_HANDLE);
	DWORD prev_mode;
	if (GetConsoleMode(hInput, &prev_mode))
	{
		DWORD new_mode = prev_mode & ~ENABLE_QUICK_EDIT_MODE;
		SetConsoleMode(hInput, new_mode);
	}
}