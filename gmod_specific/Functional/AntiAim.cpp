#include "AntiAim.h"
#include "../../engine/Engine.h"
#include <chrono> 
#include "../Hooks/Hooks.h"
enum AAPitch {
	Dance = 1,
	Up,
	Down,
	FakeUp,
	FakeDown,
};
enum AAYaw {
	Backward = 1,
	Sideways,
	Spin,
	Random,
	Jitter,
	Lisp
};
enum AAFakePitch {
	fNone,
	fDance,
	fUp,
	fDown,
	fFakeUp,
	fFakeDown,
};
enum AAFakeYaw {
	fNoney,
	fBackward,
	fSideways,
	fSpin,
	fRandom,
	fJitter
	//	fLisp
};
int random_int(int min, int max)
{
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(min, max);

	return dis(gen);
}

void AntiAim::AntiAim_Tick(CUserCmd * pCmd, bool* bSendPacket) {
	this->pCmd = pCmd;
	this->bSendPacket = bSendPacket;
	Create_Delta();
	//360000.00

	auto local = C_GMOD_Player::GetLocalPlayer();

	if (Engine::Var::Instance().AntiAim_Enable.Get() && local) {

		if (!Engine::Var::Instance().Randomize.Get()) {
			if (local->WeaponAbleToShot() && pCmd->buttons&IN_ATTACK) {}
			else {
				pCmd->buttons &= ~IN_ATTACK;
			}
		}

		if (!(pCmd->buttons& IN_ATTACK) && !(pCmd->buttons& IN_USE)) {
			
			srand(time(NULL));
			rand();
			rand();
			rand();
			rand();
		//	pCmd->viewangles[PITCH] += rand() % 6;
		//	pCmd->viewangles[YAW] += rand() % 6;
			pCmd->viewangles.Normalize();


			
			if (have_delta&&*this->bSendPacket == true && (Engine::Var::Instance().AntiAim_Fake_Yaw.Get() > 0)) {
				pCmd->viewangles[YAW] += Engine::Var::Instance().AntiAim_Fake_Yaw_Add.Get();
				pCmd->viewangles.Normalize();
			}
			switch (Engine::Var::Instance().AntiAim_Pitch.Get())
			{
			case Dance:
				Pitch.Dance();
				break;
			case Up:
				Pitch.Up();
				break;
			case Down:
				Pitch.Down();
				break;
			case FakeUp:
				Pitch.FakeUp();
				break;
			case FakeDown:
				Pitch.FakeDown();
				break;
			default:
				break;
			}
			if ((have_delta&&*this->bSendPacket == false) || !have_delta) {

				//if (!have_delta) {
					switch (Engine::Var::Instance().AntiAim_Yaw.Get())
					{
					case Backward:
						Yaw.Backward();
						break;
					case Sideways:
						Yaw.Sideways();
						break;
					case Spin:
						Yaw.Spin();
						break;
					case Random:
						Yaw.Random();
						break;
					case Jitter:
						Yaw.Jitter();
						break;
					case Lisp:
						Yaw.Lisp();
						break;
					default:
						break;
					}
			/*	}
				else {
					switch (Engine::Var::Instance().AntiAim_Fake_Yaw.Get())
					{
					case fBackward:
						Yaw.Backward();
						break;
					case fSideways:
						Yaw.Sideways();
						break;
					case fSpin:
						Yaw.Spin();
						break;
					case fRandom:
						Yaw.Random();
						break;
					case fJitter:
						Yaw.Jitter();
						break;
					default:
						break;
					}
				}*/
			}
			else if(have_delta&&*this->bSendPacket == true){

				switch (Engine::Var::Instance().AntiAim_Fake_Yaw.Get())
				{
				case fBackward:
					Yaw.Backward();
					break;
				case fSideways:
					Yaw.Sideways();
					break;
				case fSpin:
					Yaw.Spin();
					break;
				case fRandom:
					Yaw.Random();
					break;
				case fJitter:
					Yaw.Jitter();
					break;
				default:
					break;
				}
			}

			if (Engine::Var::Instance().AntiAim_CAC_Safe.Get()) {

				SDK::Function::RandomSeed(pCmd->command_number);
				pCmd->viewangles[YAW] += SDK::Function::RandomFloat(-1.5f, 1.5f);
				pCmd->mousedx = pCmd->command_number % 2 == 0 ? -1 * rand() % 100 : 1 * rand() % 100;
				pCmd->mousedy = pCmd->command_number % 2 == 0 ? -1 * rand() % 100 : 1 * rand() % 100;
				pCmd->viewangles.Normalize();

			}
		}

	}
}

void AntiAim::Create_Delta() {
	if (!Engine::Var::Instance().Fake_Lag.Get() && (Engine::Var::Instance().AntiAim_Fake_Yaw.Get() == 0)) {
		have_delta = false;
		return;
	}
	have_delta = true;
	static int delta_counter = 0;

	if (this->pCmd->command_number % 2 == 0) {
		*this->bSendPacket = true;
	}
	else {
		*this->bSendPacket = false;
	}

	if (Engine::Var::Instance().Fake_Lag.Get() && !delta_counter) {
		*this->bSendPacket = true;
		delta_counter = Engine::Var::Instance().Fake_Lag.Get();
		return;
	}
	else if (Engine::Var::Instance().Fake_Lag.Get() && delta_counter) {
		*this->bSendPacket = false;
		delta_counter--;
		return;
	}
	else {
		delta_counter = 0;
	}







}

void AntiAim::PitchAA::Dance() {

	if (random_int(0, 1) == 1) {
		AntiAim::Instance().pCmd->viewangles[PITCH] = 89.f;
	}
	else {
		AntiAim::Instance().pCmd->viewangles[PITCH] = -89.f;
	}

}

void AntiAim::PitchAA::Up() {
	AntiAim::Instance().pCmd->viewangles[PITCH] = -89.f;
}

void AntiAim::PitchAA::Down() {
	if (Engine::Var::Instance().AntiAim_CAC_Safe.Get()) {
		AntiAim::Instance().pCmd->viewangles[PITCH] = 69.69f;
	}
	else {
		AntiAim::Instance().pCmd->viewangles[PITCH] = 89.f;
	}
}

void AntiAim::PitchAA::FakeUp() {
	AntiAim::Instance().pCmd->viewangles[PITCH] = SDK::Function::RandomFloat(-182.f, -192.f);
}

void AntiAim::PitchAA::FakeDown() {

	AntiAim::Instance().pCmd->viewangles[PITCH] = -540.0f;
}



void AntiAim::YawAA::Backward() {
	AntiAim::Instance().pCmd->viewangles[YAW] -= 180.f;
}

void AntiAim::YawAA::Sideways() {
	AntiAim::Instance().pCmd->viewangles[YAW] -= 90.f;
}

void AntiAim::YawAA::Spin() {
	static float add = 0.f;
	float speed = 5.165217391309513f;
	//	float Step = (1.f / speed)*g_pCGlobalVars->frametime;
	add += speed;
	if (add >= 360.f)
		add /= 360.f;
	AntiAim::Instance().pCmd->viewangles[YAW] += add;

}

void AntiAim::YawAA::Random() {
	SDK::Function::RandomSeed(AntiAim::Instance().pCmd->command_number);
	AntiAim::Instance().pCmd->viewangles[YAW] = SDK::Function::RandomFloat(-360.f, 360.f);
}

void AntiAim::YawAA::Jitter() {
	static bool do_it = false;
	if (do_it == 1) {
		AntiAim::Instance().pCmd->viewangles[YAW] -= 90.f;
	}
	else {
		AntiAim::Instance().pCmd->viewangles[YAW] += 90.f;
	}
	do_it ^= 1;
}

void AntiAim::YawAA::Lisp() {

	//	g_AntiAim.pCmd->viewangles.y = 180.f + 36000000.0f;

	if (random_int(0, 1) == 1) {
		AntiAim::Instance().pCmd->viewangles[YAW] = 1260.0f;
	}
	else {
		AntiAim::Instance().pCmd->viewangles[YAW] = -1260.0f;
	}
}
