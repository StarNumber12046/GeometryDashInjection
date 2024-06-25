#include <iostream>
#include <string>
#include <stdio.h>
#include <random>
#include <fstream>
#include <Windows.h>
#include <TlHelp32.h>
#include "util.h"

namespace util
{


	std::vector<uint8_t> hexToBytes(const std::string& hex)
	{
		std::vector<uint8_t> bytes;

		for (unsigned int i = 0; i < hex.length(); i += 3)
		{
			std::string byteString = hex.substr(i, 2);
			uint8_t byte = static_cast<uint8_t>(strtol(byteString.c_str(), nullptr, 16));
			bytes.push_back(byte);
		}

		return bytes;
	}
	DWORD FindProcessId(const std::wstring& processName)
	{
		PROCESSENTRY32 processInfo;
		processInfo.dwSize = sizeof(processInfo);

		HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
		if (processesSnapshot == INVALID_HANDLE_VALUE) {
			return 0; // Failed to create snapshot
		}

		DWORD processId = 0; // Default to 0 (not found)

		if (Process32First(processesSnapshot, &processInfo)) {
			do {
				if (_wcsicmp(processInfo.szExeFile, processName.c_str()) == 0) {
					processId = processInfo.th32ProcessID;
					break; // Found the process, break out of loop
				}
			} while (Process32Next(processesSnapshot, &processInfo));
		}

		CloseHandle(processesSnapshot);
		return processId;
	}

	uintptr_t GetModuleBaseAddress(DWORD processID, const wchar_t* moduleName) {
		uintptr_t moduleBaseAddress = 0;
		HANDLE hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
		if (hModuleSnap == INVALID_HANDLE_VALUE) {
			return 0;
		}

		MODULEENTRY32 me32;
		me32.dwSize = sizeof(MODULEENTRY32);

		if (Module32First(hModuleSnap, &me32)) {
			do {
				if (!_wcsicmp(me32.szModule, moduleName)) {
					moduleBaseAddress = (uintptr_t)me32.modBaseAddr;
					break;
				}
			} while (Module32Next(hModuleSnap, &me32));
		}

		CloseHandle(hModuleSnap);
		return moduleBaseAddress;
	}



	BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
		DWORD processId;
		GetWindowThreadProcessId(hwnd, &processId);

		// Check if the window belongs to the target process
			int length = GetWindowTextLength(hwnd);
			if (length > 0) {
				std::wstring windowTitle;
				windowTitle.resize(length + 1);

				// Get window title and print it
				GetWindowText(hwnd, &windowTitle[0], length + 1);
				std::wcout << L"Window Title: " << windowTitle << std::endl;
				
				if (windowTitle.find(L"Geometry Dash") != std::wstring::npos) {

					printf("Found\n");
					*((HWND*)lParam) = hwnd;
					return FALSE; // Stop enumerating after finding the first window
				}
			// Store the window handle in the provided pointer
			}
			return TRUE;
	
	}
	// Function to enumerate and print windows of a process by process ID
	HWND FindMainWindow(DWORD processId) {
		HWND mainWindow = nullptr;

		// Enumerate all top-level windows
		if (!EnumWindows(EnumWindowsProc, (LPARAM)&mainWindow)) {
			DWORD lastError = GetLastError();
			if (lastError != ERROR_SUCCESS) {
				std::cerr << "Error while enumerating windows: " << lastError << std::endl;
			}
		}
		printf("mainWindow: %p\n", mainWindow);
		return mainWindow;
	}
}