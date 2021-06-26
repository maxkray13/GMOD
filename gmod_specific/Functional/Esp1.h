#pragma once
#include "../../engine/Core/Basic/Basic.h"
#include "../SDK.h"
#include"../../engine/Render/Render.h"


class cEsp : public Basic::Singleton<cEsp> {
public:
	void Esp_Tick();
private:

	void GPlayer_Tick(C_GMOD_Player* ply);
	void BaseEnt_Tick(C_BaseEntity* ent);
	void DrawBox2D(C_BaseEntity* ent);
	void DrawBox3D(C_BaseEntity* ent);
	void DrawSkeleton(C_GMOD_Player* ply);
	void DrawHeadCircle(C_GMOD_Player* ply);
	void DrawInfo(C_GMOD_Player* ply, player_info_t& info);
private:
	std::vector<int> ttt_players;
	C_GMOD_Player * pLocal;
	IClientEntity * pEntity;
	int health;
	float Distance;
	const char * RpName;
	int Team;
	cColor TeamColor;
	std::string TeamName;
	Lua::Lua_Role Role;
	bool b_use_team_col = false;
	bool b_is_friend = false;

	matrix3x4_t pBoneToWorldOut[MAXSTUDIOBONE];
	Vector pointsTransformed[8];
	Vector flb, brt, blb, frt, frb, brb, blt, flt;
	int left, top, right, bottom, x, y, w, h;

	struct KeypadRender {
		int EntityIdx;
		bool secured;
		std::string TempCode;
		std::string StaticCode;
	};

	std::vector<KeypadRender> KeypadList;
	std::vector<Core::VarUtil::unk_Var*>Prop_List;


};

class EntEsp : public Basic::Singleton<EntEsp> {
public:
	void EntEsp_tick(C_BaseEntity* ent);
	void Update_Temp_Ent();
	void Force_Update_Temp_Ent();
	void Update_Ent_vec();
	void Vis_PropEsp_tick();

	std::vector<std::string> Temp_Ent;
	std::vector<Core::VarUtil::unk_Var*>Ent_vec;

	bool exist_in_temp(std::string& name);
	bool exist_in_list(std::string& name);
	bool is_vec_change = false;
private:

	std::string get_model_name(C_BaseEntity* ent);

	Vector pointsTransformed[8];
	Vector flb, brt, blb, frt, frb, brb, blt, flt;


};


