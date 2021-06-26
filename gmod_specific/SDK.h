#pragma once
#include "../engine/Core/Memory/Memory.h"
#include "../engine/Core/Basic/Basic.h"
#include "Math\core.hpp"
#include "Math\qangle.hpp"
#include "Math\vector.hpp"
#include "Math\vector2d.hpp"
#include "Math\vector4d.hpp"
#include "../engine/Render/Render.h"
#include "SDK\Engine\Consth.h"
#include "SDK\Engine\Definition.h"
#include "SDK/Engine/bitvec.h"
#include "SDK/tslist.h"
#include "SDK/Engine/memalloc.h"
#include "SDK/Engine/linkedlist.h"
extern IVEngineClient* g_pEngine;
extern IBaseClientDLL* g_pClient;
extern INetChannel* g_pNetChanel;
extern IClientEntityList* g_pEntList;
extern CInput* g_pInput;
extern ClientModeShared* g_pClientMode;
extern IVModelRender* g_pVModelRender;
extern IVModelInfo * g_pVModelInfo;
extern IVDebugOverlay* g_pVDebugOverlay;
extern IEngineTrace* g_pEngineTrace;
extern IShaderAPI * g_pShaderAPI;
extern IVRenderView * g_pVRenderView;
extern IViewRender * g_pViewRender;
extern IPlayerInfoManager * g_pPlayerInfoManager;
extern CGlobalVars * g_pGlobalVars;
extern IPrediction * g_pPrediction;
extern IGameMovement * g_pGameMovement;
extern IMoveHelper * g_pMoveHelper;
extern Lua_Shared * g_pLua_Shared;
extern ILuaInterface *g_pLuaShitInterface;
extern IPanel * g_pPanel;
extern IMaterialSystem * g_pMaterialSystem;
extern IStudioRender * g_pStudioRender;
extern CClientState * g_pClientState;
extern IPredictableList * g_pPredictableList;
extern IClientTools * g_pClientTools;
extern CEngineVGui * g_pEngineVGui;
extern ISurface *  g_pSurface;
extern IInputSystem * g_pInputSystem;
extern ICvar * g_pCvar;
extern CRender * g_pRender;
extern IPhysicsSurfaceProps * g_PhysicsSurfaceProps;
extern IEngineSound * g_pEngineSound;
extern IMemAlloc * g_pMemAlloc;
class ITexture;
#define CLIENT_MODULE       "client.dll"
#define ENGINE_MODULE       "engine.dll"
#define SERVER_MODULE       "server.dll"
#define MATERIAL_SYS_MODULE "materialsystem.dll"
#define SHADER_APIDX9_MODULE "shaderapidx9.dll"
#define VGUI_MODULE          "vgui2.dll"
#define LUASHIT_MODULE       "lua_shared.dll"
#define STUDIORENDER_MODULE       "studiorender.dll"
#define VSTLIB_MODULE       "vstdlib.dll"
#define VGUIMATERIALSURFACE_MODULE "vguimatsurface.dll"
#define INPUTSYSTEM_MODULE "inputsystem.dll"
#define VPHYSICS_MODULE       "vphysics.dll"
#define CLIENT_DLL_INTERFACE_VERSION		        "VClient017"
#define ENGINE_DLL_INTERFACE_VERSION		        "VEngineClient015"
#define MODEL_RENDER_INTERFACE_VERSION		        "VEngineModel016"
#define MODEL_INFO_CLIENT_INTERFACE_VERSION		    "VModelInfoClient006"
#define ENGINE_TRACE_INTERFACE_VERSION	         	 "EngineTraceClient003"
#define DEBUG_OVERLAY_INTERFACE_VERSION		    "VDebugOverlay003"
#define ENTLIST_ENGINE_DLL_INTERFACE_VERSION		"VClientEntityList003"
#define SHADER_APIDX9_INTERFACE_VERSION		        "ShaderApi030"
#define VENGINE_RENDERVIEW_INTERFACE_VERSION       "VEngineRenderView014"
#define INTERFACEVERSION_PLAYERINFOMANAGER			"PlayerInfoManager002"
#define PREDIVTION_INTERFACEVERSION "VClientPrediction001"
#define LUASHIT_INTERFACEVERSION "LUASHARED003"
#define VGUI_INTERFACEVERSION "VGUI_Panel009"
#define MaterialSystem_INTERFACEVERSION "VMaterialSystem080"
#define STUDIO_RENDER_INTERFACE_VERSION "VStudioRender025"
#define ENGINE_CVAR_INTERFACE_VERSION "VEngineCvar004"
#define CLIENT_TOOLS_INTERFACE_VERSION "VCLIENTTOOLS001"
#define VENGINE_VGUI_VERSION	"VEngineVGui001"
#define VGUI_SURFACE_INTERFACE_VERSION "VGUI_Surface030"
#define INPUTSYSTEM_VERSION	"InputSystemVersion001"
#define VPHYSICS_SURFACEPROPS_INTERFACE_VERSION	"VPhysicsSurfaceProps001"
#define IENGINESOUND_CLIENT_INTERFACE_VERSION	"IEngineSoundClient003"

#define CREATEINTERFACE_PROCNAME				"CreateInterface"




#include "SDK\Engine\buf.h"
#include "SDK\Engine\UtlVector.h"
#include"SDK\Engine\UtlBuffer.h"
#include "SDK\Engine\utlstack.h"


#include"SDK\Engine\ReciveProp.h"
#include "SDK\Engine\Client.h"
#include "SDK\Engine\NetWork.h"
#include "SDK\Engine\IVModelRender.h"
#include "SDK\Engine\IVModelInfo.h"
#include "SDK\Engine\debug_overlay.h"
#include "SDK\Engine\CInput.h"
#include "SDK\Engine\EntList.h"
#include "SDK\Engine\IVRenderView.h"
#include "SDK\Engine\IEngineTrace.h"
#include "SDK\Engine\ClentModeShared.h"
#include "SDK\Engine\IViewRender.h"
#include "SDK\Engine\sharedapix9.h"
#include "SDK\Engine\IPlayerInfoManager.h"
#include "SDK\Engine\Prediction.h"
#include "SDK\Engine\checksum.h"
#include "SDK\Engine\KeyValues.h"
#include "SDK\Engine\VGui.h"
#include "SDK\Engine\IMaterialSystem.h"
#include "SDK\Engine\StudioRender.h"
#include "SDK\Engine\Glow_outline_effect.h"
#include "SDK\Engine\CClientState.h"
#include "SDK\Engine\StudioRender.h"
#include "SDK\Engine\CRender.h"
#include "SDK\Engine\CClientState.h"
#include "SDK\Engine\VEngineCvar.h"
#include "SDK\Engine\IPhysicProps.h"
#include "SDK\Lua\Lua_shit.h"
#include "Util\netprop_manager.h"
#include "SDK\Engine\IClientTools.h"
#include "SDK\Engine\ISurface.h"
#include "SDK\Engine\IEngineVGui.h"
#include "SDK\Engine\CInputSystem.h"
#include "SDK\Engine\EngineSound.h"


#define TICK_INTERVAL			(g_pGlobalVars->interval_per_tick)

#define TIME_TO_TICKS( dt )		( (int)( 0.5 + (double)(dt) / TICK_INTERVAL ) )
#define TICKS_TO_TIME( t )		( TICK_INTERVAL *( t ) )

#include "SDK\Entity.h"

namespace SDK {
	extern Core::Memory::BasicModuleInfo Client;
	extern Core::Memory::BasicModuleInfo Engine;
	extern Core::Memory::BasicModuleInfo Server;
	extern Core::Memory::BasicModuleInfo Material_sys;
	extern Core::Memory::BasicModuleInfo VGuiMaterial_sys;

	extern VMatrix g_w2s_matrix;
	float GetLerpTime();
	extern bool W2S(const Vector& vOrigin, Vector& vScreen);

	class CDraw:public Basic::Singleton<CDraw>
	{
	public:
		void Init();
		void Reset();
		void Triangle(Vector2D p1, Vector2D p2, Vector2D p3, cColor clr);
		void BoxEx(Vector2D p1, Vector2D p2, Vector2D p3, Vector2D p4, cColor clr);
		void Line(int x1, int y1, int x2, int y2, cColor clr, int Thickness);
		void Line_outline(int x1, int y1, int x2, int y2, int l_Thickness, int o_Thickness);
		void Line_Outlined(int x1, int y1, int x2, int y2, cColor clr, int l_Thickness, int o_Thickness);
		void Box(int x, int y, int w, int h, cColor clr);
		void Outline(int x, int y, int w, int h, cColor clr, int Thickness);
		void Box_Outlined(int x, int y, int w, int h, cColor clr,int Thickness);
		void text(int x, int y, const wchar_t* string, HFont font, cColor color, bool centeredx, bool centeredy);
		void text(int x, int y, const wchar_t* string, HFont font, cColor color, bool centered = false);
		Vector2D get_text_size(const wchar_t* string, HFont font);

		void ColoredCircle(int x, int y, int radius, cColor clr);


		
		class LinePoint {
		public:
			LinePoint() = default;
			void Push(int x1, int y1, int x2, int y2, cColor clr, int l_Thickness,bool is_end);
			void Push(int x2, int y2, cColor clr, int l_Thickness, bool is_end);
			void Draw();
		private:
			struct Line {
				int x1, y1, x2, y2, l_Thickness;
				cColor clr;
				float tg;
			};

			std::vector<Line> points;
		};
	//	int t_Texture;
		HFont font_esp;
	};
	/*
	struct Line {
		int x1, y1, x2, y2, l_Thickness, o_Thickness;
		cColor clr;
		float tg;
	};
	class LinePoint {
	public:
		LinePoint() = default;
		void Push(int x1, int y1, int x2, int y2, cColor clr, int l_Thickness, int o_Thickness = 0);
		void Draw();
	private:
		std::vector<Line> points;
	};*/

	namespace ClassId {
		 int CGMOD_Player();
		 int CWeaponSWEP();
		 int CWeaponPistol();

		 int CSENT_anim();
		 int CSENT_point();
		 int CLuaNextBot();
		 int CSENT_AI();
	}

	bool Initialize();
	namespace Feature {
		extern bool PreEnginePrediction(CUserCmd* pCmd);
		extern void PostEnginePrediction();
	}
	
	namespace Vars {
		extern DWORD_PTR CMatSystemSurface_ptr;
		extern DWORD_PTR* m_pPredictionRandomSeed;
		extern IClientEntity ** m_pPredictionPlayer;
		extern ClientFrameStage_t* g_CurrStage;


		extern int g_tick;

	}
	namespace Function {
		using StartDrawing_fn = void(__thiscall*)(void*);
		extern StartDrawing_fn StartDrawing;

		using FinishDrawing_fn = void(__thiscall*)(void*);
		extern FinishDrawing_fn FinishDrawing;

		using PerformPrediction_fn = bool(__thiscall*)(void*, bool , IClientEntity *, int , int );
		extern PerformPrediction_fn PerformPrediction;

		using InvalidateBoneCache_fn = void(__thiscall*)(IClientEntity *);
		extern InvalidateBoneCache_fn InvalidateBoneCache;

		using	InvalidateBoneCaches_fn = void(__cdecl*)();
		extern	InvalidateBoneCaches_fn InvalidateBoneCaches;

		void	__cdecl RandomSeed(int iSeed);
		float	__cdecl RandomFloat(float fMin, float fMax);

		extern void UTIL_TraceLine(const Vector & vecAbsStart, const Vector & vecAbsEnd, unsigned int mask, IClientEntity * ignore, int collisionGroup, trace_t * ptr);

		bool IsVisible(C_GMOD_Player * pLocal, C_GMOD_Player * pEnemy, Vector pos);

	}
	extern char* Valve_Bones[];
}



class CBasePlayerAnimState {
public:
	virtual ~CBasePlayerAnimState() = 0;
	virtual void 	ClearAnimationState(void) = 0;
	virtual	void 	DoAnimationEvent(PlayerAnimEvent_t event, int nData) = 0;
	virtual	DWORD	CalcMainActivity() = 0;
	virtual	void 	Update(float eyeYaw, float eyePitch) = 0;
	virtual void 	Release() = 0;
	virtual	DWORD TranslateActivity(DWORD actDesired) = 0;
	virtual void SetRunSpeed(float flSpeed) = 0;
	virtual void SetWalkSpeed(float flSpeed) = 0;
	virtual void SetSprintSpeed(float flSpeed) = 0;

	virtual	void ShowDebugInfo(void) = 0;

	virtual void 	DebugShowAnimState(int iStartLine) = 0;
	virtual void	Init(void) = 0;
	virtual int 	SelectWeightedSequence(DWORD activity) = 0;
	virtual void 	RestartMainSequence() = 0;
	virtual void GetOuterAbsVelocity(Vector& vel) = 0;

	virtual bool HandleJumping(DWORD &idealActivity) = 0;
	virtual bool HandleDucking(DWORD &idealActivity) = 0;
	virtual bool HandleMoving(DWORD &idealActivity) = 0;
	virtual bool HandleSwimming(DWORD &idealActivity) = 0;
	virtual bool HandleDying(DWORD &idealActivity) = 0;
	virtual void RestartGesture(int iGestureSlot, DWORD iGestureActivity, bool bAutoKill = true) = 0;

	virtual float	GetGesturePlaybackRate(void) = 0;

	virtual void PlayFlinchGesture(DWORD iActivity) = 0;

	virtual float CalcMovementSpeed(bool *bIsMoving) = 0;
	virtual float CalcMovementPlaybackRate(bool *bIsMoving) = 0;

	virtual void		ComputePoseParam_MoveYaw(void *pStudioHdr) = 0;
	virtual void		ComputePoseParam_AimPitch(void *pStudioHdr) = 0;
	virtual void		ComputePoseParam_AimYaw(void *pStudioHdr) = 0;

	virtual void		EstimateYaw(void) = 0;

	virtual float GetCurrentMaxGroundSpeed() = 0;
	virtual void ComputeSequences(void *pStudioHdr) = 0;

	virtual bool ShouldUpdateAnimState() = 0;
public:

	bool							m_bForceAimYaw;//0x0004
	CUtlVector<GestureSlot_t>		m_aGestureSlots;//0x0008
	C_GMOD_Player					*m_pPlayer;//0x001C
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
	void*							m_hActiveWeapon;//0x00B0
	float							m_flLastGroundSpeedUpdateTime;//0x00B4
	char							pad_00B8[48]; //0x00B8//CInterpolatedVar<float> m_iv_flMaxGroundSpeed;
	float							m_flMaxGroundSpeed;//0x00E8
	int								m_nMovementSequence;//0x00EC
	LegAnimType_t					m_LegAnimType;//0x00F0
	C_GMOD_Player					*m_pHL2MPPlayer;//0x00F8


	/*
	char _0x0000[32];
	float m_flGoalFeetYaw2; //0x0024
	char _0x0028[36];
	float body_rot; //0x004C
	char _0x0050[8];
	float body_pitch; //0x0058
	float diff_feet; //0x005C
	char _0x0060[20];
	float m_flEyeYaw; //0x0074
	float m_flEyePitch; //0x0078
	float m_flGoalFeetYaw; //0x007C
	float m_flCurrentFeetYaw; //0x0080
	char pad_0084[48]; //0x0084
	float time; //0x00B4*/
};