#pragma once
#include "../../engine/Core/Basic/Basic.h"
#include "../SDK.h"
#include "FriendList.h"
class Aimbot :public Basic::Singleton<Aimbot>
{
public:
	bool InFOV(QAngle AimAngle, float fov, float &Dist);
	void BaseSmooth(QAngle& CrossPos, QAngle TargetPos, float SmoothBase);
	void Drop_Target();
	void AimBot_Tick(C_GMOD_Player * ent, CUserCmd * pCmd);//Iterate the entity
	void Post_AimBot_Tick(CUserCmd* pCmd, bool& bSendPacket);//Do Aimbot
	bool Did_Hit(C_GMOD_Player * pLocal, CUserCmd * pCmd);
	void Reset_Aimbot();//Clean up if not ingame 


	bool HandleVisible(C_GMOD_Player * pLocal, C_GMOD_Player * pEnemy, Vector pos);

	QAngle GetRecoil(CUserCmd* pCmd,C_GMOD_Player * pLocal);

	QAngle GetSpread(CUserCmd* pCmd);


private:
	void Update_BestTarget(C_GMOD_Player * ent);//Update the position , health etc...
	void Update_BestTarget(C_GMOD_Player * ent, matrix3x4_t* bones);
	void AimBot_Checks_Search(C_GMOD_Player * ent);// Add to vector casting
	bool AimBot_Checks_HaveTarget(C_GMOD_Player * ent);//Check if target is valid -> set ValidTarget
	void AimBot_Search_State(CUserCmd* pCmd);//Find the Best Target
	void AimBot_ValidTarget_State(CUserCmd* pCmd);//Simple Aim

	struct Target {
	public:
		void init(float Dist = 999999.f, int hp = 999999) {
			this->Dist = Dist;
			this->hp = hp;
			idx = -1;
			valid = false;
		};
		void Valid(void) {

			valid = true;
		}
		QAngle Angle;
		float Dist;
		int hp;
		int idx;
		bool valid = false;
	};
	enum AimBotState {
		AimBot_Search,
		AimBot_HaveTarget,
		AimBot_ValidTarget
	};

	Target BestTarget;

	std::vector<Target> Best_Target_Casting;
	CUserCmd * pCmd;
	AimBotState State = AimBot_Search;
};