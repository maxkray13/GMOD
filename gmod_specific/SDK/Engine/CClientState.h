#pragma once

class CClientState {
public:
	char _0x0000[304];
	__int32 m_nSignonState; //0x0130 
	char _0x0134[4];
	double m_flNextCmdTime; //0x0138 
	__int32 m_nServerCount; //0x0140 
	char _0x0144[88];
	__int32 m_nClientTick; //0x019C 
	__int32 m_nDeltaTick; //0x01A0 
	char _0x01A4[67792];
	__int32 lastoutgoingcommand; //0x10A74 
	__int32 chokedcommands; //0x10A78 
	__int32 last_command_ack; //0x10A7C
	char _0x10A7C[10692];

};