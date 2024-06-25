#pragma once
#include "cheat.h"

void setNoClipClass(Cheats::Cheat c);
void toggleNoClipEnabled(bool v);
bool getNoClipEnabled();

bool getiBypassEnabled();
void setIconBypassClass(Cheats::Cheat c);
void toggleIconBypass(bool v);

void setColorBypassClass(Cheats::Cheat c);
void toggleColorBypass(bool v);
bool getcBypassEnabled();

void set_RSHIFT(bool pressed);
bool get_RSHIFT_PRESSED();
