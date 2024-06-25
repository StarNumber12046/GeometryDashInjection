#include "cheat.h"
#include "Memory.h"

static bool RSHIFT_PRESSED = false;

static Cheats::Cheat NoClipCheat{"placeholder", "placeholder", "NoClip_placeholder"};
static Cheats::Cheat iBypass_1{ "placeholder", "placeholder", "iBypass_1" };
static Cheats::Cheat cBypass_1{ "placeholder", "placeholder", "cBypass_1" };

void setNoClipClass(Cheats::Cheat c) {
	NoClipCheat = c;
}

void toggleNoClipEnabled(bool v) {
	NoClipCheat.set(v);
}

bool getNoClipEnabled() {
	return NoClipCheat.enabled;
}


bool getiBypassEnabled()
{
	return iBypass_1.enabled;
}

void setIconBypassClass(Cheats::Cheat c)
{
	iBypass_1 = c;
}

void toggleIconBypass(bool v)
{
	iBypass_1.set(v);

}

void setColorBypassClass(Cheats::Cheat c)
{
	cBypass_1 = c;
}


void toggleColorBypass(bool v) {
	cBypass_1.set(v);
}

bool getcBypassEnabled()
{
	return cBypass_1.enabled;
}

void set_RSHIFT(bool pressed) {
	RSHIFT_PRESSED = pressed;
}

bool get_RSHIFT_PRESSED() {
	return RSHIFT_PRESSED;
}