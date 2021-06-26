#pragma once
#include "../../engine/Core/Basic/Basic.h"
#include "../SDK.h"
class AntiAim;

class AntiAim:public Basic::Singleton<AntiAim> {
public:
	void AntiAim_Tick(CUserCmd* pCmd, bool* bSendPacket);
	void Create_Delta();



	union PitchAA {
		void Dance();
		void Up();
		void Down();
		void FakeUp();
		void FakeDown();
	}Pitch;

	union YawAA {
		void Backward();
		void Sideways();
		void Spin();
		void Random();
		void Jitter();
		void Lisp();
	}Yaw;
	bool have_delta = false;
	CUserCmd* pCmd;
	bool* bSendPacket;
};
