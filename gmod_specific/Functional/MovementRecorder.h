#pragma once
#include "../../engine/Core/Basic/Basic.h"
#include "../SDK.h"
enum state : int {
	DISABLED,
	RECORDING,
	PLAYBACKING
};
struct shUserCmd {

	shUserCmd(const CUserCmd& src) {
		viewangles = src.viewangles;
		forwardmove = src.forwardmove;
		sidemove = src.sidemove;
		upmove = src.upmove;
		buttons = src.buttons;
		weaponselect = src.weaponselect;
		weaponsubtype = src.weaponsubtype;
		mousedx = src.mousedx;
		mousedy = src.mousedy;
	}

	shUserCmd& operator = (const CUserCmd& src){
		viewangles = src.viewangles;
		forwardmove = src.forwardmove;
		sidemove = src.sidemove;
		upmove = src.upmove;
		buttons = src.buttons;
		weaponselect = src.weaponselect;
		weaponsubtype = src.weaponsubtype;
		mousedx = src.mousedx;
		mousedy = src.mousedy;
	}
	Vector	Origin;
	QAngle	viewangles;
	float	forwardmove;
	float	sidemove;
	float	upmove;
	int		buttons;
	int		weaponselect;
	int		weaponsubtype;
	short	mousedx;
	short	mousedy;
};
class MovementRecorder {
public:
	void StartRecording();
	void StartPlaybacking();
	void Disable();
	void Clear();
	void RecordCB(C_GMOD_Player* p_loc, CUserCmd * pCmd);
	void PlaybackCB(C_GMOD_Player* p_loc, CUserCmd * pCmd);

	std::vector<shUserCmd>commands;
private:
	Vector start_loc;
	Vector end_loc;
	
	uint64_t curr_pos;
};
extern MovementRecorder g_MovementRecorder;

