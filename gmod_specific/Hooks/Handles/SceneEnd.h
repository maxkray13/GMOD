#pragma once
#include "../../Functional/LagCompensation.h"
#include "../../Functional/EffectHook.h"
#include "../../Functional/Teams.h"


#define GetOriginValue(name) \
DWORD o_##name;\
Render::g_pDevice->GetRenderState(name, &o_##name);


#define SetOriginValue(name) Render::g_pDevice->SetRenderState(name, o_##name);


void __fastcall hkSceneEnd(void*thisptr, void*edx) {


	Engine::WriteFile("%s ST", __FUNCTION__);
	//g_LagCompensation.process_animation();

	//g_LagCompensation.simulate_animations();

	{
		using SceneEnd_fn = void(__fastcall *)(void*thisptr, void*edx);
		Hooks::VRenderView.GetOriginalMethod<SceneEnd_fn>(9)(thisptr, edx);
	}


	if (!Render::g_pDevice || !Render::gamesurface) {

		Engine::WriteFile("%s END", __FUNCTION__);
		return;
	}
	if (!Render::game_dirty_rt) {

		g_pMaterialSystem->BeginRenderTargetAllocation();
		Render::game_dirty_rt = g_pMaterialSystem->CreateRenderTargetTexture(1, 1, RT_SIZE_FULL_FRAME_BUFFER, g_pMaterialSystem->GetBackBufferFormat(), MATERIAL_RT_DEPTH_SHARED);
		Render::game_clean_rt = g_pMaterialSystem->CreateRenderTargetTexture(1, 1, RT_SIZE_FULL_FRAME_BUFFER, g_pMaterialSystem->GetBackBufferFormat(), MATERIAL_RT_DEPTH_SHARED);

		g_pMaterialSystem->EndRenderTargetAllocation();

		/*
		auto kv = g_KeyValues.Operator_New();
		g_KeyValues.Constructor(kv, "rt_full");

		static auto mat_created = 0;
		static const char mat_struct[] =
		{
			"\"UnlitGeneric\"\
				\n{\
					\n\t\"$basetexture\" \"%s\"\
				\n}\n"
		};

		char mat_data[512] = { };
		sprintf_s(mat_data, sizeof(mat_data), mat_struct, "rt_full");

		char mat_name[512] = { };
		sprintf_s(mat_name, sizeof(mat_name), "texture_%i.vmt", mat_created);
		g_KeyValues.LoadFromBuffer(kv, mat_name, mat_data);
		Custom_material = g_pMaterialSystem->CreateMaterial(mat_name, kv);*/
	}
	/*static IMaterial* Custom_material = NULL;
	if (!Custom_material) {
		auto kv = g_KeyValues.Operator_New();
		g_KeyValues.Constructor(kv, "UnlitGeneric");

		static auto mat_created = 0;
		static const char mat_struct[] =
		{
			"\"UnlitGeneric\"\
				\n{\
					\n\t\"$basetexture\" \"vgui/white_additive\"\
					\n\t\"$model\" \"1\"\
					\n\t\"$translucent\" \"0\"\
					\n\t\"$outline\" \"1\"\
					\n\t\"$outlinecolor\" \"[0.7 0.7 0.5]\"\
					\n\t\"$outlinestart0\" \"0.3\"\
					\n\t\"$outlinestart1\" \"0.49\"\
					\n\t\"$outlineend0\" \"0.57\"\
					\n\t\"$outlineend1\" \"0.73\"\
					\n\t\"$ignorez\" \"1\"\
					\n\t\"$znearer\" \"0\"\
				\n}\n"
		};

		char mat_name[512] = { };
		sprintf_s(mat_name, sizeof(mat_name), "texture_%i.vmt", mat_created);
		g_KeyValues.LoadFromBuffer(kv, mat_name, mat_struct);
		Custom_material = g_pMaterialSystem->CreateMaterial(mat_name, kv);
	}*/


	auto render_ctx = g_pMaterialSystem->GetRenderContext();
	render_ctx->BeginRender();
	render_ctx->CopyRenderTargetToTexture(Render::game_clean_rt);

	[]() {
		if (Render::Screen_Shot || g_pEngine->IsTakingScreenshot()) {
			return;
		}
		if (Render::s_timer.Elapsed() < 10000)
			return;



		auto pLocal = C_GMOD_Player::GetLocalPlayer();
		if (!pLocal)
			return;
		auto mat_cologone = g_pMaterialSystem->FindMaterial("models/inventory_items/cologne_prediction/cologne_prediction_glass", TEXTURE_GROUP_MODEL);
		auto mat_chams = g_pMaterialSystem->FindMaterial("white_outline", TEXTURE_GROUP_MODEL);
		auto mat_wireframe = g_pMaterialSystem->FindMaterial("debug/debugwireframe", TEXTURE_GROUP_MODEL);
		auto mat_flat = g_pMaterialSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL);
		if (!mat_chams || !mat_flat || !mat_cologone || !mat_wireframe)
			return;
		std::vector<C_GMOD_Player*>ents;

		/*auto l_Team = Lua::GetTeam(pLocal->entindex());
		if (l_Team == -1337)
			return;
		auto l_Team_col = Lua::GetTeamColor(l_Team);*/

		Teams::Team l_team_info = {};
		if (!Teams::GetTeamInfo(l_team_info, pLocal))
			return;
		if (Engine::Var::Instance().Esp_Tracer.Get()) {
			if (EffectHook::g_Impact_data.size()) {
				for (auto it = 0; it < EffectHook::g_Impact_data.size(); it++) {
					auto& pOverlay = EffectHook::g_Impact_data[it];

					if (TICKS_TO_TIME(SDK::Vars::g_tick) < pOverlay->m_flEndTime) {
						EffectHook::DrawOverlay(pOverlay);
					}
					else {
						EffectHook::OverlayLine_t *pCurrLine = static_cast<EffectHook::OverlayLine_t*>(pOverlay);
						delete pCurrLine;
						EffectHook::g_Impact_data.erase(EffectHook::g_Impact_data.begin() + it);
						it--;
					}
				}
			}
		}
		g_pVRenderView->SetBlend(Engine::Var::Instance().Chams_Blend.Get());

		for (int i_Num = 0; i_Num <= g_pEngine->GetMaxClients(); i_Num++) {
			auto pEntity = C_GMOD_Player::GetGPlayer(i_Num);

			if (!pEntity || pEntity == pLocal || !pEntity->IsGood())
				continue;
			//pEntity->MoveToLastReceivedPosition();
			if (!SDK::W2S(pEntity->m_vecOrigin(), Vector())) {
				continue;
			}
			player_info_t info;
			if (!g_pEngine->GetPlayerInfo(pEntity->entindex(), &info))
				continue;
			if (Engine::Var::Instance().Esp_Bot_Check.Get())
				if (info.fakeplayer)
					continue;
			Teams::Team team_info = {};
			if (!Teams::GetTeamInfo(team_info, pEntity))
				continue;
			if (Engine::Var::Instance().Esp_Team_Check.Get()) {
				if (Teams::PlayerWhitelisted(pEntity))
					continue;
			}


			if (Engine::Var::Instance().Esp_Halo.Get()) {
				ents.push_back(pEntity);
			}
			if (!Engine::Var::Instance().Chams.Get())
				continue;
			pEntity->m_bReadyToDraw() = true;

			cColor vis_col;
			cColor invis_col;

			if (FriendList::Instance().IsFriend(pEntity)) {
				auto e_vis_col = Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor();
				vis_col = e_vis_col;
				invis_col = e_vis_col;
				invis_col[0] = invis_col[0] >> 2;
				invis_col[1] = invis_col[1] >> 2;
				invis_col[2] = invis_col[2] >> 2;
			}
			else if (Engine::Var::Instance().Esp_Colors_By_Class.Get()) {
				vis_col = team_info.Color;
				invis_col = team_info.Color;
				invis_col[0] = invis_col[0] >> 2;
				invis_col[1] = invis_col[1] >> 2;
				invis_col[2] = invis_col[2] >> 2;
			}
			else {
				auto e_vis_col = Color(Engine::Var::Instance().Chams_Visible.Get()).to_cColor();
				vis_col = e_vis_col;
				auto e_inv_col = Color(Engine::Var::Instance().Chams_Invisible.Get()).to_cColor();
				invis_col = e_inv_col;
			}
			float inv[4] = { invis_col[0] / 255.f, invis_col[1] / 255.f, invis_col[2] / 255.f, invis_col[3] / 255.f, };
			float vis[4] = { vis_col[0] / 255.f, vis_col[1] / 255.f, vis_col[2] / 255.f, vis_col[3] / 255.f, };
			inv[3] = vis[3] = 1.f;
			if (Engine::Var::Instance().Chams_Type.Get() == 0) {
				g_pVRenderView->SetColorModulation(inv);
				mat_flat->IncrementReferenceCount();
				mat_flat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				mat_flat->AlphaModulate(inv[3]);
				mat_flat->ColorModulate(inv[0], inv[1], inv[2]);
				g_pVModelRender->ForcedMaterialOverride(mat_flat);

				pEntity->DrawModel(0x1);

				if (pEntity->GetActiveWeapon())
					pEntity->GetActiveWeapon()->DrawModel(0x1);

				g_pVRenderView->SetColorModulation(vis);
				mat_flat->IncrementReferenceCount();
				mat_flat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				mat_flat->AlphaModulate(vis[3]);
				mat_flat->ColorModulate(vis[0], vis[1], vis[2]);
				g_pVModelRender->ForcedMaterialOverride(mat_flat);

				pEntity->DrawModel(0x1);
				if (pEntity->GetActiveWeapon())
					pEntity->GetActiveWeapon()->DrawModel(0x1);
			}
			else if (Engine::Var::Instance().Chams_Type.Get() == 1) {
				g_pVRenderView->SetColorModulation(inv);
				mat_chams->IncrementReferenceCount();
				mat_chams->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				mat_chams->AlphaModulate(inv[3]);
				mat_chams->ColorModulate(inv[0], inv[1], inv[2]);
				g_pVModelRender->ForcedMaterialOverride(mat_chams);

				pEntity->DrawModel(0x1);
				if (pEntity->GetActiveWeapon())
					pEntity->GetActiveWeapon()->DrawModel(0x1);

				g_pVRenderView->SetColorModulation(vis);
				mat_chams->IncrementReferenceCount();
				mat_chams->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				mat_chams->AlphaModulate(vis[3]);
				mat_chams->ColorModulate(vis[0], vis[1], vis[2]);
				g_pVModelRender->ForcedMaterialOverride(mat_chams);

				pEntity->DrawModel(0x1);
				if (pEntity->GetActiveWeapon())
					pEntity->GetActiveWeapon()->DrawModel(0x1);


			}
			else if (Engine::Var::Instance().Chams_Type.Get() == 2) {
				auto config = g_pMaterialSystem->GetCurrentConfigForVideoCard();
				auto save = config;
				config.nFullbright = true;
				g_pMaterialSystem->OverrideConfig(config, false);

				g_pVRenderView->SetColorModulation(inv);
				mat_cologone->IncrementReferenceCount();
				mat_cologone->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				mat_cologone->AlphaModulate(inv[3]);
				mat_cologone->ColorModulate(inv[0], inv[1], inv[2]);
				g_pVModelRender->ForcedMaterialOverride(mat_cologone);

				pEntity->DrawModel(0x1);
				if (pEntity->GetActiveWeapon())
					pEntity->GetActiveWeapon()->DrawModel(0x1);


				g_pVRenderView->SetColorModulation(vis);
				mat_cologone->IncrementReferenceCount();
				mat_cologone->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				mat_cologone->AlphaModulate(vis[3]);
				mat_cologone->ColorModulate(vis[0], vis[1], vis[2]);
				g_pVModelRender->ForcedMaterialOverride(mat_cologone);


				pEntity->DrawModel(0x1);
				if (pEntity->GetActiveWeapon())
					pEntity->GetActiveWeapon()->DrawModel(0x1);
				g_pMaterialSystem->OverrideConfig(save, false);
			}
			else if (Engine::Var::Instance().Chams_Type.Get() == 3) {

				g_pVRenderView->SetColorModulation(inv);
				mat_wireframe->IncrementReferenceCount();
				mat_wireframe->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				mat_wireframe->AlphaModulate(inv[3]);
				mat_wireframe->ColorModulate(inv[0], inv[1], inv[2]);
				g_pVModelRender->ForcedMaterialOverride(mat_wireframe);

				pEntity->DrawModel(0x1);
				if (pEntity->GetActiveWeapon())
					pEntity->GetActiveWeapon()->DrawModel(0x1);

				g_pVRenderView->SetColorModulation(vis);
				mat_wireframe->IncrementReferenceCount();
				mat_wireframe->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				mat_wireframe->AlphaModulate(vis[3]);
				mat_wireframe->ColorModulate(vis[0], vis[1], vis[2]);
				g_pVModelRender->ForcedMaterialOverride(mat_wireframe);

				pEntity->DrawModel(0x1);
				if (pEntity->GetActiveWeapon())
					pEntity->GetActiveWeapon()->DrawModel(0x1);

			}
			else if (Engine::Var::Instance().Chams_Type.Get() == 4) {
				//	pEntity->DrawModel(0x1);
				//	if (pEntity->GetActiveWeapon())
				//		pEntity->GetActiveWeapon()->DrawModel(0x1);
				auto model = pEntity->GetModel();
				if (!model) {
					continue;
				}
				auto hdr = g_pVModelInfo->GetStudiomodel(model);
				if (!hdr) {
					continue;
				}
				IMaterial **pMaterials = new IMaterial *[hdr->numtextures];
				g_pVModelInfo->GetModelMaterials(model, hdr->numtextures, pMaterials);
				for (int m = 0; m < (hdr->numtextures - 1); m++) {

					if (!pMaterials[m])
						break;

					pMaterials[m]->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
					pMaterials[m]->SetMaterialVarFlag(MATERIAL_VAR_NOCULL, false);
					//MATERIAL_VAR_TRANSLUCENT
				}
				pEntity->DrawModel(0x1);
				for (int m = 0; m < (hdr->numtextures - 1); m++) {

					if (!pMaterials[m])
						break;

					pMaterials[m]->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
					pMaterials[m]->SetMaterialVarFlag(MATERIAL_VAR_NOCULL, true);
				}
				pEntity->DrawModel(0x1);

				delete[] pMaterials;
			}

		}
		g_pVModelRender->ForcedMaterialOverride(nullptr);


		if (Engine::Var::Instance().Esp_Halo.Get()) {
			GlowObjectManager::Instance().RenderGlowEffects(ents, 0);
		}



		if (g_pInput->CAM_IsThirdPerson() && Engine::Var::Instance().Fake_angle_Chams.Get() && Engine::Var::Instance().AntiAim_Fake_Yaw.Get()) {
			auto fake_col = Color(Engine::Var::Instance().Fake_Angle_Color.Get()).to_cColor();
			float vis[4] = { fake_col[0] / 255.f, fake_col[1] / 255.f, fake_col[2] / 255.f, fake_col[3] / 255.f };
			QAngle save_angl;
			g_pPrediction->GetLocalViewAngles(save_angl);
			pLocal->SetAbsOrigin(Hooks::Variables::PositionWSend);
			if (Engine::Var::Instance().AntiAim_Fake_Yaw.Get() > 0) {
				g_pPrediction->SetLocalViewAngles(Hooks::Variables::Angle2Send);
				auto anim_state = pLocal->PlayerAnimState();
				if (anim_state) {
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


			pLocal->UpdateClientSideAnimation();
			pLocal->InvalidateBoneCache();

			g_pVRenderView->SetBlend(Engine::Var::Instance().Fake_angle_Chams_Blend.Get());
			if (Engine::Var::Instance().Fake_angle_Chams_Type.Get() == 0) {

				g_pVRenderView->SetColorModulation(vis);
				mat_flat->IncrementReferenceCount();
				mat_flat->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				mat_flat->AlphaModulate(vis[3]);
				mat_flat->ColorModulate(vis[0], vis[1], vis[2]);
				g_pVModelRender->ForcedMaterialOverride(mat_flat);

				pLocal->DrawModel(0x1);
			}
			else if (Engine::Var::Instance().Fake_angle_Chams_Type.Get() == 1) {


				g_pVRenderView->SetColorModulation(vis);
				mat_chams->IncrementReferenceCount();
				mat_chams->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				mat_chams->AlphaModulate(vis[3]);
				mat_chams->ColorModulate(vis[0], vis[1], vis[2]);
				g_pVModelRender->ForcedMaterialOverride(mat_chams);

				pLocal->DrawModel(0x1);

			}
			else if (Engine::Var::Instance().Fake_angle_Chams_Type.Get() == 2) {
				auto config = g_pMaterialSystem->GetCurrentConfigForVideoCard();
				auto save = config;
				config.nFullbright = true;
				config.bDrawFlat = true;
				g_pMaterialSystem->OverrideConfig(config, false);

				g_pVRenderView->SetColorModulation(vis);
				mat_cologone->IncrementReferenceCount();
				mat_cologone->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				mat_cologone->AlphaModulate(vis[3]);
				mat_cologone->ColorModulate(vis[0], vis[1], vis[2]);
				g_pVModelRender->ForcedMaterialOverride(mat_cologone);


				pLocal->DrawModel(0x1);
				g_pMaterialSystem->OverrideConfig(save, false);
			}
			else if (Engine::Var::Instance().Fake_angle_Chams_Type.Get() == 3) {


				g_pVRenderView->SetColorModulation(vis);
				mat_wireframe->IncrementReferenceCount();
				mat_wireframe->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				mat_wireframe->AlphaModulate(vis[3]);
				mat_wireframe->ColorModulate(vis[0], vis[1], vis[2]);
				g_pVModelRender->ForcedMaterialOverride(mat_wireframe);

				pLocal->DrawModel(0x1);
			}
			else if (Engine::Var::Instance().Chams_Type.Get() == 4) {
				pLocal->DrawModel(0x1);
			}
			pLocal->SetAbsOrigin(pLocal->m_vecOrigin());
			g_pPrediction->SetLocalViewAngles(save_angl);

			g_pVModelRender->ForcedMaterialOverride(nullptr);

		}
		/*g_pVModelRender->ForcedMaterialOverride(nullptr);


			g_pVRenderView->SetColorModulation(vis);
			mat_wireframe->IncrementReferenceCount();
			mat_wireframe->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			mat_wireframe->AlphaModulate(vis[3]);
			mat_wireframe->ColorModulate(vis[0], vis[1], vis[2]);
			g_pVModelRender->ForcedMaterialOverride(mat_wireframe);

			pLocal->DrawModel(0x1);
			g_pVModelRender->ForcedMaterialOverride(nullptr);*/

	endl:;
		g_pVRenderView->SetBlend(1.f);
		g_pVModelRender->ForcedMaterialOverride(nullptr);
		float vis[4] = { 1.f,  1.f,  1.f,  1.f };
		g_pVRenderView->SetColorModulation(vis);
	}();

	render_ctx->CopyRenderTargetToTexture(Render::game_dirty_rt);
	int w, h;
	h = Render::game_clean_rt->GetActualHeight();
	w = Render::game_clean_rt->GetActualWidth();
	Rect_t t_rect = {};
	t_rect.height = h;
	t_rect.width = w;
	t_rect.x = t_rect.y = 0;
	render_ctx->CopyTextureToRenderTargetEx(0, Render::game_clean_rt, &t_rect, &t_rect);
	render_ctx->EndRender();


	Engine::WriteFile("%s END", __FUNCTION__);
}