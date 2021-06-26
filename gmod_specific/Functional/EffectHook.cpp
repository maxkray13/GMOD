#include "../../include/stdafx.h"
#include "../SDK.h"
#include "EffectHook.h"
#include "ServerAnimations.h"
#include "../Functional/LagCompensation.h"
namespace EffectHook {

	OverlayLine_t * AddLineOverlay(const Vector& origin, const Vector& dest, int r, int g, int b, int a, bool noDepthTest, float flDuration)
	{
		OverlayLine_t *new_loverlay = new OverlayLine_t;
		new_loverlay->origin[0] = origin[0];
		new_loverlay->origin[1] = origin[1];
		new_loverlay->origin[2] = origin[2];

		new_loverlay->dest[0] = dest[0];
		new_loverlay->dest[1] = dest[1];
		new_loverlay->dest[2] = dest[2];

		new_loverlay->r = r;
		new_loverlay->g = g;
		new_loverlay->b = b;
		new_loverlay->a = a;

		new_loverlay->noDepthTest = noDepthTest;
		new_loverlay->m_flEndTime = TICKS_TO_TIME(SDK::Vars::g_tick) + flDuration;
		return new_loverlay;
	}


	std::vector<OverlayBase_t*> g_Impact_data;
	void DrawOverlay(OverlayBase_t * pOverlay)
	{
		using t = void(__cdecl*)(OverlayBase_t *);
		static t fn = NULL;
		if (!fn) {
			fn = (t)Core::Memory::FindPattern((PBYTE)SDK::Engine.GetStartAddr(), SDK::Engine.GetSize(),
				(const char *)"\x55\x8B\xEC\x83\xEC\x14\xCC\xCC\x8B\xCC\xCC\xFF\xCC\xCC\xCC\xCC\xCC\x8B",//xxxxxxxxxx
				0xCC);
		}

		
		return fn(pOverlay);
	}
	void PrintfAllNetworkedEffects()
	{
		auto s_head = (CClientEffectRegistration**)(SDK::Client.GetStartAddr() + 0x72A180);
		for (CClientEffectRegistration *pReg = *s_head; pReg; pReg = pReg->m_pNext)
		{
			char buff_[260];
			sprintf_s(buff_, "echo %s", pReg->m_pEffectName);
			g_pEngine->ClientCmd(buff_);
		}
	}

	ClientEffectCallback Hook(ClientEffectCallback func, const char * effect_name)
	{
		ClientEffectCallback original = nullptr;
		//\x8B\xCC\xCC\xCC\xCC\xCC\x85\xCC\x74\xCC\x8D\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xFF\xCC\xE8
		static CClientEffectRegistration** s_head = NULL;
		if (!s_head) {
			uint32_t temp = (uint32_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
				(const char *)"\x8B\xCC\xCC\xCC\xCC\xCC\x85\xCC\x74\xCC\x8D\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xFF\xCC\xE8",//xxxxxxxxxx
				0xCC);
			temp += 0x2;
			s_head = *(CClientEffectRegistration***)temp;
		}
		//auto s_head = (CClientEffectRegistration**)(SDK::Client.GetStartAddr() + 0x72A180);//DispatchEffectToCallback
		for (CClientEffectRegistration *pReg = *s_head; pReg; pReg = pReg->m_pNext)
		{
			if (pReg->m_pEffectName && strcmp(pReg->m_pEffectName, effect_name) == 0) {
				original = pReg->m_pFunction;
				pReg->m_pFunction = func;
			}
		}
		return original;
	}
	ClientEffectCallback o_Impact;
	void INITIALIZE()
	{
		
		o_Impact = Hook(Impact, "Impact");

	}
	C_BaseEntity* GetBaseEntityFromHandle(CBaseHandle hEnt)
	{
		IClientUnknown* pEnt = (IClientUnknown*)g_pEntList->GetClientUnknownFromHandle((void*)hEnt);
		return pEnt ? pEnt->GetBaseEntity() : 0;
	}
	void Impact(const CEffectData &data)
	{

		Engine::WriteFile("%s ST", __FUNCTION__);


		auto pLocal = C_GMOD_Player::GetLocalPlayer();
		if (!pLocal || !pLocal->IsGood()) {
			Engine::WriteFile("%s END", __FUNCTION__);

			return o_Impact(data);
		}
			
		
		if (Engine::Var::Instance().Esp_Tracer.Get()) {
			auto vecShotDir = (data.m_vOrigin - data.m_vStart);
			vecShotDir.Normalize();
			//impact.Start = data.m_vStart + vecShotDir ;
			//impact.End = data.m_vOrigin;
			auto col = Color(Engine::Var::Instance().Tracer_Color.Get()).to_cColor();
			cColor curr2 = {};
			curr2.SetColor(col.b(), col.g(), col.r(), 255);
			g_Impact_data.push_back(AddLineOverlay(data.m_vStart, data.m_vOrigin, col.r(), col.g(), col.b(), 255, !Engine::Var::Instance().Only_Visible_Traces.Get(), Engine::Var::Instance().Tracer_holdtime.Get()));

			//impact.duration = TICKS_TO_TIME(SDK::Vars::g_tick) + Engine::Var::Instance().Tracer_holdtime.Get();
			//g_Impact_data.push_back(impact);
		}
		else {
			g_Impact_data.clear();
		}
		

		//g_pVDebugOverlay->AddLineOverlay(data.m_vStart, data.m_vOrigin, 0, 255, 0, false, Engine::Var::Instance().Tracer_holdtime.Get() + 10.f);

		/*if (data.m_nEntIndex) {
			auto ent = GetBaseEntityFromHandle(data.m_nEntIndex);
			//ent damaged not damager
			if (ent&&ent->IsValid()) {
				auto vector_less = [](const Vector& src, const Vector& v2) -> bool {
					return src.m_x < v2.m_x && src.m_y < v2.m_y&&src.m_z < v2.m_z;
				};

				C_GMOD_Player * damager = nullptr;
				for (int it = 0; it <= g_pEngine->GetMaxClients(); it++) {//it < g_pEntList->GetMaxEntities()
					auto ply = C_GMOD_Player::GetGPlayer(it);
					if (!ply || !ply->IsValid())
						continue;
					
					auto mins = (ply->OBBMins() * 1.5f) + ply->m_vecOrigin();
					auto maxs = (ply->OBBMaxs() * 1.5f) + ply->m_vecOrigin();
					maxs.m_z += ply->m_vecViewOffset();
					g_pClientState;
					if (vector_less(mins ,data.m_vStart) && vector_less(data.m_vStart, maxs))
						damager = ply;
				}


				auto CClientState__GetTime = [](CClientState* state)-> float {
					using t = float(__thiscall*)(void*);
					static t CClientState__GetTime_f = NULL;
					if (!CClientState__GetTime_f) {
						CClientState__GetTime_f = (t)Core::Memory::FindPattern((PBYTE)SDK::Engine.GetStartAddr(), SDK::Engine.GetSize(),
							(const char *)"\x55\x8B\xEC\x51\x80\xB9",//xx????xxxxx
							0xCC);
					}
					return CClientState__GetTime_f(state);
				};


				auto vecShotDir = (data.m_vOrigin - data.m_vStart);
				vecShotDir.Normalize();
				auto angle_shot = vecShotDir.ToEulerAngles();
				angle_shot.Normalize();
				if (damager && damager != pLocal) {
					g_pEngine->Con_NPrintf(5, "Set ang");
					damager->m_angEyeAngles() = angle_shot;
					/*if (g_ServerAnimations.g_Tracker[damager->entindex()].m_PlayerAnimState) {
						//g_ServerAnimations.g_Tracker[damager->entindex()].m_PlayerAnimState->m_bForceAimYaw = true;
						g_ServerAnimations.g_Tracker[damager->entindex()].m_PlayerAnimState->Update(angle_shot[YAW], angle_shot[PITCH]);
						//g_ServerAnimations.g_Tracker[damager->entindex()].m_PlayerAnimState->m_bForceAimYaw = false;
					}
					g_ServerAnimations.ProcessPlayer(damager);
					g_pEngine->Con_NPrintf(6, "time %f", TICKS_TO_TIME(g_pGlobalVars->tickcount));
					auto tick = g_LagCompensation.build_tick(damager);
					tick.m_flSimulationTime = TICKS_TO_TIME(g_pGlobalVars->tickcount);
					tick.m_priority = tick_trusted;
					//tick.m_flSimulationTime = SDK::Vars::g_tick * g_pGlobalVars->interval_per_tick - SDK::GetLerpTime();
					g_LagCompensation.push_tick(damager, tick);
				}
			}

		}*/
		Engine::WriteFile("%s END", __FUNCTION__);
		return o_Impact(data);
	}
}

