#pragma once
#include "../Functional/Esp1.h"
#include "RenderView.h"
#include "CreateMove.h"
#include "../../../engine/Render/Render.h"
#include "../../Functional/Teams.h"

void __fastcall hkPaint(void* thisptr, void*edx, PaintMode_t Mode) {


	Engine::WriteFile("%s ST", __FUNCTION__);
	{
		using Paint_fn = void(__fastcall *)(void*, void*, PaintMode_t);
		Hooks::VEngineVGui.GetOriginalMethod<Paint_fn>(13)(thisptr, edx, Mode);
	}

	if (!g_pEngine->IsInGame()|| g_pEngine->IsDrawingLoadingImage()) {
		tr_outsq_num = -1;
		Hooks::Variables::g_pLastCmd = nullptr;
	}
		
	if (!Render::g_pDevice) {

		Engine::WriteFile("%s END", __FUNCTION__);
		return;
	}
		
	Teams::ProcessTeams();
	Draw::BeginDraw(Render::g_pDevice, &Render::game_scene,false);
	Gui::ProcessInput();


	//if (Render::Screen_Shot || g_pEngine->IsTakingScreenshot() || Render::s_timer.Elapsed() < 10000)
	//	return;




	if ( Mode & PAINT_UIPANELS) {



		



		/*
		if (g_pEngine->IsInGame() && !g_pEngine->IsDrawingLoadingImage()) {
			cEsp::Instance().Esp_Tick();
			EntEsp::Instance().Vis_PropEsp_tick();
		}
		*/
		if (g_pEngine->IsInGame() && !g_pEngine->IsDrawingLoadingImage()) {
			cEsp::Instance().Esp_Tick();
			EntEsp::Instance().Vis_PropEsp_tick();
		}
		
		/*
		SDK::Function::StartDrawing((void*)SDK::Vars::CMatSystemSurface_ptr);
		auto kk = g_pEngineVGui->GetPanel(PANEL_ROOT);
		g_pSurface->PushMakeCurrent(kk, false);
		auto& EngineRender = SDK::CDraw::Instance();
		static bool should_reset_fonts = false;
		if (g_pEngine->IsInGame() && !g_pEngine->IsDrawingLoadingImage() && g_pClientState->m_nSignonState == SIGNONSTATE_FULL) {
			if (should_reset_fonts) {
				g_pSurface->ResetFontCaches();
				EngineRender.Reset();
				should_reset_fonts = false;
			}
			EngineRender.Reset();
			cEsp::Instance().Esp_Tick();
			EntEsp::Instance().Vis_PropEsp_tick();
		}
		else {
			should_reset_fonts = true;
		}
		g_pSurface->PopMakeCurrent(kk);
		SDK::Function::FinishDrawing((void*)SDK::Vars::CMatSystemSurface_ptr);*/
	}
	Render::proc_menu(Render::g_pDevice);
	
	//Hooks::g_Sync.lock();
	//if (Hooks::g_Sync.try_lock())
	{
		/*
		if (Render::game_clean_rt&&Render::game_dirty_rt&&Render::transmit_clean_rt&&Render::transmit_dirty_rt) {
			auto render_ctx = g_pMaterialSystem->GetRenderContext();
			render_ctx->BeginRender();
			render_ctx->PushRenderTargetAndViewport();
			render_ctx->SetRenderTarget(Render::game_clean_rt);
			render_ctx->CopyRenderTargetToTexture(Render::transmit_clean_rt);
			render_ctx->SetRenderTarget(Render::game_dirty_rt);
			render_ctx->CopyRenderTargetToTexture(Render::transmit_dirty_rt);
			render_ctx->PopRenderTargetAndViewport();
			render_ctx->EndRender();
		}*/
		Hooks::g_Sync.lock();
		Draw::BeginDraw(Render::g_pDevice, &Render::transmit_scene, false);
		Render::transmit_scene.g_pos_vertex = Render::game_scene.g_pos_vertex;
		memcpy_fast(Render::transmit_scene.g_Vertex_buff, Render::game_scene.g_Vertex_buff, Render::game_scene.g_pos_vertex * sizeof(Vertex));
		Hooks::g_Sync.unlock();
	}
	
	Engine::WriteFile("%s END", __FUNCTION__);
}
//Hooks::Variables::g_pLastCmd