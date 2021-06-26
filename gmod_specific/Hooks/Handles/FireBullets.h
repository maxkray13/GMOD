#pragma once

#include "../SDK.h"
#include "../../Functional/ServerAnimations.h"
using FireBullet_fn = void(__fastcall*)(IClientEntity *, int, FireBulletsInfo_t&);
FireBullet_fn oFireBullet_;
void __fastcall hkFireBullet(IClientEntity * ent, int edx, FireBulletsInfo_t& Info) {


	

	if (!g_pEngine->IsInGame() || g_pEngine->IsDrawingLoadingImage())
		return oFireBullet_(ent, edx, Info);


	auto pLocal = C_GMOD_Player::GetLocalPlayer();


	if (!pLocal||!pLocal->IsGood()) {
		return oFireBullet_(ent, edx, Info);
	}
	if (ent == pLocal && Engine::Var::Instance().Aim_NoSpread.Get()) {
		Hooks::Variables::Spread = Info.m_vecSpread;
	}
	/*else if (ent != pLocal && ToGMOD_Player(ToBasePlayer((C_BaseEntity*)ent))) {
		auto player = ToGMOD_Player(ToBasePlayer((C_BaseEntity*)ent));
		auto dir_angle = Info.m_vecDirShooting.ToEulerAngles();
		dir_angle.Normalize();
		player->m_angEyeAngles() = dir_angle;
		if (player->PlayerAnimState()) {
			player->PlayerAnimState()->m_bForceAimYaw = true;
			player->PlayerAnimState()->Update(dir_angle[YAW], dir_angle[PITCH]);
		}
		if (g_ServerAnimations.g_Tracker[player->entindex()].m_PlayerAnimState) {
			g_ServerAnimations.g_Tracker[player->entindex()].m_PlayerAnimState->m_bForceAimYaw = true;
			g_ServerAnimations.g_Tracker[player->entindex()].m_PlayerAnimState->Update(dir_angle[YAW], dir_angle[PITCH]);
		}
	}*/

	return oFireBullet_(ent, edx, Info);
}
