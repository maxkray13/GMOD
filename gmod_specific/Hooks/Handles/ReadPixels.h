#pragma once
using tdReadPix = void(__fastcall*)(void* ecx, void* edx, int x, int y, int width, int height, unsigned char *data, ImageFormat dstFormat);
tdReadPix o_ReadPix;
void __fastcall hkReadPixels(void* ecx, void* edx, int x, int y, int width, int height, unsigned char *data, ImageFormat dstFormat) {

	Engine::WriteFile("%s ST", __FUNCTION__);


	static DWORD lua_shit_capture = 0;
	if (!lua_shit_capture) {
		auto temp_i = (DWORD)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xFF\x52\x2C\x8B\x06",//xxxxxx
			0xCC);
		temp_i += 3;
		lua_shit_capture = temp_i;
	}
	if ((DWORD)_ReturnAddress() == lua_shit_capture) {

		Engine::WriteFile("%s END", __FUNCTION__);
		return o_ReadPix(ecx, edx, x, y, width, height, data, dstFormat);
	}
	
	static auto render_capture_retaddr = 0u;
	if (!render_capture_retaddr) {
		render_capture_retaddr = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xFF\xCC\x2C\x6A\x20\x8D",
			0xCC);
		render_capture_retaddr += 0x3;
	}
	if ((DWORD)_ReturnAddress() != render_capture_retaddr) {
		Engine::WriteFile("%s END", __FUNCTION__);
		return o_ReadPix(ecx, edx, x, y, width, height, data, dstFormat);
	}

	Render::Screen_Shot = true;
	Render::s_timer.Reset();
	int w, h;
	vrect_t Rec = {};
	g_pEngine->GetScreenSize(w, h);
	Rec.width = w;
	Rec.height = h;
	Rec.x = 0;
	Rec.y = 0;
	Rec.pnext = nullptr;
	g_pClient->FrameStageNotify(FRAME_RENDER_START);
	g_pClient->View_Render(&Rec);
	g_pClient->FrameStageNotify(FRAME_RENDER_END);
	/*static PDWORD pImage = 0;
	if (!pImage) {
		auto temp_i = (DWORD)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x8B\xCC\xCC\xCC\xCC\xCC\x8B\xCC\xCC\x8B\xCC\xCC\x85\xCC\x74\xCC\xA1",//xxxxxx
			0xCC);
		temp_i += 2;
		pImage = *(DWORD**)temp_i;
	}

	if (*pImage)
		free((void*)*pImage);
	*pImage = (DWORD)malloc(w * 3 * h);
	ZeroMemory((void*)*pImage, w * 3 * h);
	*/
	o_ReadPix(ecx, edx, x, y, width, height, data, dstFormat);
	Render::Screen_Shot = false;

	Engine::WriteFile("%s END", __FUNCTION__);
}
using tdReadPixAndStretch = void (__fastcall*)(void* ecx, void* edx, Rect_t *pSrcRect, Rect_t *pDstRect, unsigned char *pBuffer, ImageFormat dstFormat, int nDstStride);
tdReadPixAndStretch o_ReadPixAndStretch;
void __fastcall hkReadPixelsAndStretch(void* ecx, void* edx,Rect_t *pSrcRect, Rect_t *pDstRect, unsigned char *pBuffer, ImageFormat dstFormat, int nDstStride) {


	Engine::WriteFile("%s ST", __FUNCTION__);



	Render::Screen_Shot = true;
	Render::s_timer.Reset();
	int w, h;
	vrect_t Rec = {};
	g_pEngine->GetScreenSize(w, h);
	Rec.width = w;
	Rec.height = h;
	Rec.x = 0;
	Rec.y = 0;
	Rec.pnext = nullptr;
	g_pClient->FrameStageNotify(FRAME_RENDER_START);
	g_pClient->View_Render(&Rec);
	g_pClient->FrameStageNotify(FRAME_RENDER_END);
	/*static PDWORD pImage = 0;
	if (!pImage) {
		auto temp_i = (DWORD)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x8B\xCC\xCC\xCC\xCC\xCC\x8B\xCC\xCC\x8B\xCC\xCC\x85\xCC\x74\xCC\xA1",//xxxxxx
			0xCC);
		temp_i += 2;
		pImage = *(DWORD**)temp_i;
	}

	if (*pImage)
		free((void*)*pImage);
	*pImage = (DWORD)malloc(w * 3 * h);
	ZeroMemory((void*)*pImage, w * 3 * h);
	*/
	o_ReadPixAndStretch( ecx, edx,pSrcRect, pDstRect, pBuffer,  dstFormat, nDstStride);
	Render::Screen_Shot = false;

	Engine::WriteFile("%s END", __FUNCTION__);
}