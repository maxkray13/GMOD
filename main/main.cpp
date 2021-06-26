#include "../include/stdafx.h"

#include "../engine/Engine.h"
#//include "../engine/Core/Memory/Memory.h"
#include "../gmod_specific/Math/core.hpp"

BOOL APIENTRY DllMain(HMODULE hDll, DWORD dwReason, LPVOID lpReserved) {
	DisableThreadLibraryCalls(hDll);
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		Engine::Initialize(hDll);
	}

	return true;
}