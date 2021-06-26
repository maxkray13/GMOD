#include "MovementRecorder.h"
#include "../../engine/Engine.h"
MovementRecorder g_MovementRecorder;
void MovementRecorder::StartRecording() {
	Engine::Var::Instance().CurrStateMovementRecorder.Get() = RECORDING;
	if (commands.size()) {
		Clear();
	}
	curr_pos = 0ull;
}

void MovementRecorder::StartPlaybacking() {
	Engine::Var::Instance().CurrStateMovementRecorder.Get() = PLAYBACKING;
	curr_pos = 0ull;
}

void MovementRecorder::Disable() {
	Engine::Var::Instance().CurrStateMovementRecorder.Get() = DISABLED;
	curr_pos = 0ull;
}

void MovementRecorder::Clear() {
	commands.clear();
	start_loc = {};
	end_loc = {};
}

void MovementRecorder::RecordCB(C_GMOD_Player * p_loc, CUserCmd * pCmd) {
	if (Engine::Var::Instance().CurrStateMovementRecorder.Get() == RECORDING) {
		if (!commands.size()) {
			start_loc = p_loc->EyePosition();
		}
		shUserCmd s_cmd = *pCmd;
		
		s_cmd.Origin = p_loc->m_vecOrigin();
		commands.push_back(s_cmd);
		end_loc = p_loc->EyePosition();
	}
}

void MovementRecorder::PlaybackCB(C_GMOD_Player * p_loc, CUserCmd * pCmd) {
	if (Engine::Var::Instance().CurrStateMovementRecorder.Get() == PLAYBACKING && commands.size()) {
		auto eq_latency = [](Vector one, Vector two, float latency)->bool {
			return (abs(two[0] - one[0]) <= latency &&
				abs(two[1] - one[1]) <= latency);
		};
		if (curr_pos == 0ull && !eq_latency(p_loc->EyePosition(), start_loc, 5.f)) {
			auto direction = start_loc - p_loc->EyePosition();
			auto angles = direction.ToEulerAngles();
			angles.Normalize();
			pCmd->viewangles = angles;
			auto f_mov = direction.Length() + 10.f;
			if (f_mov > 10000.f) {
				f_mov = 10000.f;
			}
			else if (f_mov < 100.f) {
				pCmd->buttons |= IN_DUCK;
			}
			pCmd->forwardmove = f_mov;
			
			return;
		}
		if (curr_pos + 1 < commands.size()) {

			pCmd->viewangles = commands[curr_pos].viewangles;
			pCmd->forwardmove = commands[curr_pos].forwardmove;
			pCmd->sidemove = commands[curr_pos].sidemove;
			pCmd->upmove = commands[curr_pos].upmove;
			pCmd->buttons |= commands[curr_pos].buttons;
			pCmd->weaponselect = commands[curr_pos].weaponselect;
			pCmd->weaponsubtype = commands[curr_pos].weaponsubtype;
			pCmd->mousedx = commands[curr_pos].mousedx;
			pCmd->mousedy = commands[curr_pos].mousedy;
			++curr_pos;
		}
		else if (Engine::Var::Instance().LoopMovementRecorder.Get()) {

			if (eq_latency(p_loc->EyePosition(), end_loc, 1.f)) {
				curr_pos = 0ull;
			}
			else {
				auto direction = end_loc - p_loc->EyePosition();
				auto angles = direction.ToEulerAngles();
				angles.Normalize();
				pCmd->viewangles = angles;
				auto f_mov = direction.Length() + 10.f;
				if (f_mov > 10000.f) {
					f_mov = 10000.f;
				}
				else if (f_mov < 100.f) {
					pCmd->buttons |= IN_DUCK;
				}
				pCmd->forwardmove = f_mov;
			}
		}
		else {
			Engine::Var::Instance().CurrStateMovementRecorder.Get() = DISABLED;
			curr_pos = 0ull;
		}
	}
}