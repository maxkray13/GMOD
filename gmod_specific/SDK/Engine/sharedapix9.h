#pragma once
#include "../../../include/stdafx.h"

template< typename Function > Function call_vfunc(PVOID Base, DWORD Index) {
	PDWORD* VTablePointer = (PDWORD*)Base;
	PDWORD VTableFunctionBase = *VTablePointer;
	DWORD dwAddress = VTableFunctionBase[Index];
	return (Function)(dwAddress);
}

class IShaderAPI {
public:
	void ReadPixels(int a1, int a2, int a3, int a4, int a5, int a6) {//g_pShaderAPI + 0x27C CMatRenderContext::ReadPixels
	
		typedef  void(__thiscall* ReadPixelsFn)(int a1, int a2, int a3, int a4, int a5, int a6);
		return call_vfunc<ReadPixelsFn>(this, 159)(a1, a2, a3, a4, a5, a6);
	}

	int ReadPixelsAndStretch(int a1, int a2, int a3, int a4, int a5, int a6) {//g_pShaderAPI + 0x278  CMatRenderContext::ReadPixelsAndStretch
	
		typedef  int(__thiscall* ReadPixelsAndStretchFn)(int a1, int a2, int a3, int a4, int a5, int a6);
		return call_vfunc<ReadPixelsAndStretchFn>(this, 158)(a1, a2, a3, a4, a5, a6);
	}





};