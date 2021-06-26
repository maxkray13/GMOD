#pragma once
#include "../../../include/stdafx.h"
#include "CRC.h"

#include "buf.h"
#include "UtlVector.h"
#include"UtlBuffer.h"
#include "IMaterialSystem.h"
#include "IVModelRender.h"

enum ButtonCode_t;
struct ScreenFade_t;
enum ClientFrameStage_t;
typedef unsigned long CRC32_t;
typedef struct player_info_s player_info_t;
struct client_textmessage_t;
typedef struct D3DMATRIXG D3DMATRIXG, *LPD3DMATRIXG;
class ClientClass;
enum ShouldTransmitState_t;
enum DataUpdateType_t;
class IClientNetworkable;
class IClientRenderable;
using CBaseHandle = unsigned long;
struct model_t;
struct spritedata_t;
struct brushdata_t;
struct Ray_t;
struct ModelRenderInfo_t;
struct mstudiobbox_t;
struct mstudiohitboxset_t;
struct mstudiobone_t;
struct studiohdr_t;
struct studiomeshdata_t;
struct studioloddata_t;
struct studiohwdata_t;
struct DrawModelState_t;
struct StaticPropRenderInfo_t;
struct cplane_t;
class ICvar;
class CRender;
enum SendPropType;
class DVariant;
class CRecvProxyData;
class CStandardRecvProxies;
enum netadrtype_t;
typedef struct netadr_s netadr_t;
struct CameraThirdData_t;
class CUserCmd;
struct kbutton_t;
class IEngineSound;

class INetChannel;
class INetMessage;
class INetChannelHandler;
class INetChannelInfo;
class IClientTools;
class IClientUnknown;
class IVModelInfo;
extern IVModelInfo *g_pVModelInfo;
//-----------------------------------------------------------------------------
// Handles to a client shadow
//-----------------------------------------------------------------------------
typedef unsigned short ClientShadowHandle_t;
//-----------------------------------------------------------------------------
// Handle to an renderable in the client leaf system
//-----------------------------------------------------------------------------
typedef unsigned short ClientRenderHandle_t;
//-----------------------------------------------------------------------------
// What kind of shadows to render?
//-----------------------------------------------------------------------------
enum ShadowType_t
{
	SHADOWS_NONE = 0,
	SHADOWS_SIMPLE,
	SHADOWS_RENDER_TO_TEXTURE,
	SHADOWS_RENDER_TO_TEXTURE_DYNAMIC,	// the shadow is always changing state
	SHADOWS_RENDER_TO_DEPTH_TEXTURE,
};

class RecvProp;
class RecvTable;
class CRecvDecoder;

typedef IClientNetworkable*	(*CreateClientClassFn)(int entnum, int serialNum);
typedef IClientNetworkable*	(*CreateEventFn)();
typedef void(*RecvVarProxyFn)(const CRecvProxyData *pData, void *pStruct, void *pOut);
typedef void(*ArrayLengthRecvProxyFn)(void *pStruct, int objectID, int currentArrayLength);
typedef void(*DataTableRecvVarProxyFn)(const RecvProp *pProp, void **pOut, void *pData, int objectID);


class IVEngineClient;
class IBaseClientDLL;
class INetChannel;
class IClientEntityList;
class CInput;
class ClientModeShared;
class IVModelRender;
class IVModelInfo;
class IVDebugOverlay;
class IEngineTrace;
class IShaderAPI;
class IVRenderView;
class IViewRender;
class IPlayerInfoManager;
class CGlobalVars;
class IPrediction;
class IGameMovement;
class IMoveHelper;
class Lua_Shared;
class ILuaInterface;
class IPanel;
class IMaterialSystem;
class IStudioRender;
class CClientState;
class IPredictableList;
class CGameTrace;
class CEngineVGui;
class ISurface;
class IInputSystem;
class IPhysicsSurfaceProps;

typedef CGameTrace trace_t;

typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);
class IClientEntity;
#define IN_ATTACK		(1 << 0)
#define IN_JUMP			(1 << 1)
#define IN_DUCK			(1 << 2)
#define IN_FORWARD		(1 << 3)
#define IN_BACK			(1 << 4)
#define IN_USE			(1 << 5)
#define IN_CANCEL		(1 << 6)
#define IN_LEFT			(1 << 7)
#define IN_RIGHT		(1 << 8)
#define IN_MOVELEFT		(1 << 9)
#define IN_MOVERIGHT	(1 << 10)
#define IN_ATTACK2		(1 << 11)
#define IN_RUN			(1 << 12)
#define IN_RELOAD		(1 << 13)
#define IN_ALT1			(1 << 14)
#define IN_ALT2			(1 << 15)
#define IN_SCORE		(1 << 16)   // Used by client.dll for when scoreboard is held down
#define IN_SPEED		(1 << 17)	// Player is holding the speed key
#define IN_WALK			(1 << 18)	// Player holding walk key
#define IN_ZOOM			(1 << 19)	// Zoom key for HUD zoom
#define IN_WEAPON1		(1 << 20)	// weapon defines these bits
#define IN_WEAPON2		(1 << 21)	// weapon defines these bits
#define IN_BULLRUSH		(1 << 22)
#define IN_GRENADE1		(1 << 23)	// grenade 1
#define IN_GRENADE2		(1 << 24)	// grenade 2
#define	IN_LOOKSPIN		(1 << 25)

typedef unsigned int			uint;
typedef unsigned char			uint8;
typedef unsigned __int32		uint32;
typedef __int64					int64;
typedef unsigned __int64		uint64;
struct CameraThirdData_t
{
	float	m_flPitch;
	float	m_flYaw;
	float	m_flDist;
	float	m_flLag;
	Vector	m_vecHullMin;
	Vector	m_vecHullMax;
};


class CUserCmd
{
public:
	CUserCmd()
	{
		Reset();
	}

	//	virtual ~CUserCmd() { };

	void Reset()
	{
		command_number = 0;
		tick_count = 0;
		viewangles.Init();
		forwardmove = 0.0f;
		sidemove = 0.0f;
		upmove = 0.0f;
		buttons = 0;
		impulse = 0;
		weaponselect = 0;
		weaponsubtype = 0;
		random_seed = 0;
		mousedx = 0;
		mousedy = 0;

		hasbeenpredicted = false;
	}

	CUserCmd& operator =(const CUserCmd& src)
	{
		if (this == &src)
			return *this;

		command_number = src.command_number;
		tick_count = src.tick_count;
		viewangles = src.viewangles;
		forwardmove = src.forwardmove;
		sidemove = src.sidemove;
		upmove = src.upmove;
		buttons = src.buttons;
		impulse = src.impulse;
		weaponselect = src.weaponselect;
		weaponsubtype = src.weaponsubtype;
		random_seed = src.random_seed;
		mousedx = src.mousedx;
		mousedy = src.mousedy;

		hasbeenpredicted = src.hasbeenpredicted;

		return *this;
	}

	CUserCmd(const CUserCmd& src)
	{
		*this = src;
	}
	CRC32_t GetChecksum(void) const {
		CRC32_t crc;
		CRC32_Init(&crc);

		CRC32_ProcessBuffer(&crc, &command_number, sizeof(command_number));
		CRC32_ProcessBuffer(&crc, &tick_count, sizeof(tick_count));
		CRC32_ProcessBuffer(&crc, &viewangles, sizeof(viewangles));
		CRC32_ProcessBuffer(&crc, &forwardmove, sizeof(forwardmove));
		CRC32_ProcessBuffer(&crc, &sidemove, sizeof(sidemove));
		CRC32_ProcessBuffer(&crc, &upmove, sizeof(upmove));
		CRC32_ProcessBuffer(&crc, &buttons, sizeof(buttons));
		CRC32_ProcessBuffer(&crc, &impulse, sizeof(impulse));
		CRC32_ProcessBuffer(&crc, &weaponselect, sizeof(weaponselect));
		CRC32_ProcessBuffer(&crc, &weaponsubtype, sizeof(weaponsubtype));
		CRC32_ProcessBuffer(&crc, &random_seed, sizeof(random_seed));
		CRC32_ProcessBuffer(&crc, &mousedx, sizeof(mousedx));
		CRC32_ProcessBuffer(&crc, &mousedy, sizeof(mousedy));
		CRC32_Final(&crc);
		return crc;
	}

	// For matching server and client commands for debugging
	int		command_number;

	// the tick the client created this command
	int		tick_count;

	// Player instantaneous view angles.
	QAngle	viewangles;
	// Intended velocities
	//	forward velocity.
	float	forwardmove;
	//  sideways velocity.
	float	sidemove;
	//  upward velocity.
	float	upmove;
	// Attack button states
	int		buttons;
	// Impulse command issued.
	unsigned char impulse;
	// Current weapon id
	int		weaponselect;
	int		weaponsubtype;

	int		random_seed;	// For shared random functions

	short	mousedx;		// mouse accum in x from create move
	short	mousedy;		// mouse accum in y from create move

							// Client only, tracks whether we've predicted this command at least once
	bool	hasbeenpredicted;
	//288
	char _0x0000[288];


};
class CVerifiedUserCmd
{
public:

	CUserCmd  m_cmd;
	CRC32_t   m_crc;
};


struct kbutton_t
{
	// key nums holding it down
	int		down[2];
	// low bit is down state
	int		state;
};

enum ID_ClientClass {
	ID_CTestTraceline = 197,
	ID_CTEWorldDecal = 198,
	ID_CTESpriteSpray = 195,
	ID_CTESprite = 194,
	ID_CTESparks = 193,
	ID_CTESmoke = 192,
	ID_CTEShowLine = 190,
	ID_CTEProjectedDecal = 188,
	ID_CTEPlayerDecal = 187,
	ID_CTEPhysicsProp = 185,
	ID_CTEParticleSystem = 184,
	ID_CTEMuzzleFlash = 183,
	ID_CTELargeFunnel = 181,
	ID_CTEKillPlayerAttachments = 180,
	ID_CTEImpact = 179,
	ID_CTEGlowSprite = 177,
	ID_CTEShatterSurface = 189,
	ID_CTEFootprintDecal = 175,
	ID_CTEFizz = 174,
	ID_CTEExplosion = 173,
	ID_CTEEnergySplash = 172,
	ID_CTEEffectDispatch = 171,
	ID_CTEDynamicLight = 170,
	ID_CTEDecal = 168,
	ID_CTEClientProjectile = 166,
	ID_CTEBubbleTrail = 165,
	ID_CTEBubbles = 164,
	ID_CTEBSPDecal = 163,
	ID_CTEBreakModel = 162,
	ID_CTEBloodStream = 161,
	ID_CTEBloodSprite = 160,
	ID_CTEBeamSpline = 159,
	ID_CTEBeamRingPoint = 158,
	ID_CTEBeamRing = 157,
	ID_CTEBeamPoints = 156,
	ID_CTEBeamLaser = 155,
	ID_CTEBeamFollow = 154,
	ID_CTEBeamEnts = 153,
	ID_CTEBeamEntPoint = 152,
	ID_CTEBaseBeam = 151,
	ID_CTEArmorRicochet = 150,
	ID_CTEMetalSparks = 182,
	ID_CSteamJet = 146,
	ID_CSmokeStack = 141,
	ID_DustTrail = 230,
	ID_CFireTrail = 59,
	ID_SporeTrail = 237,
	ID_SporeExplosion = 236,
	ID_RocketTrail = 234,
	ID_SmokeTrail = 235,
	ID_CPropVehicleDriveable = 127,
	ID_ParticleSmokeGrenade = 233,
	ID_CParticleFire = 104,
	ID_MovieExplosion = 231,
	ID_CTEGaussExplosion = 176,
	ID_CEnvQuadraticBeam = 52,
	ID_CEmbers = 44,
	ID_CEnvWind = 57,
	ID_CPrecipitation = 118,
	ID_CBaseTempEntity = 30,
	ID_NextBotCombatCharacter = 232,
	ID_CWeaponStunStick = 227,
	ID_CWeaponSMG1 = 226,
	ID_CWeapon_SLAM = 206,
	ID_CWeaponShotgun = 225,
	ID_CWeaponRPG = 224,
	ID_CLaserDot = 90,
	ID_CWeaponPistol = 223,
	ID_CWeaponPhysCannon = 221,
	ID_CBaseHL2MPCombatWeapon = 24,
	ID_CBaseHL2MPBludgeonWeapon = 23,
	ID_CHL2MPMachineGun = 82,
	ID_CWeaponHL2MPBase = 219,
	ID_CWeaponFrag = 217,
	ID_CWeaponCrowbar = 214,
	ID_CWeaponCrossbow = 213,
	ID_CWeaponAR2 = 209,
	ID_CWeapon357 = 205,
	ID_CHL2MPGameRulesProxy = 81,
	ID_CTEHL2MPFireBullets = 178,
	ID_CTEPlayerAnimEvent = 186,
	ID_CHL2MPRagdoll = 83,
	ID_CHL2MP_Player = 80,
	ID_CCrossbowBolt = 41,
	ID_CWeaponHopwire = 0,
	ID_CWeaponOldManHarpoon = 220,
	ID_CWeaponCitizenSuitcase = 212,
	ID_CWeaponCitizenPackage = 211,
	ID_CWeaponAlyxGun = 207,
	ID_CWeaponCubemap = 215,
	ID_CWeaponGaussGun = 0,
	ID_CWeaponAnnabelle = 208,
	ID_CFlaregun = 0,
	ID_CWeaponBugBait = 210,
	ID_CWeaponBinoculars = 0,
	ID_CWeaponCycler = 216,
	ID_CVortigauntEffectDispel = 201,
	ID_CVortigauntChargeToken = 200,
	ID_CNPC_Vortigaunt = 103,
	ID_CPropVehiclePrisonerPod = 128,
	ID_CPropCrane = 122,
	ID_CPropCannon = 0,
	ID_CPropAirboat = 120,
	ID_CFlare = 61,
	ID_CTEConcussiveExplosion = 167,
	ID_CNPC_Strider = 102,
	ID_CScriptIntro = 135,
	ID_CRotorWashEmitter = 133,
	ID_CPropCombineBall = 121,
	ID_CPlasmaBeamNode = 0,
	ID_CNPC_RollerMine = 101,
	ID_CNPC_Manhack = 99,
	ID_CNPC_CombineGunship = 98,
	ID_CNPC_AntlionGuard = 95,
	ID_CInfoTeleporterCountdown = 89,
	ID_CMortarShell = 94,
	ID_CEnvStarfield = 55,
	ID_CEnvHeadcrabCanister = 49,
	ID_CAlyxEmpEffect = 11,
	ID_CCorpse = 40,
	ID_CCitadelEnergyCore = 37,
	ID_CHL2_Player = 79,
	ID_CBaseHLBludgeonWeapon = 25,
	ID_CHLSelectFireMachineGun = 85,
	ID_CHLMachineGun = 84,
	ID_CBaseHelicopter = 20,
	ID_CNPC_Barney = 97,
	ID_CNPC_Barnacle = 96,
	ID_AR2Explosion = 0,
	ID_CTEAntlionDust = 149,
	ID_CBaseHLCombatWeapon = 26,
	ID_CWeaponHandGrenade = 218,
	ID_CBaseHL1MPCombatWeapon = 22,
	ID_CBaseHL1CombatWeapon = 21,
	ID_CAIWeaponAR2 = 3,
	ID_CAIWeaponShotgun = 8,
	ID_CAIWeaponCrossbow = 4,
	ID_CAIWeaponCrowbar = 5,
	ID_CAIWeaponRPG = 7,
	ID_CAIWeaponSMG1 = 9,
	ID_CAIWeaponPistol = 6,
	ID_CAIWeapon357 = 2,
	ID_CGMOD_Player = 76,
	ID_CAIWeaponStunStick = 10,
	ID_CFleshEffectTarget = 0,
	ID_CPropJeepEpisodic = 124,
	ID_CPropScalable = 125,
	ID_CNPC_Puppet = 100,
	ID_CHandleTest = 78,
	ID_CSpriteTrail = 145,
	ID_CSpriteOriented = 144,
	ID_CSprite = 143,
	ID_CRagdollPropAttached = 131,
	ID_CRagdollProp = 130,
	ID_CPredictedViewModel = 119,
	ID_CPoseController = 117,
	ID_CGameRulesProxy = 75,
	ID_CInfoLadderDismount = 86,
	ID_CFuncLadder = 68,
	ID_CEnvDetailController = 48,
	ID_CWorld = 229,
	ID_CWaterLODControl = 204,
	ID_CWaterBullet = 203,
	ID_CVoteController = 202,
	ID_CVGuiScreen = 199,
	ID_CPropJeep = 123,
	ID_CPropVehicleChoreoGeneric = 126,
	ID_CTest_ProxyToggle_Networkable = 196,
	ID_CTesla = 191,
	ID_CTeam = 148,
	ID_CSun = 147,
	ID_CParticlePerformanceMonitor = 105,
	ID_CSpotlightEnd = 142,
	ID_CSlideshowDisplay = 140,
	ID_CShadowControl = 139,
	ID_CSceneEntity = 134,
	ID_CRopeKeyframe = 132,
	ID_CRagdollManager = 129,
	ID_CPhysicsPropMultiplayer = 111,
	ID_CPhysBoxMultiplayer = 109,
	ID_CBasePropDoor = 29,
	ID_CDynamicProp = 43,
	ID_CPointCommentaryNode = 116,
	ID_CPointCamera = 115,
	ID_CPlayerResource = 114,
	ID_CPlasma = 113,
	ID_CPhysMagnet = 112,
	ID_CPhysicsProp = 110,
	ID_CPhysBox = 108,
	ID_CParticleSystem = 106,
	ID_CMaterialModifyControl = 93,
	ID_CLightGlow = 91,
	ID_CInfoOverlayAccessor = 88,
	ID_CFuncTrackTrain = 74,
	ID_CFuncSmokeVolume = 73,
	ID_CFuncRotating = 72,
	ID_CFuncReflectiveGlass = 71,
	ID_CFuncOccluder = 70,
	ID_CFuncMonitor = 69,
	ID_CFunc_LOD = 65,
	ID_CTEDust = 169,
	ID_CFunc_Dust = 64,
	ID_CFuncConveyor = 67,
	ID_CBreakableSurface = 36,
	ID_CFuncAreaPortalWindow = 66,
	ID_CFish = 60,
	ID_CEntityFlame = 46,
	ID_CFireSmoke = 58,
	ID_CEnvTonemapController = 56,
	ID_CEnvScreenEffect = 53,
	ID_CEnvScreenOverlay = 54,
	ID_CEnvProjectedTexture = 51,
	ID_CEnvParticleScript = 50,
	ID_CFogController = 63,
	ID_CEntityParticleTrail = 47,
	ID_CEntityDissolve = 45,
	ID_CDynamicLight = 42,
	ID_CColorCorrectionVolume = 39,
	ID_CColorCorrection = 38,
	ID_CBreakableProp = 35,
	ID_CBasePlayer = 28,
	ID_CBaseFlex = 18,
	ID_CBaseEntity = 17,
	ID_CBaseDoor = 16,
	ID_CBaseCombatCharacter = 14,
	ID_CBaseAnimatingOverlay = 13,
	ID_CBoneFollower = 33,
	ID_CBaseAnimating = 12,
	ID_CInfoLightingRelative = 87,
	ID_CAI_BaseNPC = 1,
	ID_CBeam = 32,
	ID_CBaseViewModel = 31,
	ID_CBaseParticleEntity = 27,
	ID_CBaseGrenade = 19,
	ID_CBaseCombatWeapon = 15,
	ID_CWeaponPhysGun = 222,
	ID_CPhysBeam = 107,
	ID_CLuaNextBot = 92,
	ID_CFlexManipulate = 62,
	ID_CBoneManipulate = 34,
	ID_CWeaponSWEP = 228,
	ID_CSENT_point = 138,
	ID_CSENT_anim = 137,
	ID_CSENT_AI = 136,
	ID_CGMODGameRulesProxy = 77
};

enum ButtonCode_t
{
	BUTTON_CODE_INVALID = -1,
	BUTTON_CODE_NONE = 0,

	KEY_FIRST = 0,

	KEY_NONE = KEY_FIRST,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_PAD_0,
	KEY_PAD_1,
	KEY_PAD_2,
	KEY_PAD_3,
	KEY_PAD_4,
	KEY_PAD_5,
	KEY_PAD_6,
	KEY_PAD_7,
	KEY_PAD_8,
	KEY_PAD_9,
	KEY_PAD_DIVIDE,
	KEY_PAD_MULTIPLY,
	KEY_PAD_MINUS,
	KEY_PAD_PLUS,
	KEY_PAD_ENTER,
	KEY_PAD_DECIMAL,
	KEY_LBRACKET,
	KEY_RBRACKET,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_BACKQUOTE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_BACKSLASH,
	KEY_MINUS,
	KEY_EQUAL,
	KEY_ENTER,
	KEY_SPACE,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_CAPSLOCK,
	KEY_NUMLOCK,
	KEY_ESCAPE,
	KEY_SCROLLLOCK,
	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_BREAK,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LALT,
	KEY_RALT,
	KEY_LCONTROL,
	KEY_RCONTROL,
	KEY_LWIN,
	KEY_RWIN,
	KEY_APP,
	KEY_UP,
	KEY_LEFT,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_CAPSLOCKTOGGLE,
	KEY_NUMLOCKTOGGLE,
	KEY_SCROLLLOCKTOGGLE,

	KEY_LAST = KEY_SCROLLLOCKTOGGLE,
	KEY_COUNT = KEY_LAST - KEY_FIRST + 1,

	// Mouse
	MOUSE_FIRST = KEY_LAST + 1,

	MOUSE_LEFT = MOUSE_FIRST,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_4,
	MOUSE_5,
	MOUSE_WHEEL_UP,		// A fake button which is 'pressed' and 'released' when the wheel is moved up 
	MOUSE_WHEEL_DOWN,	// A fake button which is 'pressed' and 'released' when the wheel is moved down

	MOUSE_LAST = MOUSE_WHEEL_DOWN,
	MOUSE_COUNT = MOUSE_LAST - MOUSE_FIRST + 1,
};
struct vrect_t
{
	int				x, y, width, height;
	vrect_t			*pnext;
};
class CViewSetup
{
public:

public:
	__int32 x; //0x0000 
	__int32 m_nUnscaledX; //0x0004 
	__int32 y; //0x0008 
	__int32 m_nUnscaledY; //0x000C 
	__int32 width; //0x0010 
	__int32 m_nUnscaledWidth; //0x0014 
	__int32 height; //0x0018 
	__int32 m_eStereoEye; //0x001C 
	__int32 m_nUnscaledHeight; //0x0020 
	__int32 m_bOrtho; //0x0024 
	float m_OrthoLeft; //0x0028 
	float m_OrthoTop; //0x002C 
	float m_OrthoRight; //0x0030 
	float m_OrthoBottom; //0x0034 
	float fov; //0x0038 
	float viewmodel_fov; //0x003C 
	Vector Origin; //0x0040 
	QAngle Angle; //0x004C 
	float zNear; //0x0058 
	float zFar; //0x005C 
	float zNearViewmodel; //0x0060 
	float zFarViewmodel; //0x0064 
	__int32 m_bRenderToSubrectOfLargerScreen; //0x0068 
	float m_flAspectRatio; //0x006C 
	__int32		m_bOffCenter;
	float		m_flOffCenterTop;
	float		m_flOffCenterBottom;
	float		m_flOffCenterLeft;
	float		m_flOffCenterRight;
	__int32		m_bDoBloomAndToneMapping;
	bool		m_bCacheFullSceneState;


	char _0x0058[168];
};
typedef float vec_t;
typedef int qboolean;
// Note: These flags need to match the bspfile.h DISPTRI_TAG_* flags.
#define DISPSURF_FLAG_SURFACE		(1<<0)
#define DISPSURF_FLAG_WALKABLE		(1<<1)
#define DISPSURF_FLAG_BUILDABLE		(1<<2)
#define DISPSURF_FLAG_SURFPROP1		(1<<3)
#define DISPSURF_FLAG_SURFPROP2		(1<<4)
struct cplane_t
{
	Vector	normal;
	float	dist;
	byte	type;			// for fast side tests
	byte	signbits;		// signx + (signy<<1) + (signz<<1)
	byte	pad[2];

#ifdef VECTOR_NO_SLOW_OPERATIONS
	cplane_t() {}

private:
	// No copy constructors allowed if we're in optimal mode
	cplane_t(const cplane_t& vOther);
#endif
};
// This structure is sent over the net to describe a screen fade event
struct ScreenFade_t
{
	unsigned short 	duration;		// FIXED 16 bit, with SCREENFADE_FRACBITS fractional, seconds duration
	unsigned short 	holdTime;		// FIXED 16 bit, with SCREENFADE_FRACBITS fractional, seconds duration until reset (fade & hold)
	short			fadeFlags;		// flags
	unsigned char	r, g, b, a;		// fade to color ( max alpha )
};

enum ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,			// (haven't run any frames yet)
	FRAME_START,

	// A network packet is being recieved
	FRAME_NET_UPDATE_START,
	// Data has been received and we're going to start calling PostDataUpdate
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	// Data has been received and we've called PostDataUpdate on all data recipients
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	// We've received all packets, we can now do interpolation, prediction, etc..
	FRAME_NET_UPDATE_END,

	// We're about to start rendering the scene
	FRAME_RENDER_START,
	// We've finished rendering the scene.
	FRAME_RENDER_END
};



typedef struct player_info_s
{

	// scoreboard information
	char			name[128];
	// local server user ID, unique while server is running
	int				userID;
	// global unique player identifer
	char			guid[32 + 1];
	// friends identification number
	long			friendsID;
	// friends name
	char			friendsName[128];
	// true, if player is a bot controlled by game.dll
	bool			fakeplayer;
	// true if player is the HLTV proxy
	bool			ishltv;
	// custom files CRC for this player
	int				customFiles[4];
	// this counter increases each time the server downloaded a new file
	unsigned char	filesDownloaded;
	char pad_big[0x200];

} player_info_t;

struct client_textmessage_t
{
	int		effect;
	unsigned char	r1, g1, b1, a1;		// 2 colors for effects
	unsigned char	r2, g2, b2, a2;
	float	x;
	float	y;
	float	fadein;
	float	fadeout;
	float	holdtime;
	float	fxtime;
	const char *pVGuiSchemeFontName; // If null, use default font for messages
	const char *pName;
	const char *pMessage;
	bool    bRoundedRectBackdropBox;
	float	flBoxSize; // as a function of font height
	unsigned char	boxcolor[4];
	char const *pClearMessage; // message to clear
};

#define PRINTF_FORMAT_STRING _Printf_format_string_
class CPhysCollide;

class CTraceListData;


class CBaseTrace
{
public:

	// Displacement flags tests.
	bool IsDispSurface(void) { return ((dispFlags & DISPSURF_FLAG_SURFACE) != 0); }
	bool IsDispSurfaceWalkable(void) { return ((dispFlags & DISPSURF_FLAG_WALKABLE) != 0); }
	bool IsDispSurfaceBuildable(void) { return ((dispFlags & DISPSURF_FLAG_BUILDABLE) != 0); }
	bool IsDispSurfaceProp1(void) { return ((dispFlags & DISPSURF_FLAG_SURFPROP1) != 0); }
	bool IsDispSurfaceProp2(void) { return ((dispFlags & DISPSURF_FLAG_SURFPROP2) != 0); }

public:

	// these members are aligned!!
	Vector			startpos;				// start position
	Vector			endpos;					// final position
	cplane_t		plane;					// surface normal at impact

	float			fraction;				// time completed, 1.0 = didn't hit anything

	int				contents;				// contents on other side of surface hit
	unsigned short	dispFlags;				// displacement flags for marking surfaces with data

	bool			allsolid;				// if true, plane is not valid
	bool			startsolid;				// if true, the initial point was in a solid area

	CBaseTrace() {}

private:
	// No copy constructors allowed
	CBaseTrace(const CBaseTrace& vOther);
};
struct csurface_t
{
	const char	*name;
	short		surfaceProps;
	unsigned short	flags;		// BUGBUG: These are declared per surface, not per material, but this database is per-material now
};

class CGameTrace : public CBaseTrace
{
public:

	// Returns true if hEnt points at the world entity.
	// If this returns true, then you can't use GetHitBoxIndex().
	bool DidHitWorld() {
		return true;
	}

	// Returns true if we hit something and it wasn't the world.
	bool DidHitNonWorldEntity() {
		return m_pEnt != NULL && !DidHitWorld();
	};

	// Gets the entity's network index if the trace has hit an entity.
	// If not, returns -1.
	int GetEntityIndex() const;

	// Returns true if there was any kind of impact at all
	bool DidHit() {
		return fraction < 1.0f || allsolid || startsolid;
	};




public:

	float		fractionleftsolid;		// time we left a solid, only valid if we started in solid
	csurface_t	surface;				// surface hit (impact surface)

	int			hitgroup;				// 0 == generic, non-zero is specific body part
	short		physicsbone;			// physics bone hit by trace in studio


	IClientEntity *m_pEnt;


	// NOTE: this member is overloaded.
	// If hEnt points at the world entity, then this is the static prop index.
	// Otherwise, this is the hitbox index.
	int			hitbox;					// box hit by trace in studio

	CGameTrace() {}

private:
	// No copy constructors allowed
	CGameTrace(const CGameTrace& vOther);
};


enum TraceType_t
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
};
class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IClientEntity *pEntity, int contentsMask) = 0;
	virtual TraceType_t	GetTraceType() const = 0;
};


//-----------------------------------------------------------------------------
// Classes are expected to inherit these + implement the ShouldHitEntity method
//-----------------------------------------------------------------------------

// This is the one most normal traces will inherit from
class CTraceFilter : public ITraceFilter
{
public:
	bool ShouldHitEntity(IClientEntity* pEntityHandle, int contentsMask)
	{
		return !(pEntityHandle == pSkip);
	}

	TraceType_t GetTraceType() const
	{
		return TRACE_EVERYTHING;
	}

	void* pSkip;
};

enum ShaderParamType_t
{
	SHADER_PARAM_TYPE_TEXTURE,
	SHADER_PARAM_TYPE_INTEGER,
	SHADER_PARAM_TYPE_COLOR,
	SHADER_PARAM_TYPE_VEC2,
	SHADER_PARAM_TYPE_VEC3,
	SHADER_PARAM_TYPE_VEC4,
	SHADER_PARAM_TYPE_ENVMAP,	// obsolete
	SHADER_PARAM_TYPE_FLOAT,
	SHADER_PARAM_TYPE_BOOL,
	SHADER_PARAM_TYPE_FOURCC,
	SHADER_PARAM_TYPE_MATRIX,
	SHADER_PARAM_TYPE_MATERIAL,
	SHADER_PARAM_TYPE_STRING,
};

enum NormalDecodeMode_t
{
	NORMAL_DECODE_NONE = 0,
	NORMAL_DECODE_ATI2N = 1,
	NORMAL_DECODE_ATI2N_ALPHA = 2
};

//-----------------------------------------------------------------------------
// Client systems are singleton objects in the client codebase responsible for
// various tasks
// The order in which the client systems appear in this list are the
// order in which they are initialized and updated. They are shut down in
// reverse order from which they are initialized.
//-----------------------------------------------------------------------------

enum InitReturnVal_t
{
	INIT_FAILED = 0,
	INIT_OK,

	INIT_LAST_VAL,
};

// FIXME: How do I specify the actual number of matrix modes?
const int NUM_MODEL_TRANSFORMS = 53;
const int MATERIAL_MODEL_MAX = MATERIAL_MODEL + NUM_MODEL_TRANSFORMS;
// These are given to FindMaterial to reference the texture groups that Show up on the 
#define TEXTURE_GROUP_LIGHTMAP						        "Lightmaps"
#define TEXTURE_GROUP_WORLD							          "World textures"
#define TEXTURE_GROUP_MODEL							          "Model textures"
#define TEXTURE_GROUP_VGUI							          "VGUI textures"
#define TEXTURE_GROUP_PARTICLE						        "Particle textures"
#define TEXTURE_GROUP_DECAL							          "Decal textures"
#define TEXTURE_GROUP_SKYBOX						          "SkyBox textures"
#define TEXTURE_GROUP_CLIENT_EFFECTS				      "ClientEffect textures"
#define TEXTURE_GROUP_OTHER							          "Other textures"
#define TEXTURE_GROUP_PRECACHED						        "Precached"
#define TEXTURE_GROUP_CUBE_MAP						        "CubeMap textures"
#define TEXTURE_GROUP_RENDER_TARGET					      "RenderTargets"
#define TEXTURE_GROUP_UNACCOUNTED					        "Unaccounted textures"
//#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER		  "Static Vertex"
#define TEXTURE_GROUP_STATIC_INDEX_BUFFER			    "Static Indices"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_DISP		"Displacement Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_COLOR	"Lighting Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_WORLD	"World Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_MODELS	"Model Verts"
#define TEXTURE_GROUP_STATIC_VERTEX_BUFFER_OTHER	"Other Verts"
#define TEXTURE_GROUP_DYNAMIC_INDEX_BUFFER			  "Dynamic Indices"
#define TEXTURE_GROUP_DYNAMIC_VERTEX_BUFFER			  "Dynamic Verts"
#define TEXTURE_GROUP_DEPTH_BUFFER					      "DepthBuffer"
#define TEXTURE_GROUP_VIEW_MODEL					        "ViewModel"
#define TEXTURE_GROUP_PIXEL_SHADERS					      "Pixel Shaders"
#define TEXTURE_GROUP_VERTEX_SHADERS				      "Vertex Shaders"
#define TEXTURE_GROUP_RENDER_TARGET_SURFACE			  "RenderTarget Surfaces"
#define TEXTURE_GROUP_MORPH_TARGETS					      "Morph Targets"
enum MaterialPrimitiveType_t
{
	MATERIAL_POINTS = 0x0,
	MATERIAL_LINES,
	MATERIAL_TRIANGLES,
	MATERIAL_TRIANGLE_STRIP,
	MATERIAL_LINE_STRIP,
	MATERIAL_LINE_LOOP,	// a single line loop
	MATERIAL_POLYGON,	// this is a *single* polygon
	MATERIAL_QUADS,
	MATERIAL_INSTANCED_QUADS, // (X360) like MATERIAL_QUADS, but uses vertex instancing

							  // This is used for static meshes that contain multiple types of
							  // primitive types.	When calling draw, you'll need to specify
							  // a primitive type.
							  MATERIAL_HETEROGENOUS
};


// acceptable property values for MATERIAL_PROPERTY_OPACITY
enum MaterialPropertyOpacityTypes_t
{
	MATERIAL_ALPHATEST = 0,
	MATERIAL_OPAQUE,
	MATERIAL_TRANSLUCENT
};

enum MaterialBufferTypes_t
{
	MATERIAL_FRONT = 0,
	MATERIAL_BACK
};

enum MaterialCullMode_t
{
	MATERIAL_CULLMODE_CCW,	// this culls polygons with counterclockwise winding
	MATERIAL_CULLMODE_CW	// this culls polygons with clockwise winding
};

enum MaterialIndexFormat_t
{
	MATERIAL_INDEX_FORMAT_UNKNOWN = -1,
	MATERIAL_INDEX_FORMAT_16BIT = 0,
	MATERIAL_INDEX_FORMAT_32BIT,
};

enum MaterialFogMode_t
{
	MATERIAL_FOG_NONE,
	MATERIAL_FOG_LINEAR,
	MATERIAL_FOG_LINEAR_BELOW_FOG_Z,
};

enum MaterialHeightClipMode_t
{
	MATERIAL_HEIGHTCLIPMODE_DISABLE,
	MATERIAL_HEIGHTCLIPMODE_RENDER_ABOVE_HEIGHT,
	MATERIAL_HEIGHTCLIPMODE_RENDER_BELOW_HEIGHT
};

enum MaterialNonInteractiveMode_t
{
	MATERIAL_NON_INTERACTIVE_MODE_NONE = -1,
	MATERIAL_NON_INTERACTIVE_MODE_STARTUP = 0,
	MATERIAL_NON_INTERACTIVE_MODE_LEVEL_LOAD,

	MATERIAL_NON_INTERACTIVE_MODE_COUNT,
};


enum MaterialThreadMode_t
{
	MATERIAL_SINGLE_THREADED,
	MATERIAL_QUEUED_SINGLE_THREADED,
	MATERIAL_QUEUED_THREADED
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

enum MaterialContextType_t
{
	MATERIAL_HARDWARE_CONTEXT,
	MATERIAL_QUEUED_CONTEXT,
	MATERIAL_NULL_CONTEXT
};

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------

enum MaterialFindContext_t
{
	MATERIAL_FINDCONTEXT_NONE,
	MATERIAL_FINDCONTEXT_ISONAMODEL,
};

//-----------------------------------------------------------------------------
// Light structure
//-----------------------------------------------------------------------------


#if 0
enum LightType_t
{
	MATERIAL_LIGHT_DISABLE = 0,
	MATERIAL_LIGHT_POINT,
	MATERIAL_LIGHT_DIRECTIONAL,
	MATERIAL_LIGHT_SPOT,
};
enum LightType_OptimizationFlags_t
{
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION0 = 1,
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION1 = 2,
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION2 = 4,
};
struct LightDesc_t
{
	LightType_t		m_Type;
	Vector			m_Color;
	Vector	m_Position;
	Vector  m_Direction;
	float   m_Range;
	float   m_Falloff;
	float   m_Attenuation0;
	float   m_Attenuation1;
	float   m_Attenuation2;
	float   m_Theta;
	float   m_Phi;
	// These aren't used by DX8. . used for software lighting.
	float	m_ThetaDot;
	float	m_PhiDot;
	unsigned int	m_Flags;
	LightDesc_t() {}
private:
	// No copy constructors allowed
	LightDesc_t(const LightDesc_t& vOther);
};
#endif

#define CREATERENDERTARGETFLAGS_HDR				0x00000001
#define CREATERENDERTARGETFLAGS_AUTOMIPMAP		0x00000002
#define CREATERENDERTARGETFLAGS_UNFILTERABLE_OK 0x00000004
// XBOX ONLY:
#define CREATERENDERTARGETFLAGS_NOEDRAM			0x00000008 // inhibit allocation in 360 EDRAM
#define CREATERENDERTARGETFLAGS_TEMP			0x00000010 // only allocates memory upon first resolve, destroyed at level end


//-----------------------------------------------------------------------------
// allowed stencil operations. These match the d3d operations
//-----------------------------------------------------------------------------
enum StencilOperation_t
{
#if !defined( _X360 )
	STENCILOPERATION_KEEP = 1,
	STENCILOPERATION_ZERO = 2,
	STENCILOPERATION_REPLACE = 3,
	STENCILOPERATION_INCRSAT = 4,
	STENCILOPERATION_DECRSAT = 5,
	STENCILOPERATION_INVERT = 6,
	STENCILOPERATION_INCR = 7,
	STENCILOPERATION_DECR = 8,
#else
	STENCILOPERATION_KEEP = D3DSTENCILOP_KEEP,
	STENCILOPERATION_ZERO = D3DSTENCILOP_ZERO,
	STENCILOPERATION_REPLACE = D3DSTENCILOP_REPLACE,
	STENCILOPERATION_INCRSAT = D3DSTENCILOP_INCRSAT,
	STENCILOPERATION_DECRSAT = D3DSTENCILOP_DECRSAT,
	STENCILOPERATION_INVERT = D3DSTENCILOP_INVERT,
	STENCILOPERATION_INCR = D3DSTENCILOP_INCR,
	STENCILOPERATION_DECR = D3DSTENCILOP_DECR,
#endif
	STENCILOPERATION_FORCE_DWORD = 0x7fffffff
};

enum StencilComparisonFunction_t
{
#if !defined( _X360 )
	STENCILCOMPARISONFUNCTION_NEVER = 1,
	STENCILCOMPARISONFUNCTION_LESS = 2,
	STENCILCOMPARISONFUNCTION_EQUAL = 3,
	STENCILCOMPARISONFUNCTION_LESSEQUAL = 4,
	STENCILCOMPARISONFUNCTION_GREATER = 5,
	STENCILCOMPARISONFUNCTION_NOTEQUAL = 6,
	STENCILCOMPARISONFUNCTION_GREATEREQUAL = 7,
	STENCILCOMPARISONFUNCTION_ALWAYS = 8,
#else
	STENCILCOMPARISONFUNCTION_NEVER = D3DCMP_NEVER,
	STENCILCOMPARISONFUNCTION_LESS = D3DCMP_LESS,
	STENCILCOMPARISONFUNCTION_EQUAL = D3DCMP_EQUAL,
	STENCILCOMPARISONFUNCTION_LESSEQUAL = D3DCMP_LESSEQUAL,
	STENCILCOMPARISONFUNCTION_GREATER = D3DCMP_GREATER,
	STENCILCOMPARISONFUNCTION_NOTEQUAL = D3DCMP_NOTEQUAL,
	STENCILCOMPARISONFUNCTION_GREATEREQUAL = D3DCMP_GREATEREQUAL,
	STENCILCOMPARISONFUNCTION_ALWAYS = D3DCMP_ALWAYS,
#endif

	STENCILCOMPARISONFUNCTION_FORCE_DWORD = 0x7fffffff
};


//-----------------------------------------------------------------------------
// Enumeration for the various fields capable of being morphed
//-----------------------------------------------------------------------------
enum MorphFormatFlags_t
{
	MORPH_POSITION = 0x0001,	// 3D
	MORPH_NORMAL = 0x0002,	// 3D
	MORPH_WRINKLE = 0x0004,	// 1D
	MORPH_SPEED = 0x0008,	// 1D
	MORPH_SIDE = 0x0010,	// 1D
};


//-----------------------------------------------------------------------------
// The morph format type
//-----------------------------------------------------------------------------
typedef unsigned int MorphFormat_t;


//-----------------------------------------------------------------------------
// Standard lightmaps
//-----------------------------------------------------------------------------
enum StandardLightmap_t
{
	MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE = -1,
	MATERIAL_SYSTEM_LIGHTMAP_PAGE_WHITE_BUMP = -2,
	MATERIAL_SYSTEM_LIGHTMAP_PAGE_USER_DEFINED = -3
};


#define MAX_FB_TEXTURES 4

//-----------------------------------------------------------------------------
// Information about each adapter
//-----------------------------------------------------------------------------
enum
{
	MATERIAL_ADAPTER_NAME_LENGTH = 512
};

struct MaterialAdapterInfo_t
{
	char m_pDriverName[MATERIAL_ADAPTER_NAME_LENGTH];
	unsigned int m_VendorID;
	unsigned int m_DeviceID;
	unsigned int m_SubSysID;
	unsigned int m_Revision;
	int m_nDXSupportLevel;			// This is the *preferred* dx support level
	int m_nMaxDXSupportLevel;
	unsigned int m_nDriverVersionHigh;
	unsigned int m_nDriverVersionLow;
};


//-----------------------------------------------------------------------------
// Flags to be used with the Init call
//-----------------------------------------------------------------------------
enum MaterialInitFlags_t
{
	MATERIAL_INIT_ALLOCATE_FULLSCREEN_TEXTURE = 0x2,
	MATERIAL_INIT_REFERENCE_RASTERIZER = 0x4,
};

//-----------------------------------------------------------------------------
// Flags to specify type of depth buffer used with RT
//-----------------------------------------------------------------------------

// GR - this is to add RT with no depth buffer bound

enum MaterialRenderTargetDepth_t
{
	MATERIAL_RT_DEPTH_SHARED = 0x0,
	MATERIAL_RT_DEPTH_SEPARATE = 0x1,
	MATERIAL_RT_DEPTH_NONE = 0x2,
	MATERIAL_RT_DEPTH_ONLY = 0x3,
};

//-----------------------------------------------------------------------------
// A function to be called when we need to release all vertex buffers
// NOTE: The restore function will tell the caller if all the vertex formats
// changed so that it can flush caches, etc. if it needs to (for dxlevel support)
//-----------------------------------------------------------------------------
enum RestoreChangeFlags_t
{
	MATERIAL_RESTORE_VERTEX_FORMAT_CHANGED = 0x1,
};


// NOTE: All size modes will force the render target to be smaller than or equal to
// the size of the framebuffer.
enum RenderTargetSizeMode_t
{
	RT_SIZE_NO_CHANGE = 0,			// Only allowed for render targets that don't want a depth buffer
									// (because if they have a depth buffer, the render target must be less than or equal to the size of the framebuffer).
									RT_SIZE_DEFAULT = 1,				// Don't play with the specified width and height other than making sure it fits in the framebuffer.
									RT_SIZE_PICMIP = 2,				// Apply picmip to the render target's width and height.
									RT_SIZE_HDR = 3,					// frame_buffer_width / 4
									RT_SIZE_FULL_FRAME_BUFFER = 4,	// Same size as frame buffer, or next lower power of 2 if we can't do that.
									RT_SIZE_OFFSCREEN = 5,			// Target of specified size, don't mess with dimensions
									RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP = 6, // Same size as the frame buffer, rounded up if necessary for systems that can't do non-power of two textures.
									RT_SIZE_REPLAY_SCREENSHOT = 7,	// Rounded down to power of 2, essentially...
									RT_SIZE_LITERAL = 8				// Use the size passed in. Don't clamp it to the frame buffer size. Really.
};

typedef void(*MaterialBufferReleaseFunc_t)();
typedef void(*MaterialBufferRestoreFunc_t)(int nChangeFlags);	// see RestoreChangeFlags_t
typedef void(*ModeChangeCallbackFunc_t)(void);

typedef int VertexBufferHandle_t;
typedef unsigned short MaterialHandle_t;

enum MaterialSystem_Config_Flags_t
{
	MATSYS_VIDCFG_FLAGS_WINDOWED = (1 << 0),
	MATSYS_VIDCFG_FLAGS_RESIZING = (1 << 1),
	MATSYS_VIDCFG_FLAGS_NO_WAIT_FOR_VSYNC = (1 << 3),
	MATSYS_VIDCFG_FLAGS_STENCIL = (1 << 4),
	MATSYS_VIDCFG_FLAGS_FORCE_TRILINEAR = (1 << 5),
	MATSYS_VIDCFG_FLAGS_FORCE_HWSYNC = (1 << 6),
	MATSYS_VIDCFG_FLAGS_DISABLE_SPECULAR = (1 << 7),
	MATSYS_VIDCFG_FLAGS_DISABLE_BUMPMAP = (1 << 8),
	MATSYS_VIDCFG_FLAGS_ENABLE_PARALLAX_MAPPING = (1 << 9),
	MATSYS_VIDCFG_FLAGS_USE_Z_PREFILL = (1 << 10),
	MATSYS_VIDCFG_FLAGS_REDUCE_FILLRATE = (1 << 11),
	MATSYS_VIDCFG_FLAGS_ENABLE_HDR = (1 << 12),
	MATSYS_VIDCFG_FLAGS_LIMIT_WINDOWED_SIZE = (1 << 13),
	MATSYS_VIDCFG_FLAGS_SCALE_TO_OUTPUT_RESOLUTION = (1 << 14),
	MATSYS_VIDCFG_FLAGS_USING_MULTIPLE_WINDOWS = (1 << 15),
	MATSYS_VIDCFG_FLAGS_DISABLE_PHONG = (1 << 16),
	MATSYS_VIDCFG_FLAGS_VR_MODE = (1 << 17),
};

struct MaterialSystemHardwareIdentifier_t
{
	char *m_pCardName;
	unsigned int m_nVendorID;
	unsigned int m_nDeviceID;
};
enum HDRType_t
{
	HDR_TYPE_NONE,
	HDR_TYPE_INTEGER,
	HDR_TYPE_FLOAT,
};
enum LightType_t
{
	MATERIAL_LIGHT_DISABLE = 0,
	MATERIAL_LIGHT_POINT,
	MATERIAL_LIGHT_DIRECTIONAL,
	MATERIAL_LIGHT_SPOT,
};

enum VertexCompressionType_t
{
	// This indicates an uninitialized VertexCompressionType_t value
	VERTEX_COMPRESSION_INVALID = 0xFFFFFFFF,

	// 'VERTEX_COMPRESSION_NONE' means that no elements of a vertex are compressed
	VERTEX_COMPRESSION_NONE = 0,

	// Currently (more stuff may be added as needed), 'VERTEX_COMPRESSION_ON' means:
	//  - if a vertex contains VERTEX_ELEMENT_NORMAL, this is compressed
	//    (see CVertexBuilder::CompressedNormal3f)
	//  - if a vertex contains VERTEX_ELEMENT_USERDATA4 (and a normal - together defining a tangent
	//    frame, with the binormal reconstructed in the vertex shader), this is compressed
	//    (see CVertexBuilder::CompressedUserData)
	//  - if a vertex contains VERTEX_ELEMENT_BONEWEIGHTSx, this is compressed
	//    (see CVertexBuilder::CompressedBoneWeight3fv)
	VERTEX_COMPRESSION_ON = 1
};
enum
{
	VERTEX_MAX_TEXTURE_COORDINATES = 8,
	BONE_MATRIX_INDEX_INVALID = 255
};
//-----------------------------------------------------------------------------
// MaterialRect_t struct - used for DrawDebugText
//-----------------------------------------------------------------------------
struct Rect_t
{
	int x, y;
	int width, height;
};

class ICallQueue
{
public:
private:
};

// nonlinear transformations which may be applied to model vertices when rendering. must be powers of two
enum DeformationType_t
{
	DEFORMATION_CLAMP_TO_BOX_IN_WORLDSPACE = 1,							// minxyz.minsoftness / maxxyz.maxsoftness
};


struct DeformationBase_t										// base class. don't use this
{
	DeformationType_t m_eType;
};
typedef unsigned int ColorCorrectionHandle_t;

//-----------------------------------------------------------------------------
// Helper class for begin/end of pix event via constructor/destructor 
//-----------------------------------------------------------------------------
#define PIX_VALVE_ORANGE	0xFFF5940F

enum CompiledVtfFlags
{
	// flags from the *.txt config file
	TEXTUREFLAGS_POINTSAMPLE = 0x00000001,
	TEXTUREFLAGS_TRILINEAR = 0x00000002,
	TEXTUREFLAGS_CLAMPS = 0x00000004,
	TEXTUREFLAGS_CLAMPT = 0x00000008,
	TEXTUREFLAGS_ANISOTROPIC = 0x00000010,
	TEXTUREFLAGS_HINT_DXT5 = 0x00000020,
	TEXTUREFLAGS_SRGB = 0x00000040,
	TEXTUREFLAGS_NORMAL = 0x00000080,
	TEXTUREFLAGS_NOMIP = 0x00000100,
	TEXTUREFLAGS_NOLOD = 0x00000200,
	TEXTUREFLAGS_ALL_MIPS = 0x00000400,
	TEXTUREFLAGS_PROCEDURAL = 0x00000800,

	// These are automatically generated by vtex from the texture data.
	TEXTUREFLAGS_ONEBITALPHA = 0x00001000,
	TEXTUREFLAGS_EIGHTBITALPHA = 0x00002000,

	// newer flags from the *.txt config file
	TEXTUREFLAGS_ENVMAP = 0x00004000,
	TEXTUREFLAGS_RENDERTARGET = 0x00008000,
	TEXTUREFLAGS_DEPTHRENDERTARGET = 0x00010000,
	TEXTUREFLAGS_NODEBUGOVERRIDE = 0x00020000,
	TEXTUREFLAGS_SINGLECOPY = 0x00040000,

	TEXTUREFLAGS_UNUSED_00080000 = 0x00080000,
	TEXTUREFLAGS_UNUSED_00100000 = 0x00100000,
	TEXTUREFLAGS_UNUSED_00200000 = 0x00200000,
	TEXTUREFLAGS_UNUSED_00400000 = 0x00400000,

	TEXTUREFLAGS_NODEPTHBUFFER = 0x00800000,

	TEXTUREFLAGS_UNUSED_01000000 = 0x01000000,

	TEXTUREFLAGS_CLAMPU = 0x02000000,

	TEXTUREFLAGS_VERTEXTEXTURE = 0x04000000,					// Useable as a vertex texture

	TEXTUREFLAGS_SSBUMP = 0x08000000,

	TEXTUREFLAGS_UNUSED_10000000 = 0x10000000,

	// Clamp to border color on all texture coordinates
	TEXTUREFLAGS_BORDER = 0x20000000,

	TEXTUREFLAGS_UNUSED_40000000 = 0x40000000,
	TEXTUREFLAGS_UNUSED_80000000 = 0x80000000,
};

// helper class for user commands
class CBotCmd
{
public:
	CBotCmd()
	{
		Reset();
	}

	virtual ~CBotCmd() { };

	void Reset()
	{
		command_number = 0;
		tick_count = 0;
		viewangles.Init();
		forwardmove = 0.0f;
		sidemove = 0.0f;
		upmove = 0.0f;
		buttons = 0;
		impulse = 0;
		weaponselect = 0;
		weaponsubtype = 0;
		random_seed = 0;
		mousedx = 0;
		mousedy = 0;

		hasbeenpredicted = false;
	}

	CBotCmd& operator =(const CBotCmd& src)
	{
		if (this == &src)
			return *this;

		command_number = src.command_number;
		tick_count = src.tick_count;
		viewangles = src.viewangles;
		forwardmove = src.forwardmove;
		sidemove = src.sidemove;
		upmove = src.upmove;
		buttons = src.buttons;
		impulse = src.impulse;
		weaponselect = src.weaponselect;
		weaponsubtype = src.weaponsubtype;
		random_seed = src.random_seed;
		mousedx = src.mousedx;
		mousedy = src.mousedy;
		hasbeenpredicted = src.hasbeenpredicted;
		return *this;
	}

	// For matching server and client commands for debugging
	int		command_number;

	// the tick the client created this command
	int		tick_count;

	// Player instantaneous view angles.
	QAngle	viewangles;
	// Intended velocities
	//	forward velocity.
	float	forwardmove;
	//  sideways velocity.
	float	sidemove;
	//  upward velocity.
	float	upmove;
	// Attack button states
	int		buttons;
	// Impulse command issued.
	byte    impulse;
	// Current weapon id
	int		weaponselect;
	int		weaponsubtype;

	int		random_seed;	// For shared random functions

	short	mousedx;		// mouse accum in x from create move
	short	mousedy;		// mouse accum in y from create move

							// Client only, tracks whether we've predicted this command at least once
	bool	hasbeenpredicted;
};




struct edict_t
{
public:
	// The server timestampe at which the edict was freed (so we can try to use other edicts before reallocating this one)
	float		freetime;
};

//-----------------------------------------------------------------------------
// Purpose: Defines the ways that a map can be loaded.
//-----------------------------------------------------------------------------
enum MapLoadType_t
{
	MapLoad_NewGame = 0,
	MapLoad_LoadGame,
	MapLoad_Transition,
	MapLoad_Background,
};

//-----------------------------------------------------------------------------

struct string_t
{
public:
	bool operator!() const { return (pszValue == NULL); }
	bool operator==(const string_t &rhs) const { return (pszValue == rhs.pszValue); }
	bool operator!=(const string_t &rhs) const { return (pszValue != rhs.pszValue); }
	bool operator<(const string_t &rhs) const { return ((void *)pszValue < (void *)rhs.pszValue); }

	const char *ToCStr() const { return (pszValue) ? pszValue : ""; }

protected:
	const char *pszValue;
};

//hint\\
//-makereslists
//C_EnvScreenOverlay::PostDataUpdate
//view->GetScreenOverlayMaterial() == NULL

// These are set as it draws reflections, refractions, etc, so certain effects can avoid 
// drawing themselves in reflections.
enum DrawFlags_t
{
	DF_RENDER_REFRACTION = 0x1,
	DF_RENDER_REFLECTION = 0x2,

	DF_CLIP_Z = 0x4,
	DF_CLIP_BELOW = 0x8,

	DF_RENDER_UNDERWATER = 0x10,
	DF_RENDER_ABOVEWATER = 0x20,
	DF_RENDER_WATER = 0x40,

	DF_SSAO_DEPTH_PASS = 0x100,
	DF_WATERHEIGHT = 0x200,
	DF_DRAW_SSAO = 0x400,
	DF_DRAWSKYBOX = 0x800,

	DF_FUDGE_UP = 0x1000,

	DF_DRAW_ENTITITES = 0x2000,
	DF_UNUSED3 = 0x4000,

	DF_UNUSED4 = 0x8000,

	DF_UNUSED5 = 0x10000,
	DF_SAVEGAMESCREENSHOT = 0x20000,
	DF_CLIP_SKYBOX = 0x40000,

	DF_SHADOW_DEPTH_MAP = 0x100000	// Currently rendering a shadow depth map
};

enum modtype_t
{
	mod_bad = 0,
	mod_brush,
	mod_sprite,
	mod_studio
};

enum MaterialPropertyTypes_t
{
	MATERIAL_PROPERTY_NEEDS_LIGHTMAP = 0,					// bool
	MATERIAL_PROPERTY_OPACITY,								// int (enum MaterialPropertyOpacityTypes_t)
	MATERIAL_PROPERTY_REFLECTIVITY,							// vec3_t
	MATERIAL_PROPERTY_NEEDS_BUMPED_LIGHTMAPS				// bool
};
typedef unsigned short MDLHandle_t;

struct brushdata_t
{
	void	*pShared;
	int			firstmodelsurface, nummodelsurfaces;

	unsigned short	renderHandle;
	unsigned short	firstnode;
};


// only models with type "mod_sprite" have this data
struct spritedata_t
{
	int				numframes;
	int				width;
	int				height;
	void	        *sprite;
};
struct model_t
{
	void*	fnHandle;
	char				szName[96];

	int					nLoadFlags;		// mark loaded/not loaded
	int					nServerCount;	// marked at load

	modtype_t			type;
	int					flags;			// MODELFLAG_???

										// volume occupied by the model graphics	
	Vector				mins, maxs;
	float				radius;

	union
	{
		brushdata_t		brush;
		MDLHandle_t		studio;
		spritedata_t	sprite;
	};
};

enum OverrideType_t
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
};

typedef unsigned short ModelInstanceHandle_t;

typedef uint64 VertexFormat_t;


//-----------------------------------------------------------------------------
// Preview image return values
//-----------------------------------------------------------------------------
enum PreviewImageRetVal_t
{
	MATERIAL_PREVIEW_IMAGE_BAD = 0,
	MATERIAL_PREVIEW_IMAGE_OK,
	MATERIAL_NO_PREVIEW_IMAGE,
};

enum ImageFormat
{
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,			// Single-channel 32-bit floating point
	IMAGE_FORMAT_RGB323232F,
	IMAGE_FORMAT_RGBA32323232F,

	// Depth-stencil texture formats for shadow depth mapping
	IMAGE_FORMAT_NV_DST16,		// 
	IMAGE_FORMAT_NV_DST24,		//
	IMAGE_FORMAT_NV_INTZ,		// Vendor-specific depth-stencil texture
	IMAGE_FORMAT_NV_RAWZ,		// formats for shadow depth mapping 
	IMAGE_FORMAT_ATI_DST16,		// 
	IMAGE_FORMAT_ATI_DST24,		//
	IMAGE_FORMAT_NV_NULL,		// Dummy format which takes no video memory

								// Compressed normal map formats
								IMAGE_FORMAT_ATI2N,			// One-surface ATI2N / DXN format
								IMAGE_FORMAT_ATI1N,			// Two-surface ATI1N format


								NUM_IMAGE_FORMATS
};


//-----------------------------------------------------------------------------
// Shader state flags can be read from the FLAGS materialvar
// Also can be read or written to with the Set/GetMaterialVarFlags() call
// Also make sure you add/remove a string associated with each flag below to CShaderSystem::ShaderStateString in ShaderSystem.cpp
//-----------------------------------------------------------------------------
enum MaterialVarFlags_t
{
	MATERIAL_VAR_DEBUG = (1 << 0),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
	MATERIAL_VAR_NO_DRAW = (1 << 2),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),

	MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
	MATERIAL_VAR_VERTEXALPHA = (1 << 5),
	MATERIAL_VAR_SELFILLUM = (1 << 6),
	MATERIAL_VAR_ADDITIVE = (1 << 7),
	MATERIAL_VAR_ALPHATEST = (1 << 8),
	MATERIAL_VAR_MULTIPASS = (1 << 9),
	MATERIAL_VAR_ZNEARER = (1 << 10),
	MATERIAL_VAR_MODEL = (1 << 11),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_NOCULL = (1 << 13),
	MATERIAL_VAR_NOFOG = (1 << 14),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_DECAL = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17),
	MATERIAL_VAR_NOALPHAMOD = (1 << 18),
	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19),
	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23),
	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
	MATERIAL_VAR_ENVMAPMODE = (1 << 25),
	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
	MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),

	// NOTE: Only add flags here that either should be read from
	// .vmts or can be set directly from client code. Other, internal
	// flags should to into the flag enum in IMaterialInternal.h
};



//-----------------------------------------------------------------------------
// The morph format type
//-----------------------------------------------------------------------------
typedef unsigned int MorphFormat_t;
//-----------------------------------------------------------------------------
// Various material var types
//-----------------------------------------------------------------------------
enum MaterialVarType_t
{
	MATERIAL_VAR_TYPE_FLOAT = 0,
	MATERIAL_VAR_TYPE_STRING,
	MATERIAL_VAR_TYPE_VECTOR,
	MATERIAL_VAR_TYPE_TEXTURE,
	MATERIAL_VAR_TYPE_INT,
	MATERIAL_VAR_TYPE_FOURCC,
	MATERIAL_VAR_TYPE_UNDEFINED,
	MATERIAL_VAR_TYPE_MATRIX,
	MATERIAL_VAR_TYPE_MATERIAL,
};

typedef unsigned short MaterialVarSym_t;
typedef unsigned short UtlSymId_t;


//-----------------------------------------------------------------------------
// A ray...
//-----------------------------------------------------------------------------

struct Ray_t
{
	Vector4D  m_Start;	// starting point, centered within the extents
	Vector4D  m_Delta;	// direction + length of the ray
	Vector4D  m_StartOffset;	// Add this to m_Start to get the actual ray start
	Vector4D  m_Extents;	// Describes an axis aligned box extruded along a ray
	bool	m_IsRay;	// are the extents zero?
	bool	m_IsSwept;	// is delta != 0?

	void Init(Vector const& start, Vector const& end)
	{

		m_Delta = (end - start).ToVector4D();
		m_IsSwept = (m_Delta.LengthSquared() != 0.f);
		m_Extents.Init();
		m_IsRay = true;
		m_StartOffset.Init();
		m_Start = start.ToVector4D();
	}

	void Init(Vector const& start, Vector const& end, Vector const& mins, Vector const& maxs)
	{
		m_Delta = (end - start).ToVector4D();
		m_IsSwept = (m_Delta.LengthSquared() != 0.f);
		m_Extents = (maxs - mins).ToVector4D();
		m_Extents *= 0.5f;
		m_IsRay = (m_Extents.LengthSquared() < 1e-6f);
		m_StartOffset = (maxs + mins).ToVector4D();
		m_StartOffset *= 0.5f;
		m_Start = start.ToVector4D() + m_StartOffset;
		m_StartOffset *= -1.f;
	}

	// compute inverse delta
	Vector InvDelta() const
	{
		Vector vecInvDelta;
		for (int iAxis = 0; iAxis < 3; ++iAxis)
		{
			if (m_Delta[iAxis] != 0.0f)
			{
				vecInvDelta[iAxis] = 1.0f / m_Delta[iAxis];
			}
			else
			{
				vecInvDelta[iAxis] = FLT_MAX;
			}
		}
		return vecInvDelta;
	}

};
#include <math.h>
#include <xmmintrin.h>
#include <stdio.h>
#include <string.h>
__forceinline float FastSqrt(float x)
{
	__m128 root = _mm_sqrt_ss(_mm_load_ss(&x));
	return *(reinterpret_cast<float *>(&root));
}

__forceinline float FastRSqrtFast(float x)
{
	// use intrinsics
	__m128 rroot = _mm_rsqrt_ss(_mm_load_ss(&x));
	return *(reinterpret_cast<float *>(&rroot));
}
typedef float vec_t;
typedef int qboolean;


// In case this ever changes
#ifndef M_PI
#define M_PI			3.14159265358979323846
#endif

inline uint32& FloatBits(vec_t& f)
{
	return *reinterpret_cast<uint32*>((char*)(&f));
}

inline uint32 const FloatBits(const vec_t &f)
{
	union Convertor_t
	{
		vec_t f;
		uint32 ul;
	}tmp;
	tmp.f = f;
	return tmp.ul;
}

inline vec_t BitsToFloat(uint32 i)
{
	union Convertor_t
	{
		vec_t f;
		unsigned long ul;
	}tmp;
	tmp.ul = i;
	return tmp.f;
}

inline bool IsFinite(const vec_t &f)
{
	return ((FloatBits(f) & 0x7F800000) != 0x7F800000);
}

#define FLOAT32_NAN_BITS     (uint32)0x7FC00000	// not a number!
#define FLOAT32_NAN          BitsToFloat( FLOAT32_NAN_BITS )
#define M_RADPI 57.295779513082f

#define FORWARD_DECLARE_HANDLE(name) typedef struct name##__ *name

#define VEC_T_NAN FLOAT32_NAN

#define MAX_SPLITSCREEN_CLIENT_BITS 2
// this should == MAX_JOYSTICKS in InputEnums.h
#define MAX_SPLITSCREEN_CLIENTS	( 1 << MAX_SPLITSCREEN_CLIENT_BITS ) // 4

#define SIGNONSTATE_NONE		0	// no state yet, about to connect
#define SIGNONSTATE_CHALLENGE	1	// client challenging server, all OOB packets
#define SIGNONSTATE_CONNECTED	2	// client is connected to server, netchans ready
#define SIGNONSTATE_NEW			3	// just got serverinfo and string tables
#define SIGNONSTATE_PRESPAWN	4	// received signon buffers
#define SIGNONSTATE_SPAWN		5	// ready to receive entity packets
#define SIGNONSTATE_FULL		6	// we are fully connected, first non-delta packet received
#define SIGNONSTATE_CHANGELEVEL	7	// server is changing level, please wait

#define FLOW_OUTGOING	0		
#define FLOW_INCOMING	1
#define MAX_FLOWS		2		// in & out


enum netadrtype_t
{
	NA_NULL = 0,
	NA_LOOPBACK,
	NA_BROADCAST,
	NA_IP,
};
struct netadr_s
{
public:
	netadrtype_t	type;
	unsigned char	ip[4];
	unsigned short	port;
};

class bf_read;
class bf_write;


class CMoveData
{
public:
	bool			m_bFirstRunOfFunctions : 1;
	bool			m_bGameCodeMovedPlayer : 1;

	void*			m_nPlayerHandle;	// edict index on server, client entity handle on client

	int				m_nImpulseCommand;	// Impulse command issued.
	QAngle			m_vecViewAngles;	// Command view angles (local space)
	QAngle			m_vecAbsViewAngles;	// Command view angles (world space)
	int				m_nButtons;			// Attack buttons.
	int				m_nOldButtons;		// From host_client->oldbuttons;
	float			m_flForwardMove;
	float			m_flSideMove;
	float			m_flUpMove;

	float			m_flMaxSpeed;
	float			m_flClientMaxSpeed;

	// Variables from the player edict (sv_player) or entvars on the client.
	// These are copied in here before calling and copied out after calling.
	Vector			m_vecVelocity;		// edict::velocity		// Current movement direction.
	QAngle			m_vecAngles;		// edict::angles
	QAngle			m_vecOldAngles;

	// Output only
	float			m_outStepHeight;	// how much you climbed this move
	Vector			m_outWishVel;		// This is where you tried 
	Vector			m_outJumpVel;		// This is your jump velocity

	// Movement constraints	(radius 0 means no constraint)
	Vector			m_vecConstraintCenter;
	float			m_flConstraintRadius;
	float			m_flConstraintWidth;
	float			m_flConstraintSpeedFactor;

	Vector			m_vecAbsOrigin;		// edict::origin


	char pad[0x1000];
};

enum ShouldTransmitState_t
{
	SHOULDTRANSMIT_START = 0,	// The entity is starting to be transmitted (maybe it entered the PVS).

	SHOULDTRANSMIT_END		// Called when the entity isn't being transmitted by the server.
							// This signals a good time to hide the entity until next time
							// the server wants to transmit its state.
};

// NOTE: All of these are commented out; NotifyShouldTransmit actually
// has all these in them. Left it as an enum in case we want to go back though
enum DataUpdateType_t
{
	DATA_UPDATE_CREATED = 0,	// indicates it was created +and+ entered the pvs
								//	DATA_UPDATE_ENTERED_PVS,
								DATA_UPDATE_DATATABLE_CHANGED,
								//	DATA_UPDATE_LEFT_PVS,
								//	DATA_UPDATE_DESTROYED,		// FIXME: Could enable this, but it's a little worrying
								// since it changes a bunch of existing code
};

struct StudioRenderConfig_t
{
	float fEyeShiftX;	// eye X position
	float fEyeShiftY;	// eye Y position
	float fEyeShiftZ;	// eye Z position
	float fEyeSize;		// adjustment to iris textures
	float fEyeGlintPixelWidthLODThreshold;

	int maxDecalsPerModel;
	int drawEntities;
	int skin;
	int fullbright;

	bool bEyeMove : 1;		// look around
	bool bSoftwareSkin : 1;
	bool bNoHardware : 1;
	bool bNoSoftware : 1;
	bool bTeeth : 1;
	bool bEyes : 1;
	bool bFlex : 1;
	bool bWireframe : 1;
	bool bDrawNormals : 1;
	bool bDrawTangentFrame : 1;
	bool bDrawZBufferedWireframe : 1;
	bool bSoftwareLighting : 1;
	bool bShowEnvCubemapOnly : 1;
	bool bWireframeDecals : 1;

	// Reserved for future use
	int m_nReserved[4];
};
struct DrawModelResults_t
{
	int m_ActualTriCount;
	int m_TextureMemoryBytes;
	int m_NumHardwareBones;
	int m_NumBatches;
	int m_NumMaterials;
	int m_nLODUsed;
	int m_flLODMetric;
	int m_nReserved[4];
};

//-----------------------------------------------------------------------------
// DrawModel flags
//-----------------------------------------------------------------------------
enum
{
	STUDIORENDER_DRAW_ENTIRE_MODEL = 0,
	STUDIORENDER_DRAW_OPAQUE_ONLY = 0x01,
	STUDIORENDER_DRAW_TRANSLUCENT_ONLY = 0x02,
	STUDIORENDER_DRAW_GROUP_MASK = 0x03,

	STUDIORENDER_DRAW_NO_FLEXES = 0x04,
	STUDIORENDER_DRAW_STATIC_LIGHTING = 0x08,

	STUDIORENDER_DRAW_ACCURATETIME = 0x10,		// Use accurate timing when drawing the model.
	STUDIORENDER_DRAW_NO_SHADOWS = 0x20,
	STUDIORENDER_DRAW_GET_PERF_STATS = 0x40,

	STUDIORENDER_DRAW_WIREFRAME = 0x80,

	STUDIORENDER_DRAW_ITEM_BLINK = 0x100,

	STUDIORENDER_SHADOWDEPTHTEXTURE = 0x200,

	STUDIORENDER_SSAODEPTHTEXTURE = 0x1000,

	STUDIORENDER_GENERATE_STATS = 0x8000,
};

// handles to vgui objects
// NULL values signify an invalid value
typedef unsigned long HScheme;

typedef unsigned int VPANEL;

enum EInterfaceID
{
	ICLIENTPANEL_STANDARD_INTERFACE = 0,
};

struct vcollide_t
{
	unsigned short solidCount : 15;
	unsigned short isPacked : 1;
	unsigned short descSize;
	// VPhysicsSolids
	void        	**solids;
	char			*pKeyValues;
};
enum VGuiPanel_t
{
	PANEL_ROOT = 0,
	PANEL_GAMEUIDLL,
	PANEL_CLIENTDLL,
	PANEL_TOOLS,
	PANEL_INGAMESCREENS,
	PANEL_GAMEDLL,
	PANEL_CLIENTDLL_TOOLS
};

struct IntRect
{
	int x0;
	int y0;
	int x1;
	int y1;
};
typedef unsigned long HCursor;
typedef unsigned long HTexture;
typedef unsigned long HFont;

enum FontDrawType_t
{
	// Use the "additive" value from the scheme file
	FONT_DRAW_DEFAULT = 0,

	// Overrides
	FONT_DRAW_NONADDITIVE,
	FONT_DRAW_ADDITIVE,

	FONT_DRAW_TYPE_COUNT = 2,
};
struct Vertex_t
{
	Vertex_t() {}
	Vertex_t(const Vector2D &pos, const Vector2D &coord = Vector2D(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}
	void Init(const Vector2D &pos, const Vector2D &coord = Vector2D(0, 0))
	{
		m_Position = pos;
		m_TexCoord = coord;
	}

	Vector2D	m_Position;
	Vector2D	m_TexCoord;
};

// Refactor these two
struct CharRenderInfo
{
	// Text pos
	int				x, y;
	// Top left and bottom right
	// This is now a pointer to an array maintained by the surface, to avoid copying the data on the 360
	Vertex_t		*verts;
	int				textureId;
	int				abcA;
	int				abcB;
	int				abcC;
	int				fontTall;
	HFont			currentFont;
	// In:
	FontDrawType_t	drawType;
	wchar_t			ch;

	// Out
	bool			valid;
	// In/Out (true by default)
	bool			shouldclip;
};
struct InputEvent_t
{
	int m_nType;				// Type of the event (see InputEventType_t)
	int m_nTick;				// Tick on which the event occurred
	int m_nData;				// Generic 32-bit data, what it contains depends on the event
	int m_nData2;				// Generic 32-bit data, what it contains depends on the event
	int m_nData3;				// Generic 32-bit data, what it contains depends on the event
};
// In-game panels are cropped to the current engine viewport size
enum PaintMode_t
{
	PAINT_UIPANELS = (1 << 0),
	PAINT_INGAMEPANELS = (1 << 1),
	PAINT_CURSOR = (1 << 2), // software cursor, if appropriate
};
// enumeration of level loading progress bar spots
enum LevelLoadingProgress_e
{
	PROGRESS_NONE,
	PROGRESS_CHANGELEVEL,
	PROGRESS_SPAWNSERVER,
	PROGRESS_LOADWORLDMODEL,
	PROGRESS_CRCMAP,
	PROGRESS_CRCCLIENTDLL,
	PROGRESS_CREATENETWORKSTRINGTABLES,
	PROGRESS_PRECACHEWORLD,
	PROGRESS_CLEARWORLD,
	PROGRESS_LEVELINIT,
	PROGRESS_PRECACHE,
	PROGRESS_ACTIVATESERVER,
	PROGRESS_BEGINCONNECT,
	PROGRESS_SIGNONCHALLENGE,
	PROGRESS_SIGNONCONNECT,
	PROGRESS_SIGNONCONNECTED,
	PROGRESS_PROCESSSERVERINFO,
	PROGRESS_PROCESSSTRINGTABLE,
	PROGRESS_SIGNONNEW,
	PROGRESS_SENDCLIENTINFO,
	PROGRESS_SENDSIGNONDATA,
	PROGRESS_SIGNONSPAWN,
	PROGRESS_FULLYCONNECTED,
	PROGRESS_READYTOPLAY,
	PROGRESS_HIGHESTITEM,	// must be last item in list
};
enum EFontFlags
{
	FONTFLAG_NONE,
	FONTFLAG_ITALIC = 0x001,
	FONTFLAG_UNDERLINE = 0x002,
	FONTFLAG_STRIKEOUT = 0x004,
	FONTFLAG_SYMBOL = 0x008,
	FONTFLAG_ANTIALIAS = 0x010,
	FONTFLAG_GAUSSIANBLUR = 0x020,
	FONTFLAG_ROTARY = 0x040,
	FONTFLAG_DROPSHADOW = 0x080,
	FONTFLAG_ADDITIVE = 0x100,
	FONTFLAG_OUTLINE = 0x200,
	FONTFLAG_CUSTOM = 0x400,
	FONTFLAG_BITMAP = 0x800,
};


//-----------------------------------------------------------------------------
// Callbacks for mouse getting + setting
//-----------------------------------------------------------------------------
typedef void(*GetMouseCallback_t)(int &x, int &y);
typedef void(*SetMouseCallback_t)(int x, int y);

//-----------------------------------------------------------------------------
// Callbacks for sound playing
//-----------------------------------------------------------------------------
typedef void(*PlaySoundFunc_t)(const char *pFileName);



enum AnalogCode_t
{
	ANALOG_CODE_INVALID = -1,
	MOUSE_X = 0,
	MOUSE_Y,
	MOUSE_XY,		// Invoked when either x or y changes
	MOUSE_WHEEL,

	JOYSTICK_FIRST_AXIS,
	JOYSTICK_LAST_AXIS,
	ANALOG_CODE_LAST,
};
#define INVALID_USER_ID		-1
#define MAXSTUDIOPOSEPARAM	24
#define MAXSTUDIOBONE 256

#define BONE_USED_MASK				0x0007FF00
#define BONE_USED_BY_ANYTHING		0x0007FF00
#define BONE_USED_BY_HITBOX			0x00000100	// bone (or child) is used by a hit box
#define BONE_USED_BY_ATTACHMENT		0x00000200	// bone (or child) is used by an attachment point
#define BONE_USED_BY_VERTEX_MASK	0x0003FC00
#define BONE_USED_BY_VERTEX_LOD0	0x00000400	// bone (or child) is used by the toplevel model via skinned vertex
#define BONE_USED_BY_VERTEX_LOD1	0x00000800	
#define BONE_USED_BY_VERTEX_LOD2	0x00001000  
#define BONE_USED_BY_VERTEX_LOD3	0x00002000
#define BONE_USED_BY_VERTEX_LOD4	0x00004000
#define BONE_USED_BY_VERTEX_LOD5	0x00008000
#define BONE_USED_BY_VERTEX_LOD6	0x00010000
#define BONE_USED_BY_VERTEX_LOD7	0x00020000
#define BONE_USED_BY_BONE_MERGE		0x00040000	// bone is available for bone merge to occur against it



struct FireBulletsInfo_t
{
	FireBulletsInfo_t()
	{
		m_iShots = 1;
		m_vecSpread.Init(0, 0, 0);
		m_flDistance = 8192;
		m_iTracerFreq = 4;
		m_iDamage = 0;
		m_iPlayerDamage = 0;
		m_pAttacker = NULL;
		m_nFlags = 0;
		m_pAdditionalIgnoreEnt = NULL;
		m_flDamageForceScale = 1.0f;

#ifdef _DEBUG
		m_iAmmoType = -1;
		m_vecSrc.Init(VEC_T_NAN, VEC_T_NAN, VEC_T_NAN);
		m_vecDirShooting.Init(VEC_T_NAN, VEC_T_NAN, VEC_T_NAN);
#endif
		m_bPrimaryAttack = true;
	}

	FireBulletsInfo_t(int nShots, const Vector &vecSrc, const Vector &vecDir, const Vector &vecSpread, float flDistance, int nAmmoType, bool bPrimaryAttack = true)
	{
		m_iShots = nShots;
		m_vecSrc = vecSrc;
		m_vecDirShooting = vecDir;
		m_vecSpread = vecSpread;
		m_flDistance = flDistance;
		m_iAmmoType = nAmmoType;
		m_iTracerFreq = 4;
		m_iDamage = 0;
		m_iPlayerDamage = 0;
		m_pAttacker = NULL;
		m_nFlags = 0;
		m_pAdditionalIgnoreEnt = NULL;
		m_flDamageForceScale = 1.0f;
		m_bPrimaryAttack = bPrimaryAttack;
	}

	int m_iShots;
	Vector m_vecSrc;
	Vector m_vecDirShooting;
	Vector m_vecSpread;
	float m_flDistance;
	int m_iAmmoType;
	int m_iTracerFreq;
	int m_iDamage;
	int m_iPlayerDamage;	// Damage to be used instead of m_iDamage if we hit a player
	int m_nFlags;			// See FireBulletsFlags_t
	float m_flDamageForceScale;
	IClientEntity *m_pAttacker;
	IClientEntity *m_pAdditionalIgnoreEnt;
	bool m_bPrimaryAttack;
};
enum PlayerAnimEvent_t
{
	PLAYERANIMEVENT_ATTACK_PRIMARY,
	PLAYERANIMEVENT_ATTACK_SECONDARY,
	PLAYERANIMEVENT_ATTACK_GRENADE,
	PLAYERANIMEVENT_RELOAD,
	PLAYERANIMEVENT_RELOAD_LOOP,
	PLAYERANIMEVENT_RELOAD_END,
	PLAYERANIMEVENT_JUMP,
	PLAYERANIMEVENT_SWIM,
	PLAYERANIMEVENT_DIE,
	PLAYERANIMEVENT_FLINCH_CHEST,
	PLAYERANIMEVENT_FLINCH_HEAD,
	PLAYERANIMEVENT_FLINCH_LEFTARM,
	PLAYERANIMEVENT_FLINCH_RIGHTARM,
	PLAYERANIMEVENT_FLINCH_LEFTLEG,
	PLAYERANIMEVENT_FLINCH_RIGHTLEG,
	PLAYERANIMEVENT_DOUBLEJUMP,

	// Cancel.
	PLAYERANIMEVENT_CANCEL,
	PLAYERANIMEVENT_SPAWN,

	// Snap to current yaw exactly
	PLAYERANIMEVENT_SNAP_YAW,

	PLAYERANIMEVENT_CUSTOM,				// Used to play specific activities
	PLAYERANIMEVENT_CUSTOM_GESTURE,
	PLAYERANIMEVENT_CUSTOM_SEQUENCE,	// Used to play specific sequences
	PLAYERANIMEVENT_CUSTOM_GESTURE_SEQUENCE,

	// TF Specific. Here until there's a derived game solution to this.
	PLAYERANIMEVENT_ATTACK_PRE,
	PLAYERANIMEVENT_ATTACK_POST,
	PLAYERANIMEVENT_GRENADE1_DRAW,
	PLAYERANIMEVENT_GRENADE2_DRAW,
	PLAYERANIMEVENT_GRENADE1_THROW,
	PLAYERANIMEVENT_GRENADE2_THROW,
	PLAYERANIMEVENT_VOICE_COMMAND_GESTURE,
	PLAYERANIMEVENT_DOUBLEJUMP_CROUCH,
	PLAYERANIMEVENT_STUN_BEGIN,
	PLAYERANIMEVENT_STUN_MIDDLE,
	PLAYERANIMEVENT_STUN_END,
	PLAYERANIMEVENT_PASSTIME_THROW_BEGIN,
	PLAYERANIMEVENT_PASSTIME_THROW_MIDDLE,
	PLAYERANIMEVENT_PASSTIME_THROW_END,
	PLAYERANIMEVENT_PASSTIME_THROW_CANCEL,

	PLAYERANIMEVENT_ATTACK_PRIMARY_SUPER,

	PLAYERANIMEVENT_COUNT
};

// Gesture Slots.
enum
{
	GESTURE_SLOT_ATTACK_AND_RELOAD,
	GESTURE_SLOT_GRENADE,
	GESTURE_SLOT_JUMP,
	GESTURE_SLOT_SWIM,
	GESTURE_SLOT_FLINCH,
	GESTURE_SLOT_VCD,
	GESTURE_SLOT_CUSTOM,

	GESTURE_SLOT_COUNT,
};

#define GESTURE_SLOT_INVALID	-1

class C_AnimationLayer
{
public:
	int		m_nSequence;
	float	m_flPrevCycle;
	float	m_flWeight;
	int		m_nOrder;

	// used for automatic crossfades between sequence changes
	float		m_flPlaybackRate;
	float		m_flCycle;

	float	m_flLayerAnimtime;
	float	m_flLayerFadeOuttime;
	float   m_flBlendIn;
	float   m_flBlendOut;

	bool    m_bClientBlend;
};//Size: 0x002C

struct GestureSlot_t
{
	int					m_iGestureSlot;
	DWORD				m_iActivity;
	bool				m_bAutoKill;
	bool				m_bActive;
	C_AnimationLayer	*m_pAnimLayer;
};


struct MultiPlayerPoseData_t
{
	int			m_iMoveX;
	int			m_iMoveY;
	int			m_iAimYaw;
	int			m_iAimPitch;
	int			m_iBodyHeight;
	int			m_iMoveYaw;
	int			m_iMoveScale;

	float		m_flEstimateYaw;
	float		m_flLastAimTurnTime;

	void Init()
	{
		m_iMoveX = 0;
		m_iMoveY = 0;
		m_iAimYaw = 0;
		m_iAimPitch = 0;
		m_iBodyHeight = 0;
		m_iMoveYaw = 0;
		m_iMoveScale = 0;
		m_flEstimateYaw = 0.0f;
		m_flLastAimTurnTime = 0.0f;
	}
};

struct DebugPlayerAnimData_t
{
	float		m_flSpeed;
	float		m_flAimPitch;
	float		m_flAimYaw;
	float		m_flBodyHeight;
	Vector2D	m_vecMoveYaw;

	void Init()
	{
		m_flSpeed = 0.0f;
		m_flAimPitch = 0.0f;
		m_flAimYaw = 0.0f;
		m_flBodyHeight = 0.0f;
		m_vecMoveYaw.Init();
	}
};

struct MultiPlayerMovementData_t
{
	// Set speeds to -1 if they are not used.
	float		m_flWalkSpeed;
	float		m_flRunSpeed;
	float		m_flSprintSpeed;
	float		m_flBodyYawRate;
};

using Activity = DWORD;

typedef enum
{
	LEGANIM_9WAY,		// Legs use a 9-way blend, with "move_x" and "move_y" pose parameters.
	LEGANIM_8WAY,		// Legs use an 8-way blend with "move_yaw" pose param.
	LEGANIM_GOLDSRC	// Legs always point in the direction he's running and the torso rotates.
} LegAnimType_t;

class CBasePlayerAnimState;

enum
{
	PC_EVERYTHING = 0,
	PC_NON_NETWORKED_ONLY,
	PC_NETWORKED_ONLY,
};


// ---------------------------
//  Hit Group standards
// ---------------------------
#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4	
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7
#define HITGROUP_GEAR		10			// alerts NPC, but doesn't do damage or bleed (1/100th damage)



#define MAX_COORD_INTEGER			(16384)
#define COORD_EXTENT				(2*MAX_COORD_INTEGER)

static constexpr float MAX_TRACE_LENGTH = (1.732050807569 * COORD_EXTENT);



// NOTE: If you add a tex type, be sure to modify the s_pImpactEffect
// array in fx_impact.cpp to get an effect when that surface is shot.
#define CHAR_TEX_ANTLION		'A'
#define CHAR_TEX_BLOODYFLESH	'B'
#define	CHAR_TEX_CONCRETE		'C'
#define CHAR_TEX_DIRT			'D'
#define CHAR_TEX_EGGSHELL		'E' ///< the egg sacs in the tunnels in ep2.
#define CHAR_TEX_FLESH			'F'
#define CHAR_TEX_GRATE			'G'
#define CHAR_TEX_ALIENFLESH		'H'
#define CHAR_TEX_CLIP			'I'
//#define CHAR_TEX_UNUSED		'J'
//#define CHAR_TEX_UNUSED		'K'
#define CHAR_TEX_PLASTIC		'L'
#define CHAR_TEX_METAL			'M'
#define CHAR_TEX_SAND			'N'
#define CHAR_TEX_FOLIAGE		'O'
#define CHAR_TEX_COMPUTER		'P'
//#define CHAR_TEX_UNUSED		'Q'
//#define CHAR_TEX_UNUSED		'R'
#define CHAR_TEX_SLOSH			'S'
#define CHAR_TEX_TILE			'T'
//#define CHAR_TEX_UNUSED		'U'
#define CHAR_TEX_VENT			'V'
#define CHAR_TEX_WOOD			'W'
//#define CHAR_TEX_UNUSED		'X'
#define CHAR_TEX_GLASS			'Y'
#define CHAR_TEX_WARPSHIELD		'Z' ///< wierd-looking jello effect for advisor shield.


enum soundlevel_t
{
	SNDLVL_NONE = 0,

	SNDLVL_20dB = 20,			// rustling leaves
	SNDLVL_25dB = 25,			// whispering
	SNDLVL_30dB = 30,			// library
	SNDLVL_35dB = 35,
	SNDLVL_40dB = 40,
	SNDLVL_45dB = 45,			// refrigerator

	SNDLVL_50dB = 50,	// 3.9	// average home
	SNDLVL_55dB = 55,	// 3.0

	SNDLVL_IDLE = 60,	// 2.0	
	SNDLVL_60dB = 60,	// 2.0	// normal conversation, clothes dryer

	SNDLVL_65dB = 65,	// 1.5	// washing machine, dishwasher
	SNDLVL_STATIC = 66,	// 1.25

	SNDLVL_70dB = 70,	// 1.0	// car, vacuum cleaner, mixer, electric sewing machine

	SNDLVL_NORM = 75,
	SNDLVL_75dB = 75,	// 0.8	// busy traffic

	SNDLVL_80dB = 80,	// 0.7	// mini-bike, alarm clock, noisy restaurant, office tabulator, outboard motor, passing snowmobile
	SNDLVL_TALKING = 80,	// 0.7
	SNDLVL_85dB = 85,	// 0.6	// average factory, electric shaver
	SNDLVL_90dB = 90,	// 0.5	// screaming child, passing motorcycle, convertible ride on frw
	SNDLVL_95dB = 95,
	SNDLVL_100dB = 100,	// 0.4	// subway train, diesel truck, woodworking shop, pneumatic drill, boiler shop, jackhammer
	SNDLVL_105dB = 105,			// helicopter, power mower
	SNDLVL_110dB = 110,			// snowmobile drvrs seat, inboard motorboat, sandblasting
	SNDLVL_120dB = 120,			// auto horn, propeller aircraft
	SNDLVL_130dB = 130,			// air raid siren

	SNDLVL_GUNFIRE = 140,	// 0.27	// THRESHOLD OF PAIN, gunshot, jet engine
	SNDLVL_140dB = 140,	// 0.2

	SNDLVL_150dB = 150,	// 0.2

	SNDLVL_180dB = 180,			// rocket launching

								// NOTE: Valid soundlevel_t values are 0-255.
								//       256-511 are reserved for sounds using goldsrc compatibility attenuation.
};

#define	PITCH_NORM		100			// non-pitch shifted
#define PITCH_LOW		95			// other values are possible - 0-255, where 255 is very high
#define PITCH_HIGH		120
typedef void* FileNameHandle_t;
struct SndInfo_t
{
	// Sound Guid
	int			m_nGuid;
	FileNameHandle_t m_filenameHandle;		// filesystem filename handle - call IFilesystem to conver this to a string
	int			m_nSoundSource;
	int			m_nChannel;
	// If a sound is being played through a speaker entity (e.g., on a monitor,), this is the
	//  entity upon which to show the lips moving, if the sound has sentence data
	int			m_nSpeakerEntity;
	float		m_flVolume;
	float		m_flLastSpatializedVolume;
	// Radius of this sound effect (spatialization is different within the radius)
	float		m_flRadius;
	int			m_nPitch;
	Vector		*m_pOrigin;
	Vector		*m_pDirection;

	// if true, assume sound source can move and update according to entity
	bool		m_bUpdatePositions;
	// true if playing linked sentence
	bool		m_bIsSentence;
	// if true, bypass all dsp processing for this sound (ie: music)	
	bool		m_bDryMix;
	// true if sound is playing through in-game speaker entity.
	bool		m_bSpeaker;
	// for snd_show, networked sounds get colored differently than local sounds
	bool		m_bFromServer;
};


enum SoundFlags_t
{
	SND_NOFLAGS = 0,			// to keep the compiler happy
	SND_CHANGE_VOL = (1 << 0),		// change sound vol
	SND_CHANGE_PITCH = (1 << 1),		// change sound pitch
	SND_STOP = (1 << 2),		// stop the sound
	SND_SPAWNING = (1 << 3),		// we're spawning, used in some cases for ambients
									// not sent over net, only a param between dll and server.
									SND_DELAY = (1 << 4),		// sound has an initial delay
									SND_STOP_LOOPING = (1 << 5),		// stop all looping sounds on the entity.
									SND_SPEAKER = (1 << 6),		// being played again by a microphone through a speaker

									SND_SHOULDPAUSE = (1 << 7),		// this sound should be paused if the game is paused
									SND_IGNORE_PHONEMES = (1 << 8),
									SND_IGNORE_NAME = (1 << 9),		// used to change all sounds emitted by an entity, regardless of scriptname

									SND_DO_NOT_OVERWRITE_EXISTING_ON_CHANNEL = (1 << 10),
};
class IInterpolatedVar
{
public:
	virtual		 ~IInterpolatedVar() {}

	virtual void Setup(void *pValue, int type) = 0;
	virtual void SetInterpolationAmount(float seconds) = 0;

	// Returns true if the new value is different from the prior most recent value.
	virtual void NoteLastNetworkedValue() = 0;
	virtual bool NoteChanged(float changetime, bool bUpdateLastNetworkedValue) = 0;
	virtual void Reset() = 0;

	// Returns 1 if the value will always be the same if currentTime is always increasing.
	virtual int Interpolate(float currentTime) = 0;

	virtual int	 GetType() const = 0;
	virtual void RestoreToLastNetworked() = 0;
	virtual void Copy(IInterpolatedVar *pSrc) = 0;

	virtual const char *GetDebugName() = 0;
	virtual void SetDebugName(const char* pName) = 0;
};


class VarMapEntry_t
{
public:
	unsigned short type = 0u;
	unsigned short m_bNeedsToInterpolate = 0u;

	void* data = nullptr;
	IInterpolatedVar* watcher = nullptr;
};

struct VarMapping_t
{
	VarMapping_t()
	{
		m_nInterpolatedEntries = 0;
	}

	CUtlVector< VarMapEntry_t >	m_Entries;
	int m_nInterpolatedEntries = 0;
	float m_lastInterpolationTime = 0.f;
};
inline long ThreadInterlockedIncrement(long volatile *p) { return _InterlockedIncrement(p); }
inline long ThreadInterlockedDecrement(long volatile *p) { return _InterlockedDecrement(p); }
inline long ThreadInterlockedExchange(long volatile *p, long value) { return _InterlockedExchange(p, value); }
inline long ThreadInterlockedExchangeAdd(long volatile *p, long value) { return _InterlockedExchangeAdd(p, value); }
inline long ThreadInterlockedCompareExchange(long volatile *p, long value, long comperand) { return _InterlockedCompareExchange(p, value, comperand); }
inline bool ThreadInterlockedAssignIf(long volatile *p, long value, long comperand) {
	return (_InterlockedCompareExchange(p, value, comperand) == comperand);
};

inline unsigned ThreadInterlockedExchangeSubtract(unsigned volatile *p, unsigned value) { return ThreadInterlockedExchangeAdd((long volatile *)p, value); }
inline unsigned ThreadInterlockedIncrement(unsigned volatile *p) { return ThreadInterlockedIncrement((long volatile *)p); }
inline unsigned ThreadInterlockedDecrement(unsigned volatile *p) { return ThreadInterlockedDecrement((long volatile *)p); }
inline unsigned ThreadInterlockedExchange(unsigned volatile *p, unsigned value) { return ThreadInterlockedExchange((long volatile *)p, value); }
inline unsigned ThreadInterlockedExchangeAdd(unsigned volatile *p, unsigned value) { return ThreadInterlockedExchangeAdd((long volatile *)p, value); }
inline unsigned ThreadInterlockedCompareExchange(unsigned volatile *p, unsigned value, unsigned comperand) { return ThreadInterlockedCompareExchange((long volatile *)p, value, comperand); }
inline bool ThreadInterlockedAssignIf(unsigned volatile *p, unsigned value, unsigned comperand) { return ThreadInterlockedAssignIf((long volatile *)p, value, comperand); }

inline int ThreadInterlockedExchangeSubtract(int volatile *p, int value) { return ThreadInterlockedExchangeAdd((long volatile *)p, value); }
inline int ThreadInterlockedIncrement(int volatile *p) { return ThreadInterlockedIncrement((long volatile *)p); }
inline int ThreadInterlockedDecrement(int volatile *p) { return ThreadInterlockedDecrement((long volatile *)p); }
inline int ThreadInterlockedExchange(int volatile *p, int value) { return ThreadInterlockedExchange((long volatile *)p, value); }
inline int ThreadInterlockedExchangeAdd(int volatile *p, int value) { return ThreadInterlockedExchangeAdd((long volatile *)p, value); }
inline int ThreadInterlockedCompareExchange(int volatile *p, int value, int comperand) { return ThreadInterlockedCompareExchange((long volatile *)p, value, comperand); }
inline bool ThreadInterlockedAssignIf(int volatile *p, int value, int comperand) { return ThreadInterlockedAssignIf((long volatile *)p, value, comperand); }

const unsigned TT_INFINITE = 0xffffffff;
class CThreadFastMutex
{
public:
	CThreadFastMutex()
		: m_ownerID(0),
		m_depth(0)
	{
	}

private:
	FORCEINLINE bool TryLockInline(const uint32 threadId) volatile
	{
		if (threadId != m_ownerID && !ThreadInterlockedAssignIf((volatile long *)&m_ownerID, (long)threadId, 0))
			return false;

		++m_depth;
		return true;
	}

	bool TryLock(const uint32 threadId) volatile
	{
		return TryLockInline(threadId);
	}

	void Lock(const uint32 threadId, unsigned nSpinSleepTime) volatile
	{
		int i;
		if (nSpinSleepTime != TT_INFINITE)
		{
			for (i = 1000; i != 0; --i)
			{
				if (TryLock(threadId))
				{
					return;
				}
				_mm_pause();
			}

#ifdef _WIN32
			if (!nSpinSleepTime && GetThreadPriority(GetCurrentThread()) > THREAD_PRIORITY_NORMAL)
			{
				nSpinSleepTime = 1;
			}
			else
#endif

				if (nSpinSleepTime)
				{
					for (i = 4000; i != 0; --i)
					{
						if (TryLock(threadId))
						{
							return;
						}

						_mm_pause();
						Sleep(0);
					}

				}

			for (;; ) // coded as for instead of while to make easy to breakpoint success
			{
				if (TryLock(threadId))
				{
					return;
				}

				_mm_pause();
				Sleep(nSpinSleepTime);
			}
		}
		else
		{
			for (;; ) // coded as for instead of while to make easy to breakpoint success
			{
				if (TryLock(threadId))
				{
					return;
				}

				_mm_pause();
			}
		}

	};

public:
	bool TryLock() volatile
	{
#ifdef _DEBUG
		if (m_depth == INT_MAX)
			DebuggerBreak();

		if (m_depth < 0)
			DebuggerBreak();
#endif
		return TryLockInline(GetCurrentThreadId());
	}

#ifndef _DEBUG
	FORCEINLINE
#endif
		void Lock(unsigned nSpinSleepTime = 0) volatile
	{
		const uint32 threadId = GetCurrentThreadId();

		if (!TryLockInline(threadId))
		{
			_mm_pause();
			Lock(threadId, nSpinSleepTime);
		}
#ifdef _DEBUG
		if (m_ownerID != ThreadGetCurrentId())
			DebuggerBreak();

		if (m_depth == INT_MAX)
			DebuggerBreak();

		if (m_depth < 0)
			DebuggerBreak();
#endif
	}

#ifndef _DEBUG
	FORCEINLINE
#endif
		void Unlock() volatile
	{
#ifdef _DEBUG
		if (m_ownerID != ThreadGetCurrentId())
			DebuggerBreak();

		if (m_depth <= 0)
			DebuggerBreak();
#endif

		--m_depth;
		if (!m_depth)
			ThreadInterlockedExchange(&m_ownerID, 0);
	}

	bool TryLock() const volatile { return (const_cast<CThreadFastMutex *>(this))->TryLock(); }
	void Lock(unsigned nSpinSleepTime = 1) const volatile { (const_cast<CThreadFastMutex *>(this))->Lock(nSpinSleepTime); }
	void Unlock() const	volatile { (const_cast<CThreadFastMutex *>(this))->Unlock(); }

	// To match regular CThreadMutex:
	bool AssertOwnedByCurrentThread() { return true; }
	void SetTrace(bool) {}

	uint32 GetOwnerId() const { return m_ownerID; }
	int	GetDepth() const { return m_depth; }
private:
	volatile uint32	m_ownerID;
	int				m_depth;
};
template <int size>	struct CAutoLockTypeDeducer {};
template <> struct CAutoLockTypeDeducer<sizeof(CThreadFastMutex)> { typedef CThreadFastMutex Type_t; };


class  CThreadMutex
{
public:
	CThreadMutex();
	~CThreadMutex();

	//------------------------------------------------------
	// Mutex acquisition/release. Const intentionally defeated.
	//------------------------------------------------------
	void Lock();
	void Lock() const { (const_cast<CThreadMutex *>(this))->Lock(); }
	void Unlock();
	void Unlock() const { (const_cast<CThreadMutex *>(this))->Unlock(); }

	bool TryLock();
	bool TryLock() const { return (const_cast<CThreadMutex *>(this))->TryLock(); }

	//------------------------------------------------------
	// Use this to make deadlocks easier to track by asserting
	// when it is expected that the current thread owns the mutex
	//------------------------------------------------------
	bool AssertOwnedByCurrentThread();

	//------------------------------------------------------
	// Enable tracing to track deadlock problems
	//------------------------------------------------------
	void SetTrace(bool);

private:
	// Disallow copying
	CThreadMutex(const CThreadMutex &);
	CThreadMutex &operator=(const CThreadMutex &);

#if defined( _WIN32 )
	// Efficient solution to breaking the windows.h dependency, invariant is tested.
#ifdef _WIN64
#define TT_SIZEOF_CRITICALSECTION 40	
#else
#ifndef _X360
#define TT_SIZEOF_CRITICALSECTION 24
#else
#define TT_SIZEOF_CRITICALSECTION 28
#endif // !_XBOX
#endif // _WIN64
	byte m_CriticalSection[TT_SIZEOF_CRITICALSECTION];
#elif _LINUX
	pthread_mutex_t m_Mutex;
	pthread_mutexattr_t m_Attr;
#else
#error
#endif

#ifdef THREAD_MUTEX_TRACING_SUPPORTED
	// Debugging (always here to allow mixed debug/release builds w/o changing size)
	uint	m_currentOwnerID;
	uint16	m_lockCount;
	bool	m_bTrace;
#endif
};
template <class MUTEX_TYPE = CThreadMutex>
class CAutoLockT
{
public:
	FORCEINLINE CAutoLockT(MUTEX_TYPE &lock)
		: m_lock(lock)
	{
		m_lock.Lock();
	}

	FORCEINLINE CAutoLockT(const MUTEX_TYPE &lock)
		: m_lock(const_cast<MUTEX_TYPE &>(lock))
	{
		m_lock.Lock();
	}

	FORCEINLINE ~CAutoLockT()
	{
		m_lock.Unlock();
	}


private:
	MUTEX_TYPE &m_lock;

	// Disallow copying
	CAutoLockT<MUTEX_TYPE>(const CAutoLockT<MUTEX_TYPE> &);
	CAutoLockT<MUTEX_TYPE> &operator=(const CAutoLockT<MUTEX_TYPE> &);
};
#define AUTO_LOCK_( type, mutex ) \
	CAutoLockT< type > UNIQUE_ID( static_cast<const type &>( mutex ) )

#define AUTO_LOCK( mutex ) \
	AUTO_LOCK_( CAutoLockTypeDeducer<sizeof(mutex)>::Type_t, mutex )





class C_BaseAnimating;


class CBoneAccessor
{
public:

	CBoneAccessor();
	CBoneAccessor(matrix3x4_t *pBones); // This can be used to allow access to all bones.

	// Initialize.
#if defined( CLIENT_DLL )
	void Init(const C_BaseAnimating *pAnimating, matrix3x4_t *pBones);
#endif

	int GetReadableBones();
	void SetReadableBones(int flags);

	int GetWritableBones();
	void SetWritableBones(int flags);

	// Get bones for read or write access.
	const matrix3x4_t&	GetBone(int iBone) const;
	const matrix3x4_t&	operator[](int iBone) const;
	matrix3x4_t&		GetBoneForWrite(int iBone);

	matrix3x4_t			*GetBoneArrayForWrite() const;

private:

#if defined( CLIENT_DLL ) && defined( _DEBUG )
	void SanityCheckBone(int iBone, bool bReadable) const;
#endif

	// Only used in the client DLL for debug verification.
	const C_BaseAnimating *m_pAnimating;

	matrix3x4_t *m_pBones;

	int m_ReadableBones;		// Which bones can be read.
	int m_WritableBones;		// Which bones can be written.
};


inline CBoneAccessor::CBoneAccessor()
{
	m_pAnimating = NULL;
	m_pBones = NULL;
	m_ReadableBones = m_WritableBones = 0;
}

inline CBoneAccessor::CBoneAccessor(matrix3x4_t *pBones)
{
	m_pAnimating = NULL;
	m_pBones = pBones;
}

#if defined( CLIENT_DLL )
inline void CBoneAccessor::Init(const C_BaseAnimating *pAnimating, matrix3x4_t *pBones)
{
	m_pAnimating = pAnimating;
	m_pBones = pBones;
}
#endif

inline int CBoneAccessor::GetReadableBones()
{
	return m_ReadableBones;
}

inline void CBoneAccessor::SetReadableBones(int flags)
{
	m_ReadableBones = flags;
}

inline int CBoneAccessor::GetWritableBones()
{
	return m_WritableBones;
}

inline void CBoneAccessor::SetWritableBones(int flags)
{
	m_WritableBones = flags;
}

inline const matrix3x4_t& CBoneAccessor::GetBone(int iBone) const
{
#if defined( CLIENT_DLL ) && defined( _DEBUG )
	SanityCheckBone(iBone, true);
#endif
	return m_pBones[iBone];
}

inline const matrix3x4_t& CBoneAccessor::operator[](int iBone) const
{
#if defined( CLIENT_DLL ) && defined( _DEBUG )
	SanityCheckBone(iBone, true);
#endif
	return m_pBones[iBone];
}

inline matrix3x4_t& CBoneAccessor::GetBoneForWrite(int iBone)
{
#if defined( CLIENT_DLL ) && defined( _DEBUG )
	SanityCheckBone(iBone, false);
#endif
	return m_pBones[iBone];
}

inline matrix3x4_t *CBoneAccessor::GetBoneArrayForWrite(void) const
{
	return m_pBones;
}

enum MDLCacheDataType_t
{
	// Callbacks to get called when data is loaded or unloaded for these:
	MDLCACHE_STUDIOHDR = 0,
	MDLCACHE_STUDIOHWDATA,
	MDLCACHE_VCOLLIDE,

	// Callbacks NOT called when data is loaded or unloaded for these:
	MDLCACHE_ANIMBLOCK,
	MDLCACHE_VIRTUALMODEL,
	MDLCACHE_VERTEXES,
	MDLCACHE_DECODEDANIMBLOCK,
};
class IMDLCacheNotify
{
public:
	// Called right after the data is loaded
	virtual void OnDataLoaded(MDLCacheDataType_t type, MDLHandle_t handle) = 0;

	// Called right before the data is unloaded
	virtual void OnDataUnloaded(MDLCacheDataType_t type, MDLHandle_t handle) = 0;
};
struct virtualsequence_t
{
#ifdef _XBOX
	short flags;
	short activity;
	short group;
	short index;
#else
	int	flags;
	int activity;
	int group;
	int index;
#endif
};

struct virtualgeneric_t
{
#ifdef _XBOX
	short group;
	short index;
#else
	int group;
	int index;
#endif
};



class virtualgroup_t
{
public:
	virtualgroup_t(void) { cache = NULL; };
	// tool dependant.  In engine this is a model_t, in tool it's a direct pointer
	void *cache;
	// converts cache entry into a usable studiohdr_t *
	const studiohdr_t *GetStudioHdr(void) const;

	CUtlVector< int > boneMap;				// maps global bone to local bone
	CUtlVector< int > masterBone;			// maps local bone to global bone
	CUtlVector< int > masterSeq;			// maps local sequence to master sequence
	CUtlVector< int > masterAnim;			// maps local animation to master animation
	CUtlVector< int > masterAttachment;	// maps local attachment to global
	CUtlVector< int > masterPose;			// maps local pose parameter to global
	CUtlVector< int > masterNode;			// maps local transition nodes to global
};
struct virtualmodel_t
{
	void AppendSequences(int group, const studiohdr_t *pStudioHdr);
	void AppendAnimations(int group, const studiohdr_t *pStudioHdr);
	void AppendAttachments(int ground, const studiohdr_t *pStudioHdr);
	void AppendPoseParameters(int group, const studiohdr_t *pStudioHdr);
	void AppendBonemap(int group, const studiohdr_t *pStudioHdr);
	void AppendNodes(int group, const studiohdr_t *pStudioHdr);
	void AppendTransitions(int group, const studiohdr_t *pStudioHdr);
	void AppendIKLocks(int group, const studiohdr_t *pStudioHdr);
	void AppendModels(int group, const studiohdr_t *pStudioHdr);
	void UpdateAutoplaySequences(const studiohdr_t *pStudioHdr);

	virtualgroup_t *pAnimGroup(int animation) { return &m_group[m_anim[animation].group]; } // Note: user must manage mutex for this
	virtualgroup_t *pSeqGroup(int sequence)
	{
		// Check for out of range access that is causing crashes on some servers.
		// Perhaps caused by sourcemod bugs. Typical sequence in these cases is ~292
		// when the count is 234. Using unsigned math allows for free range
		// checking against zero.
		if ((unsigned)sequence >= (unsigned)m_seq.Count())
		{
			return 0;
		}
		return &m_group[m_seq[sequence].group];
	} // Note: user must manage mutex for this

	CThreadFastMutex m_Lock;

	CUtlVector< virtualsequence_t > m_seq;
	CUtlVector< virtualgeneric_t > m_anim;
	CUtlVector< virtualgeneric_t > m_attachment;
	CUtlVector< virtualgeneric_t > m_pose;
	CUtlVector< virtualgroup_t > m_group;
	CUtlVector< virtualgeneric_t > m_node;
	CUtlVector< virtualgeneric_t > m_iklock;
	CUtlVector< unsigned short > m_autoplaySequences;
};

template <typename T>
inline void DataMapAccess(T *ignored, datamap_t **p)
{
	*p = &T::m_DataMap;
}

enum MDLCacheFlush_t
{
	MDLCACHE_FLUSH_STUDIOHDR = 0x01,
	MDLCACHE_FLUSH_STUDIOHWDATA = 0x02,
	MDLCACHE_FLUSH_VCOLLIDE = 0x04,
	MDLCACHE_FLUSH_ANIMBLOCK = 0x08,
	MDLCACHE_FLUSH_VIRTUALMODEL = 0x10,
	MDLCACHE_FLUSH_AUTOPLAY = 0x20,
	MDLCACHE_FLUSH_VERTEXES = 0x40,

	MDLCACHE_FLUSH_IGNORELOCK = 0x80000000,
	MDLCACHE_FLUSH_ALL = 0xFFFFFFFF
};


class IMDLCache : public IAppSystem
{
public:
	// Used to install callbacks for when data is loaded + unloaded
	// Returns the prior notify
	virtual void SetCacheNotify(IMDLCacheNotify *pNotify) = 0;

	// NOTE: This assumes the "GAME" path if you don't use
	// the UNC method of specifying files. This will also increment
	// the reference count of the MDL
	virtual MDLHandle_t FindMDL(const char *pMDLRelativePath) = 0;

	// Reference counting
	virtual int AddRef(MDLHandle_t handle) = 0;
	virtual int Release(MDLHandle_t handle) = 0;
	virtual int GetRef(MDLHandle_t handle) = 0;

	// Gets at the various data associated with a MDL
	virtual studiohdr_t *GetStudioHdr(MDLHandle_t handle) = 0;
	virtual studiohwdata_t *GetHardwareData(MDLHandle_t handle) = 0;
	virtual vcollide_t *GetVCollide(MDLHandle_t handle) = 0;
	virtual unsigned char *GetAnimBlock(MDLHandle_t handle, int nBlock) = 0;
	virtual virtualmodel_t *GetVirtualModel(MDLHandle_t handle) = 0;
	virtual int GetAutoplayList(MDLHandle_t handle, unsigned short **pOut) = 0;
	virtual void *GetVertexData(MDLHandle_t handle) = 0;

	// Brings all data associated with an MDL into memory
	virtual void TouchAllData(MDLHandle_t handle) = 0;

	// Gets/sets user data associated with the MDL
	virtual void SetUserData(MDLHandle_t handle, void* pData) = 0;
	virtual void *GetUserData(MDLHandle_t handle) = 0;

	// Is this MDL using the error model?
	virtual bool IsErrorModel(MDLHandle_t handle) = 0;

	// Flushes the cache, force a full discard
	virtual void Flush(MDLCacheFlush_t nFlushFlags = MDLCACHE_FLUSH_ALL) = 0;

	// Flushes a particular model out of memory
	virtual void Flush(MDLHandle_t handle, int nFlushFlags = MDLCACHE_FLUSH_ALL) = 0;

	// Returns the name of the model (its relative path)
	virtual const char *GetModelName(MDLHandle_t handle) = 0;

	// faster access when you already have the studiohdr
	virtual virtualmodel_t *GetVirtualModelFast(const studiohdr_t *pStudioHdr, MDLHandle_t handle) = 0;

	// all cache entries that subsequently allocated or successfully checked 
	// are considered "locked" and will not be freed when additional memory is needed
	virtual void BeginLock() = 0;

	// reset all protected blocks to normal
	virtual void EndLock() = 0;

	// returns a pointer to a counter that is incremented every time the cache has been out of the locked state (EVIL)
	virtual int *GetFrameUnlockCounterPtrOLD() = 0;

	// Finish all pending async operations
	virtual void FinishPendingLoads() = 0;

	virtual vcollide_t *GetVCollideEx(MDLHandle_t handle, bool synchronousLoad = true) = 0;
	virtual bool GetVCollideSize(MDLHandle_t handle, int *pVCollideSize) = 0;

	virtual bool GetAsyncLoad(MDLCacheDataType_t type) = 0;
	virtual bool SetAsyncLoad(MDLCacheDataType_t type, bool bAsync) = 0;

	virtual void BeginMapLoad() = 0;
	virtual void EndMapLoad() = 0;
	virtual void MarkAsLoaded(MDLHandle_t handle) = 0;

	virtual void InitPreloadData(bool rebuild) = 0;
	virtual void ShutdownPreloadData() = 0;

	virtual bool IsDataLoaded(MDLHandle_t handle, MDLCacheDataType_t type) = 0;

	virtual int *GetFrameUnlockCounterPtr(MDLCacheDataType_t type) = 0;

	virtual studiohdr_t *LockStudioHdr(MDLHandle_t handle) = 0;
	virtual void UnlockStudioHdr(MDLHandle_t handle) = 0;

	virtual bool PreloadModel(MDLHandle_t handle) = 0;

	// Hammer uses this. If a model has an error loading in GetStudioHdr, then it is flagged
	// as an error model and any further attempts to load it will just get the error model.
	// That is, until you call this function. Then it will load the correct model.
	virtual void ResetErrorModelStatus(MDLHandle_t handle) = 0;

	virtual void MarkFrame() = 0;
};
#define STUDIO_ENABLE_PERF_COUNTERS 1

#define UTL_INVAL_SYMBOL  ((UtlSymId_t)~0)

class CUtlSymbolTable;
class CUtlSymbolTableMT;
class CUtlSymbol
{
public:
	// constructor, destructor
	CUtlSymbol() : m_Id(UTL_INVAL_SYMBOL) {}
	CUtlSymbol(UtlSymId_t id) : m_Id(id) {}
	CUtlSymbol(const char* pStr);
	CUtlSymbol(CUtlSymbol const& sym) : m_Id(sym.m_Id) {}

	// operator=
	CUtlSymbol& operator=(CUtlSymbol const& src) { m_Id = src.m_Id; return *this; }

	// operator==
	bool operator==(CUtlSymbol const& src) const { return m_Id == src.m_Id; }
	bool operator==(const char* pStr) const;

	// Is valid?
	bool IsValid() const { return m_Id != UTL_INVAL_SYMBOL; }

	// Gets at the symbol
	operator UtlSymId_t const() const { return m_Id; }

	// Gets the string associated with the symbol
	const char* String() const;

	// Modules can choose to disable the static symbol table so to prevent accidental use of them.
	static void DisableStaticSymbolTable();

protected:
	UtlSymId_t   m_Id;

	// Initializes the symbol table
	static void Initialize();

	// returns the current symbol table
	static CUtlSymbolTableMT* CurrTable();

	// The standard global symbol table
	static CUtlSymbolTableMT* s_pSymbolTable;

	static bool s_bAllowStaticSymbolTable;

	friend class CCleanupUtlSymbolTable;
};
typedef unsigned int UtlHashHandle_t;

template<class Data, typename C = bool(*)(Data const&, Data const&), typename K = unsigned int(*)(Data const&) >
class CUtlHash
{
public:
	// compare and key functions - implemented by the 
	typedef C CompareFunc_t;
	typedef K KeyFunc_t;

	// constructor/deconstructor
	CUtlHash(int bucketCount = 0, int growCount = 0, int initCount = 0,
		CompareFunc_t compareFunc = 0, KeyFunc_t keyFunc = 0);
	~CUtlHash();

	// invalid handle
	static UtlHashHandle_t InvalidHandle(void) { return (UtlHashHandle_t)~0; }
	bool IsValidHandle(UtlHashHandle_t handle) const;

	// size
	int Count(void) const;

	// memory
	void Purge(void);

	// insertion methods
	UtlHashHandle_t Insert(Data const &src);
	UtlHashHandle_t Insert(Data const &src, bool *pDidInsert);
	UtlHashHandle_t AllocEntryFromKey(Data const &src);

	// removal methods
	void Remove(UtlHashHandle_t handle);
	void RemoveAll();

	// retrieval methods
	UtlHashHandle_t Find(Data const &src) const;

	Data &Element(UtlHashHandle_t handle);
	Data const &Element(UtlHashHandle_t handle) const;
	Data &operator[](UtlHashHandle_t handle);
	Data const &operator[](UtlHashHandle_t handle) const;

	UtlHashHandle_t GetFirstHandle() const;
	UtlHashHandle_t GetNextHandle(UtlHashHandle_t h) const;

	// debugging!!
	void Log(const char *filename);

protected:

	int GetBucketIndex(UtlHashHandle_t handle) const;
	int GetKeyDataIndex(UtlHashHandle_t handle) const;
	UtlHashHandle_t BuildHandle(int ndxBucket, int ndxKeyData) const;

	bool DoFind(Data const &src, unsigned int *pBucket, int *pIndex) const;

protected:

	// handle upper 16 bits = bucket index (bucket heads)
	// handle lower 16 bits = key index (bucket list)
	typedef CUtlVector<Data> HashBucketList_t;
	CUtlVector<HashBucketList_t>	m_Buckets;

	CompareFunc_t					m_CompareFunc;			// function used to handle unique compares on data
	KeyFunc_t						m_KeyFunc;				// function used to generate the key value

	bool							m_bPowerOfTwo;			// if the bucket value is a power of two, 
	unsigned int					m_ModMask;				// use the mod mask to "mod"	
};
unsigned __fastcall HashInt(const int key);




class CStudioHdr
{
public:
	CStudioHdr(void);
	CStudioHdr(const studiohdr_t *pStudioHdr, IMDLCache *mdlcache = NULL);
	~CStudioHdr() { Term(); }

	void Init(const studiohdr_t *pStudioHdr, IMDLCache *mdlcache = NULL);
	void Term();

public:
	inline bool IsVirtual(void) { return (m_pVModel != NULL); };
	inline bool IsValid(void) { return (m_pStudioHdr != NULL); };
	inline bool IsReadyForAccess(void) const { return (m_pStudioHdr != NULL); };
	inline virtualmodel_t		*GetVirtualModel(void) const { return m_pVModel; };
	inline const studiohdr_t	*GetRenderHdr(void) const { return m_pStudioHdr; };
	const studiohdr_t *pSeqStudioHdr(int sequence);
	const studiohdr_t *pAnimStudioHdr(int animation);

private:
	mutable const studiohdr_t		*m_pStudioHdr;
	mutable virtualmodel_t	*m_pVModel;

	const virtualmodel_t * ResetVModel(const virtualmodel_t *pVModel) const;
	const studiohdr_t *GroupStudioHdr(int group);
	mutable CUtlVector< const studiohdr_t * > m_pStudioHdrCache;

	mutable int			m_nFrameUnlockCounter;
	int	*				m_pFrameUnlockCounter;
	CThreadFastMutex	m_FrameUnlockCounterMutex;

public:
	inline int			numbones(void) const { return m_pStudioHdr->numbones; };
	inline mstudiobone_t *pBone(int i) const { return m_pStudioHdr->pBone(i); };
	int					RemapAnimBone(int iAnim, int iLocalBone) const;		// maps local animations bone to global bone
	int					RemapSeqBone(int iSequence, int iLocalBone) const;	// maps local sequence bone to global bone

	bool				SequencesAvailable() const;
	int					GetNumSeq(void) const;
	mstudioanimdesc_t	&pAnimdesc(int i);
	mstudioseqdesc_t	&pSeqdesc(int iSequence);
	int					iRelativeAnim(int baseseq, int relanim) const;	// maps seq local anim reference to global anim index
	int					iRelativeSeq(int baseseq, int relseq) const;		// maps seq local seq reference to global seq index

	int					GetSequenceActivity(int iSequence);
	void				SetSequenceActivity(int iSequence, int iActivity);
	int					GetActivityListVersion(void);
	void				SetActivityListVersion(int version);
	int					GetEventListVersion(void);
	void				SetEventListVersion(int version);

	int					GetNumAttachments(void) const;
	const mstudioattachment_t &pAttachment(int i);
	int					GetAttachmentBone(int i);
	// used on my tools in hlmv, not persistant
	void				SetAttachmentBone(int iAttachment, int iBone);

	int					EntryNode(int iSequence);
	int					ExitNode(int iSequence);
	char				*pszNodeName(int iNode);
	// FIXME: where should this one be?
	int					GetTransition(int iFrom, int iTo) const;

	int					GetNumPoseParameters(void) const;
	 mstudioposeparamdesc_t &pPoseParameter(int i);
	int					GetSharedPoseParameter(int iSequence, int iLocalPose) const;

	int					GetNumIKAutoplayLocks(void) const;
	const mstudioiklock_t &pIKAutoplayLock(int i);

	inline int			CountAutoplaySequences() const { return m_pStudioHdr->CountAutoplaySequences(); };
	inline int			CopyAutoplaySequences(unsigned short *pOut, int outCount) const { return m_pStudioHdr->CopyAutoplaySequences(pOut, outCount); };
	inline int			GetAutoplayList(unsigned short **pOut) const { return m_pStudioHdr->GetAutoplayList(pOut); };

	inline int			GetNumBoneControllers(void) const { return m_pStudioHdr->numbonecontrollers; };
	inline mstudiobonecontroller_t *pBonecontroller(int i) const { return m_pStudioHdr->pBonecontroller(i); };

	inline int			numikchains() const { return m_pStudioHdr->numikchains; };
	inline int			GetNumIKChains(void) const { return m_pStudioHdr->numikchains; };
	inline mstudioikchain_t	*pIKChain(int i) const { return m_pStudioHdr->pIKChain(i); };

	inline int			numflexrules() const { return m_pStudioHdr->numflexrules; };
	inline mstudioflexrule_t *pFlexRule(int i) const { return m_pStudioHdr->pFlexRule(i); };

	inline int			numflexdesc() const { return m_pStudioHdr->numflexdesc; };
	inline mstudioflexdesc_t *pFlexdesc(int i) const { return m_pStudioHdr->pFlexdesc(i); };

	inline LocalFlexController_t			numflexcontrollers() const { return (LocalFlexController_t)m_pStudioHdr->numflexcontrollers; };
	inline mstudioflexcontroller_t *pFlexcontroller(LocalFlexController_t i) const { return m_pStudioHdr->pFlexcontroller(i); };

	inline int			numflexcontrollerui() const { return m_pStudioHdr->numflexcontrollerui; };
	inline mstudioflexcontrollerui_t *pFlexcontrollerUI(int i) const { return m_pStudioHdr->pFlexControllerUI(i); };

	//inline const char	*name() const { return m_pStudioHdr->name; }; // deprecated -- remove after full xbox merge
	inline const char	*pszName() const { return m_pStudioHdr->pszName(); };

	inline int			numbonecontrollers() const { return m_pStudioHdr->numbonecontrollers; };

	inline int			numhitboxsets() const { return m_pStudioHdr->numhitboxsets; };
	inline mstudiohitboxset_t	*pHitboxSet(int i) const { return m_pStudioHdr->pHitboxSet(i); };

	inline mstudiobbox_t *pHitbox(int i, int set) const { return m_pStudioHdr->pHitbox(i, set); };
	inline int			iHitboxCount(int set) const { return m_pStudioHdr->iHitboxCount(set); };

	inline int			numbodyparts() const { return m_pStudioHdr->numbodyparts; };
	inline mstudiobodyparts_t	*pBodypart(int i) const { return m_pStudioHdr->pBodypart(i); };

	inline int			numskinfamilies() const { return m_pStudioHdr->numskinfamilies; }

	inline Vector		eyeposition() const { return m_pStudioHdr->eyeposition; };

	inline int			flags() const { return m_pStudioHdr->flags; };

	inline char			*const pszSurfaceProp(void) const { return m_pStudioHdr->pszSurfaceProp(); };

	inline float		mass() const { return m_pStudioHdr->mass; };
	inline int			contents() const { return m_pStudioHdr->contents; }

	inline const byte	*GetBoneTableSortedByName() const { return m_pStudioHdr->GetBoneTableSortedByName(); };

	inline Vector		illumposition() const { return m_pStudioHdr->illumposition; };

	inline Vector		hull_min() const { return m_pStudioHdr->hull_min; };		// ideal movement hull size
	inline Vector		hull_max() const { return m_pStudioHdr->hull_max; };

	inline Vector		view_bbmin() const { return m_pStudioHdr->view_bbmin; };		// clipping bounding box
	inline Vector		view_bbmax() const { return m_pStudioHdr->view_bbmax; };

	inline int			numtextures() const { return m_pStudioHdr->numtextures; };

	inline int			IllumPositionAttachmentIndex() const { return m_pStudioHdr->IllumPositionAttachmentIndex(); }

	inline float		MaxEyeDeflection() const { return m_pStudioHdr->MaxEyeDeflection(); }

	inline mstudiolinearbone_t *pLinearBones() const { return m_pStudioHdr->pLinearBones(); }

	inline int			BoneFlexDriverCount() const { return m_pStudioHdr->BoneFlexDriverCount(); }
	inline const mstudioboneflexdriver_t *BoneFlexDriver(int i) const { return m_pStudioHdr->BoneFlexDriver(i); }

	inline float		VertAnimFixedPointScale() const { return m_pStudioHdr->VertAnimFixedPointScale(); }

public:
	int IsSequenceLooping(int iSequence);
	float GetSequenceCycleRate(int iSequence);

	void				RunFlexRules(const float *src, float *dest);


public:
	inline int boneFlags(int iBone) const { return m_boneFlags[iBone]; }
	inline int boneParent(int iBone) const { return m_boneParent[iBone]; }

private:
	CUtlVector< int >  m_boneFlags;
	CUtlVector< int >  m_boneParent;

public:

	// This class maps an activity to sequences allowed for that activity, accelerating the resolution
	// of SelectWeightedSequence(), especially on PowerPC. Iterating through every sequence
	// attached to a model turned out to be a very destructive cache access pattern on 360.
	// 
	// I've encapsulated this behavior inside a nested class for organizational reasons; there is
	// no particular programmatic or efficiency benefit to it. It just makes clearer what particular
	// code in the otherwise very complicated StudioHdr class has to do with this particular
	// optimization, and it lets you collapse the whole definition down to a single line in Visual
	// Studio.
	class CActivityToSequenceMapping /* final */
	{
	public:
		// A tuple of a sequence and its corresponding weight. Lists of these correspond to activities.
		struct SequenceTuple
		{
			short		seqnum;
			short		weight; // the absolute value of the weight from the sequence header
			CUtlSymbol	*pActivityModifiers;		// list of activity modifier symbols
			int			iNumActivityModifiers;
		};

		// The type of the hash's stored data, a composite of both key and value
		// (because that's how CUtlHash works):
		// key: an int, the activity #
		// values: an index into the m_pSequenceTuples array, a count of the
		// total sequences present for an activity, and the sum of their
		// weights.
		// Note this struct is 128-bits wide, exactly coincident to a PowerPC 
		// cache line and VMX register. Please consider very carefully the
		// performance implications before adding any additional fields to this.
		// You could probably do away with totalWeight if you really had to.
		struct HashValueType
		{
			// KEY (hashed)
			int activityIdx;

			// VALUE (not hashed)
			int startingIdx;
			int count;
			int totalWeight;

			HashValueType(int _actIdx, int _stIdx, int _ct, int _tW) :
				activityIdx(_actIdx), startingIdx(_stIdx), count(_ct), totalWeight(_tW) {}

			// default constructor (ought not to be actually used)
			HashValueType() : activityIdx(-1), startingIdx(-1), count(-1), totalWeight(-1)
			{
			}


			class HashFuncs
			{
			public:
				// dummy constructor (gndn)
				HashFuncs(int) {}

				// COMPARE
				// compare two entries for uniqueness. We should never have two different
				// entries for the same activity, so we only compare the activity index;
				// this allows us to use the utlhash as a dict by constructing dummy entries
				// as hash lookup keys.
				bool operator()(const HashValueType &lhs, const HashValueType &rhs) const
				{
					return lhs.activityIdx == rhs.activityIdx;
				}

				// HASH
				// We only hash on the activity index; everything else is data.
				unsigned int operator()(const HashValueType &item) const
				{
					return HashInt(item.activityIdx);
				}
			};
		};

		typedef CUtlHash<HashValueType, HashValueType::HashFuncs, HashValueType::HashFuncs> ActivityToValueIdxHash;

		// These must be here because IFM does not compile/link studio.cpp (?!?)

		// ctor
		CActivityToSequenceMapping(void)
			: m_pSequenceTuples(NULL), m_iSequenceTuplesCount(0), m_ActToSeqHash(8, 0, 0), m_expectedPStudioHdr(NULL), m_expectedVModel(NULL)
#if STUDIO_SEQUENCE_ACTIVITY_LAZY_INITIALIZE
			, m_bIsInitialized(false)
#endif
		{};

		// dtor -- not virtual because this class has no inheritors
		~CActivityToSequenceMapping()
		{
			if (m_pSequenceTuples != NULL)
			{
				if (m_pSequenceTuples->pActivityModifiers != NULL)
				{
					delete[] m_pSequenceTuples->pActivityModifiers;
				}
				delete[] m_pSequenceTuples;
			}
		}

		/// Get the list of sequences for an activity. Returns the pointer to the
		/// first sequence tuple. Output parameters are a count of sequences present,
		/// and the total weight of all the sequences. (it would be more LHS-friendly
		/// to return these on registers, if only C++ offered more than one return 
		/// value....)
		const SequenceTuple *GetSequences(int forActivity, int *outSequenceCount, int *outTotalWeight);

		/// The number of sequences available for an activity.
		int NumSequencesForActivity(int forActivity);

#if STUDIO_SEQUENCE_ACTIVITY_LAZY_INITIALIZE
		inline bool IsInitialized(void) { return m_bIsInitialized; }
#endif

	private:

		/// Allocate my internal array. (It is freed in the destructor.) Also,
		/// build the hash of activities to sequences and populate m_pSequenceTuples.
		void Initialize(CStudioHdr *pstudiohdr);

		/// Force Initialize() to occur again, even if it has already occured.
		void Reinitialize(CStudioHdr *pstudiohdr);

		/// A more efficient version of the old SelectWeightedSequence() function in animation.cpp. 
		int SelectWeightedSequence(CStudioHdr *pstudiohdr, int activity, int curSequence);

		// selects the sequence with the most matching modifiers
		int SelectWeightedSequenceFromModifiers(CStudioHdr *pstudiohdr, int activity, CUtlSymbol *pActivityModifiers, int iModifierCount);

		// Actually a big array, into which the hash values index.
		SequenceTuple *m_pSequenceTuples;
		unsigned int m_iSequenceTuplesCount; // (size of the whole array)
#if STUDIO_SEQUENCE_ACTIVITY_LAZY_INITIALIZE
		bool m_bIsInitialized;
#endif

		// we don't store an outer pointer because we can't initialize it at construction time
		// (warning c4355) -- there are ways around this but it's easier to just pass in a 
		// pointer to the CStudioHdr when we need it, since this class isn't supposed to 
		// export its interface outside the studio header anyway.
		// CStudioHdr * const m_pOuter;

		ActivityToValueIdxHash m_ActToSeqHash;

		// we store these so we can know if the contents of the studiohdr have changed
		// from underneath our feet (this is an emergency data integrity check)
		const void *m_expectedPStudioHdr;
		const void *m_expectedVModel;

		// double-check that the data I point to hasn't changed
		bool ValidateAgainst(const CStudioHdr * RESTRICT pstudiohdr);
		void SetValidationPair(const CStudioHdr *RESTRICT pstudiohdr);

		friend class CStudioHdr;
	};

	CActivityToSequenceMapping m_ActivityToSequence;

	/// A more efficient version of the old SelectWeightedSequence() function in animation.cpp. 
	/// Returns -1 on failure to find a sequence
	inline int SelectWeightedSequence(int activity, int curSequence)
	{
#if STUDIO_SEQUENCE_ACTIVITY_LAZY_INITIALIZE
		// We lazy-initialize the header on demand here, because CStudioHdr::Init() is
		// called from the constructor, at which time the this pointer is illegitimate.
		if (!m_ActivityToSequence.IsInitialized())
		{
			m_ActivityToSequence.Initialize(this);
		}
#endif
		return m_ActivityToSequence.SelectWeightedSequence(this, activity, curSequence);
	}

	inline int SelectWeightedSequenceFromModifiers(int activity, CUtlSymbol *pActivityModifiers, int iModifierCount)
	{
#if STUDIO_SEQUENCE_ACTIVITY_LAZY_INITIALIZE
		// We lazy-initialize the header on demand here, because CStudioHdr::Init() is
		// called from the constructor, at which time the this pointer is illegitimate.
		if (!m_ActivityToSequence.IsInitialized())
		{
			m_ActivityToSequence.Initialize(this);
		}
#endif
		return m_ActivityToSequence.SelectWeightedSequenceFromModifiers(this, activity, pActivityModifiers, iModifierCount);
	}

	/// True iff there is at least one sequence for the given activity.
	inline bool HaveSequenceForActivity(int activity)
	{
#if STUDIO_SEQUENCE_ACTIVITY_LAZY_INITIALIZE
		if (!m_ActivityToSequence.IsInitialized())
		{
			m_ActivityToSequence.Initialize(this);
		}
#endif
		return (m_ActivityToSequence.NumSequencesForActivity(activity) > 0);
	}

	// Force this CStudioHdr's activity-to-sequence mapping to be reinitialized
	inline void ReinitializeSequenceMapping(void)
	{
		m_ActivityToSequence.Reinitialize(this);
	}

#ifdef STUDIO_ENABLE_PERF_COUNTERS
public:
	inline void			ClearPerfCounters(void)
	{
		m_nPerfAnimatedBones = 0;
		m_nPerfUsedBones = 0;
		m_nPerfAnimationLayers = 0;
	};

	// timing info
	mutable	int			m_nPerfAnimatedBones;
	mutable	int			m_nPerfUsedBones;
	mutable	int			m_nPerfAnimationLayers;
#endif


};


// entity flags, CBaseEntity::m_iEFlags
enum
{
	EFL_KILLME = (1 << 0),	// This entity is marked for death -- This allows the game to actually delete ents at a safe time
	EFL_DORMANT = (1 << 1),	// Entity is dormant, no updates to client
	EFL_NOCLIP_ACTIVE = (1 << 2),	// Lets us know when the noclip command is active.
	EFL_SETTING_UP_BONES = (1 << 3),	// Set while a model is setting up its bones.
	EFL_KEEP_ON_RECREATE_ENTITIES = (1 << 4), // This is a special entity that should not be deleted when we restart entities only

	EFL_HAS_PLAYER_CHILD = (1 << 4),	// One of the child entities is a player.

	EFL_DIRTY_SHADOWUPDATE = (1 << 5),	// Client only- need shadow manager to update the shadow...
	EFL_NOTIFY = (1 << 6),	// Another entity is watching events on this entity (used by teleport)

	// The default behavior in ShouldTransmit is to not send an entity if it doesn't
	// have a model. Certain entities want to be sent anyway because all the drawing logic
	// is in the client DLL. They can set this flag and the engine will transmit them even
	// if they don't have a model.
	EFL_FORCE_CHECK_TRANSMIT = (1 << 7),

	EFL_BOT_FROZEN = (1 << 8),	// This is set on bots that are frozen.
	EFL_SERVER_ONLY = (1 << 9),	// Non-networked entity.
	EFL_NO_AUTO_EDICT_ATTACH = (1 << 10), // Don't attach the edict; we're doing it explicitly

	// Some dirty bits with respect to abs computations
	EFL_DIRTY_ABSTRANSFORM = (1 << 11),
	EFL_DIRTY_ABSVELOCITY = (1 << 12),
	EFL_DIRTY_ABSANGVELOCITY = (1 << 13),
	EFL_DIRTY_SURROUNDING_COLLISION_BOUNDS = (1 << 14),
	EFL_DIRTY_SPATIAL_PARTITION = (1 << 15),
	//	UNUSED						= (1<<16),

	EFL_IN_SKYBOX = (1 << 17),	// This is set if the entity detects that it's in the skybox.
											// This forces it to pass the "in PVS" for transmission.
											EFL_USE_PARTITION_WHEN_NOT_SOLID = (1 << 18),	// Entities with this flag set show up in the partition even when not solid
											EFL_TOUCHING_FLUID = (1 << 19),	// Used to determine if an entity is floating

											// FIXME: Not really sure where I should add this...
											EFL_IS_BEING_LIFTED_BY_BARNACLE = (1 << 20),
											EFL_NO_ROTORWASH_PUSH = (1 << 21),		// I shouldn't be pushed by the rotorwash
											EFL_NO_THINK_FUNCTION = (1 << 22),
											EFL_NO_GAME_PHYSICS_SIMULATION = (1 << 23),

											EFL_CHECK_UNTOUCH = (1 << 24),
											EFL_DONTBLOCKLOS = (1 << 25),		// I shouldn't block NPC line-of-sight
											EFL_DONTWALKON = (1 << 26),		// NPC;s should not walk on this entity
											EFL_NO_DISSOLVE = (1 << 27),		// These guys shouldn't dissolve
											EFL_NO_MEGAPHYSCANNON_RAGDOLL = (1 << 28),	// Mega physcannon can't ragdoll these guys.
											EFL_NO_WATER_VELOCITY_CHANGE = (1 << 29),	// Don't adjust this entity's velocity when transitioning into water
											EFL_NO_PHYSCANNON_INTERACTION = (1 << 30),	// Physcannon can't pick these up or punt them
											EFL_NO_DAMAGE_FORCES = (1 << 31),	// Doesn't accept forces from physics damage
};

#define ENTCLIENTFLAG_GETTINGSHADOWRENDERBOUNDS	0x0001		// Tells us if we're getting the real ent render bounds or the shadow render bounds.
#define ENTCLIENTFLAG_DONTUSEIK					0x0002		// Don't use IK on this entity even if its model has IK.
#define ENTCLIENTFLAG_ALWAYS_INTERPOLATE		0x0004		// Used by view models.




#define STUDIO_CONST	1	// get float
#define STUDIO_FETCH1	2	// get Flexcontroller value
#define STUDIO_FETCH2	3	// get flex weight
#define STUDIO_ADD		4
#define STUDIO_SUB		5
#define STUDIO_MUL		6
#define STUDIO_DIV		7
#define STUDIO_NEG		8	// not implemented
#define STUDIO_EXP		9	// not implemented
#define STUDIO_OPEN		10	// only used in token parsing
#define STUDIO_CLOSE	11
#define STUDIO_COMMA	12	// only used in token parsing
#define STUDIO_MAX		13
#define STUDIO_MIN		14
#define STUDIO_2WAY_0	15	// Fetch a value from a 2 Way slider for the 1st value RemapVal( 0.0, 0.5, 0.0, 1.0 )
#define STUDIO_2WAY_1	16	// Fetch a value from a 2 Way slider for the 2nd value RemapVal( 0.5, 1.0, 0.0, 1.0 )
#define STUDIO_NWAY		17	// Fetch a value from a 2 Way slider for the 2nd value RemapVal( 0.5, 1.0, 0.0, 1.0 )
#define STUDIO_COMBO	18	// Perform a combo operation (essentially multiply the last N values on the stack)
#define STUDIO_DOMINATE	19	// Performs a combination domination operation
#define STUDIO_DME_LOWER_EYELID 20	// 
#define STUDIO_DME_UPPER_EYELID 21	// 

// motion flags
#define STUDIO_X		0x00000001
#define STUDIO_Y		0x00000002	
#define STUDIO_Z		0x00000004
#define STUDIO_XR		0x00000008
#define STUDIO_YR		0x00000010
#define STUDIO_ZR		0x00000020

#define STUDIO_LX		0x00000040
#define STUDIO_LY		0x00000080
#define STUDIO_LZ		0x00000100
#define STUDIO_LXR		0x00000200
#define STUDIO_LYR		0x00000400
#define STUDIO_LZR		0x00000800

#define STUDIO_LINEAR	0x00001000

#define STUDIO_TYPES	0x0003FFFF
#define STUDIO_RLOOP	0x00040000	// controller that wraps shortest distance

// sequence and autolayer flags
#define STUDIO_LOOPING	0x0001		// ending frame should be the same as the starting frame
#define STUDIO_SNAP		0x0002		// do not interpolate between previous animation and this one
#define STUDIO_DELTA	0x0004		// this sequence "adds" to the base sequences, not slerp blends
#define STUDIO_AUTOPLAY	0x0008		// temporary flag that forces the sequence to always play
#define STUDIO_POST		0x0010		// 
#define STUDIO_ALLZEROS	0x0020		// this animation/sequence has no real animation data
//						0x0040
#define STUDIO_CYCLEPOSE 0x0080		// cycle index is taken from a pose parameter index
#define STUDIO_REALTIME	0x0100		// cycle index is taken from a real-time clock, not the animations cycle index
#define STUDIO_LOCAL	0x0200		// sequence has a local context sequence
#define STUDIO_HIDDEN	0x0400		// don't show in default selection views
#define STUDIO_OVERRIDE	0x0800		// a forward declared sequence (empty)
#define STUDIO_ACTIVITY	0x1000		// Has been updated at runtime to activity index
#define STUDIO_EVENT	0x2000		// Has been updated at runtime to event index
#define STUDIO_WORLD	0x4000		// sequence blends in worldspace
// autolayer flags
//							0x0001
//							0x0002
//							0x0004
//							0x0008
#define STUDIO_AL_POST		0x0010		// 
//							0x0020
#define STUDIO_AL_SPLINE	0x0040		// convert layer ramp in/out curve is a spline instead of linear
#define STUDIO_AL_XFADE		0x0080		// pre-bias the ramp curve to compense for a non-1 weight, assuming a second layer is also going to accumulate
//							0x0100
#define STUDIO_AL_NOBLEND	0x0200		// animation always blends at 1.0 (ignores weight)
//							0x0400
//							0x0800
#define STUDIO_AL_LOCAL		0x1000		// layer is a local context sequence
//							0x2000
#define STUDIO_AL_POSE		0x4000		// layer blends using a pose parameter instead of parent cycle
