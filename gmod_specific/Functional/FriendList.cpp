#include "FriendList.h"

bool FriendList::IsFriend(C_GMOD_Player * ply)
{
	player_info_t info = {};
	if (!g_pEngine->GetPlayerInfo(ply->entindex(), &info))
		return false;

	for (auto it : Friend_vec) {
		auto s_var = it->ExtractAs<std::string>();

		if (!s_var)
			continue;

		//if (std::string(info.guid) == s_var->Get()) {
			if (s_var->s_Name == std::string(info.name)) {
				return true;
			}

			
	//	}
			
	}
	return false;
}

void FriendList::AddToFriend(C_GMOD_Player * ply)
{
	player_info_t info = {};
	ZeroMemory(info.name, sizeof(info.name));
	if (!g_pEngine->GetPlayerInfo(ply->entindex(), &info))
		return;

	Engine::Var::Instance().EngineVars.BeginGroup("FriendList");
	auto ptr = new Core::VarUtil::Var<std::string>;
	ZeroMemory((void*)ptr, sizeof(*ptr));
	Engine::Var::Instance().EngineVars.RegisterVar(*ptr, std::string("tt"), info.name, Core::VarUtil::v_dynamic);
	Engine::Var::Instance().EngineVars.EndGroup();
	is_vec_change = true;
}

void FriendList::DelFromFriend(C_GMOD_Player * ply)
{
	player_info_t info = {};
	if (!g_pEngine->GetPlayerInfo(ply->entindex(), &info))
		return;
	auto var = Engine::Var::Instance().EngineVars.FindVar(info.name, "FriendList");
	while (var) {
		Engine::Var::Instance().EngineVars.DeleteVar(*var);
		var = Engine::Var::Instance().EngineVars.FindVar(info.name, "FriendList");
	}
	auto iter = 0u;
	for (auto it : Friend_vec) {
		if (it->s_Name == info.name) {
			Friend_vec.erase(Friend_vec.begin() + iter);
			iter--;
		}
		iter++;
	}
	is_vec_change = true;

}

void FriendList::Update()
{
	if (!is_vec_change)
		return;

	Friend_vec = Engine::Var::Instance().EngineVars.ParseVarGroup("FriendList");
	is_vec_change = false;

}
