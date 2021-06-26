#pragma once
#define MAX_LAYER_RECORDS 15

struct LayerRecord
{
	int m_sequence;
	float m_cycle;
	float m_weight;
	int m_order;
};
struct GestureSlotRecord
{
	int					m_iGestureSlot;
	DWORD				m_iActivity;
	bool				m_bAutoKill;
	bool				m_bActive;
};

struct AnimationStateRecord {
	bool							m_bForceAimYaw;//0x0004
	GestureSlotRecord				m_gustereRecords[MAX_LAYER_RECORDS];
	//CUtlVector<GestureSlot_t>		m_aGestureSlots;//0x0008
	QAngle							m_angRender;//0x0020
	bool							m_bPoseParameterInit;//0x002C
	MultiPlayerPoseData_t			m_PoseParameterData;//0x0030
	DebugPlayerAnimData_t			m_DebugAnimData;//0x0054
	bool							m_bCurrentFeetYawInitialized;//0x006C
	float							m_flLastAnimationStateClearTime;//0x0070
	float							m_flEyeYaw;//0x0074
	float							m_flEyePitch;//0x0078
	float							m_flGoalFeetYaw;//0x007C
	float							m_flCurrentFeetYaw;//0x0080
	float							m_flLastAimTurnTime;//0x0084
	MultiPlayerMovementData_t		m_MovementData;//0x0088
	bool							m_bJumping;//0x0098
	float							m_flJumpStartTime;//0x009C
	bool							m_bFirstJumpFrame;//0x00A0
	bool							m_bInSwim;//0x00A1
	bool							m_bFirstSwimFrame;//0x00A2
	bool							m_bDying;//0x00A3
	bool							m_bFirstDyingFrame;//0x00A4
	Activity						m_eCurrentMainSequenceActivity;//0x00A8
	int								m_nSpecificMainSequence;//0x00AC
	float							m_flLastGroundSpeedUpdateTime;//0x00B4
	float							m_flMaxGroundSpeed;//0x00E8
	int								m_nMovementSequence;//0x00EC
	LegAnimType_t					m_LegAnimType;//0x00F0

};
enum tick_priority {
	tick_normal,
	tick_trusted
};
struct tick {
	tick_priority			m_priority;
	float					m_flSpawnTime;
	int						m_clientReciveTick;
	Vector					m_vecOrigin;
	QAngle					m_vecAngles;
	Vector					m_vecMins;
	Vector					m_vecMaxs;

	float					m_flSimulationTime;
	float					m_flSimulationTime_temp;
	C_AnimationLayer		m_layerRecords[MAX_LAYER_RECORDS];
	int						m_masterSequence;
	float					m_masterCycle;
	float					m_flPoseParameter[24];
	
	AnimationStateRecord	m_AnimationRecord;
};

class LagCompensation {
public:


	void clear_data();

	void PostDataUpdate(C_GMOD_Player* ply);
	void remove_bad_ticks(CUserCmd* pCmd);

	bool get_last_tick(C_GMOD_Player* ply, tick & tick_out);
	tick  build_tick(C_GMOD_Player* ply);
	void set_tick(C_GMOD_Player* ply, tick target_tick);
	void set_tick_force(C_GMOD_Player* ply, tick target_tick);
	void push_tick(C_GMOD_Player* ply, tick target_tick);

	void update_animation(C_GMOD_Player* ply,float sim_time);
	void update_animation_ex(C_GMOD_Player* ply,QAngle angle, float sim_time);
	bool StartLagCompensation(C_GMOD_Player * ply, tick & tick_out);
	void FinishLagCompensation(C_GMOD_Player * ply);
private:
	using player_map = std::map<int, std::vector<tick>>;
	player_map recived_data;

};
extern LagCompensation g_LagCompensation;