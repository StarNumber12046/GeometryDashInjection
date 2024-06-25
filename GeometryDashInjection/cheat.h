#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <Windows.h>

namespace Cheats {
    class Cheat {
    public:
        // Constructor
        Cheat(const std::string& addr, const std::string& payl, const std::string& n);

        // Destructor
        virtual ~Cheat();

        // Function to toggle the cheat
        void set(bool value);

        // Members
        std::string address;
        std::string payload;
        std::string name;
        bool enabled;

    private:
        std::vector<uint8_t> bytes;
        std::vector<uint8_t> originalMemory;
        bool wasEnabled = false;
    };
}
