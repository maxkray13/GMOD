#pragma once
//-----------------------------------------------------------------------------
// Safe accessor to an entity
//-----------------------------------------------------------------------------
typedef unsigned int HTOOLHANDLE;
enum
{
	HTOOLHANDLE_INVALID = 0
};
//-----------------------------------------------------------------------------
// Opaque pointer returned from Find* methods, don't store this, you need to 
// Attach it to a tool entity or discard after searching
//-----------------------------------------------------------------------------
typedef void *EntitySearchResult;


//-----------------------------------------------------------------------------
// Purpose: Client side tool interace (right now just handles IClientRenderables).
//  In theory could support hooking into client side entities directly
//-----------------------------------------------------------------------------
class IClientTools
{
public:
	virtual void vfunc1() = 0;//0
	//virtual void vfunc2() = 0;
	// Allocates or returns the handle to an entity previously found using the Find* APIs below
	virtual HTOOLHANDLE		AttachToEntity(EntitySearchResult entityToAttach) = 0;//1
	virtual void			DetachFromEntity(EntitySearchResult entityToDetach) = 0;//2

	// Checks whether a handle is still valid.
	virtual bool			IsValidHandle(HTOOLHANDLE handle) = 0;//3

	// Iterates the list of entities which have been associated with tools
	virtual int				GetNumRecordables() = 0;//4
	virtual HTOOLHANDLE		GetRecordable(int index) = 0;//5

	// Iterates through ALL entities (separate list for client vs. server)
	virtual EntitySearchResult	NextEntity(EntitySearchResult currentEnt) = 0;//6
	EntitySearchResult			FirstEntity() { return NextEntity(NULL); }

	// Use this to turn on/off the presence of an underlying game entity
	virtual void			SetEnabled(HTOOLHANDLE handle, bool enabled) = 0;//7
	// Use this to tell an entity to post "state" to all listening tools
	virtual void			SetRecording(HTOOLHANDLE handle, bool recording) = 0;//8
	// Some entities are marked with ShouldRecordInTools false, such as ui entities, etc.
	virtual bool			ShouldRecord(HTOOLHANDLE handle) = 0;//9

	virtual HTOOLHANDLE		GetToolHandleForEntityByIndex(int entindex) = 0;//10

	virtual int				GetModelIndex(HTOOLHANDLE handle) = 0;//11
	virtual const char*		GetModelName(HTOOLHANDLE handle) = 0;//12
	virtual const char*		GetClassname(HTOOLHANDLE handle) = 0;//13

	virtual void			AddClientRenderable(IClientRenderable *pRenderable, int renderGroup) = 0;//14
	virtual void			RemoveClientRenderable(IClientRenderable *pRenderable) = 0;//15
	virtual void			SetRenderGroup(IClientRenderable *pRenderable, int renderGroup) = 0;//16
	virtual void			MarkClientRenderableDirty(IClientRenderable *pRenderable) = 0;//17
	virtual void			UpdateProjectedTexture(ClientShadowHandle_t h, bool bForce) = 0;//18

	virtual bool			DrawSprite(IClientRenderable *pRenderable, float scale, float frame, int rendermode, int renderfx, const Color &color, float flProxyRadius, int *pVisHandle) = 0;//19

	virtual EntitySearchResult	GetLocalPlayer() = 0;//20
	virtual bool			GetLocalPlayerEyePosition(Vector& org, QAngle& ang, float &fov) = 0;//21

	// See ClientShadowFlags_t above
	virtual ClientShadowHandle_t CreateShadow(CBaseHandle handle, int nFlags) = 0;//22
	virtual void			DestroyShadow(ClientShadowHandle_t h) = 0;//23

	virtual ClientShadowHandle_t CreateFlashlight(const FlashlightState_t &lightState) = 0;//24
	virtual void			DestroyFlashlight(ClientShadowHandle_t h) = 0;//25
	virtual void			UpdateFlashlightState(ClientShadowHandle_t h, const FlashlightState_t &lightState) = 0;//26

	virtual void			AddToDirtyShadowList(ClientShadowHandle_t h, bool force = false) = 0;//27
	virtual void			MarkRenderToTextureShadowDirty(ClientShadowHandle_t h) = 0;//28

	// Global toggle for recording
	virtual void			EnableRecordingMode(bool bEnable) = 0;//29
	virtual bool			IsInRecordingMode() const = 0;//30

	// Trigger a temp entity
	virtual void			TriggerTempEntity(void *pKeyValues) = 0;//31

	// get owning weapon (for viewmodels)
	virtual int				GetOwningWeaponEntIndex(int entindex) = 0;//32
	virtual int				GetEntIndex(EntitySearchResult entityToAttach) = 0;//33

	virtual int				FindGlobalFlexcontroller(char const *name) = 0;//34
	virtual char const		*GetGlobalFlexControllerName(int idx) = 0;//35

	// helper for traversing ownership hierarchy
	virtual EntitySearchResult	GetOwnerEntity(EntitySearchResult currentEnt) = 0;//36

	// common and useful types to query for hierarchically
	virtual bool			IsPlayer(EntitySearchResult currentEnt) = 0;//37
	virtual bool			IsBaseCombatCharacter(EntitySearchResult currentEnt) = 0;//38
	virtual bool			IsNPC(EntitySearchResult currentEnt) = 0;//39

	virtual Vector			GetAbsOrigin(HTOOLHANDLE handle) = 0;//40
	virtual QAngle			GetAbsAngles(HTOOLHANDLE handle) = 0;//41

	// This reloads a portion or all of a particle definition file.
	// It's up to the client to decide if it cares about this file
	// Use a UtlBuffer to crack the data
	virtual void			ReloadParticleDefintions(const char *pFileName, const void *pBufData, int nLen) = 0;//42

	// Sends a mesage from the tool to the client
	virtual void			PostToolMessage(void *pKeyValues) = 0;

	// Indicates whether the client should render particle systems
	virtual void			EnableParticleSystems(bool bEnable) = 0;

	// Is the game rendering in 3rd person mode?
	virtual bool			IsRenderingThirdPerson() const = 0;
};