#include "../../../include/stdafx.h"
#include "Glow_outline_effect.h"
#include "../../SDK.h"
#include "../../../engine/Engine.h"
#include "../../../engine/Render/Render.h"


#define FULL_FRAME_TEXTURE "_rt_FullFrameFB"





void GlowObjectManager::RenderGlowEffects(std::vector<C_GMOD_Player*>& ents, int nSplitScreenSlot)
{
	if (!ents.size()) {
		return;
	}

	this->ents = ents;

	IMatRenderContext* context = g_pMaterialSystem->GetRenderContext();
	context->BeginRender();
	//	CMatRenderContextPtr pRenderContext(g_pIMaterialSystem);

	int nX, nY, nWidth, nHeight;
	context->GetViewport(nX, nY, nWidth, nHeight);

	//	PIXEvent _pixEvent(context, "EntityGlowEffects");
	context->BeginPIXEvent(PIX_VALVE_ORANGE, "EntityGlowEffects");
	//DrawHalos();
	ApplyEntityGlowEffects(nSplitScreenSlot, context, Engine::Var::Instance().Halo_Bloom.Get(), nX, nY, nWidth, nHeight);

	context->EndPIXEvent();

	context->EndRender();

}




static void SetRenderTargetAndViewPort(ITexture *rt, int w, int h)
{
	//CMatRenderContextPtr pRenderContext(g_pIMaterialSystem);
	IMatRenderContext* context = g_pMaterialSystem->GetRenderContext();
	context->BeginRender();
	context->SetRenderTarget(rt);
	context->Viewport(0, 0, w, h);
	context->EndRender();
}
void GlowObjectManager::DrawHalos() {
	

};


void GlowObjectManager::RenderGlowModels(int nSplitScreenSlot, IMatRenderContext* pRenderContext)
{
	//==========================================================================================//
	// This renders solid pixels with the correct coloring for each object that needs the glow.	//
	// After this function returns, this image will then be blurred and added into the frame	//
	// buffer with the objects stenciled out.													//
	//==========================================================================================//
	pRenderContext->PushRenderTargetAndViewport();

	// Save modulation color and blend
	Vector vOrigColor;
	g_pVRenderView->GetColorModulation((vec_t*)&vOrigColor);
	float flOrigBlend = g_pVRenderView->GetBlend();

	// Get pointer to FullFrameFB
	ITexture *pRtFullFrame = NULL;
	pRtFullFrame = g_pMaterialSystem->FindTexture(FULL_FRAME_TEXTURE, TEXTURE_GROUP_RENDER_TARGET);

	int width, height;
	g_pEngine->GetScreenSize(width, height);

	SetRenderTargetAndViewPort(pRtFullFrame, width, height);

	pRenderContext->ClearColor3ub(0, 0, 0);
	pRenderContext->ClearBuffers(true, false, false);

	// Set override material for glow color
	IMaterial *pMatGlowColor = NULL;
	auto glow_col = Color(Engine::Var::Instance().Esp_Halo_Color.Get()).to_cColor();
	pMatGlowColor = g_pMaterialSystem->FindMaterial("dev/glow_color", TEXTURE_GROUP_OTHER, true);
	pMatGlowColor->ColorModulate(glow_col.r() / 255.f, glow_col.g() / 255.f, glow_col.b() / 255.f);
	pMatGlowColor->IncrementReferenceCount();




	//==================//
	// Draw the objects //
	//==================//

	for (auto it : ents)
	{
		g_pStudioRender->ForcedMaterialOverride(pMatGlowColor);
		auto glow_col = Color(Engine::Var::Instance().Esp_Halo_Color.Get()).to_cColor();
		g_pVRenderView->SetBlend(255);
		Vector vGlowColor(glow_col.r() / 255.f, glow_col.g() / 255.f, glow_col.b() / 255.f);
		g_pVRenderView->SetColorModulation(&vGlowColor[0]); // This only sets rgb, not alpha

		it->DrawModel(0x1);
		if (it->GetActiveWeapon())
			it->GetActiveWeapon()->DrawModel(0x1);
	}

	g_pStudioRender->ForcedMaterialOverride(NULL);
	g_pVRenderView->SetColorModulation((vec_t*)&vOrigColor);
	g_pVRenderView->SetBlend(flOrigBlend);

	ShaderStencilState_t stencilStateDisable;
	stencilStateDisable.m_bEnable = false;
	stencilStateDisable.SetStencilState(pRenderContext);

	pRenderContext->PopRenderTargetAndViewport();


}

void GlowObjectManager::ApplyEntityGlowEffects(int nSplitScreenSlot, IMatRenderContext* pRenderContext, float flBloomScale, int x, int y, int w, int h)
{
	//=======================================================//
	// Render objects into stencil buffer					 //
	//=======================================================//
	// Set override shader to the same simple shader we use to render the glow models

	static bool s_bFirstPass = true;
	auto glow_col = Color(Engine::Var::Instance().Esp_Halo_Color.Get()).to_cColor();
	IMaterial *pMatGlowColor = g_pMaterialSystem->FindMaterial("dev/glow_color", TEXTURE_GROUP_OTHER, true);
	pMatGlowColor->ColorModulate(glow_col.r() / 255.f, glow_col.g() / 255.f, glow_col.b() / 255.f);

	pMatGlowColor->IncrementReferenceCount();

	g_pStudioRender->ForcedMaterialOverride(pMatGlowColor);

	ShaderStencilState_t stencilStateDisable;
	stencilStateDisable.m_bEnable = false;
	float flSavedBlend = g_pVRenderView->GetBlend();

	// Set alpha to 0 so we don't touch any color pixels
	g_pVRenderView->SetBlend(0.0f);
	pRenderContext->OverrideDepthEnable(true, false);

	for (auto it : ents) {
		ShaderStencilState_t stencilState;
		stencilState.m_bEnable = true;
		stencilState.m_nReferenceValue = 1;
		stencilState.m_CompareFunc = STENCILCOMPARISONFUNCTION_ALWAYS;
		stencilState.m_PassOp = STENCILOPERATION_KEEP;
		stencilState.m_FailOp = STENCILOPERATION_KEEP;
		stencilState.m_ZFailOp = STENCILOPERATION_REPLACE;

		stencilState.SetStencilState(pRenderContext);

		it->DrawModel(0x1);
		if (it->GetActiveWeapon())
			it->GetActiveWeapon()->DrawModel(0x1);
	}


	for (auto it : ents) {
		ShaderStencilState_t stencilState;
		stencilState.m_bEnable = true;
		stencilState.m_nReferenceValue = 2;
		stencilState.m_CompareFunc = STENCILCOMPARISONFUNCTION_ALWAYS;
		stencilState.m_PassOp = STENCILOPERATION_REPLACE;
		stencilState.m_FailOp = STENCILOPERATION_KEEP;
		stencilState.m_ZFailOp = STENCILOPERATION_KEEP;
		stencilState.SetStencilState(pRenderContext);

		it->DrawModel(0x1);
		if (it->GetActiveWeapon())
			it->GetActiveWeapon()->DrawModel(0x1);
	}

	pRenderContext->OverrideDepthEnable(false, false);
	g_pVRenderView->SetBlend(flSavedBlend);
	stencilStateDisable.SetStencilState(pRenderContext);
	g_pStudioRender->ForcedMaterialOverride(NULL);

	//=============================================
	// Render the glow colors to _rt_FullFrameFB 
	//=============================================
	{
		pRenderContext->BeginPIXEvent(PIX_VALVE_ORANGE, "EntityGlowEffects");
		RenderGlowModels(nSplitScreenSlot, pRenderContext);
		pRenderContext->EndPIXEvent();

	}
	int nSrcWidth;
	int nSrcHeight;
	g_pEngine->GetScreenSize(nSrcWidth, nSrcHeight);
	// Get viewport

	int nViewportX, nViewportY, nViewportWidth, nViewportHeight;
	pRenderContext->GetViewport(nViewportX, nViewportY, nViewportWidth, nViewportHeight);
	///

	pRenderContext->PushRenderTargetAndViewport();
	// Get material and texture pointers
	IMaterial *pMatDownsample = g_pMaterialSystem->FindMaterial("dev/glow_downsample", TEXTURE_GROUP_OTHER, true);//  dev/downsample_non_hdr //dev/glow_downsample
	pMatDownsample->IncrementReferenceCount();
	IMaterial *pMatBlurX = g_pMaterialSystem->FindMaterial("dev/glow_blur_x", TEXTURE_GROUP_OTHER, true);
	pMatBlurX->IncrementReferenceCount();
	IMaterial *pMatBlurY = g_pMaterialSystem->FindMaterial("dev/glow_blur_y", TEXTURE_GROUP_OTHER, true);
	pMatBlurY->IncrementReferenceCount();

	ITexture *pRtFullFrame = g_pMaterialSystem->FindTexture(FULL_FRAME_TEXTURE, TEXTURE_GROUP_RENDER_TARGET);
	ITexture *pRtQuarterSize0 = g_pMaterialSystem->FindTexture("_rt_SmallFB0", TEXTURE_GROUP_RENDER_TARGET);
	ITexture *pRtQuarterSize1 = g_pMaterialSystem->FindTexture("_rt_SmallFB1", TEXTURE_GROUP_RENDER_TARGET);

	//============================================
	// Downsample _rt_FullFrameFB to _rt_SmallFB0
	//============================================

	// First clear the full target to black if we're not going to touch every pixel
	if ((pRtQuarterSize0->GetActualWidth() != (nSrcWidth / 4)) || (pRtQuarterSize0->GetActualHeight() != (nSrcHeight / 4)))
	{
		SetRenderTargetAndViewPort(pRtQuarterSize0, pRtQuarterSize0->GetActualWidth(), pRtQuarterSize0->GetActualHeight());
		pRenderContext->ClearColor3ub(0, 0, 0);
		pRenderContext->ClearBuffers(true, false, false);
	}

	// Set the viewport
	SetRenderTargetAndViewPort(pRtQuarterSize0, nSrcWidth / 4, nSrcHeight / 4);

	/*
	IMaterialVar *pbloomexpvar = pMatDownsample->FindVar("$bloomexp", NULL);
	if (pbloomexpvar != NULL)
	{
		pbloomexpvar->SetFloatValue(1.f);
	}

	IMaterialVar *pbloomsaturationvar = pMatDownsample->FindVar("$bloomsaturation", NULL);
	if (pbloomsaturationvar != NULL)
	{
		pbloomsaturationvar->SetFloatValue(1.0f);
	}*/

	// note the -2's below. Thats because we are downsampling on each axis and the shader
	// accesses pixels on both sides of the source coord
	int nFullFbWidth = nSrcWidth;
	int nFullFbHeight = nSrcHeight;

	pRenderContext->DrawScreenSpaceRectangle(pMatDownsample, 0, 0, nSrcWidth / 4, nSrcHeight / 4,
		0, 0, nFullFbWidth - 4, nFullFbHeight - 4,
		pRtFullFrame->GetActualWidth(), pRtFullFrame->GetActualHeight());

	//============================//
	// Guassian blur x rt0 to rt1 //
	//============================//

	// First clear the full target to black if we're not going to touch every pixel
	if (s_bFirstPass || (pRtQuarterSize1->GetActualWidth() != (nSrcWidth / 4)) || (pRtQuarterSize1->GetActualHeight() != (nSrcHeight / 4)))
	{
		// On the first render, this viewport may require clearing
		s_bFirstPass = false;
		SetRenderTargetAndViewPort(pRtQuarterSize1, pRtQuarterSize1->GetActualWidth(), pRtQuarterSize1->GetActualHeight());
		pRenderContext->ClearColor3ub(0, 0, 0);
		pRenderContext->ClearBuffers(true, false, false);
	}

	// Set the viewport
	SetRenderTargetAndViewPort(pRtQuarterSize1, nSrcWidth / 4, nSrcHeight / 4);

	pRenderContext->DrawScreenSpaceRectangle(pMatBlurX, 0, 0, nSrcWidth / 4, nSrcHeight / 4,
		0, 0, nSrcWidth / 4 - 1, nSrcHeight / 4 - 1,
		pRtQuarterSize0->GetActualWidth(), pRtQuarterSize0->GetActualHeight());

	//============================//
	// Gaussian blur y rt1 to rt0 //
	//============================//
	SetRenderTargetAndViewPort(pRtQuarterSize0, nSrcWidth / 4, nSrcHeight / 4);
	IMaterialVar *pBloomAmountVar = pMatBlurY->FindVar("$bloomamount", NULL);
	pBloomAmountVar->SetFloatValue(flBloomScale);

	pRenderContext->DrawScreenSpaceRectangle(pMatBlurY, 0, 0, nSrcWidth / 4, nSrcHeight / 4,
		0, 0, nSrcWidth / 4 - 1, nSrcHeight / 4 - 1,
		pRtQuarterSize1->GetActualWidth(), pRtQuarterSize1->GetActualHeight());

	// Pop RT
	pRenderContext->PopRenderTargetAndViewport();
	{
		//=======================================================================================================//
		// At this point, pRtQuarterSize0 is filled with the fully colored glow around everything as solid glowy //
		// blobs. Now we need to stencil out the original objects by only writing pixels that have no            //
		// stencil bits set in the range we care about.                                                          //
		//=======================================================================================================//
		IMaterial *pMatHaloAddToScreen = g_pMaterialSystem->FindMaterial("dev/halo_add_to_screen", TEXTURE_GROUP_OTHER, true);

		pMatHaloAddToScreen->IncrementReferenceCount();
		// Do not fade the glows out at all (weight = 1.0)
		IMaterialVar *pDimVar = pMatHaloAddToScreen->FindVar("$C0_X", NULL);
		pDimVar->SetFloatValue(1.0f);
		IMaterialVar *pDimVarY = pMatHaloAddToScreen->FindVar("$C0_Y", NULL);
		pDimVarY->SetFloatValue(1.0f);

		ShaderStencilState_t stencilState;
		stencilState.m_bEnable = true;
		stencilState.m_nWriteMask = 0x0; // We're not changing stencil
		stencilState.m_nTestMask = 0xFF;//0x3;
		stencilState.m_nReferenceValue = 0x0;
		stencilState.m_CompareFunc = STENCILCOMPARISONFUNCTION_EQUAL;
		stencilState.m_PassOp = STENCILOPERATION_KEEP;
		stencilState.m_FailOp = STENCILOPERATION_KEEP;
		stencilState.m_ZFailOp = STENCILOPERATION_KEEP;
		stencilState.SetStencilState(pRenderContext);
		//pRenderContext->SetStencilState(stencilState);
		// Draw quad
		for (int it = 0; it < Engine::Var::Instance().Halo_Passes.Get(); it++) {
		pRenderContext->DrawScreenSpaceRectangle(pMatHaloAddToScreen, 0, 0, nSrcWidth, nSrcHeight,
			0.0f, -0.5f, nSrcWidth / 4 - 1, nSrcHeight / 4 - 1,
			pRtQuarterSize1->GetActualWidth(),
			pRtQuarterSize1->GetActualHeight());
		}


			// Disable stencil
		stencilStateDisable.SetStencilState(pRenderContext);
		//	pRenderContext->SetStencilState(stencilStateDisable);
	}


	/*
	// Get material and texture pointers
	ITexture *pRtQuarterSize1 = g_pIMaterialSystem->FindTexture("_rt_SmallFB1", TEXTURE_GROUP_RENDER_TARGET);

	{
		//=======================================================================================================//
		// At this point, pRtQuarterSize0 is filled with the fully colored glow around everything as solid glowy //
		// blobs. Now we need to stencil out the original objects by only writing pixels that have no            //
		// stencil bits set in the range we care about.                                                          //
		//=======================================================================================================//
		IMaterial *pMatHaloAddToScreen = g_pIMaterialSystem->FindMaterial("dev/halo_add_to_screen", TEXTURE_GROUP_OTHER, true);
		pMatHaloAddToScreen->IncrementReferenceCount();

		// Do not fade the glows out at all (weight = 1.0)
		IMaterialVar *pDimVar = pMatHaloAddToScreen->FindVar("$C0_X", NULL);
		pDimVar->SetFloatValue(1.0f);

		// Set stencil state
		ShaderStencilState_t stencilState;
		stencilState.m_bEnable = true;
		stencilState.m_nWriteMask = 0x0; // We're not changing stencil
		stencilState.m_nTestMask = 0xFF;
		stencilState.m_nReferenceValue = 0x0;
		stencilState.m_CompareFunc = STENCILCOMPARISONFUNCTION_EQUAL;
		stencilState.m_PassOp = STENCILOPERATION_KEEP;
		stencilState.m_FailOp = STENCILOPERATION_KEEP;
		stencilState.m_ZFailOp = STENCILOPERATION_KEEP;
		stencilState.SetStencilState(pRenderContext);

		// Draw quad
		pRenderContext->DrawScreenSpaceRectangle(pMatHaloAddToScreen, 0, 0, nViewportWidth, nViewportHeight,
			0.0f, -0.5f, nSrcWidth / 4 - 1, nSrcHeight / 4 - 1,
			pRtQuarterSize1->GetActualWidth(),
			pRtQuarterSize1->GetActualHeight());

		stencilStateDisable.SetStencilState(pRenderContext);
	}

	*/
}
