#include "../../SDK.h"
#include "SendDatagram.h"
#include "SendNetMsg.h"
uint32_t o_SendDatagram;


int __fastcall hkSendDatagram(CNetChan * netchan, void * edx, bf_write * datagram)
{
	int ret = 0;


	

	//if (GetAsyncKeyState('V') & 0x8000)
	//	netchan->m_nChokedPackets = 15;
	//	netchan->m_nOutSequenceNr += 1;
	{
		ret = ((SendDatagram_fn)o_SendDatagram)(netchan, datagram);// Hooks::VNetChan.GetOriginalMethod<SendDatagram_fn>(46)(netchan, datagram);
	}
	/*
	if (GetAsyncKeyState(0x56)) {

		auto host_frametime_unbounded = (float*)(SDK::Engine.GetStartAddr() + 0x6B4B58);
		auto host_frametime_stddeviation = (float*)(SDK::Engine.GetStartAddr() + 0x6B4B5C);
		auto save_outsq = netchan->m_nOutSequenceNr;

		//netchan->m_nOutSequenceNr += 10;
		for(auto it = 0 ; it<10;it++) {
			auto nextcommand_num = g_pClientState->lastoutgoingcommand + it + 2;//+it

			g_pClient->CreateMove(nextcommand_num, g_pGlobalVars->interval_per_tick, true);
			CL_SendMove();

			NET_Tick mymsg = NET_Tick(g_pClientState->m_nDeltaTick, *host_frametime_unbounded, *host_frametime_stddeviation);//g_pClientState->m_nDeltaTick

			netchan->SendNetMsg(mymsg);
			ret = ((SendDatagram_fn)o_SendDatagram)(netchan, datagram);
		//	netchan->m_nOutSequenceNr = save_outsq;
		//	netchan->m_nOutSequenceNr += 10;
		}
		//netchan->m_nOutSequenceNr = save_outsq;
	}
	*/
	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (g_pClientState->m_nSignonState == SIGNONSTATE_FULL && pLocal&&pLocal->IsGood() && g_pEngine->IsInGame() && !g_pEngine->IsDrawingLoadingImage()) {
		//g_pPrediction->Update(g_pClientState->m_nDeltaTick, g_pClientState->m_nDeltaTick > 0, g_pClientState->last_command_ack, ret + g_pClientState->chokedcommands);
		/*
		int pc = g_pPredictableList->GetPredictableCount();
		int p;
		for (p = 0; p < pc; p++)
		{
			auto ent = g_pPredictableList->GetPredictable(p);
			if (!ent)
				continue;
			if (ent->GetPredictable())
			{
				ent->RestoreData("RestoreOriginalEntityState", -1, PC_EVERYTHING);
			}
		}
		CGlobalVars saveVars = *g_pGlobalVars;

		SDK::Function::PerformPrediction(g_pPrediction, true, pLocal, g_pClientState->last_command_ack, ret + g_pClientState->chokedcommands);
		*g_pGlobalVars = saveVars;*/
		//SDK::Function::PerformPrediction(g_pPrediction, true, pLocal, g_pClientState->last_command_ack, ret + g_pClientState->chokedcommands);
	//	g_pPrediction->Update(g_pClientState->m_nDeltaTick, g_pClientState->m_nDeltaTick > 0, g_pClientState->last_command_ack, ret + g_pClientState->chokedcommands);
	}
	return ret;
	//return tr_outsq_num;

}
