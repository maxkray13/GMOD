#include "Aimbot.h"
#include "../../engine/Engine.h"
#include "../Hooks/Hooks.h"
#include "../Hooks/Handles/PostDataUpdate.h"
#include "LagCompensation.h"


bool Aimbot::InFOV(QAngle AimAngle, float fov, float & Dist)
{
	Vector ang, aim;
	ang.Init();
	aim.Init();
	pCmd->viewangles.ToVector(&aim);
	AimAngle.ToVector(&ang);
	aim.Normalize();
	Dist = ToDegrees(std::acos(ang.Dot(aim)));

	if (isinf<float>(Dist))
		return true;

	if (isnan<float>(Dist))
		return true;

	return  (Dist <= fov);
}

void Aimbot::BaseSmooth(QAngle & CrossPos, QAngle TargetPos, float SmoothBase)
{
	QAngle AimMove;
	AimMove.Init();

	AimMove = CrossPos - TargetPos;
	AimMove.Normalize();


	CrossPos.m_x -= AimMove.m_x / SmoothBase;
	CrossPos.m_y -= AimMove.m_y / SmoothBase;
}
void AdwSmooth(QAngle & CrossPos, QAngle TargetPos, float SmoothBase)
{
	QAngle AimMove;
	AimMove.Init();

	AimMove = CrossPos - TargetPos;
	AimMove.Normalize();

	auto factor_pitch = abs(SmoothBase + SDK::Function::RandomFloat(-3.f, 0.f)) / 10.f;

	if (AimMove[0] < 0.f)
	{
		if (factor_pitch > std::abs(AimMove[0]))
			factor_pitch = std::abs(AimMove[0]) / 2.f;

		CrossPos[0] += factor_pitch;
	}
	else
	{
		if (factor_pitch > AimMove[0])
			factor_pitch = AimMove[0] / 2.f;

		CrossPos[0] -= factor_pitch;
	}
	auto factor_yaw = abs(SmoothBase + SDK::Function::RandomFloat(-3.f, 0.f)) / 10.f;

	if (AimMove[1] < 0.f)
	{
		if (factor_yaw > std::abs(AimMove[1]))
			factor_yaw = std::abs(AimMove[1]) / 2.f;

		CrossPos[1] += factor_yaw;
	}
	else
	{
		if (factor_yaw > AimMove[1])
			factor_yaw = AimMove[1] / 2.f;

		CrossPos[1] -= factor_yaw;
	}
}

void Aimbot::Drop_Target()
{

	this->State = AimBot_Search;
	this->BestTarget.init();
}

void Aimbot::AimBot_Tick(C_GMOD_Player * ent, CUserCmd * pCmd)
{
	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal || !pLocal->IsGood())
		return;
	if (!ent->IsGood())
		return;
	if (FriendList::Instance().IsFriend(ent))
		return;
	this->pCmd = pCmd;

	if (State == AimBot_ValidTarget)
		return;
	else if (State == AimBot_HaveTarget) {
		if (BestTarget.idx == -1) {
			Drop_Target();
			goto tickstub;
		}
		auto oTarget = C_GMOD_Player::GetGPlayer(BestTarget.idx);
		if (!oTarget->IsGood()) {
			Drop_Target();
			goto tickstub;
		}
		Update_BestTarget(oTarget);
		if (!AimBot_Checks_HaveTarget(oTarget)) {
			Drop_Target();
			goto tickstub;
		}
		State = AimBot_ValidTarget;
		return;

	}
tickstub:;

	AimBot_Checks_Search(ent);
}

void Aimbot::Post_AimBot_Tick(CUserCmd * pCmd, bool & bSendPacket)
{
	if (!Best_Target_Casting.size() && State == AimBot_Search) {
		Drop_Target();
		return;
	}
	if (State == AimBot_Search)
		AimBot_Search_State(pCmd);
	else if (State == AimBot_ValidTarget) {
		if (Engine::Var::Instance().Aim_pSilent.Get()) {
			auto pLocal = C_GMOD_Player::GetLocalPlayer();
			if (!pLocal || !pLocal->IsGood()) {
				Drop_Target();
				return;
			}
			if (!pLocal->m_hActiveWeapon()) {
				Drop_Target();
				return;
			}
			auto weapon = pLocal->GetActiveWeapon();
			if (!weapon->IsValid()) {
				Drop_Target();
				return;
			}
			if (pLocal->WeaponAbleToShot() && (Engine::Var::Instance().Aim_Auto_Shot.Get() || pCmd->buttons & IN_ATTACK)) {
				bSendPacket = false;
				AimBot_ValidTarget_State(pCmd);

			}
			this->State = AimBot_HaveTarget;
		}
		else {
			AimBot_ValidTarget_State(pCmd);
		}
	}

	pCmd->viewangles.Normalize();

	Best_Target_Casting.clear();
}

bool Aimbot::Did_Hit(C_GMOD_Player * pLocal, CUserCmd * pCmd)
{
	Ray_t ray = {};
	trace_t tr = {};
	Vector vEnd = {};
	CTraceFilter traceFilter;
	traceFilter.pSkip = pLocal;
	bool ret = false;

	pCmd->viewangles.ToVector(&vEnd);
	vEnd = pLocal->EyePosition() + (vEnd * MAX_TRACE_LENGTH);

	ray.Init(pLocal->EyePosition(), vEnd);
	if (Engine::Var::Instance().Aim_WallScan.Get() && pLocal->GetActiveWeapon() && pLocal->GetActiveWeapon()->IsValid()) {
		if (Lua::weapon_is_m9k(pLocal->GetActiveWeapon())) {
			C_GMOD_Player* wall_player = NULL;
			if (Lua::m9k_wallscan(pLocal->EyePosition(), vEnd, pLocal->GetActiveWeapon(), wall_player) && wall_player) {
				tr.m_pEnt = wall_player;
			}
			else {
				tr.m_pEnt = NULL;
			}
		}
		else if (Lua::weapon_is_cw2(pLocal->GetActiveWeapon())) {
			C_GMOD_Player* wall_player = NULL;
			if (Lua::cw2_wallscan(pLocal->EyePosition(), vEnd, pLocal->GetActiveWeapon(), wall_player) && wall_player) {
				tr.m_pEnt = wall_player;
			}
			else {
				tr.m_pEnt = NULL;
			}
		}
		else {
			g_pEngineTrace->TraceRay(ray, MASK_SOLID, &traceFilter, &tr);
		}
	}
	else {
		g_pEngineTrace->TraceRay(ray, MASK_SOLID, &traceFilter, &tr);
	}

	if ((tr.m_pEnt && tr.m_pEnt->GetClientClass()->m_ClassID == SDK::ClassId::CGMOD_Player()) ) {

		auto weap = pLocal->GetActiveWeapon();
		if (weap->IsValid()) {


			if (Engine::Var::Instance().Randomize.Get()) {
				ret = true;
			}
			else {
				ret = pLocal->WeaponAbleToShot();
			}


		}
	}

	return ret;
}

void Aimbot::Reset_Aimbot()
{
	Drop_Target();

	if (Best_Target_Casting.size())
		Best_Target_Casting.clear();
}

bool Aimbot::HandleVisible(C_GMOD_Player * pLocal, C_GMOD_Player * pEnemy, Vector pos)
{
	if (!Engine::Var::Instance().Aim_WallScan.Get()) {
		return SDK::Function::IsVisible(pLocal, pEnemy, pos);
	}
	else {
		if (pLocal->GetActiveWeapon() && pLocal->GetActiveWeapon()->IsValid()) {
			if (Lua::weapon_is_m9k(pLocal->GetActiveWeapon())) {
				C_GMOD_Player* wall_player = NULL;
				if (Lua::m9k_wallscan(pLocal->EyePosition(), pos, pLocal->GetActiveWeapon(), wall_player) && wall_player && wall_player == pEnemy) {
					return true;
				}

			}
			else if (Lua::weapon_is_cw2(pLocal->GetActiveWeapon())) {
				C_GMOD_Player* wall_player = NULL;
				if (Lua::cw2_wallscan(pLocal->EyePosition(), pos, pLocal->GetActiveWeapon(), wall_player) && wall_player && wall_player == pEnemy) {
					return true;
				}
			}
		}
	}

	return SDK::Function::IsVisible(pLocal, pEnemy, pos);
}

QAngle Aimbot::GetRecoil(CUserCmd* pCmd, C_GMOD_Player * pLocal)
{
	QAngle AimPunch = pLocal->m_vecPunchAngle();
	QAngle AimPunchVel = pLocal->m_vecPunchAngleVel();

	if (!pLocal)
		return QAngle(0, 0, 0);
	auto pWeap = pLocal->GetActiveWeapon();
	if (!pWeap || !pWeap->IsValid())
		return QAngle(0, 0, 0);

	if (pWeap->GetClientClass()->m_ClassID == SDK::ClassId::CWeaponSWEP()) {
		if (pWeap->GetScriptName()) {
			auto str = std::string(pWeap->GetScriptName());

			if (str.find("fas2") != -1 || Lua::weapon_is_cw2(pWeap) ) {

				//return 0;

				/*
				QAngle Out = {};
				g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
				if (!g_pLuaShitInterface)
					return Out;


				g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
				g_pLuaShitInterface->GetField(-1, "FindMetaTable");
				if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
					g_pLuaShitInterface->Pop(2);
					return Out;
				}
				g_pLuaShitInterface->PushString("Player");
				g_pLuaShitInterface->Call(1, 1);
				if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
					g_pLuaShitInterface->Pop(2);
					return Out;
				}
				g_pLuaShitInterface->GetField(-1, "GetActiveWeapon");
				if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
					g_pLuaShitInterface->Pop(3);
					return Out;
				}
				g_pLuaShitInterface->GetField(-3, "Entity");
				if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
					g_pLuaShitInterface->Pop(4);
					return Out;
				}
				g_pLuaShitInterface->PushNumber(g_pEngine->GetLocalPlayer());
				g_pLuaShitInterface->Call(1, 1);
				if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
					g_pLuaShitInterface->Pop(4);
					return Out;
				}
				g_pLuaShitInterface->Call(1, 1);
				if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
					g_pLuaShitInterface->Pop(3);
					return Out;
				}

				g_pLuaShitInterface->GetField(-1, "Primary");
				if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
					g_pLuaShitInterface->Pop(4);
					return Out;
				}
				g_pLuaShitInterface->GetField(-1, "KickDown");
				if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
					g_pLuaShitInterface->Pop(5);
					return Out;
				}
				auto KickDown = g_pLuaShitInterface->GetNumber(-1);
				g_pLuaShitInterface->Pop(1);

				g_pLuaShitInterface->GetField(-1, "KickUp");
				if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
					g_pLuaShitInterface->Pop(5);
					return Out;
				}
				auto KickUp = g_pLuaShitInterface->GetNumber(-1);
				g_pLuaShitInterface->Pop(1);

				g_pLuaShitInterface->GetField(-1, "KickHorizontal");
				if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
					g_pLuaShitInterface->Pop(5);
					return Out;
				}
				auto KickHorizontal = g_pLuaShitInterface->GetNumber(-1);
				g_pLuaShitInterface->Pop(5);



				g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
				g_pLuaShitInterface->GetField(-1, "math");
				g_pLuaShitInterface->GetField(-1, "randomseed");
				g_pLuaShitInterface->PushNumber(MD5_PseudoRandom(pCmd->command_number) & 0x7fffffff);
				g_pLuaShitInterface->Call(1, 0);
				g_pLuaShitInterface->Pop(2);

				g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
				g_pLuaShitInterface->GetField(-1, "math");
				g_pLuaShitInterface->GetField(-1, "Rand");
				g_pLuaShitInterface->PushNumber(0.85);
				g_pLuaShitInterface->PushNumber(1.3);
				g_pLuaShitInterface->Call(2, 1);
				auto damagedice = g_pLuaShitInterface->GetNumber(-1);
				g_pLuaShitInterface->Pop(3);



				g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
				g_pLuaShitInterface->GetField(-1, "math");
				g_pLuaShitInterface->GetField(-1, "Rand");
				g_pLuaShitInterface->PushNumber(-KickDown);
				g_pLuaShitInterface->PushNumber(-KickUp);
				g_pLuaShitInterface->Call(2, 1);
				auto Pitch = (float)g_pLuaShitInterface->GetNumber(-1);
				g_pLuaShitInterface->Pop(3);
				g_pEngine->Con_NPrintf(0, "Pitch %f", Pitch);
				g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
				g_pLuaShitInterface->GetField(-1, "math");
				g_pLuaShitInterface->GetField(-1, "Rand");
				g_pLuaShitInterface->PushNumber(-KickHorizontal);
				g_pLuaShitInterface->PushNumber(KickHorizontal);
				g_pLuaShitInterface->Call(2, 1);
				auto Yaw = (float)g_pLuaShitInterface->GetNumber(-1);
				g_pEngine->Con_NPrintf(1, "Yaw %f", Yaw);
				g_pLuaShitInterface->Pop(3);

				QAngle LuaShitViewKick = QAngle(Pitch, Yaw, 0.f);
				AimPunchVel *= g_pGlobalVars->interval_per_tick;
				AimPunchVel += LuaShitViewKick * 20.f;*/


				auto next_punch_angle = AimPunch;
				auto next_punch_angle_vel = AimPunchVel;
				next_punch_angle += AimPunchVel * g_pGlobalVars->interval_per_tick;

				float damping = 1.f - (9.0f * g_pGlobalVars->interval_per_tick);
				if (damping < 0)
				{
					damping = 0;
				}

				next_punch_angle_vel *= damping;

				float springForceMagnitude = 65.0f * g_pGlobalVars->interval_per_tick;
				springForceMagnitude = clamp(springForceMagnitude, 0.f, 2.f);
				next_punch_angle_vel -= next_punch_angle * springForceMagnitude;

				return -(next_punch_angle - (next_punch_angle_vel* g_pGlobalVars->interval_per_tick));
			}
		}
	}

	auto next_punch_angle = AimPunch;
	auto next_punch_angle_vel = AimPunchVel;
	next_punch_angle += AimPunchVel * g_pGlobalVars->interval_per_tick;

	float damping = 1 - (9.0f * g_pGlobalVars->interval_per_tick);
	if (damping < 0)
	{
		damping = 0;
	}

	next_punch_angle_vel *= damping;

	float springForceMagnitude = 65.0f * g_pGlobalVars->interval_per_tick;
	springForceMagnitude = clamp(springForceMagnitude, 0.f, 2.f);
	next_punch_angle_vel -= next_punch_angle * springForceMagnitude;


	return -(AimPunch + (AimPunchVel * g_pGlobalVars->interval_per_tick));

}
inline float RemapValClamped(float val, float A, float B, float C, float D)
{
	if (A == B)
		return val >= B ? D : C;
	float cVal = (val - A) / (B - A);
	cVal = clamp(cVal, 0.0f, 1.0f);

	return C + (D - C) * cVal;
}

inline void VectorLerp(const Vector& src1, const Vector& src2, vec_t t, Vector& dest)
{

	dest.m_x = src1.m_x + (src2.m_x - src1.m_x) * t;
	dest.m_y = src1.m_y + (src2.m_y - src1.m_y) * t;
	dest.m_z = src1.m_z + (src2.m_z - src1.m_z) * t;
}

inline Vector VectorLerp(const Vector& src1, const Vector& src2, vec_t t)
{
	Vector result;
	VectorLerp(src1, src2, t, result);
	return result;
}


#define VECTOR_CONE_1DEGREES     Vector( 0.00873, 0.00873, 0.00873 )
#define VECTOR_CONE_2DEGREES     Vector( 0.01745, 0.01745, 0.01745 )
#define VECTOR_CONE_3DEGREES     Vector( 0.02618, 0.02618, 0.02618 )
#define VECTOR_CONE_4DEGREES     Vector( 0.03490, 0.03490, 0.03490 )
#define VECTOR_CONE_5DEGREES     Vector( 0.04362, 0.04362, 0.04362 )
#define VECTOR_CONE_6DEGREES     Vector( 0.05234, 0.05234, 0.05234 )
#define VECTOR_CONE_7DEGREES     Vector( 0.06105, 0.06105, 0.06105 )
#define VECTOR_CONE_8DEGREES     Vector( 0.06976, 0.06976, 0.06976 )
#define VECTOR_CONE_9DEGREES     Vector( 0.07846, 0.07846, 0.07846 )
#define VECTOR_CONE_10DEGREES    Vector( 0.08716, 0.08716, 0.08716 )
#define VECTOR_CONE_15DEGREES    Vector( 0.13053, 0.13053, 0.13053 )
#define VECTOR_CONE_20DEGREES    Vector( 0.17365, 0.17365, 0.17365 )
float SDK::GetLerpTime()
{
	static ConVar * cl_updaterate = nullptr;
	static ConVar *sv_minupdaterate = nullptr;
	static ConVar *sv_maxupdaterate = nullptr;
	static ConVar *cl_interp = nullptr;
	static ConVar *cl_interp_ratio = nullptr;
	static ConVar *c_min_ratio = nullptr;
	static ConVar *c_max_ratio = nullptr;


	if (cl_updaterate&&sv_minupdaterate&&sv_maxupdaterate&&cl_interp&&cl_interp_ratio&&c_min_ratio&&c_max_ratio) {
		int ud_rate = cl_updaterate->intVal;
		ud_rate = clamp(ud_rate, (int)sv_minupdaterate->flVal, (int)sv_maxupdaterate->flVal);

		float flLerpRatio = cl_interp_ratio->intVal;
		float flLerpAmount = cl_interp->flVal;
		if (flLerpRatio == 0)
			flLerpRatio = 1.0f;

		if (c_min_ratio->flVal != -1) {
			flLerpRatio = clamp(flLerpRatio, c_min_ratio->flVal, c_max_ratio->flVal);
		}
		else {
			if (flLerpRatio == 0)
				flLerpRatio = 1.0f;
		}
		return max(flLerpAmount, (flLerpRatio / ud_rate));
	}
	else {
		cl_updaterate = g_pCvar->FindVar("cl_updaterate");
		sv_minupdaterate = g_pCvar->FindVar("sv_minupdaterate");
		sv_maxupdaterate = g_pCvar->FindVar("sv_maxupdaterate");
		cl_interp = g_pCvar->FindVar("cl_interp");
		cl_interp_ratio = g_pCvar->FindVar("cl_interp_ratio");
		c_min_ratio = g_pCvar->FindVar("sv_client_min_interp_ratio");
		c_max_ratio = g_pCvar->FindVar("sv_client_max_interp_ratio");

		return 0.0f;
	}
}

QAngle Aimbot::GetSpread(CUserCmd * pCmd)
{
	float flSpreadx, flSpready;
	Vector vForward, vRight, vUp;
	float flRandomX, flRandomY;

	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal)
		return QAngle(0, 0, 0);
	auto pWeap = pLocal->GetActiveWeapon();
	if (!pWeap || !pWeap->IsValid())
		return QAngle(0, 0, 0);
	if (pWeap->GetClientClass()->m_ClassID == SDK::ClassId::CWeaponSWEP()) {
		if (pWeap->GetScriptName()) {
			auto str = std::string(pWeap->GetScriptName());
			auto lua_base = Lua::GetWeaponBase(pWeap->entindex());
			if (str.find("fas2") != -1) {
				return Lua::fas2_nospread(pLocal, pWeap, pCmd);
			}
			else if (str.find("cw") != -1 || str.find("atow") != -1 || str.find("khr") != -1) {
				return Lua::cw2_nospread(pLocal, pWeap, pCmd);
			}
			else if (lua_base != nullptr) {
				if (std::string(lua_base).find("swb") != -1) {
					return Lua::cw2_nospread(pLocal, pWeap, pCmd);
				}
			}
			/*if (str.find("m9k") != -1) {
				auto uiSeed = MD5_PseudoRandom(pCmd->command_number) & 0x7FFFFFFF;

				SDK::Function::RandomSeed(uiSeed & 255);

				flRandomX = SDK::Function::RandomFloat(-0.5f, 0.5f) + SDK::Function::RandomFloat(-0.5f, 0.5f);
				flRandomY = SDK::Function::RandomFloat(-0.5f, 0.5f) + SDK::Function::RandomFloat(-0.5f, 0.5f);
				flSpreadx = Hooks::Variables::Spread.m_x * flRandomX;
				flSpready = Hooks::Variables::Spread.m_y * flRandomY;
				pCmd->viewangles.ToVector(&vForward, &vRight, &vUp);

				Vector vNewAngles = vForward +
					(vRight * flSpreadx) +
					(vUp * flSpready);

				QAngle res = vNewAngles.ToEulerAngles();

				QAngle Out = pCmd->viewangles - res;

				return Out;
			}*/
			str.clear();
		}
	}


	if (pWeap->GetClientClass()->m_ClassID == SDK::ClassId::CWeaponPistol()) {
		float ramp = RemapValClamped(pWeap->m_flAccuracyPenalty() + 0.2f,
			0.0f,
			1.5f,
			0.0f,
			1.0f);

		VectorLerp(VECTOR_CONE_1DEGREES, VECTOR_CONE_6DEGREES, ramp, Hooks::Variables::Spread);
	}
	auto uiSeed = MD5_PseudoRandom(pCmd->command_number) & 0x7FFFFFFF;
	SDK::Function::RandomSeed(uiSeed & 255);


	static ConVar * ai_shot_bias_min = nullptr;
	static ConVar * ai_shot_bias_max = nullptr;
	if (!ai_shot_bias_min)
		ai_shot_bias_min = g_pCvar->FindVar("ai_shot_bias_min");
	if (!ai_shot_bias_max)
		ai_shot_bias_max = g_pCvar->FindVar("ai_shot_bias_max");


	float shotBiasMin = ai_shot_bias_min->flVal;
	float shotBiasMax = ai_shot_bias_max->flVal;

	float shotBias = ((shotBiasMax - shotBiasMin) * 1.f) + shotBiasMin;

	float flatness = (fabsf(shotBias) * 0.5);

	float x, y, z;
	do
	{
		x = SDK::Function::RandomFloat(-1, 1) * flatness + SDK::Function::RandomFloat(-1, 1) * (1 - flatness);
		y = SDK::Function::RandomFloat(-1, 1) * flatness + SDK::Function::RandomFloat(-1, 1) * (1 - flatness);
		if (shotBias < 0)
		{
			x = (x >= 0) ? 1.0 - x : -1.0 - x;
			y = (y >= 0) ? 1.0 - y : -1.0 - y;
		}
		z = x * x + y * y;
	} while (z > 1);

	flRandomX = x;
	flRandomY = y;

	flSpreadx = Hooks::Variables::Spread.m_x * flRandomX;
	flSpready = Hooks::Variables::Spread.m_y * flRandomY;

	pCmd->viewangles.ToVector(&vForward, &vRight, &vUp);

	Vector vNewAngles = vForward +
		(vRight * flSpreadx) +
		(vUp * flSpready);

	QAngle res = vNewAngles.ToEulerAngles();

	QAngle Out = pCmd->viewangles - res;

	return Out;
}
static std::vector<char*> Bones_to_hitscan;
void Aimbot::Update_BestTarget(C_GMOD_Player * ent)
{

	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal || !pLocal->IsGood() || !ent || !ent->IsGood())
	{
		Drop_Target();
	}
	Vector min = {};
	Vector max = {};
	matrix3x4_t bone_data[MAXSTUDIOBONE] = {};

	if (!ent->GetBoneTransform(bone_data))
	{
		Drop_Target();
		return;
	}

	Vector final_spot;
	Vector direction;
	QAngle angles;
	if (!Bones_to_hitscan.size()) {
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Head1");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Neck1");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Spine");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Spine1");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Spine2");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Spine4");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Clavicle");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Clavicle");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_UpperArm");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_UpperArm");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Forearm");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Forearm");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Hand");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Hand");

		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Thigh");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Thigh");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Calf");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Calf");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Foot");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Foot");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Toe0");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Toe0");

		Bones_to_hitscan.push_back("Mane01");
		Bones_to_hitscan.push_back("Mane02");
		Bones_to_hitscan.push_back("Mane03");
		Bones_to_hitscan.push_back("Mane04");
		Bones_to_hitscan.push_back("Mane05");
		Bones_to_hitscan.push_back("Mane06");
		Bones_to_hitscan.push_back("Mane07");
		Bones_to_hitscan.push_back("LrigNeck2");
		Bones_to_hitscan.push_back("LrigNeck3");
	}

	bool _find_bone = false;
	if (!Engine::Var::Instance().Aim_Hitscan.Get()) {

		_find_bone = true;
		if (!ent->GetBoxWorld(SDK::Valve_Bones[Engine::Var::Instance().Aim_Bone.Get()], bone_data, final_spot))
		{
			Drop_Target();
			return;
		}
		direction = final_spot - pLocal->EyePosition();
		angles = direction.ToEulerAngles();
		angles.Normalize();
	}
	else {
		for (auto it : Bones_to_hitscan) {
			if (!ent->GetBoxWorld(it, bone_data, final_spot))
				continue;

			direction = final_spot - pLocal->EyePosition();
			angles = direction.ToEulerAngles();
			angles.Normalize();

			if (!ent || !HandleVisible(pLocal, ent, final_spot)) {
				continue;
			}
			_find_bone = true;
			break;
		}
	}
	if (!_find_bone) {
		Drop_Target();
		return;
	}

	float dist;
	if (!InFOV(angles, Engine::Var::Instance().Aim_Fov.Get(), dist)) {
		Drop_Target();
		return;
	}

	BestTarget.Angle = angles;
	BestTarget.hp = ent->m_iHealth();

}

void Aimbot::Update_BestTarget(C_GMOD_Player * ent, matrix3x4_t * bones)
{
	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal || !pLocal->IsGood() || !ent || !ent->IsGood())
	{
		Drop_Target();
	}
	Vector min = {};
	Vector max = {};


	Vector final_spot;
	Vector direction;
	QAngle angles;
	if (!Bones_to_hitscan.size()) {
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Head1");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Neck1");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Spine");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Spine1");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Spine2");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Spine4");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Clavicle");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Clavicle");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_UpperArm");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_UpperArm");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Forearm");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Forearm");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Hand");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Hand");

		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Thigh");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Thigh");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Calf");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Calf");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Foot");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Foot");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Toe0");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Toe0");

		Bones_to_hitscan.push_back("Mane01");
		Bones_to_hitscan.push_back("Mane02");
		Bones_to_hitscan.push_back("Mane03");
		Bones_to_hitscan.push_back("Mane04");
		Bones_to_hitscan.push_back("Mane05");
		Bones_to_hitscan.push_back("Mane06");
		Bones_to_hitscan.push_back("Mane07");
		Bones_to_hitscan.push_back("LrigNeck2");
		Bones_to_hitscan.push_back("LrigNeck3");
	}

	bool _find_bone = false;
	if (!Engine::Var::Instance().Aim_Hitscan.Get()) {

		_find_bone = true;
		if (!ent->GetBoxWorld(SDK::Valve_Bones[Engine::Var::Instance().Aim_Bone.Get()], bones, final_spot))
		{
			Drop_Target();
			return;
		}
		direction = final_spot - pLocal->EyePosition();
		angles = direction.ToEulerAngles();
		angles.Normalize();
	}
	else {
		for (auto it : Bones_to_hitscan) {
			if (!ent->GetBoxWorld(it, bones, final_spot))
				continue;

			direction = final_spot - pLocal->EyePosition();
			angles = direction.ToEulerAngles();
			angles.Normalize();

			if (!ent || !HandleVisible(pLocal, ent, final_spot)) {
				continue;
			}
			_find_bone = true;
			break;
		}
	}
	if (!_find_bone) {
		Drop_Target();
		return;
	}

	float dist;
	if (!InFOV(angles, Engine::Var::Instance().Aim_Fov.Get(), dist)) {
		Drop_Target();
		return;
	}

	BestTarget.Angle = angles;
	BestTarget.hp = ent->m_iHealth();
}

void Aimbot::AimBot_Checks_Search(C_GMOD_Player * ent)
{
	Target ñTarget;
	float Dist;
	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal || !pLocal->IsGood() || !ent || !ent->IsGood())
	{
		return;
	}

	matrix3x4_t transform[MAXSTUDIOBONE];
	if (!ent->GetBoneTransform(transform))
		return;
	Vector final_spot;
	Vector direction;
	QAngle angles;


	if (!Bones_to_hitscan.size()) {
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Head1");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Neck1");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Spine");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Spine1");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Spine2");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Spine4");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Clavicle");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Clavicle");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_UpperArm");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_UpperArm");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Forearm");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Forearm");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Hand");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Hand");


		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Thigh");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Thigh");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Calf");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Calf");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Foot");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Foot");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Toe0");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Toe0");

		Bones_to_hitscan.push_back("Mane01");
		Bones_to_hitscan.push_back("Mane02");
		Bones_to_hitscan.push_back("Mane03");
		Bones_to_hitscan.push_back("Mane04");
		Bones_to_hitscan.push_back("Mane05");
		Bones_to_hitscan.push_back("Mane06");
		Bones_to_hitscan.push_back("Mane07");
		Bones_to_hitscan.push_back("LrigNeck2");
		Bones_to_hitscan.push_back("LrigNeck3");
	}
	bool _find_bone = false;
	if (!Engine::Var::Instance().Aim_Hitscan.Get()) {
		_find_bone = true;
		if (!ent->GetBoxWorld(SDK::Valve_Bones[Engine::Var::Instance().Aim_Bone.Get()], transform, final_spot))
			return;

		direction = final_spot - pLocal->EyePosition();
		angles = direction.ToEulerAngles();
		angles.Normalize();

		if (!ent || !HandleVisible(pLocal, ent, final_spot)) {
			return;
		}
	}
	else {
		for (auto it : Bones_to_hitscan) {
			if (!ent->GetBoxWorld(it, transform, final_spot))
				continue;

			direction = final_spot - pLocal->EyePosition();
			angles = direction.ToEulerAngles();
			angles.Normalize();

			if (!ent || !HandleVisible(pLocal, ent, final_spot)) {
				continue;
			}
			_find_bone = true;
			break;
		}
	}
	if (!_find_bone)
		return;
	if (!InFOV(angles, Engine::Var::Instance().Aim_Fov.Get(), Dist)) {
		return;
	}
	ñTarget.Angle = angles;
	ñTarget.Dist = Dist;
	ñTarget.hp = ent->m_iHealth();
	ñTarget.idx = ent->entindex();
	ñTarget.valid = false;
	Best_Target_Casting.push_back(ñTarget);
	return;


}

bool Aimbot::AimBot_Checks_HaveTarget(C_GMOD_Player * ent)
{

	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal || !pLocal->IsGood() || !ent || !ent->IsGood())
	{
		Drop_Target();
		return false;
	}
	Vector min = {};
	Vector max = {};
	matrix3x4_t bone_data[MAXSTUDIOBONE] = {};

	if (!ent->SetupBones(bone_data, MAXSTUDIOBONE, 0x0100, 0)) {
		Drop_Target();
		return false;
	}
	Vector final_spot;
	Vector direction;
	QAngle angles;

	if (!Bones_to_hitscan.size()) {
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Head1");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Neck1");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Spine");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Spine1");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Spine2");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_Spine4");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Clavicle");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Clavicle");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_UpperArm");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_UpperArm");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Forearm");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Forearm");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Hand");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Hand");


		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Thigh");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Thigh");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Calf");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Calf");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Foot");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Foot");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_R_Toe0");
		Bones_to_hitscan.push_back("ValveBiped.Bip01_L_Toe0");

		Bones_to_hitscan.push_back("Mane01");
		Bones_to_hitscan.push_back("Mane02");
		Bones_to_hitscan.push_back("Mane03");
		Bones_to_hitscan.push_back("Mane04");
		Bones_to_hitscan.push_back("Mane05");
		Bones_to_hitscan.push_back("Mane06");
		Bones_to_hitscan.push_back("Mane07");
		Bones_to_hitscan.push_back("LrigNeck2");
		Bones_to_hitscan.push_back("LrigNeck3");
	}
	bool _find_bone = false;

	if (!Engine::Var::Instance().Aim_Hitscan.Get()) {
		_find_bone = true;
		if (!ent->GetBoxBoundWorld(SDK::Valve_Bones[Engine::Var::Instance().Aim_Bone.Get()], bone_data, min, max)) {
			Drop_Target();
			return false;
		}
		final_spot = (min + max) * 0.5f;
		direction = final_spot - pLocal->EyePosition();
		angles = direction.ToEulerAngles();
		angles.Normalize();

		if (!HandleVisible(pLocal, ent, final_spot)) {
			Drop_Target();
			return false;
		}
	}
	else {
		_find_bone = false;
		for (auto it : Bones_to_hitscan) {
			if (!ent->GetBoxWorld(it, bone_data, final_spot))
				continue;

			direction = final_spot - pLocal->EyePosition();
			angles = direction.ToEulerAngles();
			angles.Normalize();

			if (!ent || !HandleVisible(pLocal, ent, final_spot)) {
				continue;
			}
			_find_bone = true;
			break;
		}
	}
	if (!_find_bone)
		return false;
	float Dist;
	if (!InFOV(angles, Engine::Var::Instance().Aim_Fov.Get(), Dist)) {
		Drop_Target();
		return false;
	}

	auto Target = C_GMOD_Player::GetGPlayer(BestTarget.idx);
	if (!Target || !Target->IsGood())
	{
		Drop_Target();
		return false;
	}

	return true;
}

void Aimbot::AimBot_Search_State(CUserCmd * pCmd)
{
	BestTarget.init();
	if (Best_Target_Casting.size() == 0) {
		return;
	}
	if (Engine::Var::Instance().Aim_Priority.Get() == 2)//2 Dist
		for (auto it : Best_Target_Casting) {

			if (it.Dist <= BestTarget.Dist)
				BestTarget = it;
		}
	else if (Engine::Var::Instance().Aim_Priority.Get() == 1)//1 hp
		for (auto it : Best_Target_Casting) {

			if (it.hp <= BestTarget.hp)
				BestTarget = it;
		}
	else//0 random
		BestTarget = Best_Target_Casting[rand() % Best_Target_Casting.size()];

	BestTarget.Valid();

	this->State = AimBot_HaveTarget;
}

void Aimbot::AimBot_ValidTarget_State(CUserCmd * pCmd)
{
	auto nerd = C_GMOD_Player::GetGPlayer(BestTarget.idx);
	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal || !pLocal->IsGood() || !nerd || IsBadReadPtr(nerd, 0x4)) {
		Drop_Target();
		return;
	}
	Update_BestTarget(nerd);
	if (Engine::Var::Instance().Aim_LagCompensation.Get()) {
		bool do_aim = false;
		tick target_tick = {};
		if (g_LagCompensation.StartLagCompensation(nerd, target_tick)) {
			do_aim = true;
		}
		if (!do_aim) {
			//pCmd->buttons &= ~IN_ATTACK;
			return;
		}
		else {
			nerd->InvalidateBoneCache();
		}
		Update_BestTarget(nerd);

		//auto saved_tick_count = pCmd->tick_count;
		//g_pEngine->ClientCmd(std::string("echo saved_tick_count " + std::to_string(saved_tick_count)).c_str());
		pCmd->tick_count = TIME_TO_TICKS(target_tick.m_flSimulationTime + SDK::GetLerpTime());
		//g_pEngine->ClientCmd(std::string("echo sim_time " + std::to_string(nerd->m_flSimulationTime())).c_str());
		//g_pEngine->ClientCmd(std::string("echo lerp_time " + std::to_string(+SDK::GetLerpTime())).c_str());
		//g_pEngine->ClientCmd(std::string("echo curr_tick_count " + std::to_string(pCmd->tick_count)).c_str());
		//g_pEngine->ClientCmd(std::string("echo diff " + std::to_string(saved_tick_count - pCmd->tick_count)).c_str());
	}


	if (Engine::Var::Instance().Aim_Silent.Get() || Engine::Var::Instance().Aim_pSilent.Get()) {
		BaseSmooth(pCmd->viewangles, BestTarget.Angle, 1.f);
	}
	else {
		auto smuch = Engine::Var::Instance().Aim_Smooth.Get();
		if (Engine::Var::Instance().Randomize.Get()) {
			SDK::Function::RandomSeed(pCmd->command_number);
			AdwSmooth(pCmd->viewangles, BestTarget.Angle, smuch);
		}
		else {
			BaseSmooth(pCmd->viewangles, BestTarget.Angle, smuch);
		}

	}
	auto eq_latency = [](QAngle one, QAngle two, float latency)->bool {
		return (abs(two[0] - one[0]) <= latency &&
			abs(two[1] - one[1]) <= latency &&
			abs(two[2] - one[2]) <= latency);
	};






	if (Engine::Var::Instance().Aim_Auto_Shot.Get()) {
		if ((Did_Hit(pLocal,//&& eq_latency(pCmd->viewangles,BestTarget.Angle,1.f)
			pCmd)) || Engine::Var::Instance().Aim_pSilent.Get() || Engine::Var::Instance().Aim_Silent.Get() && pLocal->WeaponAbleToShot())
			pCmd->buttons |= IN_ATTACK;
	}
	if (Engine::Var::Instance().Aim_LagCompensation.Get()) {
		g_LagCompensation.FinishLagCompensation(nerd);
	}

	this->State = AimBot_HaveTarget;
	return;

}
