#pragma once
#include "../../../engine/Core/Basic/Basic.h"
class C_GMOD_Player;
class IMatRenderContext;
class GlowObjectManager:public Basic::Singleton<GlowObjectManager> {
public:
	void DrawHalos();
	void RenderGlowEffects(std::vector<C_GMOD_Player*>& ents, int nSplitScreenSlot);
	void RenderGlowModels( int nSplitScreenSlot, IMatRenderContext *pRenderContext);
	void ApplyEntityGlowEffects(int nSplitScreenSlot, IMatRenderContext *pRenderContext, float flBloomScale, int x, int y, int w, int h);
private:
	std::vector<C_GMOD_Player*> ents;
};
