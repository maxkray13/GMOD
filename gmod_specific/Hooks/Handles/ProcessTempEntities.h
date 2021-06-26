#pragma once
#include "../../Functional/ServerAnimations.h"
// Skip local host for event send.
#define FEV_NOTHOST		(1<<0)    

// Send the event reliably.  You must specify the origin and angles and use
// PLAYBACK_EVENT_FULL for this to work correctly on the server for anything
// that depends on the event origin/angles.  I.e., the origin/angles are not
// taken from the invoking edict for reliable events.
#define FEV_RELIABLE	(1<<1)	 

// Don't restrict to PAS/PVS, send this event to _everybody_ on the server ( useful for stopping CHAN_STATIC
//  sounds started by client event when client is not in PVS anymore ( hwguy in TFC e.g. ).
#define FEV_GLOBAL		(1<<2)

class C_ServerClassInfo
{
public:
	C_ServerClassInfo();
	~C_ServerClassInfo();

public:

	ClientClass	*m_pClientClass;
	char		*m_ClassName;
	char		*m_DatatableName;

	// This is an index into the network string table (cl.GetInstanceBaselineTable()).
	int			m_InstanceBaselineIndex; // INVALID_STRING_INDEX if not initialized yet.
};

class CEngineRecipientFilter : public IRecipientFilter
{
public:	// IRecipientFilter interface:

	CEngineRecipientFilter();
	virtual int		GetRecipientCount(void) const { return true; };
	virtual int		GetRecipientIndex(int slot) const { return true; };
	virtual bool	IsReliable(void) const { return m_bReliable; };
	virtual bool	IsInitMessage(void)  const { return m_bInit; };

public:

	void			Reset(void);

	void			MakeInitMessage(void);
	void			MakeReliable(void);

	void			AddAllPlayers(void);
	void			AddRecipientsByPVS(const Vector& origin);
	void			AddRecipientsByPAS(const Vector& origin);
	void			AddPlayersFromBitMask(CBitVec< ABSOLUTE_PLAYER_LIMIT >& playerbits);
	void			AddPlayersFromFilter(const IRecipientFilter *filter);
	void			AddRecipient(int playerindex);
	void			RemoveRecipient(int playerindex);
	bool			IncludesPlayer(int playerindex);

private:

	bool				m_bInit;
	bool				m_bReliable;
	CUtlVector< int >	m_Recipients;
};
CEngineRecipientFilter::CEngineRecipientFilter()
{
	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEngineRecipientFilter::Reset(void)
{
	m_bReliable = false;
	m_bInit = false;
	m_Recipients.RemoveAll();
}
class SendTable;
class SendProp;
typedef void(*SendVarProxyFn)(const SendProp *pProp, const void *pStructBase, const void *pData, DVariant *pOut, int iElement, int objectID);
typedef void* (*SendTableProxyFn)(
	const SendProp *pProp,
	const void *pStructBase,
	const void *pData,
	void *pRecipients,
	int objectID);
typedef int(*ArrayLengthSendProxyFn)(const void *pStruct, int objectID);


class SendProp
{
public:
	SendProp();
	virtual				~SendProp();

	void				Clear();

	int					GetOffset() const;
	void				SetOffset(int i);

	SendVarProxyFn		GetProxyFn() const;
	void				SetProxyFn(SendVarProxyFn f);

	SendTableProxyFn	GetDataTableProxyFn() const;
	void				SetDataTableProxyFn(SendTableProxyFn f);

	SendTable*			GetDataTable() const;
	void				SetDataTable(SendTable *pTable);

	char const*			GetExcludeDTName() const;

	// If it's one of the numbered "000", "001", etc properties in an array, then
	// these can be used to get its array property name for debugging.
	const char*			GetParentArrayPropName();
	void				SetParentArrayPropName(char *pArrayPropName);

	const char*			GetName() const;

	bool				IsSigned() const;

	bool				IsExcludeProp() const;

	bool				IsInsideArray() const;	// Returns true if SPROP_INSIDEARRAY is set.
	void				SetInsideArray();

	// Arrays only.
	void				SetArrayProp(SendProp *pProp);
	SendProp*			GetArrayProp() const;

	// Arrays only.
	void					SetArrayLengthProxy(ArrayLengthSendProxyFn fn);
	ArrayLengthSendProxyFn	GetArrayLengthProxy() const;

	int					GetNumElements() const;
	void				SetNumElements(int nElements);

	// Return the # of bits to encode an array length (must hold GetNumElements()).
	int					GetNumArrayLengthBits() const;

	int					GetElementStride() const;

	SendPropType		GetType() const;

	int					GetFlags() const;
	void				SetFlags(int flags);

	// Some property types bind more data to the SendProp in here.
	const void*			GetExtraData() const;
	void				SetExtraData(const void *pData);

public:

	RecvProp		*m_pMatchingRecvProp;	// This is temporary and only used while precalculating
												// data for the decoders.

	SendPropType	m_Type;
	int				m_nBits;
	float			m_fLowValue;
	float			m_fHighValue;

	SendProp		*m_pArrayProp;					// If this is an array, this is the property that defines each array element.
	ArrayLengthSendProxyFn	m_ArrayLengthProxy;	// This callback returns the array length.

	int				m_nElements;		// Number of elements in the array (or 1 if it's not an array).
	int				m_ElementStride;	// Pointer distance between array elements.

	union
	{
		char *m_pExcludeDTName;			// If this is an exclude prop, then this is the name of the datatable to exclude a prop from.
		char *m_pParentArrayPropName;
	};

	char			*m_pVarName;
	float			m_fHighLowMul;

private:

	int					m_Flags;				// SPROP_ flags.

	SendVarProxyFn		m_ProxyFn;				// NULL for DPT_DataTable.
	SendTableProxyFn	m_DataTableProxyFn;		// Valid for DPT_DataTable.

	SendTable			*m_pDataTable;

	// SENDPROP_VECTORELEM makes this negative to start with so we can detect that and
	// set the SPROP_IS_VECTOR_ELEM flag.
	int					m_Offset;

	// Extra data bound to this property.
	const void			*m_pExtraData;
};

class SendTable
{
public:

	typedef SendProp PropType;

	SendTable();
	SendTable(SendProp *pProps, int nProps, char *pNetTableName);
	~SendTable();

	void		Construct(SendProp *pProps, int nProps, char *pNetTableName);

	const char*	GetName() const;

	int			GetNumProps() const;
	SendProp*	GetProp(int i);

	// Used by the engine.
	bool		IsInitialized() const;
	void		SetInitialized(bool bInitialized);

	// Used by the engine while writing info into the signon.
	void		SetWriteFlag(bool bHasBeenWritten);
	bool		GetWriteFlag() const;

	bool		HasPropsEncodedAgainstTickCount() const;
	void		SetHasPropsEncodedAgainstTickcount(bool bState);

public:

	SendProp	*m_pProps;
	int			m_nProps;

	char		*m_pNetTableName;	// The name matched between client and server.

	// The engine hooks the SendTable here.
	void		*m_pPrecalc;


protected:
	bool		m_bInitialized : 1;
	bool		m_bHasBeenWritten : 1;
	bool		m_bHasPropsEncodedAgainstCurrentTickCount : 1; // m_flSimulationTime and m_flAnimTime, e.g.
};


inline const char* SendTable::GetName() const
{
	return m_pNetTableName;
}


inline int SendTable::GetNumProps() const
{
	return m_nProps;
}


inline SendProp* SendTable::GetProp(int i)
{
	Assert(i >= 0 && i < m_nProps);
	return &m_pProps[i];
}


inline bool SendTable::IsInitialized() const
{
	return m_bInitialized;
}


inline void SendTable::SetInitialized(bool bInitialized)
{
	m_bInitialized = bInitialized;
}


inline bool SendTable::GetWriteFlag() const
{
	return m_bHasBeenWritten;
}


inline void SendTable::SetWriteFlag(bool bHasBeenWritten)
{
	m_bHasBeenWritten = bHasBeenWritten;
}

inline bool SendTable::HasPropsEncodedAgainstTickCount() const
{
	return m_bHasPropsEncodedAgainstCurrentTickCount;
}

inline void SendTable::SetHasPropsEncodedAgainstTickcount(bool bState)
{
	m_bHasPropsEncodedAgainstCurrentTickCount = bState;
}
class CEventInfo
{
public:
	enum
	{
		EVENT_INDEX_BITS = 8,
		EVENT_DATA_LEN_BITS = 11,
		MAX_EVENT_DATA = 192,  // ( 1<<8 bits == 256, but only using 192 below )
	};

	inline CEventInfo()
	{
		classID = 0;
		fire_delay = 0.0f;
		bits = 0;
		flags = 0;
		pSendTable = NULL;
		pClientClass = NULL;
		pData = NULL;
	}

	~CEventInfo()
	{
		if (pData)
		{
			delete pData;
		}
	}

	CEventInfo(const CEventInfo& src)
	{
		classID = src.classID;
		fire_delay = src.fire_delay;
		bits = src.bits;
		flags = src.flags;
		pSendTable = src.pSendTable;
		pClientClass = src.pClientClass;
		filter.AddPlayersFromFilter(&src.filter);

		if (src.pData)
		{
			int size = Bits2Bytes(src.bits);
			pData = new byte[size];
			Q_memcpy(pData, src.pData, size);
		}
		else
		{
			pData = NULL;
		}

	}

	// 0 implies not in use
	short classID;

	// If non-zero, the delay time when the event should be fired ( fixed up on the client )
	float fire_delay;

	// send table pointer or NULL if send as full update
	const SendTable *pSendTable;
	const ClientClass *pClientClass;

	// Length of data bits
	int		bits;
	// Raw event data
	byte	*pData;
	// CLIENT ONLY Reliable or not, etc.
	int		flags;

	// clients that see that event
	CEngineRecipientFilter filter;
};
C_BaseEntity* GetBaseEntityFromHandle(CBaseHandle hEnt)
{
	IClientUnknown* pEnt = (IClientUnknown*)g_pEntList->GetClientUnknownFromHandle((void*)hEnt);
	return pEnt ? pEnt->GetBaseEntity() : 0;
}
#define BYTEn(x, n)   (*((uint8*)&(x)+n))

#define BYTE4(x)   BYTEn(x,  4)


bool __fastcall hkProcessTempEntities(void* ecx,void* edx,SVC_TempEntities *msg) {
	

	Engine::WriteFile("%s ST", __FUNCTION__);
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
	auto FindClientClass = [](const char *pClassName) ->ClientClass* {
		using t = ClientClass * (__cdecl*)(const char *pClassName);
		static t FindClientClass_f = NULL;
		if (!FindClientClass_f) {
			FindClientClass_f = (t)Core::Memory::FindPattern((PBYTE)SDK::Engine.GetStartAddr(), SDK::Engine.GetSize(),
				(const char *)"\x55\x8B\xEC\xCC\xCC\x8B\xCC\xCC\x85\xCC\x74\xCC\xE8",//xx????xxxxx
				0xCC);
		}
		return FindClientClass_f(pClassName);
	};
	auto RecvTable_MergeDeltas = [](RecvTable *pTable,bf_read *pOldState,bf_read *pNewState,bf_write *pOut,int objectID = -1,bool bDebugWatchInfo = false,int*pChangedProps = NULL) ->int {
		using t = int(__cdecl*)(RecvTable *pTable, bf_read *pOldState, bf_read *pNewState, bf_write *pOut, int objectID, bool bDebugWatchInfo, int*pChangedProps);
		static t RecvTable_MergeDeltas_f = NULL;
		if (!RecvTable_MergeDeltas_f) {
			RecvTable_MergeDeltas_f = (t)Core::Memory::FindPattern((PBYTE)SDK::Engine.GetStartAddr(), SDK::Engine.GetSize(),
				(const char *)"\x55\x8B\xEC\x8B\xCC\xCC\x83\xEC\x44\x85",//xx????xxxxx
				0xCC);
		}
		return RecvTable_MergeDeltas_f(pTable, pOldState, pNewState, pOut, objectID, bDebugWatchInfo, pChangedProps);
	};
	auto CL_ParseEventDelta = [](void *RawData, void *pToData, RecvTable *pRecvTable ,int bits) ->void
	{
		auto RecvTable_DecodeZeros = [](RecvTable *pTable, void *pStruct, int objectID)->void {
			using t = void(__cdecl *)(RecvTable *pTable, void *pStruct, int objectID);
			static t RecvTable_DecodeZeros_f = NULL;
			if (!RecvTable_DecodeZeros_f) {
				RecvTable_DecodeZeros_f = (t)Core::Memory::FindPattern((PBYTE)SDK::Engine.GetStartAddr(), SDK::Engine.GetSize(),
					(const char *)"\x55\x8B\xEC\x8B\xCC\xCC\x81\xEC\x4C\x03",//xx????xxxxx
					0xCC);
			}
			return RecvTable_DecodeZeros_f(pTable, pStruct, objectID);
		};
		auto RecvTable_Decode = [](RecvTable *pTable, void *pStruct, bf_read *pIn, int objectID, char unk) -> bool
		{
			using t = bool(__cdecl *)(RecvTable *pTable, void *pStruct, bf_read *pIn, int objectID);
			static t RecvTable_Decode_f = NULL;
			if (!RecvTable_Decode_f) {
				RecvTable_Decode_f = (t)Core::Memory::FindPattern((PBYTE)SDK::Engine.GetStartAddr(), SDK::Engine.GetSize(),
					(const char *)"\x55\x8B\xEC\x81\xEC\x64\x03",//xx????xxxxx
					0xCC);
			}

			return RecvTable_Decode_f(pTable, pStruct, pIn, objectID);
		};

		
		
		int64 size_bites = (Bits2Bytes(bits) + 3);

		int aligned_size = ((BYTE4(size_bites) & 3) + size_bites) & 0xFFFFFFFC;

		bf_read fromBuf("CL_ParseEventDelta->fromBuf", RawData, 0xFFF);
		RecvTable_DecodeZeros(pRecvTable, pToData, -1);

		// Now decode the data from the network on top of that.
		RecvTable_Decode(pRecvTable, pToData, &fromBuf, -1, 1);
	};
	
	bool bReliable = false;
	float fire_time = CClientState__GetTime(g_pClientState);
	
	if (msg->m_nNumEntries == 0)
	{
		bReliable = true;
		msg->m_nNumEntries = 1;
	}
	
	int flags = bReliable ? FEV_RELIABLE : 0;

	bf_read &buffer = msg->m_DataIn; // shortcut

	int classID = -1;
	void *from = NULL;
	C_ServerClassInfo *pServerClass = NULL;
	ClientClass *pClientClass = NULL;
	unsigned char data[CEventInfo::MAX_EVENT_DATA];
	bf_write toBuf(data, sizeof(data));
	CEventInfo *ei = NULL;

	for (int i = 0; i < msg->m_nNumEntries; i++)
	{
		float delay = 0.0f;

		if (buffer.ReadOneBit())
		{
			delay = (float)buffer.ReadSBitLong(8) / 100.0f;
		}
		toBuf.Reset();

		if (buffer.ReadOneBit())
		{
			from = NULL; // full update
			C_ServerClassInfo	* m_pServerClasses = *(C_ServerClassInfo **)((DWORD)g_pClientState + 0x101FC + 0x8);
			int m_nServerClassBits = *(int*)((DWORD)g_pClientState + 0x10204 + 0x8);
			classID = buffer.ReadUBitLong(m_nServerClassBits); // classID 
			// Look up the client class, etc.

			// Match the server classes to the client classes.
			pServerClass = m_pServerClasses ? &m_pServerClasses[classID - 1] : NULL;

			if (!pServerClass)
			{
				return false;
			}
			
			pClientClass = FindClientClass(pServerClass->m_ClassName);

			if (!pClientClass || !pClientClass->m_pRecvTable){
				return false;
			}
		//	char buff[260];
		//	sprintf_s(buff, "echo %s %d", pClientClass->GetName(), pClientClass->m_ClassID);
		//	g_pEngine->ClientCmd(buff);
			RecvTable_MergeDeltas(pClientClass->m_pRecvTable, NULL, &buffer, &toBuf);

		}
		else
		{
			bf_read fromBuf(ei->pData, Bits2Bytes(ei->bits));
			RecvTable_MergeDeltas(pClientClass->m_pRecvTable, &fromBuf, &buffer, &toBuf);
		}
		CUtlFixedLinkedList< CEventInfo > & events = *(CUtlFixedLinkedList< CEventInfo > *)((DWORD)g_pClientState + 0x10B44);//0x10B44

		auto add_event = [](void* events,int some)->CEventInfo*
		{
			using t = CEventInfo * (__thiscall*)(void* events, int some);
			//int __thiscall sub_101DC580(_DWORD *this, int a2)
			static t add_event_f = NULL;
			if (!add_event_f) {
				add_event_f = (t)Core::Memory::FindPattern((PBYTE)SDK::Engine.GetStartAddr(), SDK::Engine.GetSize(),
					(const char *)"\x55\x8B\xEC\xCC\xCC\x6A\xCC\x8B\xCC\xE8\xCC\xCC\xCC\xCC\x8B\xCC\x85",//xx????xxxxx
					0xCC);
			}
			return add_event_f(events, some);
		};
		ei = add_event(&events, 0);//g_pGlobalVars->curtime currtime is right
		int size = Bits2Bytes(toBuf.GetNumBitsWritten());
		ei->classID = classID;
		ei->fire_delay = 0;
		ei->flags = flags;
		ei->pClientClass = pClientClass;
		ei->bits = toBuf.GetNumBitsWritten();
		ei->pData = (BYTE*)g_pMemAlloc->Alloc((size + 3) & 0xFFFFFFFC); // copy raw data
		ei->filter = CEngineRecipientFilter();
		Q_memcpy(ei->pData, data, size);
	}
	/*
	using fn = bool(__fastcall*)(void* ecx, void* edx, SVC_TempEntities *msg);
	auto ret = Hooks::VClientState.GetOriginalMethod<fn>(24)(ecx, edx, msg);

	//CUtlFixedLinkedList< CEventInfo > & events = *(CUtlFixedLinkedList< CEventInfo > *)((DWORD)g_pClientState + 0x10B44);//0x10B44
	struct CEventInfo_fukup {
		CEventInfo_fukup * next;

		int edx;
		// 0 implies not in use
		short classID;

		// If non-zero, the delay time when the event should be fired ( fixed up on the client )
		float fire_delay;

		// send table pointer or NULL if send as full update
		const SendTable *pSendTable;
		const ClientClass *pClientClass;

		// Length of data bits
		int		bits;
		// Raw event data
		byte	*pData;
		// CLIENT ONLY Reliable or not, etc.
		int		flags;

		// clients that see that event
		CEngineRecipientFilter filter;
	};


	CEventInfo_fukup *ei_t = *(CEventInfo_fukup**)((DWORD)g_pClientState + 0x10B44);

#define EzOffset(array_name, prop_name) \
static int o_##prop_name;\
if(!o_##prop_name){\
		o_##prop_name = SDK::Util::NetPropManager::Instance().Get( #array_name , #prop_name ); \
	}

	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal || !pLocal->IsGood())
		return ret;

	if (!ei_t) {
		return ret;
	}
	for (auto it = ei_t; it; it = it->next) {
		it->fire_delay = 0;
		/*if (it->pClientClass->m_pCreateEventFn && it->classID == 170)
		{
			IClientNetworkable *pCE = it->pClientClass->m_pCreateEventFn();
			auto baseptr = pCE->GetDataTableBasePtr();
			if ( baseptr) {

			//	pCE->PreDataUpdate(DATA_UPDATE_CREATED);
				CL_ParseEventDelta(it->pData, pCE->GetDataTableBasePtr(), it->pClientClass->m_pRecvTable, it->bits);

				EzOffset(DT_TEHL2MPFireBullets, m_vecOrigin);
				EzOffset(DT_TEHL2MPFireBullets, m_vecDir);
				EzOffset(DT_TEHL2MPFireBullets, m_iAmmoID);
				EzOffset(DT_TEHL2MPFireBullets, m_iSeed);
				EzOffset(DT_TEHL2MPFireBullets, m_iShots);
				EzOffset(DT_TEHL2MPFireBullets, m_iPlayer);
				EzOffset(DT_TEHL2MPFireBullets, m_iWeaponIndex);
				EzOffset(DT_TEHL2MPFireBullets, m_flSpread);
				EzOffset(DT_TEHL2MPFireBullets, m_bDoImpacts);
				EzOffset(DT_TEHL2MPFireBullets, m_bDoTracers);
				
				auto m_vecOrigin = *(Vector*)(DWORD(baseptr) + o_m_vecOrigin);
				auto m_vecDir = *(Vector*)(DWORD(baseptr) + o_m_vecDir);
				auto m_iAmmoID = *(int*)(DWORD(baseptr) + o_m_iAmmoID);
				auto m_iSeed = *(int*)(DWORD(baseptr) + o_m_iSeed);
				auto m_iShots = *(int*)(DWORD(baseptr) + o_m_iShots);
				auto m_iPlayer = *(int*)(DWORD(baseptr) + o_m_iPlayer);
				auto m_iWeaponIndex = *(int*)(DWORD(baseptr) + o_m_iWeaponIndex);
				auto m_flSpread = *(float*)(DWORD(baseptr) + o_m_flSpread);
				auto m_bDoImpacts = *(int*)(DWORD(baseptr) + o_m_bDoImpacts);
				auto m_bDoTracers = *(int*)(DWORD(baseptr) + o_m_bDoTracers);

				auto damager = C_GMOD_Player::GetGPlayer(m_iPlayer);
				if (damager && damager->IsValid() && damager->IsGood()) {
					trace_t Trace = {};
					//SDK::Function::UTIL_TraceLine(m_vecOrigin, m_vecOrigin + m_vecDir * MAX_TRACE_LENGTH, MASK_SOLID, damager, 0, &Trace);
					//g_pEngine->Con_NPrintf(3, "damager idx %d", damager->entindex());

					auto angle_shot = m_vecDir.ToEulerAngles();
					angle_shot.Normalize();

					damager->m_angEyeAngles() = angle_shot;
					if (g_ServerAnimations.g_Tracker[damager->entindex()].m_PlayerAnimState) {
					//	g_ServerAnimations.g_Tracker[damager->entindex()].m_PlayerAnimState->m_bForceAimYaw = true;
						g_ServerAnimations.g_Tracker[damager->entindex()].m_PlayerAnimState->Update(angle_shot[YAW], angle_shot[PITCH]);
					//	g_ServerAnimations.g_Tracker[damager->entindex()].m_PlayerAnimState->m_bForceAimYaw = false;
					}
					auto tick = g_LagCompensation.build_tick(damager);
					tick.m_flSimulationTime = g_pGlobalVars->curtime;
					tick.m_priority = tick_trusted;
					//tick.m_flSimulationTime = SDK::Vars::g_tick * g_pGlobalVars->interval_per_tick - SDK::GetLerpTime();
					g_LagCompensation.push_tick(damager, tick);
					
				}
			}
		}

	}*/
	g_pEngine->FireEvents();

	Engine::WriteFile("%s END", __FUNCTION__);
	return true;

}