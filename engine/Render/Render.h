#pragma once
#include "../../include/stdafx.h"
#include "../Core/Basic/Basic.h"
#include "Color.h"

#include "Menu/Gui.h"
static constexpr unsigned int FSEX300_size = 575668;
extern const unsigned int FSEX300_data[575668 / 4];
class ITexture;
class IMaterial;
namespace Render {
	extern bool Screen_Shot;
	extern Basic::Timer s_timer;
	extern bool ShowMenu;
	extern IDirect3DDevice9* g_pDevice;

	extern Draw::Scene g_Scene;
	extern Draw::Scene transmit_scene;
	extern Draw::Scene game_scene;

	static constexpr auto Black = D3DCOLOR_ARGB(255, 0, 0, 0);
	static constexpr auto Orange = 0xFFFF8000;

	using GetRenderTarget_fn = HRESULT(WINAPI*)(LPDIRECT3DDEVICE9, DWORD, IDirect3DSurface9 **);
	using Reset_fn = HRESULT(WINAPI*)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
	using Present_fn = HRESULT(WINAPI*)(LPDIRECT3DDEVICE9, const RECT*, RECT*, HWND, RGNDATA*);
	static Reset_fn oReset;
	static HRESULT WINAPI hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* params);
	static DWORD Reset_PTR;
	static Present_fn oPresent;
	static HRESULT WINAPI hkPresent(LPDIRECT3DDEVICE9 pDevice, RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion);
	static DWORD Present_PTR;

	void Attach();
	void Detach();

	void proc_menu(IDirect3DDevice9* pDevice);

	extern IDirect3DSurface9* gamesurface;
	extern IDirect3DTexture9* gametexture;
	extern IDirect3DVertexBuffer9* v_buffer;
	extern ITexture * game_clean_rt;
	extern ITexture * game_dirty_rt;
	extern ITexture* transmit_clean_rt;
	extern ITexture* transmit_dirty_rt;
	extern ITexture* _clean_rt;
	extern ITexture* _dirty_rt;
}