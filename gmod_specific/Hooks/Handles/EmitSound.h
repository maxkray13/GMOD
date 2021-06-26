#pragma once
void __fastcall hkEmitSound4(void*thisptr,void* edx,IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample,//4
	float flVolume, float flAttenuation, int iFlags = 0, int iPitch = PITCH_NORM, int iSpecialDSP = 0,
	const Vector *pOrigin = NULL, const Vector *pDirection = NULL, CUtlVector< Vector >* pUtlVecOrigins = NULL, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1) {


	if (strstr(pSample, "null")) {
		iFlags &= SND_STOP;
		iPitch = 256;
		pSample = "";
	}
		


	Hooks::VEngineSound.GetOriginalMethod<decltype(&hkEmitSound4)>(4)(thisptr, edx, filter, iEntIndex, iChannel, pSample, flVolume, flAttenuation, iFlags, iPitch, iSpecialDSP, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity);
}


void __fastcall hkEmitSound5(void*thisptr, void* edx, IRecipientFilter& filter, int iEntIndex, int iChannel, const char *pSample,
	float flVolume, soundlevel_t iSoundlevel, int iFlags = 0, int iPitch = PITCH_NORM, int iSpecialDSP = 0,
	const Vector *pOrigin = NULL, const Vector *pDirection = NULL, CUtlVector< Vector >* pUtlVecOrigins = NULL, bool bUpdatePositions = true, float soundtime = 0.0f, int speakerentity = -1)
{

	if (strstr(pSample, "null")) {
		iFlags &= SND_STOP;
		iPitch = 256;
		pSample = "";
	}

	Hooks::VEngineSound.GetOriginalMethod<decltype(&hkEmitSound5)>(5)(thisptr, edx, filter, iEntIndex, iChannel, pSample, flVolume, iSoundlevel, iFlags, iPitch, iSpecialDSP, pOrigin, pDirection, pUtlVecOrigins, bUpdatePositions, soundtime, speakerentity);

}