#include "SDK.h"
#include "../engine/Engine.h"
#include "Util\EngineDraw.h"
#include "Hooks\Hooks.h"
IVEngineClient* g_pEngine;
IBaseClientDLL* g_pClient;
INetChannel* g_pNetChanel;
IClientEntityList* g_pEntList;
CInput* g_pInput;
ClientModeShared* g_pClientMode;
IVModelRender* g_pVModelRender;
IVModelInfo * g_pVModelInfo;
IVDebugOverlay* g_pVDebugOverlay;
IEngineTrace* g_pEngineTrace;
IShaderAPI * g_pShaderAPI;
IVRenderView * g_pVRenderView;
IViewRender * g_pViewRender;
IPlayerInfoManager * g_pPlayerInfoManager;
CGlobalVars * g_pGlobalVars;
IPrediction * g_pPrediction;
IGameMovement * g_pGameMovement;
IMoveHelper * g_pMoveHelper;
Lua_Shared * g_pLua_Shared;
ILuaInterface *g_pLuaShitInterface;
IPanel * g_pPanel;
IMaterialSystem * g_pMaterialSystem;
IStudioRender * g_pStudioRender;
CClientState * g_pClientState;
ICvar * g_pCvar;
IPredictableList * g_pPredictableList;
IClientTools * g_pClientTools;
CEngineVGui * g_pEngineVGui;
ISurface *g_pSurface;
IInputSystem * g_pInputSystem;
CRender * g_pRender;
IPhysicsSurfaceProps * g_PhysicsSurfaceProps;
IEngineSound * g_pEngineSound;
IMemAlloc * g_pMemAlloc;
//g_pVModelInfo

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------

const studiohdr_t * CStudioHdr::pSeqStudioHdr(int sequence)
{
	if(m_pVModel == NULL)
	{
		return m_pStudioHdr;
	}

	const studiohdr_t *pStudioHdr = GroupStudioHdr(m_pVModel->m_seq[sequence].group);

	return pStudioHdr;
}

const studiohdr_t * CStudioHdr::pAnimStudioHdr(int animation)
{
	if (m_pVModel == NULL)
	{
		return m_pStudioHdr;
	}

	const studiohdr_t *pStudioHdr = GroupStudioHdr(m_pVModel->m_anim[animation].group);

	return pStudioHdr;
}

const virtualmodel_t * CStudioHdr::ResetVModel(const virtualmodel_t *pVModel) const
{
	if (pVModel != NULL)
	{
		m_pVModel = (virtualmodel_t *)pVModel;
		Assert(!pVModel->m_Lock.GetOwnerId());
		m_pStudioHdrCache.SetCount(m_pVModel->m_group.Count());

		int i;
		for (i = 0; i < m_pStudioHdrCache.Count(); i++)
		{
			m_pStudioHdrCache[i] = NULL;
		}

		return const_cast<virtualmodel_t *>(pVModel);
	}
	else
	{
		m_pVModel = NULL;
		return NULL;
	}
}
const studiohdr_t * CStudioHdr::GroupStudioHdr(int i)
{
	if (!this)
	{
	}

	if (m_nFrameUnlockCounter != *m_pFrameUnlockCounter)
	{
		m_FrameUnlockCounterMutex.Lock();
		if (*m_pFrameUnlockCounter != m_nFrameUnlockCounter) // i.e., this thread got the mutex
		{
			memset(m_pStudioHdrCache.Base(), 0, m_pStudioHdrCache.Count() * sizeof(studiohdr_t *));
			m_nFrameUnlockCounter = *m_pFrameUnlockCounter;
		}
		m_FrameUnlockCounterMutex.Unlock();
	}

	if (!m_pStudioHdrCache.IsValidIndex(i))
	{
		return m_pStudioHdr; // return something known to probably exist, certainly things will be messed up, but hopefully not crash before the warning is noticed
	}

	const studiohdr_t *pStudioHdr = m_pStudioHdrCache[i];

	if (pStudioHdr == NULL)
	{
		Assert(!m_pVModel->m_Lock.GetOwnerId());
		virtualgroup_t *pGroup = &m_pVModel->m_group[i];
		pStudioHdr = pGroup->GetStudioHdr();
		m_pStudioHdrCache[i] = pStudioHdr;
	}

	Assert(pStudioHdr);
	return pStudioHdr;
}
int studiohdr_t::GetNumIKAutoplayLocks(void) const
{
	if (numincludemodels == 0)
	{
		return numlocalikautoplaylocks;
	}

	virtualmodel_t *pVModel = (virtualmodel_t *)GetVirtualModel();

	return pVModel->m_iklock.Count();
}
int studiohdr_t::GetAutoplayList(unsigned short ** pOut) const
{
	return g_pVModelInfo->GetAutoplayList(this, pOut);
}
virtualmodel_t		*studiohdr_t::GetVirtualModel(void) const
{
	if (numincludemodels == 0)
		return NULL;
	return (virtualmodel_t*)g_pVModelInfo->GetVirtualModel(this);
}
byte * studiohdr_t::GetAnimBlock(int i) const
{
	return g_pVModelInfo->GetAnimBlock(this, i);
}
;
int CStudioHdr::RemapSeqBone(int iSequence, int iLocalBone) const
{
	// remap bone
	if (m_pVModel)
	{
		const virtualgroup_t *pSeqGroup = m_pVModel->pSeqGroup(iSequence);
		return pSeqGroup->masterBone[iLocalBone];
	}
	return iLocalBone;
}
bool CStudioHdr::SequencesAvailable() const
{
	if (m_pStudioHdr->numincludemodels == 0)
	{
		return true;
	}

	if (m_pVModel == NULL)
	{
		// repoll m_pVModel
		return (ResetVModel(m_pStudioHdr->GetVirtualModel()) != NULL);
	}
	else
		return true;
}

int CStudioHdr::GetNumSeq(void) const
{
	if (m_pVModel == NULL)
	{
		return m_pStudioHdr->numlocalseq;
	}

	return m_pVModel->m_seq.Count();
}

mstudioanimdesc_t & CStudioHdr::pAnimdesc(int i)
{
	if (m_pVModel == NULL)
	{
		return *m_pStudioHdr->pLocalAnimdesc(i);
	}

	const studiohdr_t *pStudioHdr = GroupStudioHdr(m_pVModel->m_anim[i].group);

	return *pStudioHdr->pLocalAnimdesc(m_pVModel->m_anim[i].index);
}
mstudioseqdesc_t & CStudioHdr::pSeqdesc(int i)
{
	if (i < 0 || i >= GetNumSeq())
	{
		if (GetNumSeq() <= 0)
		{
			// Return a zero'd out struct reference if we've got nothing.
			// C_BaseObject::StopAnimGeneratedSounds was crashing due to this function
			//	returning a reference to garbage. It should now see numevents is 0,
			//	and bail.
			static mstudioseqdesc_t s_nil_seq;
			return s_nil_seq;
		}

		// Avoid reading random memory.
		i = 0;
	}

	if (m_pVModel == NULL)
	{
		return *m_pStudioHdr->pLocalSeqdesc(i);
	}

	const studiohdr_t *pStudioHdr = GroupStudioHdr(m_pVModel->m_seq[i].group);

	return *pStudioHdr->pLocalSeqdesc(m_pVModel->m_seq[i].index);
}
int CStudioHdr::iRelativeAnim(int baseseq, int relanim) const
{
	if (m_pVModel == NULL)
	{
		return relanim;
	}

	virtualgroup_t *pGroup = &m_pVModel->m_group[m_pVModel->m_seq[baseseq].group];

	return pGroup->masterAnim[relanim];
}
int CStudioHdr::iRelativeSeq(int baseseq, int relseq) const
{
	if (m_pVModel == NULL)
	{
		return relseq;
	}
	virtualgroup_t *pGroup = &m_pVModel->m_group[m_pVModel->m_seq[baseseq].group];

	return pGroup->masterSeq[relseq];
}
const studiohdr_t * virtualgroup_t::GetStudioHdr(void) const
{
	return g_pVModelInfo->FindModel(this->cache);
};

 mstudioposeparamdesc_t & CStudioHdr::pPoseParameter(int i)
{
	if (m_pVModel == NULL)
	{
		return *m_pStudioHdr->pLocalPoseParameter(i);
	}

	if (m_pVModel->m_pose[i].group == 0)
		return *m_pStudioHdr->pLocalPoseParameter(m_pVModel->m_pose[i].index);

	const studiohdr_t *pStudioHdr = GroupStudioHdr(m_pVModel->m_pose[i].group);

	return *pStudioHdr->pLocalPoseParameter(m_pVModel->m_pose[i].index);
}

 int CStudioHdr::GetSharedPoseParameter(int iSequence, int iLocalPose) const
 {
	 if (m_pVModel == NULL)
	 {
		 return iLocalPose;
	 }

	 if (iLocalPose == -1)
		 return iLocalPose;

	 Assert(m_pVModel);

	 virtualgroup_t *pGroup = &m_pVModel->m_group[m_pVModel->m_seq[iSequence].group];

	 return pGroup->masterPose[iLocalPose];
 }

 int CStudioHdr::GetNumIKAutoplayLocks(void) const
 {
	 if (m_pVModel == NULL)
	 {
		 return m_pStudioHdr->numlocalikautoplaylocks;
	 }

	 return m_pVModel->m_iklock.Count();
 }

 const mstudioiklock_t & CStudioHdr::pIKAutoplayLock(int i)
 {
	 if (m_pVModel == NULL)
	 {
		 return *m_pStudioHdr->pLocalIKAutoplayLock(i);
	 }

	 const studiohdr_t *pStudioHdr = GroupStudioHdr(m_pVModel->m_iklock[i].group);
	 return *pStudioHdr->pLocalIKAutoplayLock(m_pVModel->m_iklock[i].index);
 }

 static LARGE_INTEGER g_PerformanceFrequency;
 static LARGE_INTEGER g_MSPerformanceFrequency;
 static LARGE_INTEGER g_ClockStart;

 static void InitTime()
 {
	 if (!g_PerformanceFrequency.QuadPart)
	 {
		 QueryPerformanceFrequency(&g_PerformanceFrequency);
		 g_MSPerformanceFrequency.QuadPart = g_PerformanceFrequency.QuadPart / 1000;
		 QueryPerformanceCounter(&g_ClockStart);
	 }
 }

 double Plat_FloatTime()
 {
	 InitTime();

	 LARGE_INTEGER CurrentTime;

	 QueryPerformanceCounter(&CurrentTime);

	 double fRawSeconds = (double)(CurrentTime.QuadPart - g_ClockStart.QuadPart) / (double)(g_PerformanceFrequency.QuadPart);

#if !defined(STEAM) && !defined(_X360)
	
	return fRawSeconds;

#else
	 return fRawSeconds;
#endif
 }


 mstudioanim_t * mstudioanimdesc_t::pAnimBlock(int block, int index) const
 {
	 if (block == -1)
	 {
		 return (mstudioanim_t *)NULL;
	 }
	 if (block == 0)
	 {
		 return (mstudioanim_t *)(((byte *)this) + index);
	 }

	 byte *pAnimBlock = pStudiohdr()->GetAnimBlock(block);
	 if (pAnimBlock)
	 {
		 return (mstudioanim_t *)(pAnimBlock + index);
	 }

	 return (mstudioanim_t *)NULL;
 }


 mstudioikrule_t * mstudioanimdesc_t::pIKRule(int i) const
 {
	 if (ikruleindex)
	 {
		 return (mstudioikrule_t *)(((byte *)this) + ikruleindex) + i;
	 }
	 else if (animblockikruleindex)
	 {
		 if (animblock == 0)
		 {
			 return  (mstudioikrule_t *)(((byte *)this) + animblockikruleindex) + i;
		 }
		 else
		 {
			 byte *pAnimBlocks = pStudiohdr()->GetAnimBlock(animblock);

			 if (pAnimBlocks)
			 {
				 return (mstudioikrule_t *)(pAnimBlocks + animblockikruleindex) + i;
			 }
		 }
	 }

	 return NULL;
 }
 mstudiolocalhierarchy_t *mstudioanimdesc_t::pHierarchy(int i) const
 {
	 if (localhierarchyindex)
	 {
		 if (animblock == 0)
		 {
			 return  (mstudiolocalhierarchy_t *)(((byte *)this) + localhierarchyindex) + i;
		 }
		 else
		 {
			 byte *pAnimBlocks = pStudiohdr()->GetAnimBlock(animblock);

			 if (pAnimBlocks)
			 {
				 return (mstudiolocalhierarchy_t *)(pAnimBlocks + localhierarchyindex) + i;
			 }
		 }
	 }

	 return NULL;
 }
 bool ThreadInterlockedAssignIf64(volatile int64 *pDest, int64 value, int64 comperand)
 {

#if defined(_WIN32) && !defined(_X360)
	 __asm
	 {
		 lea esi, comperand;
		 lea edi, value;

		 mov eax, [esi];
		 mov edx, 4[esi];
		 mov ebx, [edi];
		 mov ecx, 4[edi];
		 mov esi, pDest;
		 lock CMPXCHG8B[esi];
		 mov eax, 0;
		 setz al;
	 }
#else
	 return (ThreadInterlockedCompareExchange64(pDest, value, comperand) == comperand);
#endif
 }

namespace SDK {
	Core::Memory::BasicModuleInfo Client;
	Core::Memory::BasicModuleInfo Engine;
	Core::Memory::BasicModuleInfo Server;
	Core::Memory::BasicModuleInfo Material_sys;
	Core::Memory::BasicModuleInfo VGuiMaterial_sys;

	VMatrix g_w2s_matrix;
	namespace Vars {
		DWORD_PTR CMatSystemSurface_ptr = 0u;
		DWORD_PTR* m_pPredictionRandomSeed = 0u;
		IClientEntity ** m_pPredictionPlayer = nullptr;
		ClientFrameStage_t* g_CurrStage;
		int g_tick;
	}
	namespace Function {
		StartDrawing_fn StartDrawing;
		FinishDrawing_fn FinishDrawing;
		PerformPrediction_fn PerformPrediction;
		InvalidateBoneCache_fn InvalidateBoneCache;
		InvalidateBoneCaches_fn InvalidateBoneCaches;
		void __cdecl RandomSeed(int iSeed)
		{
			using RandomSeed_fn = decltype(&RandomSeed);
			static DWORD Ptr = 0;
			if (!Ptr) {
				Core::Memory::BasicModuleInfo vstdlib("vstdlib.dll");
				Ptr = (DWORD)(Core::Memory::FindPattern((PBYTE)vstdlib.GetStartAddr(), vstdlib.GetSize(),
					(const char *)"\x55\x8B\xEC\x8B\x0D\xCC\xCC\xCC\xCC\xFF\x75\x08\x8B\x01\xFF\x10",//xxxxx????xxxxxxx
					0xCC));

			}

			RandomSeed_fn pRandomSeed = (RandomSeed_fn)Ptr;// (RandomSeed_t)(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomSeed"));
			pRandomSeed(iSeed);
		}
		float __cdecl RandomFloat(float fMin, float fMax)
		{
			using RandomFloat_fn = decltype(&RandomFloat);
			
			static DWORD Ptr = 0;
			if (!Ptr) {
				Core::Memory::BasicModuleInfo vstdlib("vstdlib.dll");
				Ptr = (DWORD)(Core::Memory::FindPattern((PBYTE)vstdlib.GetStartAddr(), vstdlib.GetSize(),
					(const char *)"\x55\x8B\xEC\x8B\x0D\xCC\xCC\xCC\xCC\x83\xEC\x08",//xxxxx????xxx
					0xCC));

			}

			RandomFloat_fn pRandomFloat = (RandomFloat_fn)Ptr;//(GetProcAddress(GetModuleHandle("vstdlib.dll"), "RandomFloat"));
			return pRandomFloat(fMin, fMax);
		}
		void UTIL_TraceLine(const Vector & vecAbsStart, const Vector & vecAbsEnd, unsigned int mask, IClientEntity * ignore, int collisionGroup, trace_t * ptr)
		{
			Ray_t ray = {};
			ray.Init(vecAbsStart, vecAbsEnd);
			CTraceFilter traceFilter = {};
			traceFilter.pSkip = ignore;
			g_pEngineTrace->TraceRay(ray, mask, &traceFilter, ptr);
		}
		bool IsVisible(C_GMOD_Player * pLocal, C_GMOD_Player * pEnemy, Vector pos)
		{
			trace_t Trace = {};
			Ray_t ray = {};
			ray.Init(pLocal->EyePosition(), pos);
			CTraceFilter traceFilter;
			traceFilter.pSkip = pLocal;
			g_pEngineTrace->TraceRay(ray, MASK_SOLID, &traceFilter, &Trace);
			//auto SurfaceData = g_PhysicsSurfaceProps->GetSurfaceData(Trace.surface.surfaceProps);

			//CHAR_TEX_FLESH
			if (Trace.m_pEnt == pEnemy || Trace.fraction >= 0.99f /*|| SurfaceData->game.material == CHAR_TEX_FLESH*/)
			{
				return true;
			}
			return false;
		}
	}
	char* Valve_Bones[] = {
	"ValveBiped.Bip01_Spine",
	"ValveBiped.Bip01_Spine1",
	"ValveBiped.Bip01_Spine2",
	"ValveBiped.Bip01_Spine4",
	"ValveBiped.Bip01_Neck1",
	"ValveBiped.Bip01_Head1",
	"ValveBiped.Bip01_R_Clavicle",
	"ValveBiped.Bip01_R_UpperArm",
	"ValveBiped.Bip01_R_Forearm",
	"ValveBiped.Bip01_R_Hand",
	"ValveBiped.Bip01_L_Clavicle",
	"ValveBiped.Bip01_L_UpperArm",
	"ValveBiped.Bip01_L_Forearm",
	"ValveBiped.Bip01_L_Hand"/*,
	"ValveBiped.Bip01_R_Thigh",
	"ValveBiped.Bip01_R_Calf",
	"ValveBiped.Bip01_R_Foot",
	"ValveBiped.Bip01_R_Toe0",
	"ValveBiped.Bip01_L_Thigh",
	"ValveBiped.Bip01_L_Calf",
	"ValveBiped.Bip01_L_Foot",
	"ValveBiped.Bip01_L_Toe0"*/
	};
	bool ScreenTransform(const Vector & vPoint, Vector & vScreen) {

		float w = 0.f;
		bool bRet = false;
		//auto worldToScreen = SDK::g_w2s_matrix;
		vScreen.m_x = SDK::g_w2s_matrix[0][0] * vPoint.m_x + SDK::g_w2s_matrix[0][1] * vPoint.m_y + SDK::g_w2s_matrix[0][2] * vPoint.m_z + SDK::g_w2s_matrix[0][3];
		vScreen.m_y = SDK::g_w2s_matrix[1][0] * vPoint.m_x + SDK::g_w2s_matrix[1][1] * vPoint.m_y + SDK::g_w2s_matrix[1][2] * vPoint.m_z + SDK::g_w2s_matrix[1][3];
		vScreen.m_z = 0.0f;

		w = SDK::g_w2s_matrix[3][0] * vPoint.m_x + SDK::g_w2s_matrix[3][1] * vPoint.m_y + SDK::g_w2s_matrix[3][2] * vPoint.m_z + SDK::g_w2s_matrix[3][3];

		if (w < 0.001f)
		{
			bRet = true;

			vScreen.m_x *= 100000;
			vScreen.m_y *= 100000;
		}
		else
		{
			float invw = 1.0f / w;

			vScreen.m_x *= invw;
			vScreen.m_y *= invw;
		}

		return bRet;
	}
	bool W2S(const Vector & vOrigin, Vector & vScreen)
	{

		int width = 0, height = 0;
		float fScreenX = 0.f, fScreenY = 0.f;
		bool bRet = false;

		if (!ScreenTransform(vOrigin, vScreen))
		{
			g_pEngine->GetScreenSize(width, height);

			fScreenX = float(width / 2);
			fScreenY = float(height / 2);

			fScreenX += 0.5f * vScreen.m_x * width + 0.5f;
			fScreenY -= 0.5f * vScreen.m_y * height + 0.5f;

			vScreen.m_x = fScreenX;
			vScreen.m_y = fScreenY;

			bRet = true;
		}

		return bRet;
	}

	bool Initialize()
	{
		Client = Core::Memory::BasicModuleInfo(CLIENT_MODULE);
		Engine = Core::Memory::BasicModuleInfo(ENGINE_MODULE);
		Server = Core::Memory::BasicModuleInfo(SERVER_MODULE);
		Material_sys = Core::Memory::BasicModuleInfo(MATERIAL_SYS_MODULE);
		VGuiMaterial_sys = Core::Memory::BasicModuleInfo(VGUIMATERIALSURFACE_MODULE);

		auto CaptureFactory =[](char* FactoryModule) -> CreateInterfaceFn {
			CreateInterfaceFn Interface = 0;
			while (!Interface) {
				HMODULE hFactoryModule = GetModuleHandleA(FactoryModule);
				if (hFactoryModule)
					Interface = (CreateInterfaceFn)(GetProcAddress(hFactoryModule, CREATEINTERFACE_PROCNAME));
				Sleep(100);
			}
			return Interface;
		};
		auto CaptureInterface = [](CreateInterfaceFn Interface, char* InterfaceName)->PDWORD  {
			PDWORD dwPointer = nullptr;
			while (!dwPointer) {
				dwPointer = (PDWORD)(Interface(InterfaceName, 0));
				Sleep(100);
			}
			return dwPointer;
		};
		
		auto ClientFactory = CaptureFactory(CLIENT_MODULE);
		auto ServerFactory = CaptureFactory(SERVER_MODULE);
		auto EngineFactory = CaptureFactory(ENGINE_MODULE);
		auto MaterialFactory = CaptureFactory(MATERIAL_SYS_MODULE);
		auto ShaderApiDX9Factory = CaptureFactory(SHADER_APIDX9_MODULE);
		auto LUASHITFactory = CaptureFactory(LUASHIT_MODULE);
		auto VGuiFactory = CaptureFactory(VGUI_MODULE);
		auto studiorenderFactory = CaptureFactory(STUDIORENDER_MODULE);
		auto VSTFactory = CaptureFactory(VSTLIB_MODULE);
		auto vguimatsurfaceFactory = CaptureFactory(VGUIMATERIALSURFACE_MODULE);
		auto inputsysFactory = CaptureFactory(INPUTSYSTEM_MODULE);
		auto vphysicFactory = CaptureFactory(VPHYSICS_MODULE);
		
		g_pMemAlloc = *(IMemAlloc**)(GetProcAddress(GetModuleHandle("tier0.dll"), "g_pMemAlloc"));

		g_PhysicsSurfaceProps = (IPhysicsSurfaceProps*)(CaptureInterface(vphysicFactory, VPHYSICS_SURFACEPROPS_INTERFACE_VERSION));
		if (!g_PhysicsSurfaceProps)
			return false;
		g_pEngineSound = (IEngineSound*)(CaptureInterface(EngineFactory, IENGINESOUND_CLIENT_INTERFACE_VERSION));
		if(!g_pEngineSound)
			return false;
		g_pStudioRender = (IStudioRender*)(CaptureInterface(studiorenderFactory, STUDIO_RENDER_INTERFACE_VERSION));
		if (!g_pStudioRender)
			return false;
		g_pPanel = (IPanel*)(CaptureInterface(VGuiFactory, VGUI_INTERFACEVERSION));
		if (!g_pPanel)
			return false;
		g_pLua_Shared = (Lua_Shared*)(CaptureInterface(LUASHITFactory, LUASHIT_INTERFACEVERSION));
		if (!g_pLua_Shared)
			return false;
		g_pEngine = (IVEngineClient*)(CaptureInterface(EngineFactory, ENGINE_DLL_INTERFACE_VERSION));
		if (!g_pEngine)
			return false;
		g_pClient = (IBaseClientDLL*)(CaptureInterface(ClientFactory, CLIENT_DLL_INTERFACE_VERSION));
		if (!g_pClient)
			return false;
		g_pShaderAPI = (IShaderAPI*)(CaptureInterface(ShaderApiDX9Factory, SHADER_APIDX9_INTERFACE_VERSION));
		if (!g_pShaderAPI)
			return false;
		g_pPlayerInfoManager = (IPlayerInfoManager *)(CaptureInterface(ServerFactory, INTERFACEVERSION_PLAYERINFOMANAGER));
		if (!g_pPlayerInfoManager)
			return false;
		g_pGlobalVars = g_pPlayerInfoManager->GetGlobalVars();
		if (!g_pGlobalVars)
			return false;
		g_pCvar = (ICvar *)(CaptureInterface(VSTFactory, ENGINE_CVAR_INTERFACE_VERSION));
		if (!g_pCvar)
			return false;
		g_pEntList = (IClientEntityList*)(CaptureInterface(ClientFactory, ENTLIST_ENGINE_DLL_INTERFACE_VERSION));
		if (!g_pEntList)
			return false;
		g_pClientMode = **(ClientModeShared***)((DWORD)(Core::Memory::FindPattern((PBYTE)Client.GetStartAddr(), Client.GetSize(),
			(const char *)"\x8B\x0D\xCC\xCC\xCC\xCC\xD9\x45\x0C\x56",//xx????xxxx
			0xCC)) + 0x2);
		if (!g_pClientMode)
			return false;
		g_pViewRender = **(IViewRender***)((DWORD)(Core::Memory::FindPattern((PBYTE)Client.GetStartAddr(), Client.GetSize(),
			(const char *)"\x8B\x0D\xCC\xCC\xCC\xCC\x8B\x01\xFF\x50\x54\x85\xC0\x75\x52",//xx????xxxxxxxxx
			0xCC)) + 0x2);
		if (!g_pViewRender)
			return false;
		auto ClientVTBL = *(PDWORD*)g_pClient;
		g_pInput = **(CInput***)(ClientVTBL[15] + 0x2);//IN_ActivateMouse
		if (!g_pInput)
			return false;
		g_pVModelRender = (IVModelRender*)(CaptureInterface(EngineFactory, MODEL_RENDER_INTERFACE_VERSION));
		if (!g_pVModelRender)
			return false;
		g_pVRenderView = (IVRenderView *)(CaptureInterface(EngineFactory, VENGINE_RENDERVIEW_INTERFACE_VERSION));
		if (!g_pVRenderView)
			return false;
		g_pVModelInfo = (IVModelInfo*)(CaptureInterface(EngineFactory, MODEL_INFO_CLIENT_INTERFACE_VERSION));
		if (!g_pVModelInfo)
			return false;
		g_pVDebugOverlay = (IVDebugOverlay*)(CaptureInterface(EngineFactory, DEBUG_OVERLAY_INTERFACE_VERSION));
		if (!g_pVDebugOverlay)
			return false;
		g_pEngineTrace = (IEngineTrace*)(CaptureInterface(EngineFactory, ENGINE_TRACE_INTERFACE_VERSION));
		if (!g_pEngineTrace)
			return false;
		g_pEngineVGui = (CEngineVGui*)(CaptureInterface(EngineFactory, VENGINE_VGUI_VERSION));
		if (!g_pEngineVGui)
			return false;
		g_pPrediction = (IPrediction*)(CaptureInterface(ClientFactory, PREDIVTION_INTERFACEVERSION));
		if (!g_pPrediction)
			return false;
		g_pGameMovement = (IGameMovement*)(CaptureInterface(ClientFactory, "GameMovement001"));
		if (!g_pGameMovement)
			return false;
		g_pClientTools = (IClientTools*)(CaptureInterface(ClientFactory, CLIENT_TOOLS_INTERFACE_VERSION));
		if (!g_pClientTools)
			return false;
		g_pMaterialSystem = (IMaterialSystem *)(CaptureInterface(MaterialFactory, MaterialSystem_INTERFACEVERSION));
		if (!g_pMaterialSystem)
			return false;
		g_pSurface = (ISurface*)(CaptureInterface(vguimatsurfaceFactory, VGUI_SURFACE_INTERFACE_VERSION));
		if (!g_pSurface)
			return false;
		g_pInputSystem = (IInputSystem*)(CaptureInterface(inputsysFactory, INPUTSYSTEM_VERSION));
		if (!g_pInputSystem)
			return false;
		//Bogus pmove player movetype %i on (%i) 0=cl 1=sv
		g_pMoveHelper = **(IMoveHelper***)((DWORD)(Core::Memory::FindPattern((PBYTE)Client.GetStartAddr(), Client.GetSize(),
			(const char *)"\x8B\x0D\xCC\xCC\xCC\xCC\x8B\x01\xFF\x50\x04\x8B\x07",//xx????xxxxxxx
			0xCC)) + 0x2);
		if (!g_pMoveHelper)
			return false;
		//C_BaseEntity::AddVisibleEntities
		//8B 0D ? ? ? ? ? ? 8B ? FF ? 04 8B ? 33
		g_pPredictableList = **(IPredictableList***)((DWORD)(Core::Memory::FindPattern((PBYTE)Client.GetStartAddr(), Client.GetSize(),
			(const char *)"\x8B\x0D\xCC\xCC\xCC\xCC\xCC\xCC\x8B\xCC\xFF\xCC\x04\x8B\xCC\x33",//xx????xxxxxxxxxxxxx
			0xCC)) + 0x2);
		if (!g_pPredictableList)
			return false;
		g_pClientState = *(CClientState**)((DWORD)(Core::Memory::FindPattern((PBYTE)Engine.GetStartAddr(), Engine.GetSize(),
			(const char *)"\x84\xC0\x75\x09\xE8\xCC\xCC\xCC\xCC\x84\xC0\x74\x05",//xxxxx????xxxx
			0xCC)) - 0xE);
		if (!g_pClientState)
			return false;
		g_pRender = **(CRender***)((DWORD)(Core::Memory::FindPattern((PBYTE)Engine.GetStartAddr(), Engine.GetSize(),
			(const char *)"\x8B\x0D\xCC\xCC\xCC\xCC\xFF\x75\x0C\xFF\x75\x08\x8B\x01\xFF\x50\x0C",//xx????xxxxxxxxxxx
			0xCC)) + 0x2);
		if (!SDK::Util::NetPropManager::Instance().Create(g_pClient))
			return false;
		//SDK::Util::NetPropManager::Instance().Dump("test_dump.txt");
	//	Engine::msg_box("start");
		//SDK::Util::NetPropManager::Instance().Dump("de_dump.txt");
		Render::s_timer.Reset();
		Render::s_timer.m_start -= 10000ull;
		
		g_KeyValues.Init();
		{
			Function::StartDrawing = (Function::StartDrawing_fn)Core::Memory::FindPattern((PBYTE)VGuiMaterial_sys.GetStartAddr(), VGuiMaterial_sys.GetSize(),
				(const char *)"\x55\x8B\xEC\x64\xA1\xCC\xCC\xCC\xCC\x6A\xFF\x68\xCC\xCC\xCC\xCC\x50\x64\x89\x25\xCC\xCC\xCC\xCC\x83\xEC\x14",//xxxxx????xxx????xxxx????xxx
				0xCC);
			Function::FinishDrawing = (Function::FinishDrawing_fn)Core::Memory::FindPattern((PBYTE)VGuiMaterial_sys.GetStartAddr(), VGuiMaterial_sys.GetSize(),
				(const char *)"\x55\x8B\xEC\x6A\xFF\x68\xCC\xCC\xCC\xCC\x64\xA1\xCC\xCC\xCC\xCC\x50\x64\x89\x25\xCC\xCC\xCC\xCC\x51\x56\x6A\x00",//xxxxxx????xx????xxxx????xxxx
				0xCC);

			Function::PerformPrediction = (Function::PerformPrediction_fn)Core::Memory::FindPattern((PBYTE)Client.GetStartAddr(), Client.GetSize(),
				(const char *)"\x55\x8B\xEC\x83\xEC\x18\x53\x56\x8B\x35\xCC\xCC\xCC\xCC\x8B\xD9",//xxxxxxxxxx????xx
				0xCC);
			auto InvalidateBoneCache_call_ = (DWORD)Core::Memory::FindPattern((PBYTE)Client.GetStartAddr(), Client.GetSize(),
				(const char *)"\xE8\xCC\xCC\xCC\xCC\x8B\x07\x8B\xCF\x8B\x40\x24\xFF\xD0\xD9\x45\x10",//x????xxxxxxxxxxxx
				0xCC);
			Function::InvalidateBoneCache = (Function::InvalidateBoneCache_fn)Core::Memory::follow_to(InvalidateBoneCache_call_);
			auto InvalidateBoneCaches_call_ = (DWORD)Core::Memory::FindPattern((PBYTE)Client.GetStartAddr(), Client.GetSize(),
				(const char *)"\xE8\xCC\xCC\xCC\xCC\x6A\x01\x6A\x01\x8D\x4D\x0F",//x????xxxxxxx
				0xCC);
			Function::InvalidateBoneCaches = (Function::InvalidateBoneCaches_fn)Core::Memory::follow_to(InvalidateBoneCaches_call_);
		}

		{
			auto CMatSysSurf_mem_usage_ = (DWORD)Core::Memory::FindPattern((PBYTE)Engine.GetStartAddr(), Engine.GetSize(),
				(const char *)"\x8B\x0D\xCC\xCC\xCC\xCC\x8D\x55\xF4\x52\x8D\x55\xF0\x52\x8B\x01\xFF\x90",//xx????xxxxxxxxxxxx
				0xCC);
			Vars::CMatSystemSurface_ptr = *(DWORD*)Core::Memory::get_disp(CMatSysSurf_mem_usage_);

			Vars::m_pPredictionRandomSeed = *(DWORD_PTR**)((DWORD)(Core::Memory::FindPattern((PBYTE)Client.GetStartAddr(), Client.GetSize(),
				(const char *)"\x55\x8B\xEC\x8B\x45\x08\x85\xC0\x75\x0C",//xxxxxxxxxx
				0xCC)) + 0xC);
			Vars::m_pPredictionPlayer = *(IClientEntity***)((DWORD)(Core::Memory::FindPattern((PBYTE)Client.GetStartAddr(), Client.GetSize(),
				(const char *)"\x89\x3D\xCC\xCC\xCC\xCC\xF3\x0F\x2A\x87",//xx????xxxx
				0xCC)) + 0x2);

			DWORD g_currptr = (DWORD)Core::Memory::FindPattern((PBYTE)Client.GetStartAddr(), Client.GetSize(),
				(const char *)"\xA3\xCC\xCC\xCC\xCC\x83\xF8\x06",//x????xxx
				0xCC);
			g_currptr += 0x1;
			Vars::g_CurrStage = *(ClientFrameStage_t**)g_currptr;
		}

		CDraw::Instance().Init();

		Hooks::Initialize();

		return true;
	}

	namespace Feature {
		int	 m_iOldFlags;
		int	m_iOldTickBase;
		float m_flOldCurtime;
		float m_flOldFrametime;
		CMoveData m_MoveData;
		bool PreEnginePrediction(CUserCmd* pCmd)
		{
			if (!Vars::m_pPredictionRandomSeed || !Vars::m_pPredictionPlayer || !g_pEngine->IsInGame() || g_pEngine->IsDrawingLoadingImage())
				return false;
			auto pLocal = C_GMOD_Player::GetLocalPlayer();
			if(!pLocal||!pLocal->IsGood())
				return false;
			/*
			auto old_button = pCmd->buttons;
			if (pCmd->buttons & IN_ATTACK) {
				pCmd->buttons &= ~IN_ATTACK;
			}
			if (pCmd->buttons & IN_ATTACK2) {
				pCmd->buttons &= ~IN_ATTACK2;
			}
			Vector OldOrig = pLocal->m_vecOrigin();
			m_iOldTickBase = pLocal->m_nTickBase();
			g_pPrediction->RunCommand(pLocal, pCmd, g_pMoveHelper);
			Vector NewOrig = pLocal->m_vecOrigin();
			pLocal->m_vecOrigin() = ((OldOrig + NewOrig) / 2);
			pLocal->m_nTickBase() = m_iOldTickBase;

			pCmd->buttons = old_button;
			*/
			
			m_iOldTickBase = pLocal->m_nTickBase();
			m_iOldFlags = pLocal->m_fFlags();
			m_flOldCurtime = g_pGlobalVars->curtime;
			m_flOldFrametime = g_pGlobalVars->frametime;
			pLocal->m_pCurrentCommand(pCmd) ;
			*Vars::m_pPredictionRandomSeed = MD5_PseudoRandom(pCmd->command_number) & 0x7fffffff;
			*Vars::m_pPredictionPlayer = pLocal;
			g_pGlobalVars->curtime = pLocal->m_nTickBase() * g_pGlobalVars->interval_per_tick;
			g_pGlobalVars->frametime = g_pGlobalVars->interval_per_tick;

			ZeroMemory(&m_MoveData, sizeof(m_MoveData));
			Vector OldOrig = pLocal->m_vecOrigin();
			g_pGameMovement->StartTrackPredictionErrors(pLocal);

		//	pLocal->UpdateButtonState(pCmd->buttons);
		//	pLocal->RunPreThink();
		//	pLocal->RunThink();
		//	pLocal->RunPreThink();
		//	pLocal->RunThink();


			g_pPrediction->SetupMove(pLocal, pCmd, g_pMoveHelper, &m_MoveData);
			g_pGameMovement->ProcessMovement(pLocal, &m_MoveData);
			g_pPrediction->FinishMove(pLocal, pCmd, &m_MoveData);

		//	pLocal->SetAbsOrigin(m_MoveData.m_vecAbsOrigin);
		//	pLocal->m_vecOrigin() = m_MoveData.m_vecAbsOrigin;
		//	pLocal->RunPostThink();
			return true;
		}
		void PostEnginePrediction()
		{
			auto pLocal = C_GMOD_Player::GetLocalPlayer();
			if (!pLocal || !pLocal->IsGood())
				return;
		//	pLocal->RunPostThink();
			
			g_pGameMovement->FinishTrackPredictionErrors(pLocal);
		//	pLocal->m_nTickBase() = m_iOldTickBase;
		//	pLocal->m_fFlags() = m_iOldFlags;
			g_pGlobalVars->curtime = m_flOldCurtime;
			g_pGlobalVars->frametime = m_flOldFrametime;
			pLocal->m_pCurrentCommand(NULL);
			*Vars::m_pPredictionPlayer = NULL;
			*Vars::m_pPredictionRandomSeed = -1;
		}
	}
}

#define DECL_CLASSID(CLASS_NAME)\
int SDK::ClassId::CLASS_NAME()\
{\
	static int CLASS_NAME##ID = 0;\
	if (!CLASS_NAME##ID) {\
		for (auto entry = g_pClient->GetAllClasses(); entry; entry = entry->m_pNext)\
		{\
			if (strcmp(entry->GetName(), #CLASS_NAME) == 0) {\
				CLASS_NAME##ID = entry->m_ClassID;\
				break;\
			}\
		}\
	}\
	return CLASS_NAME##ID;\
}

DECL_CLASSID(CGMOD_Player)
DECL_CLASSID(CWeaponSWEP)
DECL_CLASSID(CWeaponPistol)


DECL_CLASSID(CSENT_anim)
DECL_CLASSID(CSENT_point)
DECL_CLASSID(CLuaNextBot)
DECL_CLASSID(CSENT_AI)