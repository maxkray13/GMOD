#pragma once
#include "../SDK.h"
#include "../../engine/Render/Render.h"
#include "../../Functional/EffectHook.h"
void __fastcall hkPaintTraverse(void *thisptr, void *edx, VPANEL vguiPanel, bool forceRepaint, bool allowForce) {

	Engine::WriteFile("%s START",__FUNCTION__ );



	{
		using PaintTraverse_fn = void(__fastcall*)(void*, void*, VPANEL, bool, bool);
		Hooks::VGui.GetOriginalMethod<PaintTraverse_fn>(41)(thisptr, edx, vguiPanel, forceRepaint, allowForce);
	}

	if (Render::Screen_Shot||g_pEngine->IsTakingScreenshot()) {

		Engine::WriteFile("%s END", __FUNCTION__);
		return;
	}
	
	auto Name = g_pPanel->GetName(vguiPanel);

	if (!Name) {

		Engine::WriteFile("%s END", __FUNCTION__);
		return;
	}

	static VPANEL OverlayPopupPanel = -1;
	if (OverlayPopupPanel == -1) {
		if (strcmp("OverlayPopupPanel", Name) == 0) {
			OverlayPopupPanel = vguiPanel;
		}
	}
	else if (vguiPanel == OverlayPopupPanel) {
//		auto& gui = Render::CGui::Instance();
		if (Render::ShowMenu) {
			g_pPanel->SetEnabled(vguiPanel, true);
			g_pPanel->SetKeyBoardInputEnabled(vguiPanel, true);
			g_pPanel->SetMouseInputEnabled(vguiPanel, true);
		}
		else {
			g_pPanel->SetEnabled(vguiPanel, false);
			g_pPanel->SetKeyBoardInputEnabled(vguiPanel, false);
			g_pPanel->SetMouseInputEnabled(vguiPanel, false);
		}
	}
	
	static VPANEL MatSystemTopPanel = -1;
	if (MatSystemTopPanel == -1) {
		if (strcmp("MatSystemTopPanel", Name) == 0) {
			MatSystemTopPanel = vguiPanel;
		}
	}
	else if (vguiPanel == MatSystemTopPanel) {
		if (g_pEngine->IsInGame()) {


			VMatrix pWorldToView, pViewToProjection, pWorldToProjection, pWorldToPixels;
			g_pVRenderView->GetMatricesForView(*g_pViewRender->GetViewSetup(), &pWorldToView, &pViewToProjection, &SDK::g_w2s_matrix, &pWorldToPixels);

		}
	}

	Engine::WriteFile("%s END", __FUNCTION__);
}