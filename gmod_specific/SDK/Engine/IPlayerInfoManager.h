#pragma once




class IPlayerInfo
{
public:
	// returns the players name (UTF-8 encoded)
	virtual const char *GetName() = 0;
	// returns the userid (slot number)
	virtual int		GetUserID() = 0;
	// returns the string of their network (i.e Steam) ID
	virtual const char *GetNetworkIDString() = 0;
	// returns the team the player is on
	virtual int GetTeamIndex() = 0;
	// changes the player to a new team (if the game dll logic allows it)
	virtual void ChangeTeam(int iTeamNum) = 0;
	// returns the number of kills this player has (exact meaning is mod dependent)
	virtual int	GetFragCount() = 0;
	// returns the number of deaths this player has (exact meaning is mod dependent)
	virtual int	GetDeathCount() = 0;
	// returns if this player slot is actually valid
	virtual bool IsConnected() = 0;
	// returns the armor/health of the player (exact meaning is mod dependent)
	virtual int	GetArmorValue() = 0;

	// extensions added to V2

	// various player flags
	virtual bool IsHLTV() = 0;
	virtual bool IsPlayer() = 0;
	virtual bool IsFakeClient() = 0;
	virtual bool IsDead() = 0;
	virtual bool IsInAVehicle() = 0;
	virtual bool IsObserver() = 0;

	// player position and size
	virtual const Vector GetAbsOrigin() = 0;
	virtual const QAngle GetAbsAngles() = 0;
	virtual const Vector GetPlayerMins() = 0;
	virtual const Vector GetPlayerMaxs() = 0;
	// the name of the weapon currently being carried
	virtual const char *GetWeaponName() = 0;
	// the name of the player model in use
	virtual const char *GetModelName() = 0;
	// current player health
	virtual const int GetHealth() = 0;
	// max health value
	virtual const int GetMaxHealth() = 0;
	// the last user input from this player
	virtual CBotCmd GetLastUserCommand() = 0;
};

//-----------------------------------------------------------------------------
// Purpose: Global variables used by shared code
//-----------------------------------------------------------------------------
class CGlobalVarsBase
{
public:

	CGlobalVarsBase(bool bIsClient);

	// This can be used to filter debug output or to catch the client or server in the act.
	bool IsClient() const;

	// for encoding m_flSimulationTime, m_flAnimTime
	int GetNetworkBase(int nTick, int nEntity);

public:

	// Absolute time (per frame still - Use Plat_FloatTime() for a high precision real time 
	//  perf clock, but not that it doesn't obey host_timescale/host_framerate)
	float			realtime;
	// Absolute frame counter
	int				framecount;
	// Non-paused frametime
	float			absoluteframetime;

	// Current time 
	//
	// On the client, this (along with tickcount) takes a different meaning based on what
	// piece of code you're in:
	// 
	//   - While receiving network packets (like in PreDataUpdate/PostDataUpdate and proxies),
	//     this is set to the SERVER TICKCOUNT for that packet. There is no interval between
	//     the server ticks.
	//     [server_current_Tick * tick_interval]
	//
	//   - While rendering, this is the exact client clock 
	//     [client_current_tick * tick_interval + interpolation_amount]
	//
	//   - During prediction, this is based on the client's current tick:
	//     [client_current_tick * tick_interval]
	float			curtime;

	// Time spent on last server or client frame (has nothing to do with think intervals)
	float			frametime;
	// current maxplayers setting
	int				maxClients;

	// Simulation ticks
	int				tickcount;

	// Simulation tick interval
	float			interval_per_tick;

	// interpolation amount ( client-only ) based on fraction of next tick which has elapsed
	float			interpolation_amount;
	int				simTicksThisFrame;

	int				network_protocol;

	// current saverestore data
	void *pSaveData;//CSaveRestoreData

private:
	// Set to true in client code.
	bool			m_bClient;

	// 100 (i.e., tickcount is rounded down to this base and then the "delta" from this base is networked
	int				nTimestampNetworkingBase;
	// 32 (entindex() % nTimestampRandomizeWindow ) is subtracted from gpGlobals->tickcount to set the networking basis, prevents
	//  all of the entities from forcing a new PackedEntity on the same tick (i.e., prevents them from getting lockstepped on this)
	int				nTimestampRandomizeWindow;

};


//-----------------------------------------------------------------------------
// Purpose: Global variables shared between the engine and the game .dll
//-----------------------------------------------------------------------------
class CGlobalVars : public CGlobalVarsBase
{
public:

	CGlobalVars(bool bIsClient);

public:

	// Current map
	string_t		mapname;
	int				mapversion;
	string_t		startspot;
	MapLoadType_t	eLoadType;		// How the current map was loaded
	bool			bMapLoadFailed;	// Map has failed to load, we need to kick back to the main menu

									// game specific flags
	bool			deathmatch;
	bool			coop;
	bool			teamplay;
	// current maxentities
	int				maxEntities;
};


class IPlayerInfoManager
{
public:
	virtual IPlayerInfo *GetPlayerInfo(edict_t *pEdict) = 0;
	virtual CGlobalVars *GetGlobalVars() = 0;
};

