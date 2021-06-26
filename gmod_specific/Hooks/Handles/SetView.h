#pragma once

#include "../../SDK.h"
#include "../../../engine/Engine.h"
#include "../../../engine/Core/Memory/Memory.h"
#include "../Hooks.h"

void __fastcall hkSetViewAngles(void* thisptr,void* edx, QAngle& va) {


	Engine::WriteFile("%s ST", __FUNCTION__);
	static DWORD Legit1 = 0;
	if (!Legit1) {
		Legit1 = (DWORD)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x76\x05\xF3\x0F\x11\x4D\xCC\x8B\x0D",//xxxxxx?xx
			0xCC);
		Legit1 += 0x16;
	}

	static DWORD Legit2 = 0;
	if (!Legit2) {
		Legit2 = (DWORD)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x5F\x5E\x5B\x85\xC0\x74\x3C",//xxxxxx?xx
			0xCC);
		Legit2 += 0x52;
	}

	static DWORD LUASHITMODIFICATION = 0;
	if (!LUASHITMODIFICATION) {
		LUASHITMODIFICATION = (DWORD)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x74\x0F\x8B\x0D\xCC\xCC\xCC\xCC\x8D\x55\xF4",//xxxx????xxx
			0xCC);
		LUASHITMODIFICATION += 0x11;
	}

	if (Engine::Var::Instance().Aim_NoRecoil.Get() || Engine::Var::Instance().No_Visible_Recoil.Get()) {
		if ((DWORD)_ReturnAddress() != Legit1 && (DWORD)_ReturnAddress() != Legit2) {
			return;
		}
	}
	Engine::WriteFile("%s END", __FUNCTION__);
	using SetViewAngles_fn = void (__fastcall *)(void* , void* , QAngle& );
	return Hooks::VEngineHook.GetOriginalMethod<SetViewAngles_fn>(20)(thisptr,edx,va);
}