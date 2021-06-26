#pragma once

namespace net_logger {
	void init_hook();
	

	enum Arg_type{
		aFLOAT,
		aDOUBLE,
		aBIT,
		aSTRING,
		aDATA,
		aVECTOR,
		aNORMAL,
		aANGLE,
		aMATRIX,
		aINT,
		aUINT
	};

	template <typename T>
	struct Argument{
		Arg_type	type;
		long long	min;
		long long	max;
		int			bit_size;
		T			val;
	};

	struct unk_Argument {
		Arg_type	type;
		long long	min;
		long long	max;
		int			bit_size;
	};

	template <typename T>
	Argument<T>* resolve_type(unk_Argument* unk) {
		if (unk->type == aFLOAT) {
			return (Argument<float>*)unk;
		}
		else if (unk->type == aDOUBLE) {
			return (Argument<double>*)unk;
		}
		else if (unk->type == aBIT) {
			return (Argument<char>*)unk;
		}
		else if (unk->type == aSTRING) {
			return (Argument<std::string>*)unk;
		}
		else if (unk->type == aDATA) {
			return (Argument<std::string>*)unk;
		}
		else if (unk->type == aVECTOR) {
			return (Argument<Vector>*)unk;
		}
		else if (unk->type == aNORMAL) {
			return (Argument<Vector>*)unk;
		}
		else if (unk->type == aANGLE) {
			return (Argument<QAngle>*)unk;
		}
		else if (unk->type == aMATRIX) {
			return (Argument<VMatrix>*)unk;
		}
		else if (unk->type == aINT) {
			return (Argument<int>*)unk;
		}
		else if (unk->type == aUINT) {
			return (Argument<UINT>*)unk;
		}
		return nullptr;
	}

	struct Log {
		Log() = default;
		std::string					packet_name;
		uint32_t					packet_id;
		bool						reliable;
		bool force_update = false;
		std::vector<unk_Argument*>	args;
	};

	extern std::vector <Log>		g_Log;

	extern bool						m_bStartSending;
	extern bool						m_bSendOnce;
	extern int						m_iPacketID;
	extern bool						m_bUpdateMsg;
	extern Log*						pCurrlog;
	
	void ClearLog(Log* pLog);
	void ClearGLog();
	bool PacketLogged(int id);
	Log* TargetPacket(int id);

	void ProcessSend();
	



	using lua_func = int(__cdecl*)(lua_State*);
	class lua_data {
	public:
		const char * name;
	private:
		char pad_0004[4];
	public:
		lua_func func;
	};
	class LL_Factory_net {
	private:
		char pad_0000[12];
	public:
		lua_data **data;
	};
	int __cdecl hkNetStart(lua_State* a1);
	int __cdecl hkWriteFloat(lua_State* a1);
	int __cdecl hkWriteDouble(lua_State* a1);
	int __cdecl hkWriteBit(lua_State* a1);
	int __cdecl hkWriteString(lua_State* a1);
	int __cdecl hkWriteData(lua_State* a1);
	int __cdecl hkWriteVector(lua_State* a1);
	int __cdecl hkWriteNormal(lua_State* a1);
	int __cdecl hkWriteAngle(lua_State* a1);
	int __cdecl hkWriteMatrix(lua_State* a1);
	int __cdecl hkWriteInt(lua_State* a1);
	int __cdecl hkWriteUInt(lua_State* a1);
	int __cdecl hkSendToServer(lua_State* a1);
}