#pragma once
#include "../../SDK.h"
#include "../../../engine/Engine.h"
#include "../Hooks.h"
void __fastcall hkSceneBegin(void*thisptr, void*edx) {


	Engine::WriteFile("%s ST", __FUNCTION__);
	static MaterialSystem_Config_t original_config;
	static bool at_st = true;
	if (at_st) {
			original_config = g_pMaterialSystem->GetCurrentConfigForVideoCard();
	}

	{
		using SceneEnd_fn = void(__fastcall *)(void*thisptr, void*edx);
		Hooks::VRenderView.GetOriginalMethod<SceneEnd_fn>(8)(thisptr, edx);
	}
	if (Render::Screen_Shot || g_pEngine->IsTakingScreenshot()|| Render::s_timer.Elapsed() < 10000) {
		g_pMaterialSystem->OverrideConfig(original_config, false);

		Engine::WriteFile("%s END", __FUNCTION__);
		return;
	}
	if (Engine::Var::Instance().Fullbright.Get()) {
		auto config = g_pMaterialSystem->GetCurrentConfigForVideoCard();
		config.nFullbright = true;
		g_pMaterialSystem->OverrideConfig(config, false);
	}

	Engine::WriteFile("%s END", __FUNCTION__);

}