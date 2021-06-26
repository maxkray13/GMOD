#include "Misc.h"
#include "../SDK.h"
#include "../Hooks/Hooks.h"


void Misc::Misc_Tick(CUserCmd * pCmd) {
	this->pCmd = pCmd;

	if (!g_pEngine->GetNetChannelInfo())
		return;
	this->nci = g_pEngine->GetNetChannelInfo();
	
	Local = C_GMOD_Player::GetLocalPlayer();
	if (!Local)
		return;

	/*if (Engine::Var::Instance().Slow_Motion.Get()) {
		SlowMo();
	}*/
	if (Engine::Var::Instance().Bhop.Get()) {
		Bhop();
	}
	if (Engine::Var::Instance().Ground_Strafe.Get()) {
		Ground_Strafe();
	}
	if (Engine::Var::Instance().Achievement_Spam.Get()) {
		Achievement_Spam();
	}
	if (Engine::Var::Instance().Strafe_Helper.Get()) {
		Strafe_Helper();
	}


}
inline void RotateVector(Vector& dst, float Deg) {

	float RadRot = DEG2RAD(Deg);

	dst.m_x = dst.m_x*cos(RadRot) - dst.m_y*sin(RadRot);
	dst.m_y = dst.m_x*sin(RadRot) + dst.m_y*cos(RadRot);
}
void Misc::SlowMo() {
	static int Slowmo_Do = 0;
	static int saveval_Slow_mo = 0;
	if (GetAsyncKeyState(Engine::Var::Instance().Slow_Motion_Key.Get()) && !saveval_Slow_mo) {//E
		nci->SetTimeout(-1.337f);
		saveval_Slow_mo = nci->m_nOutSequenceNr;
		nci->m_nOutSequenceNr = -1;
		Slowmo_Do = 10;
	}
	if (saveval_Slow_mo && !Slowmo_Do)
	{
		nci->m_nOutSequenceNrAck = saveval_Slow_mo + 40;
		nci->m_nOutSequenceNr = saveval_Slow_mo + 40;
		saveval_Slow_mo = 0;
	}
	if (Slowmo_Do > 0)
		Slowmo_Do--;
}

void Misc::Teleport_Walk() {
	static bool Teleport_Delta = false;

	//if (GetAsyncKeyState(Engine::Var::Instance().Teleport_Walk_Key.Get())) {
		nci->SetTimeout(-1.337f);
		nci->m_nOutSequenceNr += 500;
		Teleport_Delta ? nci->m_nInSequenceNr -= 10 : nci->m_nInSequenceNr += 11;
		Teleport_Delta ^= 1;
//	}

}

void Misc::Bhop() {
	if (!Engine::Var::Instance().Bhop_Safe_Mod.Get()) {
		//Engine::Var::Instance()
		if (pCmd->buttons & IN_JUMP && !(Local->m_fFlags() & FL_ONGROUND))
			pCmd->buttons &= ~IN_JUMP;

	}
	else {

		if (pCmd->buttons & IN_JUMP) {
			static float last_jump_time;
			
			if ((Local->m_fFlags() & FL_ONGROUND) ) {

				if (rand()%2==1) {
				//	pCmd->buttons &= IN_JUMP;
					last_jump_time = g_pGlobalVars->curtime;

				}
				else {
					pCmd->buttons &= ~IN_JUMP;
				}
				
				
			}
			else {
				if (rand() % 3 >= 1) {
					pCmd->buttons &= ~IN_JUMP;
				}
			}

		}
	}
}
inline void RotateRelZero(float& x, float& y, const float Deg) {
	float RadRot = ToRadians(Deg);
	auto o_x = x;
	auto o_y = y;
	x = o_x * cos(RadRot) - o_y * sin(RadRot);
	y = o_x * sin(RadRot) + o_y * cos(RadRot);
}

static bool swit = false;
void Misc::Ground_Strafe() {
	char buff[260];

	if (Local->m_fFlags() & FL_ONGROUND) {
		Vector Move;
		Move.Init(pCmd->forwardmove, pCmd->sidemove);
		enum move_dir{
			forward,
			back,
			left,
			right,
			forward_left,
			forward_right,
			back_left,
			back_right,
			nil
		};
		move_dir dir;
		if (pCmd->forwardmove > 0.f && pCmd->sidemove == 0.f)
			dir = forward;
		else if (pCmd->forwardmove < 0.f && pCmd->sidemove == 0.f)
			dir = back;
		else if (pCmd->forwardmove == 0.f && pCmd->sidemove > 0.f)
			dir = right;
		else if (pCmd->forwardmove == 0.f && pCmd->sidemove < 0.f)
			dir = left;
		else if (pCmd->forwardmove > 0.f && pCmd->sidemove < 0.f)
			dir = forward_left;
		else if (pCmd->forwardmove > 0.f && pCmd->sidemove > 0.f)
			dir = forward_right;
		else if (pCmd->forwardmove < 0.f && pCmd->sidemove > 0.f)
			dir = back_right;
		else if (pCmd->forwardmove < 0.f && pCmd->sidemove < 0.f)
			dir = back_left;
		else
			dir = nil;


		if (pCmd->command_number % 2 == 1) {
			RotateRelZero(Move.m_x, Move.m_y, 30.f);
		}
		else if (pCmd->command_number % 2 == 0) {
			RotateRelZero(Move.m_x, Move.m_y, -30.f);
		}
		switch (dir)
		{
		case forward:
			Move.m_x = 10000.f;
			break;
		case back:
			Move.m_x = -10000.f;
			break;
		case left:
			Move.m_y = -10000.f;
			break;
		case right:
			Move.m_y = 10000.f;
			break;
		case forward_left:
			Move *= 0.73205080757125595715910735037473;
			break;
		case forward_right:
			Move *= 0.73205080757125595715910735037473;
			break;
		case back_left:
			Move *= 0.73205080757125595715910735037473;
			break;
		case back_right:
			Move *= 0.73205080757125595715910735037473;
			break;
		case nil:
			break;
		default:
			break;
		}


		pCmd->forwardmove = Move.m_x;
		pCmd->sidemove = Move.m_y;
	}

}

void Misc::Achievement_Spam() {
	auto key = g_KeyValues.Constructor(g_KeyValues.Operator_New(), "AchievementEarned");
	g_KeyValues.SetInt(key, "achievementID", 1 + rand() % 29);//29 Achievement 0 - NULL_FAKE
	g_pEngine->ServerCmdKeyValues(key);
}

void Misc::Strafe_Helper() {
	if (Local->GetMoveType() != MOVETYPE_NOCLIP && Local->GetMoveType() != MOVETYPE_LADDER && Local->GetMoveType() != MOVETYPE_OBSERVER)
		if (!(Local->m_fFlags() & FL_ONGROUND) &&
			!((pCmd->buttons & IN_MOVELEFT) || (pCmd->buttons & IN_MOVERIGHT))) {//pCmd->buttons & IN_FORWARD || pCmd->buttons & IN_BACK ||
			if (!(Local->m_fFlags() & FL_ONGROUND)) {
				if (pCmd->mousedx < 0) pCmd->sidemove = -10000.f;
				if (pCmd->mousedx > 0)  pCmd->sidemove = 10000.f;
			}
		}
}


void Misc::AutoStrafe(CUserCmd* cmd,C_GMOD_Player* pLocal)
{
	if (pLocal->GetMoveType() == MOVETYPE_NOCLIP || pLocal->GetMoveType() == MOVETYPE_LADDER || !pLocal->IsGood()) return;

	if (!g_pInputSystem->IsButtonDown(ButtonCode_t::KEY_SPACE) ||
		g_pInputSystem->IsButtonDown(ButtonCode_t::KEY_A) ||
		g_pInputSystem->IsButtonDown(ButtonCode_t::KEY_D) ||
		g_pInputSystem->IsButtonDown(ButtonCode_t::KEY_S) ||
		g_pInputSystem->IsButtonDown(ButtonCode_t::KEY_W))
		return;

	if (!(pLocal->m_fFlags() & FL_ONGROUND) ) {
		if (cmd->mousedx > 1 || cmd->mousedx < -1) {
			cmd->sidemove = cmd->mousedx < 0.f ? -10000.f : 10000.f;
		}
		else {
			Vector vel = {};//4500
			pLocal->EstimateAbsVelocity(vel);
			auto speed = vel.Length2D();
			if (!speed)
				speed = 0.001f;
			cmd->forwardmove = (36000.f * 4.f) / speed;
			cmd->sidemove = (cmd->command_number % 2) == 0 ? -10000.f : 10000.f;

			cmd->forwardmove = clamp(cmd->forwardmove, -10000.f, 10000.f);
			cmd->sidemove = clamp(cmd->sidemove, -10000.f, 10000.f);
		}
	}
	else if (pLocal->m_fFlags() & FL_ONGROUND && pCmd->buttons & IN_JUMP) {
		if (cmd->mousedx == 0) {
			cmd->forwardmove = 10000.f;
		}
	}


}
