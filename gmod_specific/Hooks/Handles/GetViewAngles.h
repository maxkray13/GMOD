#pragma once
void __fastcall hkGetViewAngles(void* thisptr, void* edx, QAngle& va) {

	g_pPrediction->GetLocalViewAngles(va);
	//va = Hooks::Variables::PostFinalAngle;

	return;
}