#pragma once
#include "../../engine/Core/Basic/Basic.h"
#include "../SDK.h"

class Misc : public Basic::Singleton<Misc> {
public:
	void Misc_Tick(CUserCmd* pCmd);
	void Bhop();
	void AutoStrafe(CUserCmd* cmd, C_GMOD_Player* pLocal);
private:
	void SlowMo();
	void Teleport_Walk();

	void Ground_Strafe();
	void Achievement_Spam();
	void Strafe_Helper();

	CUserCmd* pCmd;
	CNetChan	* nci;
	C_GMOD_Player* Local;

};