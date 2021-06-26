#pragma once
void __fastcall hkRunCommand(void* thisptr,void* edx,IClientEntity * player, CUserCmd *ucmd, IMoveHelper *moveHelper) {

	using RunCommand_fn = void(__thiscall *)(void*, IClientEntity *, CUserCmd *, IMoveHelper *);

	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal || !pLocal->IsGood()) {
		return Hooks::VPrediction.GetOriginalMethod<RunCommand_fn>(17)(thisptr, player, ucmd, moveHelper);
	}

	if (pLocal == player) {
		return;
	}
	return Hooks::VPrediction.GetOriginalMethod<RunCommand_fn>(17)(thisptr, player, ucmd, moveHelper);
}