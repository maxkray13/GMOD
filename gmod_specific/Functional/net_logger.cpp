#include "../SDK.h"
#include "net_logger.h"

namespace net_logger {


	bool						m_bStartSending = false;
	int							m_iPacketID = 0;
	bool						m_bSendOnce = false;
	bool						m_bUpdateMsg = true;
	std::vector <Log> g_Log;

	char * g_Write = nullptr;
	bf_write* net_buff = nullptr;
	bool * g_Started = nullptr;
	bool * g_Reliable = nullptr;

	using network_string_get_td = int(__cdecl*)(const char*);
	network_string_get_td network_string_get;
	using get_vector_td = Vector * (__cdecl*)(int);
	using get_angle_td = QAngle * (__cdecl*)(int);
	using get_vmatrix_td = VMatrix * (__cdecl*)(int);
	get_vector_td Get_Vector;
	get_angle_td Get_Angle;
	get_vmatrix_td Get_VMatrix;
	lua_func o_SendToServer;

	
	Log curr_Log;
	Log* pCurrlog = &curr_Log;



	void ClearLog(Log * pLog)
	{
		
		if (pLog == pCurrlog)
			return;
		if (pLog->packet_id == m_iPacketID) {
			m_bStartSending = false;
			m_iPacketID = 0;
		}
		for (auto it : pLog->args) {
			if (it) {
				free(it);
			}
		}
		pLog->args.clear();

	//	free(pLog);
	}

	void ClearGLog()
	{
		for (auto& it : g_Log) {
			ClearLog(&it);
		}
		g_Log.clear();
		m_bStartSending = false;
		m_iPacketID = 0;
		m_bUpdateMsg = true;
	}

	bool PacketLogged(int id)
	{
		for (auto& it : g_Log) {
			if (it.packet_id == id) {
				if (!it.force_update) {
					return true;
				}
				else {
					pCurrlog = &it;
				}
			}
		}
		return false;
	}

	Log * TargetPacket(int id)
	{
		for (auto& it : g_Log) {
			if (it.packet_id == id)
				return &it;
		}
		return nullptr;
	}

	void ProcessSend()
	{
		if (!m_bSendOnce) {
			if (!Engine::Var::Instance().NSend.Get() || m_iPacketID == 0)
				return;
			if (Engine::Var::Instance().NSend_Key.Get() && !(GetAsyncKeyState(Engine::Var::Instance().NSend_Key.Get()) & 0x8000))
				return;
		}
		
		auto nci = g_pEngine->GetNetChannelInfo();
		if (!nci)
			return;

		if (m_iPacketID == 0 && g_Log.size()) {
			m_iPacketID = g_Log[0].packet_id;
		}
		auto pLog = TargetPacket(m_iPacketID);
		if (!pLog) {

			m_bSendOnce = false;
			return;
		}


		static char m_buff[0x10000];
		static bf_write write;
		if (m_bUpdateMsg) {
			write = {};
			write.StartWriting(m_buff, 0x10000 , 0);
			write.WriteByte(0);
			write.WriteWord(m_iPacketID);


			for (auto it : pLog->args) {
				if (it->type == aFLOAT) {
					auto curr = (Argument<float>*)it;
					write.WriteBits(&curr->val, 32);
				}
				else if (it->type == aDOUBLE) {
					auto curr = (Argument<double>*)it;
					write.WriteBits(&curr->val, 64);
				}
				else if (it->type == aBIT) {
					auto curr = (Argument<char>*)it;
					write.WriteUBitLong(curr->val, 1);
				}
				else if (it->type == aSTRING) {
					auto curr = (Argument<std::string>*)it;
					write.WriteString(curr->val.data());
				}
				else if (it->type == aDATA) {
					auto curr = (Argument<std::string>*)it;
					write.WriteBytes(curr->val.data(), curr->bit_size);
				}
				else if (it->type == aVECTOR) {
					auto curr = (Argument<Vector>*)it;
					write.WriteBitVec3Coord(curr->val);
				}
				else if (it->type == aNORMAL) {
					auto curr = (Argument<Vector>*)it;
					write.WriteBitVec3Normal(curr->val);
				}
				else if (it->type == aANGLE) {
					auto curr = (Argument<QAngle>*)it;
					write.WriteBitAngles(curr->val);
				}
				else if (it->type == aMATRIX) {
					auto curr = (Argument<VMatrix>*)it;
					auto v0 = &curr->val;
					write.WriteFloat(*(float *)v0);
					write.WriteFloat(*(float *)(DWORD(v0) + 4));
					write.WriteFloat(*(float *)(DWORD(v0) + 8));
					write.WriteFloat(*(float *)(DWORD(v0) + 12));
					write.WriteFloat(*(float *)(DWORD(v0) + 16));
					write.WriteFloat(*(float *)(DWORD(v0) + 20));
					write.WriteFloat(*(float *)(DWORD(v0) + 24));
					write.WriteFloat(*(float *)(DWORD(v0) + 28));
					write.WriteFloat(*(float *)(DWORD(v0) + 32));
					write.WriteFloat(*(float *)(DWORD(v0) + 36));
					write.WriteFloat(*(float *)(DWORD(v0) + 40));
					write.WriteFloat(*(float *)(DWORD(v0) + 44));
					write.WriteFloat(*(float *)(DWORD(v0) + 48));
					write.WriteFloat(*(float *)(DWORD(v0) + 52));
					write.WriteFloat(*(float *)(DWORD(v0) + 56));
					write.WriteFloat(*(float *)(DWORD(v0) + 60));
				}
				else if (it->type == aINT) {
					auto curr = (Argument<int>*)it;
					write.WriteSBitLong(curr->val, curr->bit_size);
				}
				else if (it->type == aUINT) {
					auto curr = (Argument<UINT>*)it;
					write.WriteUBitLong(curr->val, curr->bit_size);
				}
			}
			m_bUpdateMsg = false;
		}

		if (m_bSendOnce) {
			g_pEngine->GMOD_SendToServer((void*)write.m_pData, write.m_iCurBit, true);

			m_iPacketID = 0;
			m_bSendOnce = false;
			return;
		}
		if (Engine::Var::Instance().NSend_Key.Get() == 1) {
			g_pEngine->GMOD_SendToServer((void*)write.m_pData, write.m_iCurBit, true);
		}
		else {
			for (auto it = 0; it < Engine::Var::Instance().NSend_Key.Get(); it++) {
				g_pEngine->GMOD_SendToServer((void*)write.m_pData, write.m_iCurBit, true);
				nci->Transmit();
				//nci->SendDatagram(&write);
			}
		}
		
		
	}

	int __cdecl hkNetStart(lua_State* a1) {


		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface && *g_Started == false) {
			a1->m_interface->SetState(a1);
			auto str = g_pLuaShitInterface->CheckString(1);
			auto id = network_string_get(str);
			if (id > 0) {
				net_buff->StartWriting(g_Write, 0x10000, 0);
				net_buff->WriteByte(0);
				net_buff->WriteWord(id);
				if (!PacketLogged(id)) {
					if (pCurrlog != &curr_Log) {
						for (auto it : pCurrlog->args) {
							if (it) {
								free(it);
							}
						}
						pCurrlog->args.clear();
					}
					pCurrlog->packet_name = str;
					pCurrlog->packet_id = id;
				}
				else {
					pCurrlog->packet_name = str;
					pCurrlog->packet_id = NULL;
				}
				*g_Started = true;
				*g_Reliable = g_pLuaShitInterface->GetBool(2) == 0;
				pCurrlog->reliable = *g_Reliable;

				g_pLuaShitInterface->PushBool(true);
				return 1;
			}
		}
		return 0;
	}


	int __cdecl hkWriteFloat(lua_State* a1) {

		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			a1->m_interface->SetState(a1);
			if (*g_Started) {
				auto val = g_pLuaShitInterface->CheckNumber(1);
				if (pCurrlog->packet_id) {
					Argument<float>* t_arg = (Argument<float>*)malloc(sizeof(Argument<float>));
					t_arg->type = aFLOAT;
					t_arg->min = (long long) FLT_MIN;
					t_arg->max = (long long)FLT_MAX;
					t_arg->bit_size = 32;
					t_arg->val = val;
					pCurrlog->args.push_back((unk_Argument*)t_arg);
				}
				net_buff->WriteBits(&val, 32);
			}
		}
		return 0;
	}

	int __cdecl hkWriteDouble(lua_State* a1) {
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			a1->m_interface->SetState(a1);
			if (*g_Started) {
				auto val = g_pLuaShitInterface->CheckNumber(1);
				if (pCurrlog->packet_id) {
					Argument<double>* t_arg = (Argument<double>*)malloc(sizeof(Argument<double>));
					t_arg->type = aDOUBLE;
					t_arg->min = (long long)DBL_MIN;
					t_arg->max = (long long)DBL_MAX;
					t_arg->bit_size = 64;
					t_arg->val = val;
					pCurrlog->args.push_back((unk_Argument*)t_arg);

				}
				net_buff->WriteBits(&val, 64);
			}
		}
		return 0;
	}

	int __cdecl hkWriteBit(lua_State* a1) {
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			a1->m_interface->SetState(a1);
			if (*g_Started) {
				char bit;
				if (g_pLuaShitInterface->GetType(1) == Type::NUMBER) {
					bit = g_pLuaShitInterface->GetNumber(1);
				}
				else {
					bit = g_pLuaShitInterface->GetBool(1);
				}
				if (pCurrlog->packet_id) {
					Argument<char>* t_arg = (Argument<char>*)malloc(sizeof(Argument<char>));
					t_arg->type = aBIT;
					t_arg->min = -128;
					t_arg->max = 127;
					t_arg->bit_size = 1;
					t_arg->val = bit;
					pCurrlog->args.push_back((unk_Argument*)t_arg);
				}
				net_buff->WriteUBitLong(bit, 1);
			}
		}
		return 0;
	}
	int __cdecl hkWriteString(lua_State* a1) {
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			a1->m_interface->SetState(a1);
			if (*g_Started) {
				auto str = g_pLuaShitInterface->CheckString(1);
				if (pCurrlog->packet_id) {
					Argument<std::string>* t_arg = (Argument<std::string>*)malloc(sizeof(Argument<std::string>));
					ZeroMemory(t_arg, sizeof(Argument<std::string>));
					t_arg->type = aSTRING;
					t_arg->val = std::string();
					t_arg->val = str;
					pCurrlog->args.push_back((unk_Argument*)t_arg);
				}

				net_buff->WriteString(str);

			}
		}
		return 0;
	}

	int __cdecl hkWriteData(lua_State* a1) {

		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			a1->m_interface->SetState(a1);
			if (*g_Started) {
				auto numb = (int)g_pLuaShitInterface->CheckNumber(2);
				if (numb) {
					auto data = g_pLuaShitInterface->CheckString(1);
					if (pCurrlog->packet_id) {
						Argument<std::string>* t_arg = (Argument<std::string>*)malloc(sizeof(Argument<std::string>));
						ZeroMemory(t_arg, sizeof(Argument<std::string>));
						t_arg->type = aDATA;
						t_arg->bit_size = numb;
						t_arg->val = std::string();
						t_arg->val = std::string(data, numb);
						pCurrlog->args.push_back((unk_Argument*)t_arg);
					}
					net_buff->WriteBytes(data, numb);
				}

			}
		}
		return 0;
	}



	int __cdecl hkWriteVector(lua_State* a1) {
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			a1->m_interface->SetState(a1);
			if (*g_Started) {
				auto p = Get_Vector(1);
				if (pCurrlog->packet_id) {
					Argument<Vector>* t_arg = (Argument<Vector>*)malloc(sizeof(Argument<Vector>));
					t_arg->type = aVECTOR;
					t_arg->val = *p;
					pCurrlog->args.push_back((unk_Argument*)t_arg);
				}
				net_buff->WriteBitVec3Coord(*p);
			}
		}
		return 0;
	}
	int __cdecl hkWriteNormal(lua_State* a1) {
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			a1->m_interface->SetState(a1);
			if (*g_Started) {
				auto p = Get_Vector(1);
				if (pCurrlog->packet_id) {
					Argument<Vector>* t_arg = (Argument<Vector>*)malloc(sizeof(Argument<Vector>));
					t_arg->type = aNORMAL;
					t_arg->val = *p;
					pCurrlog->args.push_back((unk_Argument*)t_arg);
				}
				net_buff->WriteBitVec3Normal(*p);
			}
		}
		return 0;
	}

	int __cdecl hkWriteAngle(lua_State* a1) {
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			a1->m_interface->SetState(a1);
			if (*g_Started) {
				auto p = Get_Angle(1);
				if (pCurrlog->packet_id) {
					Argument<QAngle>* t_arg = (Argument<QAngle>*)malloc(sizeof(Argument<QAngle>));
					t_arg->type = aANGLE;
					t_arg->val = *p;
					pCurrlog->args.push_back((unk_Argument*)t_arg);
				}
				net_buff->WriteBitAngles(*p);
			}
		}
		return 0;
	}
	int __cdecl hkWriteMatrix(lua_State* a1) {
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			a1->m_interface->SetState(a1);
			if (*g_Started) {
				
				auto v0 = Get_VMatrix(1);

				if (pCurrlog->packet_id) {
					Argument<VMatrix>* t_arg = (Argument<VMatrix>*)malloc(sizeof(Argument<VMatrix>));
					t_arg->type = aMATRIX;
					t_arg->val = *v0;
					pCurrlog->args.push_back((unk_Argument*)t_arg);
				}
				net_buff->WriteFloat(*(float *)v0);
				net_buff->WriteFloat(*(float *)(DWORD(v0) + 4));
				net_buff->WriteFloat(*(float *)(DWORD(v0) + 8));
				net_buff->WriteFloat(*(float *)(DWORD(v0) + 12));
				net_buff->WriteFloat(*(float *)(DWORD(v0) + 16));
				net_buff->WriteFloat(*(float *)(DWORD(v0) + 20));
				net_buff->WriteFloat(*(float *)(DWORD(v0) + 24));
				net_buff->WriteFloat(*(float *)(DWORD(v0) + 28));
				net_buff->WriteFloat(*(float *)(DWORD(v0) + 32));
				net_buff->WriteFloat(*(float *)(DWORD(v0) + 36));
				net_buff->WriteFloat(*(float *)(DWORD(v0) + 40));
				net_buff->WriteFloat(*(float *)(DWORD(v0) + 44));
				net_buff->WriteFloat(*(float *)(DWORD(v0) + 48));
				net_buff->WriteFloat(*(float *)(DWORD(v0) + 52));
				net_buff->WriteFloat(*(float *)(DWORD(v0) + 56));
				net_buff->WriteFloat(*(float *)(DWORD(v0) + 60));

			}
		}
		return 0;
	}

	int __cdecl hkWriteInt(lua_State* a1) {
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			a1->m_interface->SetState(a1);
			if (*g_Started) {
				int num_bits = g_pLuaShitInterface->CheckNumber(2);
				if (num_bits) {
					int data = g_pLuaShitInterface->CheckNumber(1);
					if (pCurrlog->packet_id) {
						Argument<int>* t_arg = (Argument<int>*)malloc(sizeof(Argument<int>));
						t_arg->type = aINT;
						t_arg->min = (long long)(1ll << (long long)(num_bits - 1ll));
						t_arg->max = (long long)(1ll << (long long)(num_bits - 1ll)) - 1ll;
						t_arg->bit_size = num_bits;
						t_arg->val = data;
						pCurrlog->args.push_back((unk_Argument*)t_arg);
					}
					net_buff->WriteSBitLong(data, num_bits);
				}
			}
		}
		return 0;
	}


	int __cdecl hkWriteUInt(lua_State* a1) {
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);
		if (g_pLuaShitInterface) {
			a1->m_interface->SetState(a1);
			if (*g_Started) {
				int num_bits = g_pLuaShitInterface->CheckNumber(2);
				if (num_bits) {
					unsigned int data = g_pLuaShitInterface->CheckNumber(1);
					if (pCurrlog->packet_id) {
						Argument<UINT>* t_arg = (Argument<UINT>*)malloc(sizeof(Argument<UINT>));
						t_arg->type = aUINT;
						t_arg->min = 0;
						t_arg->max = (long long)(1ll << (long long)num_bits) - 1ll;
						t_arg->bit_size = num_bits;
						t_arg->val = data;
						pCurrlog->args.push_back((unk_Argument*)t_arg);
					}
					net_buff->WriteUBitLong(data, num_bits);
				}
			}
		}
		return 0;
	}

	
	int __cdecl hkSendToServer(lua_State* a1) {


		bool ignore = false;// pCurrlog->packet_name == "backup_data_transfer" || pCurrlog->packet_name == "m_check_synced_data";
		//render.ReadPixel;
		if (pCurrlog != &curr_Log) {
			pCurrlog->force_update = false;
			m_bUpdateMsg = true;
			pCurrlog = &curr_Log;
			*pCurrlog = Log();
		}
		else {
			if (pCurrlog->packet_id) {
				//auto pNew = (Log*)malloc(sizeof(Log));
				//ZeroMemory(pNew, sizeof(Log));
				//*pNew = Log();
				//*pNew = pCurrlog;
				g_Log.push_back(*pCurrlog);
			}
			ClearLog(pCurrlog);
			pCurrlog = &curr_Log;
			*pCurrlog = Log();
		}
		
		a1->m_interface->SetState(a1);
		if (*g_Started) {

			//backup_data_transfer

			g_pEngine->GMOD_SendToServer((void*)net_buff->m_pData, net_buff->m_iCurBit, *g_Reliable);
			
			*g_Started = false;
		}

		return 0;

		//return o_SendToServer(a1);
	}
	
	

	void init_hook()
	{
		//LL_Factory_net* net = (LL_Factory_net*)(SDK::Client.GetStartAddr() + 0x64F70C);
		LL_Factory_net* net = NULL;
		auto start_addr = SDK::Client.GetStartAddr();
		while (!net) {
			auto ptr = (uint32_t)((DWORD)(Core::Memory::FindPattern((PBYTE)start_addr, SDK::Client.GetSize(),
				(const char *)"\x68\xCC\xCC\xCC\xCC\xB9\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x68\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x59\xC3",//x????x????x????xxx
				0xCC)));
			ptr += 1;
			if (strcmp(*(char**)ptr, "net") == 0) {
				net = *(LL_Factory_net**)(ptr + 5);
			}
			start_addr = ptr + 1;
		}
		

		auto temp = (uint32_t)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x6A\xFF\x6A\xCC\x68\xCC\xCC\x01\xCC\x68\xCC\xCC\xCC\xCC\xB9",//x????x????x????xxx
			0xCC)));
		temp += 10u;
		g_Write = *(char **)temp;

		temp += 5u;
		net_buff = *(bf_write**)temp;
		


		temp = (uint32_t)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x6A\x02\xC6\x05\xCC\xCC\xCC\xCC\xCC\x8B\x01",//xxxx?????xx
			0xCC)));
		temp += 4u;
		g_Started = *(bool **)temp;

		temp = (uint32_t)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x0F\x94\x05\xCC\xCC\xCC\xCC\x8B\x01",//xxx????xx
			0xCC)));
		temp += 3u;

		g_Reliable = *(bool **)temp;

		temp = (uint32_t)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x8B\x55\x08\x85\xD2\x74\xCC\x8B\xCC\xCC\xCC\xCC\xCC\xCC\x8B\xCC\xFF\xCC\x30\xCC\xFF\xFF\x00\x00",// 55 8B EC 8B 55 08 85 D2 74 ? 8B ? ? ? ? ? ? 8B ? FF ? 30 ? FF FF 00 00
			0xCC)));
		network_string_get = (network_string_get_td)temp;

		for (auto iter = 0;; iter++) {
			auto it = (net->data[iter]);

			if (!it)
				break;

			if (strcmp(it->name, "Start") == 0) {
				it->func = hkNetStart;
			}
			else if (strcmp(it->name, "WriteFloat") == 0) {
				it->func = hkWriteFloat;
			}
			else if (strcmp(it->name, "WriteDouble") == 0) {
				it->func = hkWriteDouble;
			}
			else if (strcmp(it->name, "WriteBit") == 0) {
				it->func = hkWriteBit;
			}
			else if (strcmp(it->name, "WriteString") == 0) {
				it->func = hkWriteString;
			}
			else if (strcmp(it->name, "WriteData") == 0) {
				it->func = hkWriteData;
			}
			else if (strcmp(it->name, "WriteVector") == 0) {
				HDE_STRUCT hde32 = {};
				uint32_t temp = (uint32_t)it->func;
				DWORD len = hde_disasm((void*)temp, &hde32);
				do {
					temp += len;
					len = hde_disasm((void*)temp, &hde32);
				} while (hde32.opcode != 0xE8);
				Get_Vector = (get_vector_td)Core::Memory::follow_to(temp);
				it->func = hkWriteVector;
			}
			else if (strcmp(it->name, "WriteNormal") == 0) {
				it->func = hkWriteNormal;
			}
			else if (strcmp(it->name, "WriteAngle") == 0) {
				HDE_STRUCT hde32 = {};
				uint32_t temp = (uint32_t)it->func;
				DWORD len = hde_disasm((void*)temp, &hde32);
				do {
					temp += len;
					len = hde_disasm((void*)temp, &hde32);
				} while (hde32.opcode != 0xE8);
				Get_Angle = (get_angle_td)Core::Memory::follow_to(temp);

				it->func = hkWriteAngle;
			}
			else if (strcmp(it->name, "WriteMatrix") == 0) {
				HDE_STRUCT hde32 = {};
				uint32_t temp = (uint32_t)it->func;
				DWORD len = hde_disasm((void*)temp, &hde32);
				do {
					temp += len;
					len = hde_disasm((void*)temp, &hde32);
				} while (hde32.opcode != 0xE8);
				Get_VMatrix = (get_vmatrix_td)Core::Memory::follow_to(temp);

				it->func = hkWriteMatrix;
			}
			else if (strcmp(it->name, "SendToServer") == 0) {
				o_SendToServer = it->func;
				it->func = hkSendToServer;
			}
			else if (strcmp(it->name, "WriteInt") == 0) {
				it->func = hkWriteInt;
			}
			else if (strcmp(it->name, "WriteUInt") == 0) {
				it->func = hkWriteUInt;
			}

		}
	}

}