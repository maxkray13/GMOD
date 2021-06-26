#pragma once
#include "../../engine/Core/Memory/Memory.h"
#include "../../engine/Core/Basic/Basic.h"

namespace Hooks {
	void Initialize();

	
	extern Core::Memory::VMHook VEngineVGui;
	extern Core::Memory::VMHook VGui;
	extern Core::Memory::VMHook VRenderView;
	extern Core::Memory::VMHook VClientMode;
	extern Core::Memory::VMHook VLocalTable;
	extern Core::Memory::VMHook VEngineHook;
	extern Core::Memory::VMHook VViewRender;
	extern Core::Memory::VMHook VNetChan;
	extern Core::Memory::VMHook VPrediction;
	extern Core::Memory::VMHook VRender;
	extern Core::Memory::VMHook VEngineSound;
	extern Core::Memory::VMHook VClient;
	extern Core::Memory::VMHook VMaterialSystem;
	extern Core::Memory::VMHook VClientState;

	extern std::mutex g_Sync;
	extern PVOID Game_tls[64];
	extern PVOID p_extended_tls;
	extern DWORD Game_threadId;
	
	namespace Variables {
		extern QAngle PureAngle;
		extern QAngle FinalAngle;
		extern QAngle Angle2Send;
		extern QAngle ChokedAngle;
		extern QAngle PostFinalAngle;
		extern Vector PositionWSend;
		extern Vector Spread;

		extern int OriginalButtons;
		extern bool InDebugCam;
		extern bool InThirdPerson;
		extern QAngle DebugCamAngle;
		extern CUserCmd* g_pLastCmd;
		extern float curr_fov;
	}
	

}