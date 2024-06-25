#include <iostream>
#include <string>
#include <stdio.h>
#include <random>
#include <fstream>
#include <Windows.h>
#include <TlHelp32.h>
#include "gui.h"
#include "cheat.h"
#include "Memory.h"
#include "util.h"





int main(int argc, char** argv) {
	if (!util::FindProcessId(L"GeometryDash.exe")) {
		printf("The game process could not be found. Did you start the game?");
		return EXIT_FAILURE;
	}

	std::string address = "0x2E67D3";

	Cheats::Cheat NoClip = Cheats::Cheat{ "0x2E67D3", "E9 42 07 00 00", "NoClip" };
	setNoClipClass(NoClip);

	Cheats::Cheat IconBypass = Cheats::Cheat{ "0x122078", "B0 01 90 90 90", "Icon Bypass" };
	setIconBypassClass(IconBypass);
	Cheats::Cheat ColorBypass = Cheats::Cheat{ "0x122547", "B0 01 90 90 90", "Color Bypass" };
	setColorBypassClass(ColorBypass);
	createWindow();

	return 0;
}