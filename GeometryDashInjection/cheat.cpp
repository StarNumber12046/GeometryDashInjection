#include "cheat.h"
#include "util.h"  // Assuming util.h contains the utility functions like hexToBytes, FindProcessId, GetModuleBaseAddress
#include <TlHelp32.h>
#include <iostream>



namespace Cheats {
    Cheat::Cheat(const std::string& addr, const std::string& payl, const std::string& n)
        : address(addr), payload(payl), name(n), enabled(false),
        bytes(util::hexToBytes(payload)), originalMemory(bytes.size()) {}

    Cheat::~Cheat() = default;

    void Cheat::set(bool value) {
        DWORD PID = util::FindProcessId(L"GeometryDash.exe");
        if (PID == 0) {
            std::cerr << "Game process not found\n";
            return;
        }

        HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
        if (hProcess == NULL) {
            std::cerr << "Failed to open process\n";
            return;
        }

        uintptr_t moduleBase = util::GetModuleBaseAddress(PID, L"GeometryDash.exe");
        if (moduleBase == 0) {
            std::cerr << "Failed to find module base address\n";
            CloseHandle(hProcess);
            return;
        }

        uintptr_t relativeAddress = std::stoul(address, nullptr, 16);
        uintptr_t absoluteAddress = moduleBase + relativeAddress;

        if (value) {
            if (wasEnabled) return;
            std::cout << "Enabling " << name << std::endl;
            SIZE_T bytesRead;
            BOOL OriginalMemoryReturn = ReadProcessMemory(hProcess, (LPCVOID)absoluteAddress, originalMemory.data(), bytes.size(), &bytesRead);
            if (!OriginalMemoryReturn || bytesRead != bytes.size()) {
                std::cerr << "Failed to read memory\n";
                std::cerr << "Error: " << GetLastError() << "\n";
                std::cerr << "Bytes read: " << bytesRead << "\n";
                CloseHandle(hProcess);
                return;
            }

            SIZE_T bytesWritten;
            BOOL InjectedMemoryReturn = WriteProcessMemory(hProcess, (LPVOID)absoluteAddress, bytes.data(), bytes.size(), &bytesWritten);
            if (!InjectedMemoryReturn || bytesWritten != bytes.size()) {
                std::cerr << "Failed to write memory\n";
                std::cerr << "Error: " << GetLastError() << "\n";
                std::cerr << "Bytes written: " << bytesWritten << "\n";
                CloseHandle(hProcess);
                return;
            }
            wasEnabled = true;
        }
        else {
            if (!wasEnabled) return;
            std::cout << "Disabling " << name << std::endl;
            SIZE_T bytesWritten;

            BOOL ResetMemoryReturn = WriteProcessMemory(hProcess, (LPVOID)absoluteAddress, originalMemory.data(), originalMemory.size(), &bytesWritten);
            if (!ResetMemoryReturn || bytesWritten != originalMemory.size()) {
                std::cerr << "Failed to reset memory\n";
                std::cerr << "Error: " << GetLastError() << "\n";
                std::cerr << "Bytes written: " << bytesWritten << "\n";
                CloseHandle(hProcess);
                return;
            }
            wasEnabled = false;
        }

        enabled = value;
        CloseHandle(hProcess);
    }
}
