#include "../../include/stdafx.h"
#include "../SDK.h"
#include "Teams.h"

namespace Teams {
	std::vector<Team> g_teams;

	int __cdecl ForEachLua(lua_State* st) {
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			g_pLuaShitInterface->SetState(st);
			Team curr = {};
			curr.whitelisted = false;
			curr.team_id = (int)g_pLuaShitInterface->CheckNumber(1);
			g_pLuaShitInterface->GetField(2, "Name");
			curr.Name = g_pLuaShitInterface->GetString(-1);
			g_pLuaShitInterface->GetField(2, "Color");
			ILuaObject* colorObject = g_pLuaShitInterface->GetObjectC(-1);
			cColor ret(0, 0, 0, 255);
			if (colorObject) {
				ret.SetColor(
					(int)colorObject->GetMemberDouble("r", 0.0),
					(int)colorObject->GetMemberDouble("g", 0.0),
					(int)colorObject->GetMemberDouble("b", 0.0),
					(int)colorObject->GetMemberDouble("a", 255.0)
				);
			}
			curr.Color = ret;
			g_pLuaShitInterface->Pop(1);
			if (g_teams.size()) {
				bool add = true;
				for (auto it : g_teams) {
					if (it.team_id == curr.team_id) 
						add = false;
				}
				if(add)
					g_teams.push_back(curr);
			}
			else {
				g_teams.push_back(curr);
			}
		}
		return 0;
	}

	void ProcessTeams()
	{
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			std::vector<std::string> teams = {};
			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "team");
			g_pLuaShitInterface->GetField(-1, "GetAllTeams");
			g_pLuaShitInterface->Call(0, 1);

			g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
			g_pLuaShitInterface->GetField(-1, "table");
			g_pLuaShitInterface->GetField(-1, "foreach");

			g_pLuaShitInterface->Push(-4);
			g_pLuaShitInterface->PushCFunction(ForEachLua);
			g_pLuaShitInterface->Call(2, 0);
			g_pLuaShitInterface->Pop(g_pLuaShitInterface->Top());
		}
		for (int it = 0; it <= g_pEngine->GetMaxClients(); it++) {//it < g_pEntList->GetMaxEntities()
			auto ply = C_GMOD_Player::GetGPlayer(it);
			if (!ply || !ply->IsGood())
				continue;
			Team curr = {};
			curr.whitelisted = false;
			curr.team_id = ply->m_iTeamNum();
			curr.Name = ply->GetTeamName();
			auto hash = std::hash<std::string>()(ply->GetTeamName());
			char* tempa = (char*)&hash;
			curr.Color = cColor(tempa[0], tempa[1], tempa[2], 255);
			bool add = true;
			for (auto it : g_teams) {
				if (it.team_id == curr.team_id)
					add = false;
			}
			if (add)
				g_teams.push_back(curr);
		}
	}

	void Reset()
	{
		g_teams.clear();
	}

	bool PlayerWhitelisted(C_GMOD_Player * ply)
	{
		auto team = Lua::GetTeam(ply->entindex());
		if (team == -1337) {
			team = ply->m_iTeamNum();
		}
		bool whitelisted = false;
		for (auto it : g_teams) {
			if (it.team_id == team)
				whitelisted = it.whitelisted;
		}

		return whitelisted;
	}

	bool GetTeamInfo(Team & out,C_GMOD_Player * ply)
	{
		out = {};
		bool ret = false;
		auto team = Lua::GetTeam(ply->entindex());
		if (team == -1337) {
			team = ply->m_iTeamNum();
		}
		for (auto it : g_teams) {
			if (it.team_id == team) {
				ret = true;
				out = it;
			}
		}
		return ret;
	}
}
