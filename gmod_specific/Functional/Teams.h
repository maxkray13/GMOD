#pragma once
namespace Teams {
	struct Team {
		int team_id;
		std::string Name;
		cColor Color;
		bool whitelisted = false;
	};
	extern std::vector<Team> g_teams;
	void ProcessTeams();
	void Reset();
	bool PlayerWhitelisted(C_GMOD_Player* ply);
	bool GetTeamInfo(Team & out, C_GMOD_Player * ply);
	
	
}