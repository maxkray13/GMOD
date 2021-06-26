#pragma once

#define MULTIPLAYER_BACKUP			90
#define NUM_NEW_COMMAND_BITS		4
#define MAX_NEW_COMMANDS			((1 << NUM_NEW_COMMAND_BITS)-1)
#define MAX_CMD_BUFFER				4000
#define NUM_BACKUP_COMMAND_BITS		3
#define MAX_BACKUP_COMMANDS			((1 << NUM_BACKUP_COMMAND_BITS)-1)


/*
static int counter = 0;
typedef int(__fastcall *NET_CHANNEL_tdSendDatagram)(INetChannel* netchan, void*, bf_write *datagram);
int __fastcall hooked_SendDatagram(INetChannel* netchan, void*th, bf_write *datagram) {

	int instate = netchan->m_nInReliableState;
	int insequencenr = netchan->m_nInSequenceNr;


	static	int	index = 2;
	if (GetAsyncKeyState(0x48))//H
	{
		netchan->m_nOutSequenceNr += index ? 50000 : 0;
		index--;

	}

	if (index <= 0)
		index = 2;


	int ret = cGMOD_hooks.NetChannelHook.GetOriginalMethod<NET_CHANNEL_tdSendDatagram>(46)(netchan, th, datagram);


	netchan->m_nInReliableState = instate;
	netchan->m_nInSequenceNr = insequencenr;

	return ret;
}
int firsttime = 0;
typedef bool(__fastcall *NET_CHANNEL_tdSendNetMsg)(INetChannel* netchan, void*, INetMessage &msg, bool bForceReliable, bool bVoice);
bool __fastcall hooked_SendNetMsg(INetChannel* netchan, void*th, INetMessage &msg, bool bForceReliable, bool bVoice) {
	__asm PUSHAD

	if (strcmp("clc_Move", msg.GetName()) == 0 && firsttime) {
		firsttime--;
		auto Cl_Move = (CLC_Move&)msg;

		//	Cl_Move.m_DataOut

		//	mTools.MSGBox("%p", Cl_Move.m_DataOut.GetData());

		cGMOD_hooks.NetChannelHook.GetOriginalMethod<NET_CHANNEL_tdSendNetMsg>(40)(netchan, th, msg, bForceReliable, bVoice);
		netchan->Transmit(false);
		netchan->m_nOutSequenceNr += 200;
		g_pClient->CreateMove(Lastsequence_number + 1, 0, true);
	}
	firsttime = 3;


	__asm POPAD

	return cGMOD_hooks.NetChannelHook.GetOriginalMethod<NET_CHANNEL_tdSendNetMsg>(40)(netchan, th, msg, bForceReliable, bVoice);
}
*/

class INetMessage
{
public:
	virtual	~INetMessage() {};

	// Use these to setup who can hear whose voice.
	// Pass in client indices (which are their ent indices - 1).

	virtual void	SetNetChannel(INetChannel * netchan) = 0; // netchannel this message is from/for
	virtual void	SetReliable(bool state) = 0;	// set to true if it's a reliable message

	virtual bool	Process(void) = 0; // calles the recently set handler to process this message

	virtual	bool	ReadFromBuffer(bf_read &buffer) = 0; //bf_read &buffer returns true if parsing was OK
	virtual	bool	WriteToBuffer(bf_write &buffer) = 0;	//bf_write &buffer returns true if writing was OK

	virtual bool	IsReliable(void) const = 0;  // true, if message needs reliable handling

	virtual int				GetType(void)const = 0; // returns module specific header tag eg svc_serverinfo
	virtual int				GetGroup(void) const = 0;	// returns net message group of this message
	virtual const char		*GetName(void)const = 0;	// returns network message name, eg "svc_serverinfo"
	virtual INetChannel		*GetNetChannel(void) const = 0;
	virtual const char		*ToString(void)const = 0; // returns a human readable string about message content
};


class clc_GmodToServer : public INetMessage
{
public:
	char _0x0004[12];
	__int32 NumberOfMod; //0x0010 
	char _0x0014[24];
	DWORD pMods; //0x002C 
};
class INetChannelHandler
{
public:
	virtual	~INetChannelHandler(void) {};

	virtual void ConnectionStart(INetChannel *chan) = 0;	// called first time network channel is established

	virtual void ConnectionClosing(const char *reason) = 0; // network channel is being closed by remote site

	virtual void ConnectionCrashed(const char *reason) = 0; // network error occured

	virtual void PacketStart(int incoming_sequence, int outgoing_acknowledged) = 0;	// called each time a new packet arrived

	virtual void PacketEnd(void) = 0; // all messages has been parsed

	virtual void FileRequested(const char *fileName, unsigned int transferID) = 0; // other side request a file for download

	virtual void FileReceived(const char *fileName, unsigned int transferID) = 0; // we received a file

	virtual void FileDenied(const char *fileName, unsigned int transferID) = 0;	// a file request was denied by other side
};

class INetChannelInfo
{
public:

	enum {
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};

	virtual const char  *GetName(void) const = 0;	// get channel name
	virtual const char  *GetAddress(void) const = 0; // get channel IP address as string
	virtual float		GetTime(void) const = 0;	// current net time
	virtual float		GetTimeConnected(void) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize(void) const = 0;	// netchannel packet history size
	virtual int			GetDataRate(void) const = 0; // send data rate in byte/sec

	virtual bool		IsLoopback(void) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut(void) const = 0;	// true if timing out
	virtual bool		IsPlayback(void) const = 0;	// true if demo playback

	virtual float		GetLatency(int flow) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAvgLatency(int flow) const = 0; // average packet latency in seconds
	virtual float		GetAvgLoss(int flow) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAvgChoke(int flow) const = 0;	 // avg packet choke[0..1]
	virtual float		GetAvgData(int flow) const = 0;	 // data flow in bytes/sec
	virtual float		GetAvgPackets(int flow) const = 0; // avg packets/sec
	virtual int			GetTotalData(int flow) const = 0;	 // total flow in/out in bytes
	virtual int			GetSequenceNr(int flow) const = 0;	// last send seq number
	virtual bool		IsValidPacket(int flow, int frame_number) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime(int flow, int frame_number) const = 0; // time when packet was send
	virtual int			GetPacketBytes(int flow, int frame_number, int group) const = 0; // group size of this packet
	virtual bool		GetStreamProgress(int flow, int *received, int *total) const = 0;  // TCP progress if transmitting
	virtual float		GetTimeSinceLastReceived(void) const = 0;	// get time since last recieved packet in seconds
	virtual	float		GetCommandInterpolationAmount(int flow, int frame_number) const = 0;
	virtual void		GetPacketResponseLatency(int flow, int frame_number, int *pnLatencyMsecs, int *pnChoke) const = 0;
	virtual void		GetRemoteFramerate(float *pflFrameTime, float *pflFrameTimeStdDeviation) const = 0;

	virtual float		GetTimeoutSeconds() const = 0;
};

class INetChannel : public INetChannelInfo
{
public:
	virtual	~INetChannel(void) {};

	virtual void	SetDataRate(float rate) = 0;
	virtual bool	RegisterMessage(INetMessage *msg) = 0;
	virtual bool	StartStreaming(unsigned int challengeNr) = 0;
	virtual void	ResetStreaming(void) = 0;
	virtual void	SetTimeout(float seconds) = 0;
	virtual void	SetDemoRecorder() = 0;
	virtual void	SetChallengeNr(unsigned int chnr) = 0;

	virtual void	Reset(void) = 0;
	virtual void	Clear(void) = 0;
	virtual void	Shutdown(const char *reason) = 0;//36

	virtual void	ProcessPlayback(void) = 0;
	virtual bool	ProcessStream(void) = 0;
	virtual void	ProcessPacket(struct netpacket_s* packet, bool bHasHeader) = 0;

	virtual bool	SendNetMsg(INetMessage &msg, bool bForceReliable = false, bool bVoice = false) = 0;//40

	virtual bool	SendData(bf_write &msg, bool bReliable = true) = 0;//bf_write &msg
	virtual bool	SendFile(const char *filename, unsigned int transferID) = 0;
	virtual void	DenyFile(const char *filename, unsigned int transferID) = 0;
	virtual void	RequestFile_OLD(const char *filename, unsigned int transferID) = 0;	// get rid of this function when we version the 
	virtual void	SetChoked(void) = 0;
	virtual int		SendDatagram(bf_write *datagram) = 0;//46
	virtual bool	Transmit(bool onlyReliable = false) = 0;

	virtual const netadr_t	&GetRemoteAddress(void) const = 0;
	virtual INetChannelHandler *GetMsgHandler(void) const = 0;
	virtual int				GetDropNumber(void) const = 0;
	virtual int				GetSocket(void) const = 0;
	virtual unsigned int	GetChallengeNr(void) const = 0;
	virtual void			GetSequenceData(int &nOutSequenceNr, int &nInSequenceNr, int &nOutSequenceNrAck) = 0;
	virtual void			SetSequenceData(int nOutSequenceNr, int nInSequenceNr, int nOutSequenceNrAck) = 0;

	virtual void	UpdateMessageStats(int msggroup, int bits) = 0;
	virtual bool	CanPacket(void) const = 0;
	virtual bool	IsOverflowed(void) const = 0;
	virtual bool	IsTimedOut(void) const = 0;
	virtual bool	HasPendingReliableData(void) = 0;

	virtual void	SetFileTransmissionMode(bool bBackgroundMode) = 0;
	virtual void	SetCompressionMode(bool bUseCompression) = 0;
	virtual unsigned int RequestFile(const char *filename) = 0;

	virtual void	SetMaxBufferSize(bool bReliable, int nBytes, bool bVoice = false) = 0;

	virtual bool	IsNull() const = 0;
	virtual int		GetNumBitsWritten(bool bReliable) = 0;
	virtual void	SetInterpolationAmount(float flInterpolationAmount) = 0;
	virtual void	SetRemoteFramerate(float flFrameTime, float flFrameTimeStdDeviation) = 0;

	// Max # of payload bytes before we must split/fragment the packet
	virtual void	SetMaxRoutablePayloadSize(int nSplitSize) = 0;
	virtual int		GetMaxRoutablePayloadSize() = 0;

	virtual int     GetProtocolVersion() = 0;


};
class CNetChan :public INetChannel
{
private: // netchan structurs

public:
	char _0x0004[4];
	// last send outgoing sequence number
	int			m_nOutSequenceNr;
	// last received incoming sequnec number
	int			m_nInSequenceNr;
	// last received acknowledge outgoing sequnce number
	int			m_nOutSequenceNrAck;

	// state of outgoing reliable data (0/1) flip flop used for loss detection
	int			m_nOutReliableState;
	// state of incoming reliable data
	int			m_nInReliableState;

	int			m_nChokedPackets;	//number of choked packets

									// Reliable data buffer, send which each packet (or put in waiting list)
	bf_write	m_StreamReliable;
	CUtlMemory<byte> m_ReliableDataBuffer;

	// unreliable message buffer, cleared which each packet
	bf_write	m_StreamUnreliable;
	CUtlMemory<byte> m_UnreliableDataBuffer;

	bf_write	m_StreamVoice;
	CUtlMemory<byte> m_VoiceDataBuffer;

	// don't use any vars below this (only in net_ws.cpp)
	char _0x0008[8];

	int			m_Socket; //0x8C  // NS_SERVER or NS_CLIENT index, depending on channel.
	int			m_StreamSocket;//0x90	// TCP socket handle

	unsigned int m_MaxReliablePayloadSize;	// max size of reliable payload in a single packet	

											// Address this channel is talking to.
	netadr_t	remote_address;

	// For timeouts.  Time last message was received.
	float		last_received;
	// Time when channel was connected.
	double      connect_time;

	char _0x0001[8];

	// Bandwidth choke
	// Bytes per second
	int			m_Rate;

	char _0x0002[4];

	// If realtime > cleartime, free to send next packet
	double		m_fClearTime;

};

class CNetMessage : public INetMessage
{
public:
	CNetMessage() {
		m_bReliable = true;
		m_NetChannel = NULL;
	}

	virtual ~CNetMessage() {};

	virtual int		GetGroup() const { return INetChannelInfo::GENERIC; }
	INetChannel		*GetNetChannel() const { return m_NetChannel; }

	virtual void	SetReliable(bool state) { m_bReliable = state; };
	virtual bool	IsReliable() const { return m_bReliable; };
	virtual void    SetNetChannel(INetChannel * netchan) { m_NetChannel = netchan; }
	virtual bool	Process() { return false; };	// no handler set

protected:
	bool				m_bReliable;	// true if message should be send reliable
	INetChannel			*m_NetChannel;	// netchannel this message is from/for
};
#define NETMSG_TYPE_BITS 6

#define DECLARE_BASE_MESSAGE( msgtype )						\
	public:													\
		bool			ReadFromBuffer( bf_read &buffer );	\
		bool			WriteToBuffer( bf_write &buffer );	\
		const char		*ToString() const;					\
		int				GetType() const { return msgtype; } \
		const char		*GetName() const { return #msgtype;}\

#define DECLARE_NET_MESSAGE( name )			\
	DECLARE_BASE_MESSAGE( net_##name );		\
	void *m_pMessageHandler;	\
	bool Process() { return false; }\


class NET_StringCmd : public CNetMessage
{
public:

	bool WriteToBuffer(bf_write &buffer)
	{
		buffer.WriteUBitLong(GetType(), 6);
		return buffer.WriteString(m_szCommand ? m_szCommand : " NET_StringCmd NULL");
	}

	const char *ToString(void) const
	{
		char s_text[1024];
		sprintf_s(s_text, "%s: \"%s\"", GetName(), m_szCommand);
		return s_text;
	}

	bool ReadFromBuffer(bf_read &buffer)
	{
		m_szCommand = m_szCommandBuffer;

		return buffer.ReadString(m_szCommandBuffer, sizeof(m_szCommandBuffer));
	}

	int	GetGroup() const { return INetChannelInfo::STRINGCMD; }

	int				GetType() const { return 4; }
	const char		*GetName() const { return "net_StringCmd"; }
	bool Process() { return false; };

	void *m_pMessageHandler;
	NET_StringCmd() { m_szCommand = NULL; };
	NET_StringCmd(const char *cmd) { m_szCommand = cmd; };

public:
	const char	*m_szCommand;	// execute this command

private:
	char		m_szCommandBuffer[1024];	// buffer for received messages

};

class NET_SetConVar : public CNetMessage
{
public:
	bool ReadFromBuffer(bf_read &buffer) { 
		int numvars = buffer.ReadByte();
		m_ConVars.RemoveAll();

		for (int i = 0; i< numvars; i++)
		{
			cvar_t cvar;
			buffer.ReadString(cvar.name, sizeof(cvar.name));
			buffer.ReadString(cvar.value, sizeof(cvar.value));
			m_ConVars.AddToTail(cvar);

		}
		return !buffer.IsOverflowed();
	};

	bool WriteToBuffer(bf_write &buffer) {
		buffer.WriteUBitLong(5, 6);

		int numvars = m_ConVars.Count();

		// Note how many we're sending
		buffer.WriteByte(numvars);

		for (int i = 0; i< numvars; i++)
		{
			cvar_t * cvar = &m_ConVars[i];
			buffer.WriteString(cvar->name);
			buffer.WriteString(cvar->value);
		}

		return !buffer.IsOverflowed();
	};
	const char		*ToString() const {
		char s_text[1024];
		sprintf_s(s_text, "%s: %i cvars, \"%s\"=\"%s\"",
			GetName(), m_ConVars.Count(),
			m_ConVars[0].name, m_ConVars[0].value);
		return s_text;
	};
	int				GetType() const { return 5; }
	const char		*GetName() const { return "net_SetConVar"; }

	void *m_pMessageHandler;
	bool Process() { return false; };
	//	DECLARE_NET_MESSAGE(SetConVar);
	//

	int	GetGroup() const { return INetChannelInfo::STRINGCMD; }

	NET_SetConVar() {}
	NET_SetConVar(const char * name, const char * value) {
		cvar_t cvar;
		strcpy_s(cvar.name, name);
		strcpy_s(cvar.value, value);
		m_ConVars.AddToTail(cvar);
	}

public:

	typedef struct cvar_s
	{
		char	name[260];
		char	value[260];
	} cvar_t;

	CUtlVector<cvar_t> m_ConVars;
};

#define NET_TICK_SCALEUP	100000.0f
#define clamp(val, min, max) (((val) > (max)) ? (max) : (((val) < (min)) ? (min) : (val)))

class NET_Tick : public CNetMessage
{
public:
	bool			ReadFromBuffer(bf_read &buffer) {
		m_nTick = buffer.ReadLong();
		m_flHostFrameTime = (float)buffer.ReadUBitLong(16) / NET_TICK_SCALEUP;
		m_flHostFrameTimeStdDeviation = (float)buffer.ReadUBitLong(16) / NET_TICK_SCALEUP;
		return !buffer.IsOverflowed();
	};
	bool			WriteToBuffer(bf_write &buffer) {
		buffer.WriteUBitLong(GetType(), 6);
		buffer.WriteLong(m_nTick);
		buffer.WriteUBitLong(clamp((int)(NET_TICK_SCALEUP * m_flHostFrameTime), 0, 65535), 16);
		buffer.WriteUBitLong(clamp((int)(NET_TICK_SCALEUP * m_flHostFrameTimeStdDeviation), 0, 65535), 16);
		return !buffer.IsOverflowed();
	};
	const char		*ToString() const {
		char s_text[1024];
		sprintf_s(s_text, "%s: tick %i", GetName(), m_nTick);
		return s_text;
	};
	int				GetType() const { return 3; }
	const char		*GetName() const { return "net_Tick"; }
	void *m_pMessageHandler;
	bool Process() { return true; };
	//	DECLARE_NET_MESSAGE(SetConVar);
	//

	int	GetGroup() const { return INetChannelInfo::GENERIC; }

	NET_Tick()
	{
		m_bReliable = false;
		m_flHostFrameTime = 0;
		m_flHostFrameTimeStdDeviation = 0;
	};

	NET_Tick(int tick, float host_frametime, float host_frametime_stddeviation)
	{
		m_bReliable = false;
		m_nTick = tick;
		m_flHostFrameTime = host_frametime;
		m_flHostFrameTimeStdDeviation = host_frametime_stddeviation;
	};

public:
	int			m_nTick;
	float		m_flHostFrameTime;
	float		m_flHostFrameTimeStdDeviation;
};

class CLC_ClientInfo : public CNetMessage
{
public:
	bool CLC_ClientInfo::ReadFromBuffer(bf_read &buffer)
	{

		m_nServerCount = buffer.ReadLong();
		m_nSendTableCRC = buffer.ReadLong();
		m_bIsHLTV = buffer.ReadOneBit() != 0;
		m_nFriendsID = buffer.ReadLong();
		buffer.ReadString(m_FriendsName, sizeof(m_FriendsName));

		for (int i = 0; i<MAX_CUSTOM_FILES; i++)
		{
			if (buffer.ReadOneBit() != 0)
			{
				m_nCustomFiles[i] = buffer.ReadUBitLong(32);
			}
			else
			{
				m_nCustomFiles[i] = 0;
			}
		}


		return !buffer.IsOverflowed();
	}
	bool CLC_ClientInfo::WriteToBuffer(bf_write &buffer)
	{
		buffer.WriteUBitLong(GetType(), NETMSG_TYPE_BITS);

		buffer.WriteLong(m_nServerCount);
		buffer.WriteLong(m_nSendTableCRC);
		buffer.WriteOneBit(m_bIsHLTV ? 1 : 0);
		buffer.WriteLong(m_nFriendsID);
		buffer.WriteString(m_FriendsName);

		for (int i = 0; i<MAX_CUSTOM_FILES; i++)
		{
			if (m_nCustomFiles[i] != 0)
			{
				buffer.WriteOneBit(1);
				buffer.WriteUBitLong(m_nCustomFiles[i], 32);
			}
			else
			{
				buffer.WriteOneBit(0);
			}
		}

		return !buffer.IsOverflowed();
	}
	const char		*ToString() const {
		char s_text[1024];
		snprintf(s_text, sizeof(s_text), "%s: SendTableCRC %i", GetName(),
			m_nSendTableCRC);
		return s_text;
	};
	int				GetType() const { return 8; }
	const char		*GetName() const { return "clc_ClientInfo"; }
	void *m_pMessageHandler;
	bool Process() { return true; };


	int	GetGroup() const { return INetChannelInfo::GENERIC; }

public:
	CRC32_t			m_nSendTableCRC;
	int				m_nServerCount;
	bool			m_bIsHLTV;
	uint32			m_nFriendsID;
	char			m_FriendsName[MAX_PLAYER_NAME_LENGTH];
	CRC32_t			m_nCustomFiles[MAX_CUSTOM_FILES];
};

class NET_SignonState : public CNetMessage
{
public:
	bool			ReadFromBuffer(bf_read &buffer) {
		m_nSignonState = buffer.ReadByte();
		m_nSpawnCount = buffer.ReadLong();

		return !buffer.IsOverflowed();
	};
	bool			WriteToBuffer(bf_write &buffer) {
		buffer.WriteUBitLong(GetType(), NETMSG_TYPE_BITS);
		buffer.WriteByte(m_nSignonState);
		buffer.WriteLong(m_nSpawnCount);

		return !buffer.IsOverflowed();
	};
	const char		*ToString() const {
		char s_text[1024];
		snprintf(s_text, sizeof(s_text), "%s: state %i, count %i", GetName(), m_nSignonState, m_nSpawnCount);
		return s_text;
	};
	int				GetType() const { return 6; }
	const char		*GetName() const { return "net_SignonState"; }
	void *m_pMessageHandler;
	bool Process() { return true; };
	//	DECLARE_NET_MESSAGE(SetConVar);
	//

	int	GetGroup() const { return INetChannelInfo::SIGNON; }

	NET_SignonState() {};
	NET_SignonState(int state, int spawncount) { m_nSignonState = state; m_nSpawnCount = spawncount; };

public:
	int			m_nSignonState;			// See SIGNONSTATE_ defines
	int			m_nSpawnCount;			// server spawn count (session number)
};

#define Bits2Bytes(b) ((b+7)>>3)
class CLC_Move : public CNetMessage
{
public:
	bool			ReadFromBuffer(bf_read &buffer) {
		m_nNewCommands = buffer.ReadUBitLong(4);
		m_nBackupCommands = buffer.ReadUBitLong(3);
		m_nLength = buffer.ReadWord();
		m_DataIn = buffer;
		return buffer.SeekRelative(m_nLength);
	};
	bool			WriteToBuffer(bf_write &buffer) {
		buffer.WriteUBitLong(GetType(), NETMSG_TYPE_BITS);
		//m_nLength = m_DataOut.GetNumBitsWritten();

		buffer.WriteUBitLong(m_nNewCommands, 4);
		buffer.WriteUBitLong(m_nBackupCommands, 3);

		buffer.WriteWord(m_nLength);

		return buffer.WriteBits(m_DataOut.GetData(), m_nLength);
	};
	const char		*ToString() const {
		char s_text[1024];
		snprintf(s_text, sizeof(s_text), "%s: backup %i, new %i, bytes %i", GetName(),
			m_nNewCommands, m_nBackupCommands, Bits2Bytes(m_nLength));
		return s_text;
	};
	int				GetType() const { return 9; }
	const char		*GetName() const { return "clc_Move"; }
	void *m_pMessageHandler;
	bool Process() { return true; };
	//	DECLARE_NET_MESSAGE(SetConVar);
	//

	int	GetGroup() const { return INetChannelInfo::MOVE; }

	CLC_Move() { m_bReliable = false; }

public:
	int				m_nBackupCommands;
	int				m_nNewCommands;
	int				m_nLength;
	bf_read			m_DataIn;
	bf_write		m_DataOut;
};



class CLC_VoiceData : public CNetMessage
{
public:
	bool			ReadFromBuffer(bf_read &buffer) {
		m_nLength = buffer.ReadWord();	// length in bits
		m_DataIn = buffer;

		return buffer.SeekRelative(m_nLength);
	};
	bool			WriteToBuffer(bf_write &buffer) {
		buffer.WriteUBitLong(GetType(), NETMSG_TYPE_BITS);

	//	m_nLength = m_DataOut.GetNumBitsWritten();

		buffer.WriteWord(m_nLength);	// length in bits

										//Send this client's XUID (only needed on the 360)

		return buffer.WriteBits(m_DataOut.GetBasePointer(), m_nLength);
	};
	const char		*ToString() const {
		char s_text[1024];
		snprintf(s_text, sizeof(s_text), "%s: %i bytes", GetName(), Bits2Bytes(m_nLength));
		return s_text;
	};
	int				GetType() const { return 10; }
	const char		*GetName() const { return "clc_VoiceData"; }
	void *m_pMessageHandler;
	bool Process() { return true; };
	//	DECLARE_NET_MESSAGE(SetConVar);
	//

	int	GetGroup() const { return INetChannelInfo::VOICE; }

	CLC_VoiceData() { m_bReliable = false; };

public:
	int				m_nLength;
	bf_read			m_DataIn;
	bf_write		m_DataOut;
	uint64			m_xuid;
};




class SVC_TempEntities : public CNetMessage
{
public:
	SVC_TempEntities() { m_bReliable = false; }

	int	GetGroup() const { return INetChannelInfo::EVENTS; }

	int			temp;
	int			m_nNumEntries;
	int			m_nLength;
	bf_read		m_DataIn;
	bf_write	m_DataOut;
};
