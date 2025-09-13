#include "core.h"

void c_core::start()
{
	while (!Memory::FindSignature("client.dll", "48 8B 0D ? ? ? ? 48 8B 10 48 8B 19 48 8B C8 FF 92"))
	{
		unload = GetAsyncKeyState(VK_F11) & 0x8000;
		if (unload)
			return;

		Sleep(500);
	}

	U::SignatureManager->InitializeAllSignatures();
	U::InterfaceManager->InitializeAllInterfaces();
	U::HookManager->InitializeAllHooks();

	// init your stuff here

	I::CVar->ConsoleColorPrintf({ 255, 255, 255, 255 }, "loaded\n");
	I::MatSystemSurface->PlaySound("buttons\\button10.wav");
}

void c_core::loop()
{
	while (true)
	{
		bool bShouldUnload = GetAsyncKeyState(VK_F11) & 0x8000 && sdk::isgamewindowinfocus() || unload;
		if (bShouldUnload)
			break;

		Sleep(50);
	}
}

void c_core::shutdown()
{
	if (!unload)
	{
		U::HookManager->FreeAllHooks();

		// uninit your stuff here
	}

	I::CVar->ConsoleColorPrintf({ 255, 255, 255, 255 }, "unloaded\n");
	I::MatSystemSurface->PlaySound("buttons\\button10.wav");
}