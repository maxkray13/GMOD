#pragma once
#include "../Util/netprop_manager.h"
#include "../../engine/Engine.h"


// This provides a way for entities to know when they've entered or left the PVS.
// Normally, server entities can use NotifyShouldTransmit to get this info, but client-only
// entities can use this. Store a CPVSNotifyInfo in your 
//
// When bInPVS=true, it's being called DURING rendering. It might be after rendering any
// number of views.
//
// If no views had the entity, then it is called with bInPVS=false after rendering.
class IPVSNotify
{
public:
	virtual void OnPVSStatusChanged(bool bInPVS) = 0;
};



class CClientThinkHandlePtr;
typedef CClientThinkHandlePtr* ClientThinkHandle_t;

// An IHandleEntity-derived class can go into an entity list and use ehandles.
class IHandleEntity
{
public:
	virtual ~IHandleEntity() {}
	virtual void SetRefEHandle(const CBaseHandle &handle) = 0;
	virtual const CBaseHandle& GetRefEHandle() const = 0;
};

class ICollideable
{
public:
	// Gets at the entity handle associated with the collideable
	virtual IHandleEntity	*GetEntityHandle() = 0;

	// These methods return the bounds of an OBB measured in "collision" space
	// which can be retreived through the CollisionToWorldTransform or
	// GetCollisionOrigin/GetCollisionAngles methods
	virtual const Vector&	OBBMinsPreScaled() const = 0;
	virtual const Vector&	OBBMaxsPreScaled() const = 0;
	virtual const Vector&	OBBMins() const = 0;
	virtual const Vector&	OBBMaxs() const = 0;

	// Returns the bounds of a world-space box used when the collideable is being traced
	// against as a trigger. It's only valid to call these methods if the solid flags
	// have the FSOLID_USE_TRIGGER_BOUNDS flag set.
	virtual void			WorldSpaceTriggerBounds(Vector *pVecWorldMins, Vector *pVecWorldMaxs) const = 0;

	// custom collision test
	virtual bool			TestCollision(const Ray_t &ray, unsigned int fContentsMask, trace_t& tr) = 0;

	// Perform hitbox test, returns true *if hitboxes were tested at all*!!
	virtual bool			TestHitboxes(const Ray_t &ray, unsigned int fContentsMask, trace_t& tr) = 0;

	// Returns the BRUSH model index if this is a brush model. Otherwise, returns -1.
	virtual int				GetCollisionModelIndex() = 0;

	// Return the model, if it's a studio model.
	virtual const model_t*	GetCollisionModel() = 0;

	// Get angles and origin.
	virtual const Vector&	GetCollisionOrigin() const = 0;
	virtual const QAngle&	GetCollisionAngles() const = 0;
	virtual const matrix3x4_t&	CollisionToWorldTransform() const = 0;

	// Return a SOLID_ define.
	virtual SolidType_t		GetSolid() const = 0;
	virtual int				GetSolidFlags() const = 0;

	// Gets at the containing class...
	virtual IClientUnknown*	GetIClientUnknown() = 0;

	// We can filter out collisions based on collision group
	virtual int				GetCollisionGroup() const = 0;

	// Returns a world-aligned box guaranteed to surround *everything* in the collision representation
	// Note that this will surround hitboxes, trigger bounds, physics.
	// It may or may not be a tight-fitting box and its volume may suddenly change
	virtual void			WorldSpaceSurroundingBounds(Vector *pVecMins, Vector *pVecMaxs) = 0;

	virtual bool			ShouldTouchTrigger(int triggerSolidFlags) const = 0;

	// returns NULL unless this collideable has specified FSOLID_ROOT_PARENT_ALIGNED
	virtual const matrix3x4_t	*GetRootParentToWorldTransform() const = 0;
};

class IClientThinkable
{
public:
	virtual IClientUnknown*		GetIClientUnknown() = 0;
	virtual void				ClientThink() = 0;
	virtual ClientThinkHandle_t	GetThinkHandle() = 0;
	virtual void				SetThinkHandle(ClientThinkHandle_t* hThink) = 0;
	virtual void				Release() = 0;
};
class C_BaseEntity;

class IClientUnknown
{
public:
	virtual void IHandleEntity_destructor() = 0;
	virtual void SetRefEHandle(const CBaseHandle &handle) = 0;
	virtual const CBaseHandle& GetRefEHandle() const = 0;
	virtual ICollideable*		GetCollideable() = 0;
	virtual IClientNetworkable*	GetClientNetworkable() = 0;
	virtual IClientRenderable*	GetClientRenderable() = 0;
	virtual IClientEntity*		GetIClientEntity() = 0;
	virtual C_BaseEntity*		GetBaseEntity() = 0;
	virtual IClientThinkable*	GetClientThinkable() = 0;
};
class IClientRenderable {
public:
	// Gets at the containing class...
	virtual IClientUnknown*	GetIClientUnknown() = 0;

	// Data accessors
	virtual Vector &			GetRenderOrigin(void) = 0;
	virtual QAngle &			GetRenderAngles(void) = 0;
	virtual bool					ShouldDraw(void) = 0;
	virtual bool					IsTransparent(void) = 0;
	virtual bool					UsesPowerOfTwoFrameBufferTexture() = 0;
	virtual bool					UsesFullFrameBufferTexture() = 0;

	virtual ClientShadowHandle_t	GetShadowHandle() const = 0;

	// Used by the leaf system to store its render handle.
	virtual ClientRenderHandle_t&	RenderHandle() = 0;

	// Render baby!
	virtual const model_t*			GetModel() const = 0;
	virtual int						DrawModel(int flags) = 0;

	// Get the body parameter
	virtual int		GetBody() = 0;

	// Determine alpha and blend amount for transparent objects based on render state info
	virtual void	ComputeFxBlend() = 0;
	virtual int		GetFxBlend(void) = 0;

	// Determine the color modulation amount
	virtual void	GetColorModulation(float* color) = 0;

	// Returns false if the entity shouldn't be drawn due to LOD. 
	// (NOTE: This is no longer used/supported, but kept in the vtable for backwards compat)
	virtual bool	LODTest() = 0;

	// Call this to get the current bone transforms for the model.
	// currentTime parameter will affect interpolation
	// nMaxBones specifies how many matrices pBoneToWorldOut can hold. (Should be greater than or
	// equal to studiohdr_t::numbones. Use MAXSTUDIOBONES to be safe.)
	virtual bool	SetupBones(matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) = 0;

	virtual void	SetupWeights(const matrix3x4_t *pBoneToWorld, int nFlexWeightCount, float *pFlexWeights, float *pFlexDelayedWeights) = 0;
	virtual void	DoAnimationEvents(void) = 0;

	// Return this if you want PVS notifications. See IPVSNotify for more info.	
	// Note: you must always return the same value from this function. If you don't,
	// undefined things will occur, and they won't be good.
	virtual IPVSNotify* GetPVSNotifyInterface() = 0;

	// Returns the bounds relative to the origin (render bounds)
	virtual void	GetRenderBounds(Vector& mins, Vector& maxs) = 0;

	// returns the bounds as an AABB in worldspace
	virtual void	GetRenderBoundsWorldspace(Vector& mins, Vector& maxs) = 0;

	// These normally call through to GetRenderAngles/GetRenderBounds, but some entities custom implement them.
	virtual void	GetShadowRenderBounds(Vector &mins, Vector &maxs, ShadowType_t shadowType) = 0;

	// Should this object be able to have shadows cast onto it?
	virtual bool	ShouldReceiveProjectedTextures(int flags) = 0;

	// These methods return true if we want a per-renderable shadow cast direction + distance
	virtual bool	GetShadowCastDistance(float *pDist, ShadowType_t shadowType) const = 0;
	virtual bool	GetShadowCastDirection(Vector *pDirection, ShadowType_t shadowType) const = 0;

	// Other methods related to shadow rendering
	virtual bool	IsShadowDirty() = 0;
	virtual void	MarkShadowDirty(bool bDirty) = 0;

	// Iteration over shadow hierarchy
	virtual IClientRenderable *GetShadowParent() = 0;
	virtual IClientRenderable *FirstShadowChild() = 0;
	virtual IClientRenderable *NextShadowPeer() = 0;

	// Returns the shadow cast type
	virtual ShadowType_t ShadowCastType() = 0;

	// Create/get/destroy model instance
	virtual void CreateModelInstance() = 0;
	virtual ModelInstanceHandle_t GetModelInstance() = 0;

	// Returns the transform from RenderOrigin/RenderAngles to world
	virtual const matrix3x4_t &RenderableToWorldTransform() = 0;

	// Attachments
	virtual int LookupAttachment(const char *pAttachmentName) = 0;
	virtual	bool GetAttachment(int number, Vector &origin, QAngle &angles) = 0;
	virtual bool GetAttachment(int number, matrix3x4_t &matrix) = 0;

	// Rendering clip plane, should be 4 floats, return value of NULL indicates a disabled render clip plane
	virtual float *GetRenderClipPlane(void) = 0;

	// Get the skin parameter
	virtual int		GetSkin() = 0;

	// Is this a two-pass renderable?
	virtual bool	IsTwoPass(void) = 0;

	virtual void	OnThreadedDrawSetup() = 0;

	virtual bool	UsesFlexDelayedWeights() = 0;

	virtual void	RecordToolMessage() = 0;
};
class IClientNetworkable
{

public:
	// Gets at the containing class...
	virtual void	GetIClientUnknown() = 0;

	// Called by the engine when the server deletes the entity.
	virtual void			Release() = 0;

	// Supplied automatically by the IMPLEMENT_CLIENTCLASS macros.
	virtual  ClientClass*	GetClientClass() = 0;

	// This tells the entity what the server says for ShouldTransmit on this entity.
	// Note: This used to be EntityEnteredPVS/EntityRemainedInPVS/EntityLeftPVS.
	virtual void			NotifyShouldTransmit(ShouldTransmitState_t state) = 0;



	//
	// NOTE FOR ENTITY WRITERS: 
	//
	// In 90% of the cases, you should hook OnPreDataChanged/OnDataChanged instead of 
	// PreDataUpdate/PostDataUpdate.
	//
	// The DataChanged events are only called once per frame whereas Pre/PostDataUpdate
	// are called once per packet (and sometimes multiple times per frame).
	//
	// OnDataChanged is called during simulation where entity origins are correct and 
	// attachments can be used. whereas PostDataUpdate is called while parsing packets
	// so attachments and other entity origins may not be valid yet.
	//

	virtual void			OnPreDataChanged(DataUpdateType_t updateType) = 0;
	virtual void			OnDataChanged(DataUpdateType_t updateType) = 0;

	// Called when data is being updated across the network.
	// Only low-level entities should need to know about these.
	virtual void			PreDataUpdate(DataUpdateType_t updateType) = 0;
	virtual void			PostDataUpdate(DataUpdateType_t updateType) = 0;


	// Objects become dormant on the client if they leave the PVS on the server.
	virtual bool			IsDormant(void) = 0;

	// Ent Index is the server handle used to reference this entity.
	// If the index is < 0, that indicates the entity is not known to the server
	virtual int				entindex(void) const = 0;

	// Server to client entity message received
	virtual void			ReceiveMessage(int classID, bf_read & msg) = 0;//

																		   // Get the base pointer to the networked data that GetClientClass->m_pRecvTable starts at.
																		   // (This is usually just the "this" pointer).
	virtual void*			GetDataTableBasePtr() = 0;

	// Tells the entity that it's about to be destroyed due to the client receiving
	// an uncompressed update that's caused it to destroy all entities & recreate them.
	virtual void			SetDestroyedOnRecreateEntities(void) = 0;

	virtual void			OnDataUnchangedInPVS() = 0;
};

#define netprop( type, name, array_name, prop_name ) \
type name ## ()  \
{ \
	static int offset = 0;\
	if(!offset){\
		offset = SDK::Util::NetPropManager::Instance().Get( array_name, prop_name ); \
	}\
	type result = *( type* )( (DWORD)(this) + offset );\
	return result; \
}

#define netprop_ex( type, name, array_name, prop_name, extra ) \
type& name ## ()  \
{ \
	static int offset = 0;\
	if(!offset){\
		offset = SDK::Util::NetPropManager::Instance().Get( array_name, prop_name ) + extra; \
	}\
	return *( type* )(  (DWORD)(this) + offset ); \
}

#define netprop_raw( type, name, array_name, prop_name ) \
type name ## ()  \
{ \
	static int offset = 0;\
	if(!offset){\
		offset = SDK::Util::NetPropManager::Instance().Get( array_name, prop_name ); \
	}\
	return ( type )( (DWORD)(this) + offset ); \
}

class __declspec (novtable) IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:

	Vector & OBBMins() {
		return *(Vector*)((DWORD)(this) + 0x1BC);
	}

	Vector& OBBMaxs() {
		return *(Vector*)((DWORD)(this) + 0x1C8);
	}
	bool IsValid();
	char* GetScriptName();

	netprop_ex(const matrix3x4_t, m_rgflCoordinateFrame, "DT_BaseEntity", "m_CollisionGroup", -0x4C);


	bool GetPredictable();
	int RestoreData(const char * context, int slot, int type);


	QAngle* GetRenderAngle();
	Vector* GetRenderPos();
	void GetVectors(Vector* pForward, Vector* pRight, Vector* pUp);
private:
	char* SENT_anim_script_name() {
		static int offset = 0; \
			if (!offset) {

				offset = SDK::Util::NetPropManager::Instance().Get("DT_SENT_anim", "m_ScriptedEntity") + 0x10;
			}
		return (char*)((DWORD)(this) + offset);
	}
	char* SENT_AI_script_name() {
		static int offset = 0; \
			if (!offset) {

				offset = SDK::Util::NetPropManager::Instance().Get("DT_SENT_AI", "m_ScriptedEntity") + 0x10;
			}
		return (char*)((DWORD)(this) + offset);
	}
	char* SENT_point_script_name() {
		static int offset = 0; \
			if (!offset) {

				offset = SDK::Util::NetPropManager::Instance().Get("DT_SENT_point", "m_ScriptedEntity") + 0x10;
			}
		return (char*)((DWORD)(this) + offset);
	}
	char* LuaNextBot_script_name() {
		static int offset = 0; \
			if (!offset) {

				offset = SDK::Util::NetPropManager::Instance().Get("DT_LuaNextBot", "m_ScriptedEntity") + 0x10;
			}
		return (char*)((DWORD)(this) + offset);
	}
	char* WeaponSWEP_script_name() {
		static int offset = 0; \
			if (!offset) {

				offset = SDK::Util::NetPropManager::Instance().Get("DT_WeaponSWEP", "m_ScriptedEntity") + 0x10;
			}
		return (char*)((DWORD)(this) + offset);
	}


	//netprop_ex(char*, SENT_anim_script_name, "DT_SENT_anim", "m_ScriptedEntity", 0x10);
	//netprop_ex(char*, SENT_AI_script_name, "DT_SENT_AI", "m_ScriptedEntity", 0x10);
	//netprop_ex(char*, SENT_point_script_name, "DT_SENT_point", "m_ScriptedEntity", 0x10);
	//netprop_ex(char*, LuaNextBot_script_name, "DT_LuaNextBot", "m_ScriptedEntity", 0x10);
	//netprop_ex(char*, WeaponSWEP_script_name, "DT_WeaponSWEP", "m_ScriptedEntity", 0x10);
};

class C_Team {
public:
	netprop(int, m_iTeamNum, "DT_Team", "m_iTeamNum");
	netprop(int, m_iScore, "DT_Team", "m_iScore");
	netprop(int, m_iRoundsWon, "DT_Team", "m_iRoundsWon");
	netprop(char*, m_szTeamname, "DT_Team", "m_szTeamname");
};

class C_BaseEntity : public IClientEntity
{
public:
	static C_BaseEntity* GetBaseEntity(int index);

	bool IsWeapon();
	float m_flSpawnTime();
	//netprop(float, m_flSpawnTime, "DT_ParticleSmokeGrenade", "m_flSpawnTime");
	netprop(int, m_iTeamNum, "DT_BaseEntity", "m_iTeamNum");

	//C_Team * GetGlobalTeam();

	const char * GetTeamName();
	netprop(float, m_flSimulationTime, "DT_BaseEntity", "m_flSimulationTime");
	netprop(CBaseHandle, m_hOwnerEntity, "DT_BaseEntity", "m_hOwnerEntity");
	netprop(Vector, m_vecOrigin, "DT_BaseEntity", "m_vecOrigin");

	netprop(int, m_fEffects, "DT_BaseEntity", "m_fEffects");
	void EstimateAbsVelocity(Vector& vel);
	bool Teleported();
	netprop(int, m_nTickBase, "DT_BasePlayer", "m_nTickBase");
//	int& m_nTickBase();
	void	SetNextThink(float nextThinkTime, const char *szContext = NULL);
	bool PhysicsRunThink();
	void PreThink();
	int GetNextThinkTick();

	void PostThink();

	void RunPreThink();
	void RunThink();
	void RunPostThink();



	void Think();

};



class CBoneBitList : public CBitVec<MAXSTUDIOBONES>
{
public:
	inline void MarkBone(int iBone)
	{
		Set(iBone);
	}
	inline bool IsBoneMarked(int iBone)
	{
		return Get(iBone) != 0 ? true : false;
	}
};
class CIKContext {
public:
	void Construct();
	void ClearTargets();
	void Init(const CStudioHdr *pStudioHdr, const QAngle &angles, const Vector &pos, float flTime, int iFramecounter, int boneMask);
	void UpdateTargets(Vector pos[], Quaternion q[], matrix3x4_t boneToWorld[], CBoneBitList &boneComputed);
	void SolveDependencies(Vector pos[], Quaternion q[], matrix3x4_t boneToWorld[], CBoneBitList &boneComputed);
private:
	char pad[0x1060];
};
class CBoneSetup
{
public:
	void InitPose(Vector pos[], Quaternion q[]);
	void AccumulatePose(Vector pos[], Quaternion q[], int sequence, float cycle, float flWeight, float flTime, CIKContext *pIKContext);
	void CalcAutoplaySequences(Vector pos[], Quaternion q[], float flRealTime, CIKContext *pIKContext);

public:
	CStudioHdr *m_pStudioHdr;
	int m_boneMask;
	float *m_flPoseParameter;
	void *m_pPoseDebugger;
};

class C_BaseAnimating : public C_BaseEntity
{
public:
	QAngle& GetAbsAngles();

	int& m_iPrevBoneMask();
	bool IsToolRecording();
	CThreadFastMutex& m_BoneSetupLock();

	unsigned long& m_iMostRecentModelBoneCounter();
	float& m_flLastBoneSetupTime();
	void MoveToLastReceivedPosition();
	CBoneAccessor& m_BoneAccessor();
	int& m_iAccumulatedBoneMask();
	CIKContext ** m_pIk();
	bool IsRagdoll();
	void UpdateIKLocks(float currentTime);
	void CalculateIKLocks(float currentTime);
	void BuildTransformations(CStudioHdr *hdr, Vector *pos, Quaternion *q, const matrix3x4_t &cameraTransform, int boneMask, CBoneBitList &boneComputed);
	CUtlVector< matrix3x4_t >& m_CachedBoneData();
	bool SetupBones_AttachmentHelper(CStudioHdr *hdr);
	void ControlMouth(CStudioHdr *pstudiohdr);
	void StandardBlendingRules(CStudioHdr *hdr, Vector pos[], Quaternion q[], float currentTime, int boneMask);

	//void CalcAutoplaySequences(const CStudioHdr *pStudioHdr, CIKContext *pIKContext, Vector pos[], Quaternion q[], const float poseParameters[], int boneMask, float realTime);
	void CalcBoneAdj(const CStudioHdr *pStudioHdr, Vector pos[], Quaternion q[], const float controllers[], int boneMask);


	bool GetBoneTransform(matrix3x4_t* output, float time = 0.f);
	bool GetBoneWorld(int index, matrix3x4_t* transform, Vector& output);
	bool GetBoxBoundWorld(int index, matrix3x4_t* transform, Vector& min, Vector& max);
	bool GetBoxBoundWorld(const char* name, matrix3x4_t* transform, Vector& min, Vector& max);
	bool GetBoxWorld(int index, matrix3x4_t* transform, Vector& output);
	bool GetBoxWorld(const char* name, matrix3x4_t* transform, Vector& output);

	bool Interpolate(float time);
	bool& m_bReadyToDraw();
	void UpdateClientSideAnimation();
	CStudioHdr * GetModelPtr();
	int LookupPoseParameter(void* model_ptr, const char* name);
	float	SetPoseParameter(void* model_ptr, int iParameter, float flValue);
	void InvalidateBoneCache();
	Vector GetNetworkOrigin();
	QAngle GetNetworkAngles();
	float& m_flAnimTime();

	CUtlVector< C_AnimationLayer >& C_BaseAnimating::m_AnimOverlay();


public:
	netprop(int, m_nHitboxSet, "DT_BaseAnimating", "m_nHitboxSet");
	//netprop(int, m_nSequence, "DT_BaseAnimating", "m_nSequence");
	//netprop(float, m_flCycle, "DT_ServerAnimationData", "m_flCycle");
	int& m_nSequence();
	float& m_flCycle();
	float* m_flPoseParameter();
	float* m_flEncodedController();
	//netprop_raw(float*, m_flPoseParameter, "DT_BaseAnimating", "m_flPoseParameter");
};
class C_BaseCombatWeapon : public C_BaseEntity
{
public:
	netprop(int, m_iClip1, "DT_BaseCombatWeapon", "m_iClip1");
	netprop(int, m_iClip2, "DT_BaseCombatWeapon", "m_iClip2");
	netprop(int, m_iPrimaryAmmoType, "DT_BaseCombatWeapon", "m_iPrimaryAmmoType");
	netprop(int, m_iSecondaryAmmoType, "DT_BaseCombatWeapon", "m_iSecondaryAmmoType");

	float m_flAccuracyPenalty();
	double GetFireRate();
//	float m_flNextPrimaryAttack();
	netprop(float, m_flNextPrimaryAttack, "DT_BaseCombatWeapon", "m_flNextPrimaryAttack");
	netprop(float, m_flNextSecondaryAttack, "DT_BaseCombatWeapon", "m_flNextSecondaryAttack");

};

class C_BaseCombatCharacter : public C_BaseAnimating
{
public:
	C_BaseCombatWeapon * GetActiveWeapon()
	{
		auto handle = m_hActiveWeapon();
		return reinterpret_cast<C_BaseCombatWeapon*>(g_pEntList->GetClientEntityFromHandle(handle));
	}

public:
	netprop(CBaseHandle, m_hActiveWeapon, "DT_BaseCombatCharacter", "m_hActiveWeapon");
	netprop(float, m_flNextAttack, "DT_BaseCombatCharacter", "m_flNextAttack");
};


class C_BasePlayer :public C_BaseCombatCharacter {
public:
	Vector EyePosition();
	void m_pCurrentCommand(CUserCmd* pCmd);

	bool IsLocal();
	bool IsGood();
	float GetServerTime() {
		return m_nTickBase()*g_pGlobalVars->interval_per_tick;
	}
	bool WeaponAbleToShot();

	MoveType_t GetMoveType();
	netprop(char, m_lifeState, "DT_BasePlayer", "m_lifeState");
	netprop(int, m_fFlags, "DT_BasePlayer", "m_fFlags");
	netprop(int, m_iHealth, "DT_BaseEntity", "m_iHealth");
	netprop(int, m_iMaxHealth, "DT_BaseEntity", "m_iMaxHealth");
	netprop(CBaseHandle, m_hObserverTarget, "DT_BasePlayer", "m_hObserverTarget");
	netprop(int, m_iFOV, "DT_BasePlayer", "m_iFOV");
	netprop(int, m_iDefaultFOV, "DT_BasePlayer", "m_iDefaultFOV");

	CBasePlayerAnimState * PlayerAnimState();
	void SetAbsOrigin(Vector& Origin);
	void SetAbsAngles(QAngle& Angle);
	VarMapping_t* GetVarMapping();
	void DisableInterpolation();
	void EnableInterpolation();
	//
	netprop(Vector, m_vecVelocity, "DT_BasePlayer", "m_vecVelocity[0]");

	//QAngle& m_angEyeAngles();
	netprop(QAngle, m_angEyeAngles, "DT_HL2MP_Player", "m_angEyeAngles[0]");
//	netprop(DWORD, m_Local, "DT_BasePlayer", "m_Local");
	float m_vecViewOffset();
//	netprop(DWORD, m_vecViewOffset_off, "DT_BasePlayer", "m_vecViewOffset[0]");
	netprop(int, m_bClientSideAnimation, "DT_BaseAnimating", "m_bClientSideAnimation");
	//QAngle m_vecPunchAngle();
	//QAngle m_vecPunchAngleVel();

	netprop(QAngle, m_vecPunchAngle, "DT_BasePlayer", "m_vecPunchAngle");
	netprop(QAngle, m_vecPunchAngleVel, "DT_BasePlayer", "m_vecPunchAngleVel");
	netprop(CBaseHandle, m_hGroundEntity, "DT_LocalPlayerExclusive", "m_hGroundEntity");
	netprop_ex(QAngle, v_angle, "DT_BasePlayer", "deadflag", 0x04);
};


class C_GMOD_Player : public C_BasePlayer
{
public:
	static C_GMOD_Player* GetGPlayer(int index);
	static C_GMOD_Player* GetLocalPlayer();

	void UpdateButtonState(int Button);


	Vector GetAttackSpread();


private:





};

inline C_GMOD_Player* ToGMOD_Player(C_BasePlayer* base_player)
{
	if (!base_player || !g_pClientTools->IsPlayer(base_player))// || !g_pClientTools->IsPlayer(base_player)
		return nullptr;

	return reinterpret_cast<C_GMOD_Player*>(base_player);
}
inline C_BasePlayer* ToBasePlayer(C_BaseEntity* base_entity);

inline C_BasePlayer* ToBasePlayer(C_BaseCombatCharacter* base_character)
{
	if (!base_character || !base_character->IsValid() || !g_pClientTools->IsPlayer(base_character))
		return nullptr;

	return reinterpret_cast<C_BasePlayer*>(base_character);
}
inline C_BaseCombatCharacter* ToBaseCombatCharacter(C_BaseEntity* base_entity)
{
	if (!base_entity || !base_entity->IsValid() || !g_pClientTools->IsBaseCombatCharacter(base_entity))
		return nullptr;

	return reinterpret_cast<C_BaseCombatCharacter*>(base_entity);
}