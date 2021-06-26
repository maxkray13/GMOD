#pragma once
#include "../Functional/Aimbot.h"
#include "../Functional/MovementRecorder.h"
#include "FireBullets.h"
#include "../Functional/AntiAim.h"
#include "../../engine/Render/Render.h"
#include "../Functional/Dudos.h"
#include "../Functional/Misc.h"
#include "SetView.h"
#include "SendDatagram.h"
#include "SendNetMsg.h"
#include "EyeAngles.h"
#include "SetupBones.h"
#include "PostDataUpdate.h"
#include "../../Functional/LagCompensation.h"
#include "../../Functional/net_logger.h"
#include "../../Functional/ServerAnimations.h"
#include "../../Functional/Teams.h"

ConVar* m_cvar_sensitivity = nullptr;
ConVar* m_cvar_m_pitch = nullptr;
ConVar* m_cvar_m_yaw = nullptr;


using UpdateClientSideAnimation_fn = void(__fastcall*)(void*, void*);
UpdateClientSideAnimation_fn oUpdateClientSideAnimation;



void __fastcall hkUpdateClientSideAnimation(C_GMOD_Player* ecx,void* edx)
{

	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal || !ecx || !pLocal->IsGood() || !ecx->IsGood() || ecx == pLocal)
	{
		return oUpdateClientSideAnimation(ecx, edx);
	}
	float OldCurTime = g_pGlobalVars->curtime;
	float OldRealTime = g_pGlobalVars->realtime;
	float OldFrameTime = g_pGlobalVars->frametime;
	float OldAbsFrameTime = g_pGlobalVars->absoluteframetime;
	float OldInterpAmount = g_pGlobalVars->interpolation_amount;
	int OldFrameCount = g_pGlobalVars->framecount;
	int OldTickCount = g_pGlobalVars->tickcount;
	static ConVar * host_timescale = nullptr;
	if (!host_timescale)
	{
		host_timescale = g_pCvar->FindVar(("host_timescale"));
	}
	float fl_lerp = SDK::GetLerpTime();

	g_pGlobalVars->curtime = TICKS_TO_TIME(SDK::Vars::g_tick) - fl_lerp;
	g_pGlobalVars->realtime = TICKS_TO_TIME(SDK::Vars::g_tick) - fl_lerp;

	//g_pGlobalVars->frametime = 0.f;//g_pGlobalVars->interval_per_tick * host_timescale->flVal;
	//g_pGlobalVars->absoluteframetime = 0.f;//g_pGlobalVars->interval_per_tick * host_timescale->flVal;

	g_pGlobalVars->interpolation_amount = 0;
	g_pGlobalVars->framecount = SDK::Vars::g_tick - TIME_TO_TICKS(fl_lerp);
	g_pGlobalVars->tickcount = SDK::Vars::g_tick - TIME_TO_TICKS(fl_lerp);

	VarMapping_t* map = ecx->GetVarMapping();
	if (map) {
		for (int i = 0; i < map->m_nInterpolatedEntries; i++) {
			VarMapEntry_t *e = &map->m_Entries[i];

			if (e&&e->watcher&&e->watcher->GetDebugName())
			{



				constexpr auto str = "C_BaseAnimating::m_iv_flEncodedController";

				constexpr auto s1 = str[24];


				if (*(DWORD*)&e->watcher->GetDebugName()[28] == 'esoP'|| *(DWORD*)&e->watcher->GetDebugName()[24] == 'lcyC' ||
					*(DWORD*)&e->watcher->GetDebugName()[24] == 'ocnE' ) {

					e->m_bNeedsToInterpolate = false;
				}
				//if (strcmp(e->watcher->GetDebugName(), "C_BaseAnimating::m_iv_flPoseParameter") == 0 || strcmp(e->watcher->GetDebugName(), "C_BaseAnimating::m_iv_flCycle") == 0 ||
				//	strcmp(e->watcher->GetDebugName(), "C_BaseAnimating::m_iv_flEncodedController") == 0
				//	/* ||strcmp(e->watcher->GetDebugName(), "C_BaseEntity::m_iv_angRotation") == 0|| strcmp(e->watcher->GetDebugName(), "C_HL2MP_Player::m_iv_angEyeAngles") == 0*/
				//	) {
				//	e->m_bNeedsToInterpolate = false;
				//}
			}
		}
	}

	oUpdateClientSideAnimation(ecx, edx);


	g_pGlobalVars->curtime = OldCurTime;
	g_pGlobalVars->realtime = OldRealTime;
	g_pGlobalVars->frametime = OldFrameTime;
	g_pGlobalVars->absoluteframetime = OldAbsFrameTime;
	g_pGlobalVars->interpolation_amount = OldInterpAmount;
	g_pGlobalVars->framecount = OldFrameCount;
	g_pGlobalVars->tickcount = OldTickCount;

}






auto AnglesToPixels(const QAngle& angles_begin, const QAngle& angles_end) -> QAngle
{
	if (m_cvar_sensitivity&&m_cvar_m_pitch&&m_cvar_m_yaw) {
		auto sensitivity = m_cvar_sensitivity->flVal;

		auto m_pitch = m_cvar_m_pitch->flVal;
		auto m_yaw = m_cvar_m_yaw->flVal;

		auto angles_distance = angles_begin - angles_end;
		angles_distance.Normalize();

		auto pixel_move_x = (-angles_distance[0]) / (m_yaw * sensitivity);
		auto pixel_move_y = (angles_distance[1]) / (m_pitch * sensitivity);

		return { pixel_move_y, pixel_move_x, 0.f };
	}
	else {
		m_cvar_sensitivity = g_pCvar->FindVar("sensitivity");

		m_cvar_m_pitch = g_pCvar->FindVar("m_pitch");

		m_cvar_m_yaw = g_pCvar->FindVar("m_yaw");
	}

	return { 0.f , 0.f , 0.f };
}

auto PixelsDistanceToAnglesDistance(const QAngle& pixels_distance) -> QAngle
{
	if (m_cvar_sensitivity&&m_cvar_m_pitch&&m_cvar_m_yaw) {
		auto sensitivity = m_cvar_sensitivity->flVal;

		auto m_pitch = m_cvar_m_pitch->flVal;
		auto m_yaw = m_cvar_m_yaw->flVal;

		auto angle_move_x = (-pixels_distance[0]) * (m_yaw * sensitivity);
		auto angle_move_y = (pixels_distance[1]) * (m_pitch * sensitivity);

		return { angle_move_y, angle_move_x, 0.f };
	}
	else {
		m_cvar_sensitivity = g_pCvar->FindVar("sensitivity");

		m_cvar_m_pitch = g_pCvar->FindVar("m_pitch");

		m_cvar_m_yaw = g_pCvar->FindVar("m_yaw");
	}
	return { 0.f , 0.f , 0.f };
}
bool __fastcall hkCreateMove(void*thisptr, void*edx, float flInputSampleTime, CUserCmd* pCmd) {

	Engine::WriteFile("%s END", __FUNCTION__);

	if (pCmd->command_number == 0 || pCmd->weaponselect != 0 || IsBadReadPtr(pCmd, 0x4))
		goto endlF;
	if (!g_pEngine->IsInGame() || g_pEngine->IsDrawingLoadingImage())
		goto endlF;
	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal || !pLocal->IsGood())
		goto endlF;

	PVOID pebp;
	__asm mov pebp, ebp;
	bool* pbSendPacket = (bool*)((***(DWORD***)pebp) - 0x1);
	bool& bSendPacket = *pbSendPacket;
	
	bool sh_skip = Engine::Var::Instance().CurrStateMovementRecorder.Get() == PLAYBACKING && Engine::Var::Instance().Debug_Camera.Get() && Hooks::Variables::InDebugCam;
	Hooks::Variables::OriginalButtons = pCmd->buttons;
	if (Engine::Var::Instance().Debug_Camera.Get() && Hooks::Variables::InDebugCam) {
		pCmd->buttons = NULL;
		pCmd->forwardmove = 0.f;
		pCmd->sidemove = 0.f;
		pCmd->upmove = 0.f;
		Hooks::Variables::DebugCamAngle[YAW] = (-1 * pCmd->mousedx * 0.022f) + pCmd->viewangles[YAW];
		Hooks::Variables::DebugCamAngle[PITCH] = (pCmd->mousedy* 0.022f) + pCmd->viewangles[PITCH];
		pCmd->mousedx = pCmd->mousedy = 0;

		pCmd->viewangles = Hooks::Variables::FinalAngle;
		if (!sh_skip)
			goto endl;

	}
	bSendPacket = true;
	Teams::ProcessTeams();
	Misc::Instance().Misc_Tick(pCmd);
	if (Engine::Var::Instance().Auto_Strafe.Get()) {
		Misc::Instance().AutoStrafe(pCmd, pLocal);//&& !(pCmd->buttons & IN_ATTACK)
	}
	if (pLocal->GetActiveWeapon()) {
		auto weap = pLocal->GetActiveWeapon();

		if (Engine::Var::Instance().Aim_AutoPistol.Get() && weap->GetFireRate() > 0.0 && weap->m_iClip1() > 0) {
			if (pLocal->WeaponAbleToShot() && pCmd->buttons&IN_ATTACK) {
				//	rand() % 2 ? pCmd->buttons &= ~IN_ATTACK :
				//		pCmd->buttons &= IN_ATTACK;
			}
			else {
				pCmd->buttons &= ~IN_ATTACK;
			}
		}

	}
	if (!SDK::Feature::PreEnginePrediction(pCmd))
		goto endl;

	g_MovementRecorder.PlaybackCB(pLocal, pCmd);
	Hooks::Variables::PureAngle = pCmd->viewangles;
	if (sh_skip)
		goto endl;

	if (!Hooks::Variables::g_pLastCmd || IsBadReadPtr(Hooks::Variables::g_pLastCmd, 0x4) || Hooks::Variables::g_pLastCmd->hasbeenpredicted) {

		SDK::Vars::g_tick = pLocal->m_nTickBase();// TIME_TO_TICKS(nci->GetLatency(FLOW_INCOMING)) + pLocal->m_nTickBase();
	}
	else {
		++SDK::Vars::g_tick;
	}
	Hooks::Variables::g_pLastCmd = pCmd;
	if (Engine::Var::Instance().Aim_LagCompensation.Get()) {
		g_LagCompensation.remove_bad_ticks(pCmd);
	}
	//Engine::Var::Instance().Fake_Lag.Get() = 14;
	//Engine::Var::Instance().Fake_Lag.Get() = 20;
	//auto nci = g_pEngine->GetNetChannelInfo();
	//if (nci) {
		//NET_SetConVar conv = NET_SetConVar("name","\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\fо");
		//nci->SendNetMsg(conv);
	//}
	
	//g_ServerAnimations.ProcessPlayer()
	

	if (Engine::Var::Instance().Aim_Enable.Get()) {


		if (Engine::Var::Instance().Aim_If_Can_Shot.Get()) {
			if (pLocal->GetActiveWeapon()) {
				if (!pLocal->WeaponAbleToShot())
					goto Out_Aimbot_Stub;
			}
		}

		if (Engine::Var::Instance().Aim_Key.Get() > 0) {
			if (GetAsyncKeyState(Engine::Var::Instance().Aim_Key.Get())) {
				goto Aimbot_Stub;
			}
			else {
				Aimbot::Instance().Drop_Target();
			}
		}
		else if (Engine::Var::Instance().Aim_Key.Get() == 0)
			goto Aimbot_Stub;
		else goto Out_Aimbot_Stub;
	Aimbot_Stub:;
		for (int it = 0; it <= g_pEngine->GetMaxClients(); it++) {//it < g_pEntList->GetMaxEntities()
			auto ply = C_GMOD_Player::GetGPlayer(it);
			if (!ply || !ply->IsGood() || ply == pLocal)
				continue;
			player_info_t info;
			if (!g_pEngine->GetPlayerInfo(it, &info))
				continue;
			if (Engine::Var::Instance().Aim_Bot_Check.Get())
				if (info.fakeplayer)
					continue;
			if (Engine::Var::Instance().Aim_Team_Check.Get()) {
				if(Teams::PlayerWhitelisted(ply))
					continue;
			}
			Aimbot::Instance().AimBot_Tick(ply, pCmd);
		}
		/*
		int lTeam;
		if (Lua::GetTeam(g_pEngine->GetLocalPlayer()) == -1337) {
			lTeam = -1337;
			goto Out_Aimbot_Stub;
		}
		lTeam = Lua::GetTeamColor(Lua::GetTeam(g_pEngine->GetLocalPlayer())).GetRawColor();
		for (int it = 0; it <= g_pEngine->GetMaxClients(); it++) {//it < g_pEntList->GetMaxEntities()
			auto ply = C_GMOD_Player::GetGPlayer(it);
			if (!ply || !ply->IsGood() || ply == pLocal)
				continue;
			player_info_t info;
			if (!g_pEngine->GetPlayerInfo(it, &info))
				continue;
			if (Engine::Var::Instance().Aim_Bot_Check.Get())
				if (info.fakeplayer)
					continue;
			if (Engine::Var::Instance().Aim_Team_Check.Get()) {
				if (lTeam == -1337 || Lua::GetTeam(it) == -1337) { continue; }
				else if (lTeam == Lua::GetTeamColor(Lua::GetTeam(it)).GetRawColor())
					continue;
			}
			Aimbot::Instance().AimBot_Tick(ply, pCmd);
		}*/
		Aimbot::Instance().Post_AimBot_Tick(pCmd, bSendPacket);
		pCmd->viewangles.Normalize();
	Out_Aimbot_Stub:;
	}

	if (!Engine::Var::Instance().Aim_Silent.Get()) {
		using SetViewAngles_fn = decltype(&hkSetViewAngles);
		Hooks::VEngineHook.GetOriginalMethod<SetViewAngles_fn>(20)(thisptr, edx, pCmd->viewangles);
	}


	Hooks::Variables::FinalAngle = pCmd->viewangles;



	if (pLocal->GetActiveWeapon()) {

		if (!Engine::Var::Instance().Randomize.Get()) {
			if (pLocal->WeaponAbleToShot())
			{
				if (pCmd->buttons & IN_ATTACK &&Engine::Var::Instance().Aim_NoRecoil.Get()
					&& strcmp("CWeaponPistol", pLocal->GetActiveWeapon()->GetClientClass()->GetName())) {
					if (pLocal->GetActiveWeapon()->GetScriptName()) {
						std::string sc_n = pLocal->GetActiveWeapon()->GetScriptName();
						if (sc_n.find("m9k") != -1) {
							/*auto uiSeed = MD5_PseudoRandom(pCmd->command_number) & 0x7FFFFFFF;
							SDK::Function::RandomSeed(uiSeed & 255);
							using t = void(__thiscall*)(void*);
							((t)(SDK::Client.GetStartAddr() + 0x9EAA0))(pLocal->GetActiveWeapon());*/
							//pLocal->UpdateButtonState(pCmd->buttons);
							//pLocal->RunPostThink();
							//if (Engine::Var::Instance().Aim_pSilent.Get())bSendPacket = false;
							//QAngle Recoil = Aimbot::Instance().GetRecoil(pCmd, pLocal);
							//pCmd->viewangles += Recoil;
						}
						else {
							if (Engine::Var::Instance().Aim_pSilent.Get())bSendPacket = false;
							QAngle Recoil = Aimbot::Instance().GetRecoil(pCmd, pLocal);
							pCmd->viewangles += Recoil;
						}
					}
					else {
						if (Engine::Var::Instance().Aim_pSilent.Get())bSendPacket = false;
						QAngle Recoil = Aimbot::Instance().GetRecoil(pCmd, pLocal);
						pCmd->viewangles += Recoil;
					}

				}
				if (pCmd->buttons & IN_ATTACK && Engine::Var::Instance().Aim_NoSpread.Get()) {
					if (Engine::Var::Instance().Aim_pSilent.Get())bSendPacket = false;
					QAngle Sp = Aimbot::Instance().GetSpread(pCmd);
					pCmd->viewangles += Sp;
				}
			}
		}
		else {
			{
				if (pCmd->buttons & IN_ATTACK &&Engine::Var::Instance().Aim_NoRecoil.Get()
					&& strcmp("CWeaponPistol", pLocal->GetActiveWeapon()->GetClientClass()->GetName())) {
					if (pLocal->GetActiveWeapon()->GetScriptName()) {
						std::string sc_n = pLocal->GetActiveWeapon()->GetScriptName();
						if (sc_n.find("m9k") != -1) {}
						else {
							if (Engine::Var::Instance().Aim_pSilent.Get())bSendPacket = false;
							QAngle Recoil = Aimbot::Instance().GetRecoil(pCmd, pLocal);
							pCmd->viewangles += Recoil;
						}
					}
					else {
						if (Engine::Var::Instance().Aim_pSilent.Get())bSendPacket = false;
						QAngle Recoil = Aimbot::Instance().GetRecoil(pCmd, pLocal);
						pCmd->viewangles += Recoil;
					}

				}
				if (pLocal->WeaponAbleToShot())
				{
					if (pCmd->buttons & IN_ATTACK && Engine::Var::Instance().Aim_NoSpread.Get()) {
						if (Engine::Var::Instance().Aim_pSilent.Get())bSendPacket = false;
						QAngle Sp = Aimbot::Instance().GetSpread(pCmd);
						pCmd->viewangles += Sp;
					}
				}
			}
		}

		if (Engine::Var::Instance().Aim_Auto_Reload.Get()) {
			if (!pLocal->GetActiveWeapon()->m_iClip1()) {
				pCmd->buttons &= ~IN_ATTACK;
				pCmd->buttons |= IN_RELOAD;
			}
		}
	}
	if (pLocal->GetMoveType() != MOVETYPE_LADDER && pLocal->GetMoveType() != MOVETYPE_NOCLIP) {
		AntiAim::Instance().AntiAim_Tick(pCmd, pbSendPacket);
	}
	if (pLocal->m_iHealth() > 0) {

		[](CUserCmd* pCmd)->void {

			static ConVar * cl_forwardspeed = nullptr;
			static ConVar * cl_sidespeed = nullptr;
			static ConVar * cl_upspeed = nullptr;

			if (cl_forwardspeed&&cl_sidespeed&&cl_upspeed) {
				if (pCmd->forwardmove >= cl_forwardspeed->flVal) pCmd->forwardmove = cl_forwardspeed->flVal;
				if (pCmd->sidemove >= cl_sidespeed->flVal) pCmd->sidemove = cl_sidespeed->flVal;
				if (pCmd->upmove >= cl_upspeed->flVal) pCmd->upmove = cl_upspeed->flVal;
				if (pCmd->forwardmove <= -cl_forwardspeed->flVal) pCmd->forwardmove = -cl_forwardspeed->flVal;
				if (pCmd->sidemove <= -cl_sidespeed->flVal) pCmd->sidemove = -cl_sidespeed->flVal;
				if (pCmd->upmove <= -cl_upspeed->flVal) pCmd->upmove = -cl_upspeed->flVal;

			}
			else {
				cl_forwardspeed = g_pCvar->FindVar("cl_forwardspeed");
				cl_sidespeed = g_pCvar->FindVar("cl_sidespeed");
				cl_upspeed = g_pCvar->FindVar("cl_upspeed");
			}


			Vector PureForward, PureRight, PureUp, CurrForward, CurrRight, CurrUp;
			Hooks::Variables::PureAngle.ToVector(&PureForward, &PureRight, &PureUp);

			auto viewangl = pCmd->viewangles;
			auto _k_flMaxEntityEulerAngle = 0x48AFC800;
			float * fl_MaxEntityEulerAngle = (float*)&_k_flMaxEntityEulerAngle;
			if (fabs(viewangl[YAW]) >= *fl_MaxEntityEulerAngle) {
				viewangl[YAW] = 0.f;
			}
			viewangl.ToVector(&CurrForward, &CurrRight, &CurrUp);
			PureForward[2] = PureRight[2] = CurrForward[2] = CurrRight[2] = 0.f;

			auto VectorNormalize = [](Vector& vec)->float {
				float radius = sqrtf(vec.m_x*vec.m_x + vec.m_y*vec.m_y + vec.m_z*vec.m_z);
				float iradius = 1.f / (radius + FLT_EPSILON);

				vec.m_x *= iradius;
				vec.m_y *= iradius;
				vec.m_z *= iradius;

				return radius;
			};
			VectorNormalize(PureForward);
			VectorNormalize(PureRight);
			VectorNormalize(CurrForward);
			VectorNormalize(CurrRight);
			Vector PureWishDir;
			for (auto i = 0u; i < 2; i++)
				PureWishDir[i] = PureForward[i] * pCmd->forwardmove + PureRight[i] * pCmd->sidemove;
			PureWishDir[2] = 0.f;

			Vector CurrWishDir;
			for (auto i = 0u; i < 2; i++)
				CurrWishDir[i] = CurrForward[i] * pCmd->forwardmove + CurrRight[i] * pCmd->sidemove;
			CurrWishDir[2] = 0.f;

			pCmd->forwardmove = (PureWishDir.m_x*CurrRight.m_y - CurrRight.m_x*PureWishDir.m_y) / (CurrRight.m_y*CurrForward.m_x - CurrRight.m_x*CurrForward.m_y);
			pCmd->sidemove = (PureWishDir.m_y*CurrForward.m_x - CurrForward.m_y*PureWishDir.m_x) / (CurrRight.m_y*CurrForward.m_x - CurrRight.m_x*CurrForward.m_y);
			pCmd->upmove = 0.f;

			if (cl_forwardspeed&&cl_sidespeed&&cl_upspeed) {
				if (pCmd->forwardmove >= cl_forwardspeed->flVal) pCmd->forwardmove = cl_forwardspeed->flVal;
				if (pCmd->sidemove >= cl_sidespeed->flVal) pCmd->sidemove = cl_sidespeed->flVal;
				if (pCmd->upmove >= cl_upspeed->flVal) pCmd->upmove = cl_upspeed->flVal;
				if (pCmd->forwardmove <= -cl_forwardspeed->flVal) pCmd->forwardmove = -cl_forwardspeed->flVal;
				if (pCmd->sidemove <= -cl_sidespeed->flVal) pCmd->sidemove = -cl_sidespeed->flVal;
				if (pCmd->upmove <= -cl_upspeed->flVal) pCmd->upmove = -cl_upspeed->flVal;




				//if (Engine::Var::Instance().Randomize.Get()) {
				if (pCmd->forwardmove >= cl_forwardspeed->flVal - 20.f&&pCmd->forwardmove != cl_forwardspeed->flVal) {
					pCmd->forwardmove = cl_forwardspeed->flVal;
				}
				if (pCmd->sidemove >= cl_sidespeed->flVal - 20.f&&pCmd->sidemove != cl_sidespeed->flVal) {
					pCmd->sidemove = cl_sidespeed->flVal;
				}

				if (pCmd->forwardmove <= -cl_forwardspeed->flVal + 20.f&&pCmd->forwardmove != -cl_forwardspeed->flVal) {
					pCmd->forwardmove = -cl_forwardspeed->flVal;
				}
				if (pCmd->sidemove <= -cl_sidespeed->flVal + 20.f&&pCmd->sidemove != -cl_sidespeed->flVal) {
					pCmd->sidemove = -cl_sidespeed->flVal;
				}
				//}
				//if (Engine::Var::Instance().Special_movefix.Get()) {
				pCmd->sidemove = std::round(pCmd->sidemove);
				pCmd->forwardmove = std::round(pCmd->forwardmove);
				//}
			}
			else {
				cl_forwardspeed = g_pCvar->FindVar("cl_forwardspeed");
				cl_sidespeed = g_pCvar->FindVar("cl_sidespeed");
				cl_upspeed = g_pCvar->FindVar("cl_upspeed");
			}



		}(pCmd);
	}
	
	
	if (Engine::Var::Instance().Freeze_Players.Get()) {
		CLC_VoiceData voice;
		static byte buff[0xFFFF];
		static byte buff2[0xFFFF];
		static bf_write MethodOneTap;

		if (MethodOneTap.m_iCurBit == 0) {
			ZeroMemory(buff, sizeof(buff));
			MethodOneTap = bf_write("CNetChan_TransmitBits->send", buff, sizeof(buff));

			voice.m_DataOut = bf_write("CNetChan_TransmitBits->send", buff2, sizeof(buff2));
			for (auto kk = 0; kk < 0x1000; kk++) {
				voice.m_DataOut.WriteByte(rand() % 0xFF);
			}
			voice.m_DataOut.SeekToBit(0x1000);
			voice.m_nLength = 0x1000;
			voice.WriteToBuffer(MethodOneTap);
		}
		auto nci = g_pEngine->GetNetChannelInfo();
		if (nci) {
			for (auto it = 0; it < 2; it++) {
				 nci->SendDatagram(&MethodOneTap);
			}
		}
	}

	g_MovementRecorder.RecordCB(pLocal, pCmd);

	Dudos::Instance().Dudos_tick();
	net_logger::ProcessSend();


	if (Render::ShowMenu)
		pCmd->buttons &= ~IN_ATTACK;

	SDK::Feature::PostEnginePrediction();

	static bool ShoulDo = true;
	if (g_pEngine->GetNetChannelInfo() && ShoulDo) {
		auto ptr = (*(DWORD*)pLocal) + 0x40;

		if (!IsBadReadPtr((void*)ptr, 4)) {

			DWORD OldProtection;
			VirtualProtect((void*)ptr, sizeof(ptr), PAGE_READWRITE, &OldProtection);

			oFireBullet_ = (FireBullet_fn) *(DWORD*)ptr;
			*(DWORD*)ptr = (DWORD)hkFireBullet;

			VirtualProtect((void*)ptr, sizeof(ptr), OldProtection, &OldProtection);


			ptr = (*(DWORD*)(DWORD(pLocal) + 0x4)) + 0x40;
			VirtualProtect((void*)ptr, sizeof(ptr), PAGE_READWRITE, &OldProtection);

			oSetupBones_ = (SetupBones_fn) *(DWORD*)ptr;
			*(DWORD*)ptr = (DWORD)hkSetupBones;


			VirtualProtect((void*)ptr, sizeof(ptr), OldProtection, &OldProtection);


			auto nci = g_pEngine->GetNetChannelInfo();
			ptr = (*(DWORD*)nci) + 0xB8;

			VirtualProtect((void*)ptr, sizeof(ptr), PAGE_READWRITE, &OldProtection);
			o_SendDatagram = (uint32_t) *(DWORD*)ptr;
			*(DWORD*)ptr = (DWORD)hkSendDatagram;
			VirtualProtect((void*)ptr, sizeof(ptr), OldProtection, &OldProtection);


			nci = g_pEngine->GetNetChannelInfo();
			ptr = (*(DWORD*)nci) + 0xA0;

			VirtualProtect((void*)ptr, sizeof(ptr), PAGE_READWRITE, &OldProtection);
			o_SendNetMsg = (SendNetMsg_td) *(DWORD*)ptr;
			*(DWORD*)ptr = (DWORD)hkSendNetMsg;
			VirtualProtect((void*)ptr, sizeof(ptr), OldProtection, &OldProtection);
			//0xA0

			//ptr = (*(DWORD*)(DWORD(pLocal) + 0x8)) + 0x1C;//PostDataUpdate
			//VirtualProtect((void*)ptr, sizeof(ptr), PAGE_READWRITE, &OldProtection);
			//oPostDataUpdate_ = (PostDataUpdate_fn) *(DWORD*)ptr;
			//*(DWORD*)ptr = (DWORD)hkPostDataUpdate;
			//VirtualProtect((void*)ptr, sizeof(ptr), OldProtection, &OldProtection);

			auto ptr = (*(DWORD*)pLocal) + 0x3AC;
			VirtualProtect((void*)ptr, sizeof(ptr), PAGE_READWRITE, &OldProtection);
			oUpdateClientSideAnimation = (UpdateClientSideAnimation_fn) *(DWORD*)ptr;
			*(DWORD*)ptr = (DWORD)hkUpdateClientSideAnimation;
			VirtualProtect((void*)ptr, sizeof(ptr), OldProtection, &OldProtection);
			//auto ptr = (*(DWORD*)pLocal) + 0x3AC;

			/*char buff_a[260];
			sprintf_s(buff_a, "prt is 0x%p", ptr);
			MessageBox(NULL, buff_a, NULL, NULL);*/

			ShoulDo = false;
		}
	}
endl:;
	if (*pbSendPacket == true) {
		Hooks::Variables::Angle2Send = pCmd->viewangles;
		Hooks::Variables::PositionWSend = pLocal->m_vecOrigin();
	}
	else {
		Hooks::Variables::ChokedAngle = pCmd->viewangles;
	}
	Hooks::Variables::PostFinalAngle = pCmd->viewangles;

	if (Hooks::Variables::PureAngle != pCmd->viewangles) {
		static QAngle pixels;
		if (pCmd->mousedx == 0) {
			pCmd->mousedx = 10;
		}
		if (pCmd->mousedy == 0) {
			pCmd->mousedy = 10;
		}

	}

	/*if (pCmd && GetAsyncKeyState('V') & 0x8000) {
		//*(int*)(pCmd + 0x38) = 7015681;

		//0x38; 7015681;
		*(int*)(pCmd + 0x3C) = 0x1000000;

		*(Vector*)(pCmd + 0x40) = QAngle(0.f, 0.f, 0.f).ToVector();

		//Engine::msg_box("pCmd is 0x%p", pCmd);
	}*/

endlF:;

	static ConVar * host_timescale = nullptr;
	if (!host_timescale) {
		host_timescale = g_pCvar->FindVar(("host_timescale"));
	}

	float OldFrameTime = g_pGlobalVars->frametime;
	float OldAbsFrameTime = g_pGlobalVars->absoluteframetime;

	g_pGlobalVars->frametime = g_pGlobalVars->interval_per_tick * host_timescale->flVal;
	g_pGlobalVars->absoluteframetime = g_pGlobalVars->interval_per_tick * host_timescale->flVal;




	using CreateMove_fn = bool(__fastcall*)(void*, void*, float, CUserCmd*);
	Hooks::VClientMode.GetOriginalMethod<CreateMove_fn>(21)(thisptr, edx, flInputSampleTime, pCmd);

	g_pGlobalVars->frametime = OldFrameTime;
	g_pGlobalVars->absoluteframetime = OldAbsFrameTime;


	Engine::WriteFile("%s END", __FUNCTION__);
	return false;
}
