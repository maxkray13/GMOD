#include "../../SDK.h"
#include "SendNetMsg.h"
#include "SendDatagram.h"
SendNetMsg_td o_SendNetMsg;
int tr_outsq_num = -1;

CUserCmd* GetUserCmd_nocheck(int sequence_number) {
	return (CUserCmd *)(*(DWORD *)(DWORD(g_pInput) + 0xC4) + 324 * (sequence_number % 90));
}
CUserCmd* GetUserCmd(int sequence_number)
{
	CUserCmd *usercmd = GetUserCmd_nocheck(sequence_number);
	if (usercmd->command_number != sequence_number)
	{
		// usercmd was overwritten by newer command
		int last_tick_count = -1;
		for (auto it = 0; it < 90; it++) {
			auto t = GetUserCmd_nocheck(it);
			if (t->tick_count > last_tick_count)
				last_tick_count = t->tick_count;
		}
		usercmd->command_number = sequence_number;
		usercmd->tick_count = last_tick_count + 1;
	}
	return usercmd;
}

void WriteUsercmd(bf_write *buf, const CUserCmd *to, const CUserCmd *from)
{
	using td = void(__cdecl*)(bf_write *, const CUserCmd *, const CUserCmd *);
	static td func = NULL;
	if (!func) {
		func = (td)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),// 55 8B EC 83 EC 18 8B ? ? ? 8B ? ? ? 8B ? ? 8B
			(const char *)"\x55\x8B\xEC\x83\xEC\x18\x8B\xCC\xCC\xCC\x8B\xCC\xCC\xCC\x8B\xCC\xCC\x8B",
			0xCC);
	}

	return func(buf, to, from);
}

bool WriteUsercmdDeltaToBuffer(bf_write *buf, int from, int to, int nextcommandnumber, bool isnewcommand)
{
	CUserCmd nullcmd = {};

	CUserCmd *f, *t;

	int startbit = buf->GetNumBitsWritten();

	if (from == -1)
	{
		f = &nullcmd;
	}
	else
	{
		f = GetUserCmd(from);

		if (!f)
		{
			f = &nullcmd;
		}

	}
	//f = &nullcmd;
	t = GetUserCmd(to);

	if (!t)
	{
		t = &nullcmd;
	}
	//t->command_number = INT_MAX;
	/*if ( to < nextcommandnumber) {
		auto c = *GetUserCmd(nextcommandnumber);
		c.tick_count = t->tick_count;
		c.command_number = t->command_number;
		WriteUsercmd(buf, &c, f);
		return true;
	}*/

	WriteUsercmd(buf, t, f);
	return true;
}

void CL_SendMove() {
	byte data[MAX_CMD_BUFFER];

	int nextcommandnr = g_pClientState->lastoutgoingcommand + g_pClientState->chokedcommands + 1;

	// send the client update packet

	CLC_Move moveMsg;

	moveMsg.m_DataOut.StartWriting(data, sizeof(data));

	// Determine number of backup commands to send along
	int cl_cmdbackup = 2;
	moveMsg.m_nBackupCommands = clamp(cl_cmdbackup, 0, MAX_BACKUP_COMMANDS);

	// How many real new commands have queued up
	moveMsg.m_nNewCommands = 1 + g_pClientState->chokedcommands;
	moveMsg.m_nNewCommands = clamp(moveMsg.m_nNewCommands, 0, MAX_NEW_COMMANDS);


	int numcmds = moveMsg.m_nNewCommands + moveMsg.m_nBackupCommands;

	int from = -1;	// first command is deltaed against zeros 

	bool bOK = true;

	for (int to = nextcommandnr - numcmds + 1; to <= nextcommandnr; to++)
	{
		bool isnewcmd = to >= (nextcommandnr - moveMsg.m_nNewCommands + 1);

		// first valid command number is 1
		bOK = bOK && WriteUsercmdDeltaToBuffer(&moveMsg.m_DataOut, from, to, nextcommandnr, isnewcmd);//23
		from = to;
	}

	if (bOK)
	{
		moveMsg.m_nLength = moveMsg.m_DataOut.GetNumBitsWritten();
		// only write message if all usercmds were written correctly, otherwise parsing would fail

		o_SendNetMsg(g_pEngine->GetNetChannelInfo(), NULL, moveMsg, false, false);
	}

};

//return numbers of suppressed commands
int CL_SendMove_test(int offset) {

	byte data[MAX_CMD_BUFFER];
	CLC_Move moveMsg;

	moveMsg.m_DataOut.StartWriting(data, sizeof(data));

	int cl_cmdbackup = 2;
	moveMsg.m_nBackupCommands = clamp(cl_cmdbackup, 0, MAX_BACKUP_COMMANDS);

	moveMsg.m_nNewCommands = 1 + g_pClientState->chokedcommands;
	moveMsg.m_nNewCommands = clamp(moveMsg.m_nNewCommands, 0, MAX_NEW_COMMANDS);

	int numcmds = moveMsg.m_nNewCommands + moveMsg.m_nBackupCommands;

	int from = -1;
	bool bOK = true;

	int nextcommandnr_ideal = g_pClientState->lastoutgoingcommand + g_pClientState->chokedcommands + 1;
	int rest = g_pClientState->chokedcommands + 1 - ( moveMsg.m_nNewCommands + offset);
	
	int nextcommandnr = g_pClientState->lastoutgoingcommand + moveMsg.m_nNewCommands + offset;
	for (int to = nextcommandnr - numcmds + 1; to <= nextcommandnr; to++)
	{
		bool isnewcmd = to >= (nextcommandnr - moveMsg.m_nNewCommands + 1);

		bOK = bOK && WriteUsercmdDeltaToBuffer(&moveMsg.m_DataOut, from, to, nextcommandnr, isnewcmd);//23
		from = to;
	}

	if (bOK)
	{
		moveMsg.m_nLength = moveMsg.m_DataOut.GetNumBitsWritten();
		o_SendNetMsg(g_pEngine->GetNetChannelInfo(), NULL, moveMsg, false, false);
	}
	return rest;
};
void CL_SendMove_backup_move(void)
{

	byte data[MAX_CMD_BUFFER];

	int nextcommandnr = g_pClientState->lastoutgoingcommand + g_pClientState->chokedcommands + 1;

	// send the client update packet

	CLC_Move moveMsg;

	moveMsg.m_DataOut.StartWriting(data, sizeof(data));

	// Determine number of backup commands to send along
	int cl_cmdbackup = 2;//2;
	moveMsg.m_nBackupCommands = clamp(cl_cmdbackup, 0, MAX_BACKUP_COMMANDS);

	// How many real new commands have queued up
	moveMsg.m_nNewCommands = 0;
	moveMsg.m_nNewCommands = clamp(moveMsg.m_nNewCommands, 0, MAX_NEW_COMMANDS);

	int numcmds = moveMsg.m_nNewCommands + moveMsg.m_nBackupCommands;

	int from = -1;	// first command is deltaed against zeros 

	bool bOK = true;



	for (int to = nextcommandnr - numcmds + 1; to <= nextcommandnr; to++)
	{
		bool isnewcmd = to >= (nextcommandnr - moveMsg.m_nNewCommands + 1);

		// first valid command number is 1
		bOK = bOK && WriteUsercmdDeltaToBuffer(&moveMsg.m_DataOut, from, to, nextcommandnr, isnewcmd);//23
		from = to;
	}

	if (bOK)
	{
		moveMsg.m_nLength = moveMsg.m_DataOut.GetNumBitsWritten();
		// only write message if all usercmds were written correctly, otherwise parsing would fail

		o_SendNetMsg(g_pEngine->GetNetChannelInfo(), NULL, moveMsg, false, false);
		//g_pEngine->GetNetChannelInfo()->SendNetMsg(moveMsg);
	}
}
#define CMD_MAXBACKUP 64
void CL_SendMove_max_move(void)
{

	byte data[MAX_CMD_BUFFER];

	int nextcommandnr = g_pClientState->lastoutgoingcommand + g_pClientState->chokedcommands + 1;

	// send the client update packet

	CLC_Move moveMsg;

	moveMsg.m_DataOut.StartWriting(data, sizeof(data));

	// Determine number of backup commands to send along
	int cl_cmdbackup = 0;//2;
	moveMsg.m_nBackupCommands = clamp(cl_cmdbackup, 0, MAX_BACKUP_COMMANDS);

	// How many real new commands have queued up
	moveMsg.m_nNewCommands = MAX_NEW_COMMANDS; //1 + g_pClientState->chokedcommands;
	moveMsg.m_nNewCommands = clamp(moveMsg.m_nNewCommands, 0, MAX_NEW_COMMANDS);

	int numcmds = moveMsg.m_nNewCommands + moveMsg.m_nBackupCommands;

	int from = -1;	// first command is deltaed against zeros 

	bool bOK = true;



	for (int to = nextcommandnr - numcmds + 1; to <= nextcommandnr; to++)
	{
		bool isnewcmd = to >= (nextcommandnr - moveMsg.m_nNewCommands + 1);

		// first valid command number is 1
		bOK = bOK && WriteUsercmdDeltaToBuffer(&moveMsg.m_DataOut, from, to, nextcommandnr, isnewcmd);//23
		from = to;
	}

	if (bOK)
	{
		moveMsg.m_nLength = moveMsg.m_DataOut.GetNumBitsWritten();
		// only write message if all usercmds were written correctly, otherwise parsing would fail

		o_SendNetMsg(g_pEngine->GetNetChannelInfo(), NULL, moveMsg, false, false);
		//g_pEngine->GetNetChannelInfo()->SendNetMsg(moveMsg);
	}
}
void cl_send_nullmove() {
	byte data[MAX_CMD_BUFFER];
	CLC_Move moveMsg;
	moveMsg.m_DataOut.StartWriting(data, sizeof(data));

	moveMsg.m_nBackupCommands = 0;
	moveMsg.m_nNewCommands = 0;
	moveMsg.m_nLength = moveMsg.m_DataOut.GetNumBitsWritten();

	o_SendNetMsg(g_pEngine->GetNetChannelInfo(), NULL, moveMsg, false, false);
}
void CL_SendMeme(void)
{

	byte data[MAX_CMD_BUFFER];

	int nextcommandnr = g_pClientState->lastoutgoingcommand + 1;

	// send the client update packet

	CLC_Move moveMsg;

	moveMsg.m_DataOut.StartWriting(data, sizeof(data));

	// Determine number of backup commands to send along
	int cl_cmdbackup = 0;//2;
	moveMsg.m_nBackupCommands = clamp(cl_cmdbackup, 0, MAX_BACKUP_COMMANDS);

	// How many real new commands have queued up
	moveMsg.m_nNewCommands = 2; //1 + g_pClientState->chokedcommands;
	moveMsg.m_nNewCommands = clamp(moveMsg.m_nNewCommands, 0, MAX_NEW_COMMANDS);

	int numcmds = moveMsg.m_nNewCommands + moveMsg.m_nBackupCommands;

	int from = -1;	// first command is deltaed against zeros 

	bool bOK = true;



	for (int to = nextcommandnr - numcmds + 1; to <= nextcommandnr; to++)
	{
		bool isnewcmd = to >= (nextcommandnr - moveMsg.m_nNewCommands + 1);

		// first valid command number is 1
		bOK = bOK && WriteUsercmdDeltaToBuffer(&moveMsg.m_DataOut, from, to, nextcommandnr, isnewcmd);//23
		from = to;
	}

	if (bOK)
	{
		moveMsg.m_nLength = moveMsg.m_DataOut.GetNumBitsWritten();
		// only write message if all usercmds were written correctly, otherwise parsing would fail

		o_SendNetMsg(g_pEngine->GetNetChannelInfo(), NULL, moveMsg, false, false);
		//g_pEngine->GetNetChannelInfo()->SendNetMsg(moveMsg);
	}
}


/*
float * phost_frametime_unbounded = (float *)(SDK::Engine.GetStartAddr() + 0x69AB84);
float * phost_frametime_stddeviation = (float *)(SDK::Engine.GetStartAddr() + 0x69AB88);
*/
bool __fastcall hkSendNetMsg(CNetChan* netchan, void*edx, INetMessage &msg, bool bForceReliable, bool bVoice)
{
	if (tr_outsq_num == -1)
	{
		tr_outsq_num = netchan->m_nOutSequenceNr;
	}
	if (strcmp("clc_Move", msg.GetName()) == 0) {
		auto move = (CLC_Move*)&msg;
		tr_outsq_num += move->m_nNewCommands;
		/*
		if (GetAsyncKeyState('H')&0x8000) {
			CL_SendMove_backup_move();
			g_pClientState->chokedcommands = 0;
			netchan->m_nChokedPackets = 0;
		}
		else {
			CL_SendMove();
			
		}*/
		
		if (Engine::Var::Instance().Slow_Motion.Get()) {
			static int SlowMoCount = 0;
			if (GetAsyncKeyState(Engine::Var::Instance().Slow_Motion_Key.Get()) ) {
				if (SlowMoCount < TIME_TO_TICKS(Engine::Var::Instance().Slow_Motion_Scale.Get())) {
					netchan->m_nOutSequenceNr += TIME_TO_TICKS(0.9);
					//CL_SendMove();
					CL_SendMove_backup_move();
					SlowMoCount++;
				}
				else {
				//	netchan->m_nOutSequenceNr += TIME_TO_TICKS(0.9);
					SlowMoCount = 0;
					CL_SendMove();
				}
			}
			else {
				SlowMoCount = 0;
				CL_SendMove();
			}

		}
		else {
			CL_SendMove();
			/*int offset = 0;
			offset = CL_SendMove_test(0);
			if (offset) {
				int save_outsq = netchan->m_nOutSequenceNrAck;
				while (offset) {
					netchan->SendDatagram(NULL);
					offset = CL_SendMove_test(offset);
				}
				netchan->m_nOutSequenceNrAck = save_outsq;
			}
			g_pEngine->Con_NPrintf(1, "chokedcommands %d", g_pClientState->chokedcommands);
			g_pEngine->Con_NPrintf(0, "suppressed %d", offset);*/
		}
		
		return true;
	}

	return o_SendNetMsg(netchan, edx, msg, bForceReliable, bVoice);
}
