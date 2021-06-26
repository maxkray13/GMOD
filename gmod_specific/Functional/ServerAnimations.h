#pragma once


struct TrackedPlayer{
	int						m_LastUpdate;
	float					m_flSpawnTime;
	float					m_flPoseParameter[24];
	Vector					m_vecOrigin;
	QAngle					m_vecAngles;
	CBaseHandle				m_ulEntHandle;
	CBasePlayerAnimState	*m_PlayerAnimState;
};


class ServerAnimations {
public:
	ServerAnimations();
	void ClearData();
	void ProcessPlayer(C_GMOD_Player * player);


	TrackedPlayer g_Tracker[512];
};
extern ServerAnimations g_ServerAnimations;