#pragma once
using SendNetMsg_td = bool(__fastcall*)(CNetChan*, void*, INetMessage &, bool, bool);
extern SendNetMsg_td o_SendNetMsg;
bool __fastcall hkSendNetMsg(CNetChan* netchan, void*edx, INetMessage &msg, bool bForceReliable , bool bVoice );
extern int tr_outsq_num;
