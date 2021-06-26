#pragma once

//-----------------------------------------------------------------------------
// Purpose: The basic player movement interface
//-----------------------------------------------------------------------------

class IGameMovement
{
public:
	virtual			~IGameMovement(void) {}

	// Process the current movement command
	virtual void	ProcessMovement(IClientEntity *pPlayer, CMoveData *pMove) = 0;
	virtual void	StartTrackPredictionErrors(IClientEntity *pPlayer) = 0;
	virtual void	FinishTrackPredictionErrors(IClientEntity *pPlayer) = 0;
	virtual void	DiffPrint(char const *fmt, ...) = 0;

	// Allows other parts of the engine to find out the normal and ducked player bbox sizes
	virtual Vector const&	GetPlayerMins(bool ducked) const = 0;
	virtual Vector const&	GetPlayerMaxs(bool ducked) const = 0;
	virtual Vector const&   GetPlayerViewOffset(bool ducked) const = 0;
};


class IMoveHelper
{
public:
	// Methods associated with a particular entity
	virtual	char const*		GetName(void * handle) const = 0;

	// Adds the trace result to touch list, if contact is not already in list.
	virtual void	ResetTouchList(void) = 0;
	virtual bool	AddToTouched(const CGameTrace& tr, const Vector& impactvelocity) = 0;
	virtual void	ProcessImpacts(void) = 0;

	// Numbered line printf
	virtual void	Con_NPrintf(int idx, char const* fmt, ...) = 0;

	// These have separate server vs client impementations
	virtual void	StartSound(const Vector& origin, int channel, char const* sample, float volume, void * soundlevel, int fFlags, int pitch) = 0;
	virtual void	StartSound(const Vector& origin, const char *soundname) = 0;
	virtual void	PlaybackEventFull(int flags, int clientindex, unsigned short eventindex, float delay, Vector& origin, Vector& angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2) = 0;

	// Apply falling damage to m_pHostPlayer based on m_pHostPlayer->m_flFallVelocity.
	virtual bool	PlayerFallingDamage(void) = 0;

	// Apply falling damage to m_pHostPlayer based on m_pHostPlayer->m_flFallVelocity.
	virtual void	PlayerSetAnimation(void* playerAnim) = 0;

	virtual void *GetSurfaceProps(void) = 0;

	virtual bool IsWorldEntity(void *handle) = 0;

protected:
	// Clients shouldn't call delete directly
	virtual			~IMoveHelper() {}

	// The global instance
	static IMoveHelper* sm_pSingleton;
};


class IPrediction {
public:

	virtual			~IPrediction(void) {};

	virtual void	Init(void) = 0;
	virtual void	Shutdown(void) = 0;

	// Run prediction
	virtual void	Update
	(
		int startframe,				// World update ( un-modded ) most recently received
		bool validframe,			// Is frame data valid
		int incoming_acknowledged,	// Last command acknowledged to have been run by server (un-modded)
		int outgoing_command		// Last command (most recent) sent to server (un-modded)
	) = 0;

	// We are about to get a network update from the server.  We know the update #, so we can pull any
	//  data purely predicted on the client side and transfer it to the new from data state.
	virtual void	PreEntityPacketReceived(int commands_acknowledged, int current_world_update_packet) = 0;
	virtual void	PostEntityPacketReceived(void) = 0;
	virtual void	PostNetworkDataReceived(int commands_acknowledged) = 0;

	virtual void	OnReceivedUncompressedPacket(void) = 0;

	// The engine needs to be able to access a few predicted values
	virtual void	GetViewOrigin(Vector& org) = 0;
	virtual void	SetViewOrigin(Vector& org) = 0;
	virtual void	GetViewAngles(QAngle& ang) = 0;
	virtual void	SetViewAngles(QAngle& ang) = 0;
	virtual void	GetLocalViewAngles(QAngle& ang) = 0;
	virtual void	SetLocalViewAngles(QAngle& ang) = 0;

	virtual void	InPrediction(void) = 0;

	virtual void	IsFirstTimePredicted(void) = 0;

	virtual void	GetIncomingPacketNumber(void) = 0;


	virtual void	RunCommand(IClientEntity * player, CUserCmd *ucmd, IMoveHelper *moveHelper) = 0;

	virtual void	SetupMove(IClientEntity* player, CUserCmd* ucmd, IMoveHelper* pHelper, CMoveData* move) = 0;
	virtual void	FinishMove(IClientEntity* player, CUserCmd* ucmd, CMoveData* move) = 0;
};

// Interface used by client and server to track predictable entities
class IPredictableList
{
public:
	// Get predictables by index
	virtual IClientEntity	*GetPredictable(int slot) = 0;
	// Get count of predictables
	virtual int				GetPredictableCount(void) = 0;
public:
	CUtlVector< void* >	m_Predictables;

};


struct PredictionContext
{
	PredictionContext()
	{
		m_bActive = false;
		m_nCreationCommandNumber = -1;
		m_pszCreationModule = NULL;
		m_nCreationLineNumber = 0;
		m_hServerEntity = NULL;
	}

	// The command_number of the usercmd which created this entity
	bool						m_bActive;
	int							m_nCreationCommandNumber;
	char const					*m_pszCreationModule;
	int							m_nCreationLineNumber;
	// The entity to whom we are attached
	unsigned long				m_hServerEntity;
};

extern void Force_Prediction_Start(CUserCmd* pCmd);
extern void Force_Prediction_End(CUserCmd* pCmd);
extern void UpdatePredict(int incoming_acknowledged, int outgoing_command);

extern void Force_Prediction_Ent(IClientEntity*ent);