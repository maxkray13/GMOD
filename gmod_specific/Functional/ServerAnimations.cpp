#include "../../include/stdafx.h"
#include "../SDK.h"
#include "ServerAnimations.h"
#include "../Functional/EffectHook.h"
ServerAnimations g_ServerAnimations;

ServerAnimations::ServerAnimations()
{
	ZeroMemory(this, sizeof(ServerAnimations));
}

void ServerAnimations::ClearData()
{
	for (auto it = 0; it < 512; it++) {
		if (g_Tracker[it].m_PlayerAnimState)
			g_pMemAlloc->Free(g_Tracker[it].m_PlayerAnimState);
	}
	ZeroMemory(this, sizeof(ServerAnimations));
}
CBasePlayerAnimState * CreateHL2MPPlayerAnimState(C_GMOD_Player * player) {
	using CreateHL2MPPlayerAnimState_td = CBasePlayerAnimState * (__cdecl*)(C_GMOD_Player * player);
	static CreateHL2MPPlayerAnimState_td CreateHL2MPPlayerAnimState_ = NULL;
	if (!CreateHL2MPPlayerAnimState_) {
		CreateHL2MPPlayerAnimState_ = (CreateHL2MPPlayerAnimState_td)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x83\xEC\x10\xCC\xCC\x8B\xCC\xCC\xCC\xCC\xCC\x8B\xCC\xCC\x8B",//xxxxxxxx????xxxxxxx
			0xCC)));
	}
	return CreateHL2MPPlayerAnimState_(player);
}
void ServerAnimations::ProcessPlayer(C_GMOD_Player * player)
{
	if (!player->IsValid())
		return;
	player->DisableInterpolation();
	auto client_animation_state = player->PlayerAnimState();
	if (!client_animation_state)
		return;

	auto current_track = &g_Tracker[player->entindex()];

	bool allocate = (current_track->m_PlayerAnimState == nullptr);
	bool change = (!allocate) && (player->GetRefEHandle() != current_track->m_ulEntHandle || current_track->m_PlayerAnimState->m_pPlayer != player);
	bool reset = (!allocate && !change) && (player->m_flSpawnTime() != current_track->m_flSpawnTime || player->m_lifeState() != LIFE_ALIVE);//

	//player->m_fEffects() &= ~EF_NODRAW;
	//*(int*)(DWORD(player) + 0x176 + 0x8) = false;//dormant

//	*(int*)(DWORD(player) + 0x48 + 0x8) = 0;//IsServerEntity
	//g_pEngine->Con_NPrintf(0, "m_PlayerAnimState 0x%p", current_track->m_PlayerAnimState);
	if (change) {
		g_pMemAlloc->Free(current_track->m_PlayerAnimState);
		current_track->m_PlayerAnimState = nullptr;
	}
	if (reset)
	{
		current_track->m_PlayerAnimState->ClearAnimationState();
		current_track->m_flSpawnTime = player->m_flSpawnTime();
	}
	if (allocate || change)
	{
		current_track->m_PlayerAnimState = CreateHL2MPPlayerAnimState(player);
		current_track->m_ulEntHandle = player->GetRefEHandle();
		current_track->m_flSpawnTime = player->m_flSpawnTime();
	}
	else if(current_track->m_PlayerAnimState){
		*(DWORD*)current_track->m_PlayerAnimState = *(DWORD*)client_animation_state;//somehow vtable not set properly
		current_track->m_PlayerAnimState->m_pPlayer = current_track->m_PlayerAnimState->m_pHL2MPPlayer = client_animation_state->m_pPlayer;
		static ConVar * host_timescale = nullptr;
		if (!host_timescale) {
			host_timescale = g_pCvar->FindVar(("host_timescale"));
		}

		auto recived_angle = player->m_angEyeAngles();
		recived_angle.Normalize();


		current_track->m_PlayerAnimState->Update(recived_angle[YAW], recived_angle[PITCH]);
		if (current_track->m_LastUpdate < SDK::Vars::g_tick) {
			/*auto sim_ticks = SDK::Vars::g_tick - current_track->m_LastUpdate;
			int counter = 1;
			while (sim_ticks) {
				float OldCurTime = g_pGlobalVars->curtime;
				float OldRealTime = g_pGlobalVars->realtime;
				float OldFrameTime = g_pGlobalVars->frametime;
				float OldAbsFrameTime = g_pGlobalVars->absoluteframetime;
				float OldInterpAmount = g_pGlobalVars->interpolation_amount;
				int OldFrameCount = g_pGlobalVars->framecount;
				int OldTickCount = g_pGlobalVars->tickcount;


				g_pGlobalVars->curtime = TICKS_TO_TIME(current_track->m_LastUpdate + counter);
				g_pGlobalVars->realtime = TICKS_TO_TIME(current_track->m_LastUpdate + counter);
				g_pGlobalVars->frametime = g_pGlobalVars->interval_per_tick * host_timescale->flVal;
				g_pGlobalVars->absoluteframetime = g_pGlobalVars->interval_per_tick * host_timescale->flVal;
				g_pGlobalVars->interpolation_amount = 0;
				g_pGlobalVars->framecount = current_track->m_LastUpdate + counter;
				g_pGlobalVars->tickcount = current_track->m_LastUpdate + counter;
				*/
				
				/*
				g_pGlobalVars->curtime = OldCurTime;
				g_pGlobalVars->realtime = OldRealTime;
				g_pGlobalVars->frametime = OldFrameTime;
				g_pGlobalVars->absoluteframetime = OldAbsFrameTime;
				g_pGlobalVars->interpolation_amount = OldInterpAmount;
				g_pGlobalVars->framecount = OldFrameCount;
				g_pGlobalVars->tickcount = OldTickCount;

				sim_ticks--;
				counter++;
			}*/
		}
		g_pEngine->Con_NPrintf(0, "diff yaw			 %f", current_track->m_PlayerAnimState->m_DebugAnimData.m_flAimYaw - client_animation_state->m_DebugAnimData.m_flAimYaw);
		client_animation_state->m_angRender = current_track->m_PlayerAnimState->m_angRender;//0x0020
		client_animation_state->m_bPoseParameterInit = current_track->m_PlayerAnimState->m_bPoseParameterInit;//0x002C
		client_animation_state->m_PoseParameterData = current_track->m_PlayerAnimState->m_PoseParameterData;//0x0030
		client_animation_state->m_DebugAnimData = current_track->m_PlayerAnimState->m_DebugAnimData;//0x0054
		client_animation_state->m_bCurrentFeetYawInitialized = current_track->m_PlayerAnimState->m_bCurrentFeetYawInitialized;//0x006C
		client_animation_state->m_flLastAnimationStateClearTime = current_track->m_PlayerAnimState->m_flLastAnimationStateClearTime;//0x0070
		client_animation_state->m_flEyeYaw = current_track->m_PlayerAnimState->m_flEyeYaw;//0x0074
		client_animation_state->m_flEyePitch = current_track->m_PlayerAnimState->m_flEyePitch;//0x0078
		client_animation_state->m_flGoalFeetYaw = current_track->m_PlayerAnimState->m_flGoalFeetYaw;//0x007C
		client_animation_state->m_flCurrentFeetYaw = current_track->m_PlayerAnimState->m_flCurrentFeetYaw;//0x0080
		client_animation_state->m_flLastAimTurnTime = current_track->m_PlayerAnimState->m_flLastAimTurnTime;//0x0084
		client_animation_state->m_MovementData = current_track->m_PlayerAnimState->m_MovementData;//0x0088
		client_animation_state->m_bJumping = current_track->m_PlayerAnimState->m_bJumping;//0x0098
		client_animation_state->m_flJumpStartTime = current_track->m_PlayerAnimState->m_flJumpStartTime;//0x009C
		client_animation_state->m_bFirstJumpFrame = current_track->m_PlayerAnimState->m_bFirstJumpFrame;//0x00A0
		client_animation_state->m_bInSwim = current_track->m_PlayerAnimState->m_bInSwim;//0x00A1
		client_animation_state->m_bFirstSwimFrame = current_track->m_PlayerAnimState->m_bFirstSwimFrame;//0x00A2
		client_animation_state->m_bDying = current_track->m_PlayerAnimState->m_bDying;//0x00A3
		client_animation_state->m_bFirstDyingFrame = current_track->m_PlayerAnimState->m_bFirstDyingFrame;//0x00A4
		client_animation_state->m_eCurrentMainSequenceActivity = current_track->m_PlayerAnimState->m_eCurrentMainSequenceActivity;//0x00A8
		client_animation_state->m_nSpecificMainSequence = current_track->m_PlayerAnimState->m_nSpecificMainSequence;//0x00AC
		client_animation_state->m_flLastGroundSpeedUpdateTime = current_track->m_PlayerAnimState->m_flLastGroundSpeedUpdateTime;//0x00B4
		client_animation_state->m_flMaxGroundSpeed = current_track->m_PlayerAnimState->m_flMaxGroundSpeed;//0x00E8
		client_animation_state->m_nMovementSequence = current_track->m_PlayerAnimState->m_nMovementSequence;//0x00EC
		client_animation_state->m_LegAnimType = current_track->m_PlayerAnimState->m_LegAnimType;//0x00F0

		auto model_ptr = player->GetModelPtr();
		if (model_ptr) {
			player->SetPoseParameter(model_ptr, current_track->m_PlayerAnimState->m_PoseParameterData.m_iMoveX, current_track->m_PlayerAnimState->m_DebugAnimData.m_vecMoveYaw.m_x);
			player->SetPoseParameter(model_ptr, current_track->m_PlayerAnimState->m_PoseParameterData.m_iMoveY, current_track->m_PlayerAnimState->m_DebugAnimData.m_vecMoveYaw.m_y);
			player->SetPoseParameter(model_ptr, current_track->m_PlayerAnimState->m_PoseParameterData.m_iAimPitch, current_track->m_PlayerAnimState->m_DebugAnimData.m_flAimPitch);
			player->SetPoseParameter(model_ptr, current_track->m_PlayerAnimState->m_PoseParameterData.m_iAimYaw, current_track->m_PlayerAnimState->m_DebugAnimData.m_flAimYaw);
			
		}

	}

	current_track->m_LastUpdate = SDK::Vars::g_tick;
}
