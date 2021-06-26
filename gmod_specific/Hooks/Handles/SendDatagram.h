#pragma once

extern uint32_t o_SendDatagram;

using SendDatagram_fn = int(__thiscall *)(CNetChan*, bf_write *);

void CL_SendMove(void);

int __fastcall hkSendDatagram(CNetChan* netchan, void*edx, bf_write *datagram);