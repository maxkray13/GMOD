#pragma once
#include "../Functional/Esp1.h"
#include "../SDK.h"
#include "../../Functional/LagCompensation.h"
#include "../../Functional/ServerAnimations.h"

void __fastcall hkFrameStageNotify(void* ecx, void*edx, ClientFrameStage_t curStage) {

	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal || !pLocal->IsGood())
		return Hooks::VClient.GetOriginalMethod<void(__thiscall*)(void*, ClientFrameStage_t)>(35)(ecx, curStage);


	for (int it = 0; it <= g_pEngine->GetMaxClients(); it++) {//it < g_pEntList->GetMaxEntities()
		auto ply = C_GMOD_Player::GetGPlayer(it);
		if (!ply || !ply->IsValid() || ply == pLocal)
			continue;
		
		//static bool st = true;
		//if (st) {
		/*VarMapping_t* map = ply->GetVarMapping();
		if (map) {
			for (int i = 0; i < map->m_nInterpolatedEntries; i++) {
				VarMapEntry_t *e = &map->m_Entries[i];
				if (e&&e->watcher&&e->watcher->GetDebugName()) {
					if (strcmp(e->watcher->GetDebugName(), "C_BaseAnimating::m_iv_flPoseParameter") == 0 || strcmp(e->watcher->GetDebugName(), "C_BaseAnimating::m_iv_flCycle") == 0 ||
						strcmp(e->watcher->GetDebugName(), "C_BaseAnimating::m_iv_flEncodedController") == 0 || strcmp(e->watcher->GetDebugName(), "C_HL2MP_Player::m_iv_angEyeAngles") == 0) {
						e->m_bNeedsToInterpolate = false;
					}
				}
			}
		}*/
		//	st = false;
		//}

		auto recived_angle = ply->m_angEyeAngles();
		auto AngleNormalize = [](float angle) ->float
		{
			angle = fmodf(angle, 360.0f);
			if (angle > 180)
			{
				angle -= 360;
			}
			if (angle < -180)
			{
				angle += 360;
			}
			return angle;
		};
		auto m_flEyeYaw = AngleNormalize(recived_angle[YAW]);
		auto m_flEyePitch = AngleNormalize(recived_angle[PITCH]);
		float flPitch = m_flEyePitch;
		while (flPitch > 180.0f)
		{
			flPitch -= 360.0f;
		}
		while (flPitch < -180.0f)
		{
			flPitch += 360.0f;
		}
		flPitch = clamp(flPitch, -90.f, 90.f);
		ply->m_angEyeAngles() = QAngle(flPitch, m_flEyeYaw, 0.f);
		/*if (curStage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_END) {
			ply->m_bClientSideAnimation() = true;
			//ply->UpdateClientSideAnimation();
			//Hooks::VClient.GetOriginalMethod<void(__thiscall*)(void*, ClientFrameStage_t)>(35)(ecx, curStage);
			ply->PlayerAnimState()->Update(m_flEyeYaw, flPitch);
			ply->m_bClientSideAnimation() = false;
		}*/
		ply->PlayerAnimState();
		//g_ServerAnimations.ProcessPlayer(ply);
		//g_LagCompensation.PostDataUpdate(ply);
	}

	return Hooks::VClient.GetOriginalMethod<void(__thiscall*)(void*, ClientFrameStage_t)>(35)(ecx, curStage);
}