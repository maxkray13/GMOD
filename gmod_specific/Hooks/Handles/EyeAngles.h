#pragma once
using EyeAngles_fn = QAngle &(__thiscall*)(C_GMOD_Player *);
EyeAngles_fn oEyeAngles_;
static QAngle PostFinalAngle;
QAngle & __fastcall hkEyeAngles(C_GMOD_Player * ply,void*edx) {

	if (ply->IsValid() && ply->IsLocal()) {
		return PostFinalAngle;

	}
	else {
		return oEyeAngles_(ply);
	}


}