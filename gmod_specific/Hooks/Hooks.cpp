#include "../SDK.h"
#include "../../engine/Engine.h"
#include "Hooks.h"
#include "Handles\Paint.h"
#include "Handles\PaintTransverse.h"
#include "Handles\SceneEnd.h"
#include "Handles\CreateMove.h"
#include "Handles\RenderView.h"
#include "Handles\SendDatagram.h"
#include "Handles\RunCommand.h"
#include "Handles\GetViewAngles.h"
#include "Handles\EmitSound.h"
#include "Handles\SceneBegin.h"
#include "Handles\FrameStageNotify.h"
#include "..\Functional\net_logger.h"
#include "Handles/ReadPixels.h"
#include "../Functional/EffectHook.h"
#include "Handles/ProcessTempEntities.h"
namespace Hooks {

	Core::Memory::VMHook VEngineVGui;
	Core::Memory::VMHook VGui;
	Core::Memory::VMHook VRenderView;
	Core::Memory::VMHook VClientMode;
	Core::Memory::VMHook VLocalTable;
	Core::Memory::VMHook VEngineHook;
	Core::Memory::VMHook VViewRender;
	Core::Memory::VMHook VNetChan;
	Core::Memory::VMHook VPrediction;
	Core::Memory::VMHook VRender;
	Core::Memory::VMHook VEngineSound;
	Core::Memory::VMHook VClient;
	Core::Memory::VMHook VMaterialSystem;
	Core::Memory::VMHook VClientState;


	std::mutex g_Sync;
	PVOID Game_tls[64];
	PVOID p_extended_tls;
	DWORD Game_threadId;


	void Initialize()
	{
		VClientState = Core::Memory::VMHook((void*)((DWORD)g_pClientState + 0x8));
		VClientState.HookVM(hkProcessTempEntities, 24);//24 idx of CClientState::ProcessTempEntities //g_pClientState + 0x8
		VClientState.ApplyVMT();
		
		VEngineVGui = Core::Memory::VMHook(g_pEngineVGui);
		VEngineVGui.HookVM(hkPaint, 13);
		VEngineVGui.ApplyVMT();

		auto context = g_pMaterialSystem->GetRenderContext();
		auto ptr = (*(DWORD*)context) + 0x2C;

		DWORD OldProtection;
		VirtualProtect((void*)ptr, sizeof(ptr), PAGE_READWRITE, &OldProtection);
		o_ReadPix = (tdReadPix) *(DWORD*)ptr;
		*(DWORD*)ptr = (DWORD)hkReadPixels;
		VirtualProtect((void*)ptr, sizeof(ptr), OldProtection, &OldProtection);

		ptr = (*(DWORD*)context) + 0x18C;
		VirtualProtect((void*)ptr, sizeof(ptr), PAGE_READWRITE, &OldProtection);
		o_ReadPixAndStretch = (tdReadPixAndStretch) *(DWORD*)ptr;
		*(DWORD*)ptr = (DWORD)hkReadPixelsAndStretch;
		VirtualProtect((void*)ptr, sizeof(ptr), OldProtection, &OldProtection);

		EffectHook::INITIALIZE();
		/*
		VMaterialSystem = Core::Memory::VMHook(g_pMaterialSystem->GetRenderContext());
		VMaterialSystem.HookVM(hkReadPixels, 11);
		VMaterialSystem.ApplyVMT();*/

		//void			FrameStageNotify(ClientFrameStage_t curStage) = 0;//35
	//	VClient = Core::Memory::VMHook(g_pClient);
	//	VClient.HookVM(hkFrameStageNotify, 35);
	//	VClient.ApplyVMT();

		/*VEngineSound = Core::Memory::VMHook(g_pEngineSound);
		VEngineSound.HookVM(hkEmitSound4, 4);
		VEngineSound.HookVM(hkEmitSound5, 5);
		VEngineSound.ApplyVMT();*/
	//	VPrediction = Core::Memory::VMHook(g_pPrediction);
	//	VPrediction.HookVM(hkRunCommand, 17);
	//	VPrediction.ApplyVMT();
		/**/
		VGui = Core::Memory::VMHook(g_pPanel);
		VGui.HookVM(hkPaintTraverse, 41);
		VGui.ApplyVMT();

		VRenderView = Core::Memory::VMHook(g_pVRenderView);
		VRenderView.HookVM(hkSceneBegin, 8);
		VRenderView.HookVM(hkSceneEnd, 9);
		VRenderView.ApplyVMT();
		
		VClientMode = Core::Memory::VMHook(g_pClientMode);
		VClientMode.HookVM(hkCreateMove, 21);
		VClientMode.ApplyVMT();
		
		VEngineHook = Core::Memory::VMHook(g_pEngine);
		VEngineHook.HookVM(hkSetViewAngles, 20);
		VEngineHook.ApplyVMT();

		VViewRender = Core::Memory::VMHook(g_pViewRender);
		VViewRender.HookVM(hkRenderView, 6);
		VViewRender.ApplyVMT();

		net_logger::init_hook();
	}

	namespace Variables {

		QAngle PureAngle;
		QAngle FinalAngle;
		QAngle Angle2Send;
		Vector PositionWSend;
		QAngle ChokedAngle;
		QAngle PostFinalAngle;
		
		Vector Spread;

		int OriginalButtons;
		bool InDebugCam;
		bool InThirdPerson;
		QAngle DebugCamAngle;
		CUserCmd* g_pLastCmd = nullptr;

		float curr_fov;
	}
}
