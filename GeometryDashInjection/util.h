#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

namespace util
{
    // Converts a hex string to a vector of bytes.
    std::vector<uint8_t> hexToBytes(const std::string& hex);

    // Finds the process ID of a process given its name.
    DWORD FindProcessId(const std::wstring& processName);

    // Gets the base address of a module in a given process.
    uintptr_t GetModuleBaseAddress(DWORD processID, const wchar_t* moduleName);
    HWND FindMainWindow(DWORD pid);
    BOOL EnumWindowsProc(HWND hwnd, LPARAM lParam);
}
