#pragma once
#include "../../SDK.h"
#include "../../../engine/Engine.h"
#include "../Hooks.h"
#include "../../Functional/Aimbot.h"
#include "../../Functional/EffectHook.h"
#include "../../Functional/Teams.h"

static CViewSetup save_ViewSetup;

void __fastcall hkRenderView(void*thisptr, void*edx, CViewSetup &view, int nClearFlags, int whatToDraw) {


	Engine::WriteFile("%s ST", __FUNCTION__);



	static Vector newOrigin;
	Teams::Team l_team_info = {};
	if (!g_pEngine->IsInGame()) {
		goto endl;
	}
	if (Render::Screen_Shot || Render::s_timer.Elapsed() < 10000 || g_pEngine->IsTakingScreenshot()) {
		goto endl;
	}
	auto local = C_GMOD_Player::GetLocalPlayer();
	if (!local|| !local->IsGood()) {
		goto endl;
	}

	
	if (!Teams::GetTeamInfo(l_team_info, local))
		return;

	if (Engine::Var::Instance().Chams.Get()) {

		for (int i_Num = 0; i_Num <= g_pEngine->GetMaxClients(); i_Num++) {
			auto pEntity = C_GMOD_Player::GetGPlayer(i_Num);

			if (!pEntity || pEntity == local || !pEntity->IsGood())
				continue;
			player_info_t info;
			if (!g_pEngine->GetPlayerInfo(pEntity->entindex(), &info))
				continue;
			if (Engine::Var::Instance().Esp_Bot_Check.Get())
				if (info.fakeplayer)
					continue;
			Teams::Team team_info = {};
			if (!Teams::GetTeamInfo(l_team_info, pEntity))
				return;

			if (Engine::Var::Instance().Esp_Team_Check.Get()) {
				if (Teams::PlayerWhitelisted(pEntity))
					continue;
			}
			if (pEntity->m_bReadyToDraw()) {
			//	pEntity->m_bReadyToDraw() = false;
			}
			 
		}
	}

	view.fov += Engine::Var::Instance().Fov_changer.Get();
	view.viewmodel_fov += Engine::Var::Instance().Viewmodel_changer.Get();


	Hooks::Variables::curr_fov = view.fov;
	
	if (GetAsyncKeyState(Engine::Var::Instance().Third_Person_Key.Get()) & 1) Hooks::Variables::InThirdPerson ^= 1;
	static bool have_tp = false;
	if (Engine::Var::Instance().Third_Person.Get()&& Hooks::Variables::InThirdPerson) {
		if (local->m_iHealth()) {
			g_pInput->CAM_ToThirdPerson();
			Vector offset;
			view.Angle.ToVector(&offset);

			offset *= -Engine::Var::Instance().Third_Person_Distance.Get();

			Vector start = view.Origin;
			Vector end = view.Origin + offset;
			trace_t Trace;
			Ray_t ray;

			ray.Init(start, end,Vector(-15.f, -15.f, -15.f), Vector(15.f, 15.f, 15.f));
			CTraceFilter traceFilter;
			traceFilter.pSkip = local;
			g_pEngineTrace->TraceRay(ray, MASK_SOLID, &traceFilter, &Trace);

			offset *= Trace.fraction;


			view.Origin += offset;
			have_tp = true;
		}
		else {
			g_pInput->CAM_ToFirstPerson();
			have_tp = false;
		}
	}
	else if (have_tp) {
		g_pInput->CAM_ToFirstPerson();
		have_tp = false;
	}
	if (GetAsyncKeyState(Engine::Var::Instance().Debug_Camera_Key.Get()) & 1) Hooks::Variables::InDebugCam ^= 1;

	if (Engine::Var::Instance().Debug_Camera.Get()&& Hooks::Variables::InDebugCam) {
		float fSpeed = Engine::Var::Instance().Debug_Camera_Speed.Get();
		Vector vForward, vRight, vUp;
		view.Angle = Hooks::Variables::DebugCamAngle;
		view.Angle.ToVector(&vForward, &vRight, &vUp);

		if (Hooks::Variables::OriginalButtons & IN_DUCK)
			fSpeed = fSpeed * 0.45;

		if (Hooks::Variables::OriginalButtons & IN_SPEED)
			fSpeed = fSpeed * 1.65;

		if (Hooks::Variables::OriginalButtons & IN_FORWARD) // W
			newOrigin += vForward * fSpeed;

		if (Hooks::Variables::OriginalButtons & IN_MOVELEFT) // A
			newOrigin -= vRight * fSpeed;

		if (Hooks::Variables::OriginalButtons & IN_MOVERIGHT) // D
			newOrigin += vRight * fSpeed;

		if (Hooks::Variables::OriginalButtons & IN_BACK) // S
			newOrigin -= vForward * fSpeed;

		if (Hooks::Variables::OriginalButtons & IN_JUMP) // Space
			newOrigin += vUp * fSpeed;
		view.Origin = newOrigin;
	}
	else {
		newOrigin = view.Origin;
	}
	if (Engine::Var::Instance().No_Visible_Recoil.Get()) {
		//local;
		//*(QAngle*)((DWORD)(local)+0x22C8) = QAngle();
		//*(QAngle*)((DWORD)(local)+0x2304) = QAngle();
		view.Angle -= local->m_vecPunchAngle(); // Aimbot::Instance().GetRecoil(local);
		//save_Recoil = QAngle();
	}

	if (g_pInput->CAM_IsThirdPerson()&& Engine::Var::Instance().Fake_angle_Chams.Get()) {
		//real model 
		if (Engine::Var::Instance().AntiAim_Fake_Yaw.Get() > 0) {

			local->SetAbsOrigin(local->m_vecOrigin());
			g_pPrediction->SetLocalViewAngles(Hooks::Variables::ChokedAngle);
			auto anim_state = local->PlayerAnimState();
			if (anim_state) {
				//anim_state->Update(Hooks::Variables::ChokedAngle[YAW], Hooks::Variables::ChokedAngle[PITCH]);
				anim_state->m_DebugAnimData.m_flAimPitch = Hooks::Variables::ChokedAngle[PITCH];
				anim_state->m_PoseParameterData.m_flEstimateYaw = Hooks::Variables::ChokedAngle[YAW];
				anim_state->m_DebugAnimData.m_flAimYaw = 0.f;
				anim_state->m_flCurrentFeetYaw = Hooks::Variables::ChokedAngle[YAW];
				anim_state->m_flEyePitch = Hooks::Variables::ChokedAngle[PITCH];
				anim_state->m_flEyeYaw = Hooks::Variables::ChokedAngle[YAW];
				anim_state->m_flGoalFeetYaw = Hooks::Variables::ChokedAngle[YAW];
				anim_state->m_angRender = Hooks::Variables::ChokedAngle;
				anim_state->m_bForceAimYaw = true;
			}
		}
		else if (Engine::Var::Instance().AntiAim_Yaw.Get() >= 0 && Engine::Var::Instance().AntiAim_Pitch.Get() >= 0) {
			local->SetAbsOrigin(local->m_vecOrigin());
			g_pPrediction->SetLocalViewAngles(Hooks::Variables::Angle2Send);
			auto anim_state = local->PlayerAnimState();
			if (anim_state) {
				//anim_state->Update(Hooks::Variables::Angle2Send[YAW], Hooks::Variables::Angle2Send[PITCH]);
				anim_state->m_DebugAnimData.m_flAimPitch = Hooks::Variables::Angle2Send[PITCH];
				anim_state->m_PoseParameterData.m_flEstimateYaw = Hooks::Variables::Angle2Send[YAW];
				anim_state->m_DebugAnimData.m_flAimYaw = 0.f;
				anim_state->m_flCurrentFeetYaw = Hooks::Variables::Angle2Send[YAW];
				anim_state->m_flEyePitch = Hooks::Variables::Angle2Send[PITCH];
				anim_state->m_flEyeYaw = Hooks::Variables::Angle2Send[YAW];
				anim_state->m_flGoalFeetYaw = Hooks::Variables::Angle2Send[YAW];
				anim_state->m_angRender = Hooks::Variables::Angle2Send;
				anim_state->m_bForceAimYaw = true;
			}

		}
		local->UpdateClientSideAnimation();
		local->InvalidateBoneCache();
		
	}
	
	

endl:
	save_ViewSetup = view;

	{
		using RenderView_fn = void (__fastcall* )(void*, void*, CViewSetup&, int , int );
		Hooks::VViewRender.GetOriginalMethod<RenderView_fn>(6)(thisptr, edx, view, nClearFlags, whatToDraw);
	}
	
	Engine::WriteFile("%s END", __FUNCTION__);
}

