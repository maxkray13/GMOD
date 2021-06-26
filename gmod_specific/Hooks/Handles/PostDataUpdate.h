#pragma once
using PostDataUpdate_fn = void(__fastcall*)(IClientNetworkable*, void*, DataUpdateType_t);
extern PostDataUpdate_fn oPostDataUpdate_;

/*struct target_info {
//	uint32_t ent_id = -1;
	Vector origin;
	QAngle angle;
	uint32_t tick_base;
	float simulation_time;
	matrix3x4_t bone_data[MAXSTUDIOBONE];
};
struct player_edit {
	int idx;
	int min_delta;
};
extern std::vector<player_edit> player_edited;
//std::map<int, std::vector< target_info>> recived_data;
using player_map = std::map<int, std::vector< target_info>>;

extern player_map recived_data;
bool IsTickValid(int tick);*/


void __fastcall hkPostDataUpdate(IClientNetworkable* ecx, void* edx, DataUpdateType_t updateType);