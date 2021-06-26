#include "../../SDK.h"
#include "PostDataUpdate.h"
#include "../../Functional/LagCompensation.h"
#include "../../Functional/ServerAnimations.h"
//player_map recived_data;
PostDataUpdate_fn oPostDataUpdate_;
//std::vector<player_edit> player_edited;


void __fastcall hkPostDataUpdate(IClientNetworkable * ecx, void * edx, DataUpdateType_t updateType)
{
	auto ent = (IClientEntity*)(DWORD(ecx) - 0x8);

	if (ecx && ent && ent->IsValid() && ent->GetBaseEntity() && ToGMOD_Player(ToBasePlayer((C_BaseEntity*)ent)))
	{
		auto ply = ToGMOD_Player(ToBasePlayer((C_BaseEntity*)ent));
		auto pLocal = C_GMOD_Player::GetLocalPlayer();
		if ((pLocal || pLocal->IsGood()) && pLocal != ply) {

			//ply->m_bClientSideAnimation() = false;
			((PostDataUpdate_fn)oPostDataUpdate_)(ecx, edx, updateType);
			if (Engine::Var::Instance().Aim_LagCompensation.Get()) {
				auto recived_angle = ply->m_angEyeAngles();
				recived_angle.Normalize();
				tick rec_tick = {};
				if (g_LagCompensation.get_last_tick(ply, rec_tick)) {

					bool bChocked = TIME_TO_TICKS(ply->m_flSimulationTime() - rec_tick.m_flSimulationTime) > 1;
					if (bChocked) {
						auto randval = (int)SDK::Function::RandomFloat(0.f, 9.5f);
						switch (randval)
						{
						case 0:
							recived_angle[YAW] = 0.f;
							break;
						case 1:
							recived_angle[YAW] = 35.f;
							break;
						case 2:
							recived_angle[YAW] = 70.f;
							break;
						case 3:
							recived_angle[YAW] = 105.f;
							break;
						case 4:
							recived_angle[YAW] = 140.f;
							break;
						case 5:
							recived_angle[YAW] = 175.f;
							break;
						case 6:
							recived_angle[YAW] = 210.f;
							break;
						case 7:
							recived_angle[YAW] = 245.f;
							break;
						case 8:
							recived_angle[YAW] = 280.f;
							break;
						case 9:
							recived_angle[YAW] = 315.f;
							break;
						default:
							break;
						}
						recived_angle.Normalize();
						ply->m_angEyeAngles() = recived_angle;
						g_ServerAnimations.ProcessPlayer(ply);
					}
				}
				g_LagCompensation.PostDataUpdate(ply);
			}
		}
		else {

			((PostDataUpdate_fn)oPostDataUpdate_)(ecx, edx, updateType);

		}
	}
	else {

		((PostDataUpdate_fn)oPostDataUpdate_)(ecx, edx, updateType);

	}
}
