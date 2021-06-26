#pragma once
//c&p form uc cuz i h4te lua shit's
#include "../../Math\core.hpp"
#include "../../Math\qangle.hpp"
#include "../../Math\vector.hpp"
#include "../../Math\vector2d.hpp"
#include "../../Math\vector4d.hpp"
#include "../../SDK.h"

class ILuaInterface;
class IClientEntity;
class C_GMOD_Player;
class C_BaseCombatWeapon;
class Vector;

#define LUA_OK		0
#define LUA_YIELD	1
#define LUA_ERRRUN	2
#define LUA_ERRSYNTAX	3
#define LUA_ERRMEM	4
#define LUA_ERRGCMM	5
#define LUA_ERRERR	6

struct lua_State
{
	unsigned char	_ud[72];
	ILuaInterface*	m_interface;
};

typedef struct lua_Debug {
	int event;
	const char *name;           /* (n) */
	const char *namewhat;       /* (n) */
	const char *what;           /* (S) */
	const char *source;         /* (S) */
	int currentline;            /* (l) */
	int nups;                   /* (u) number of upvalues */
	int linedefined;            /* (S) */
	int lastlinedefined;        /* (S) */
	char short_src[60];			/* (S) */

								/* Private */
	struct lua_TObject*			_func;  /* active function */
} lua_Debug;

typedef int(__cdecl* CLuaFunction)(lua_State*);

typedef struct luaL_Reg {
	const char *name;
	CLuaFunction func;
} luaL_Reg;

struct UserData {
	void*			data;
	unsigned char	type;
};

enum
{
	SPECIAL_GLOB,		// Global table
	SPECIAL_ENV,		// Environment table
	SPECIAL_REG,		// Registry table
};

#ifdef VECTOR
#undef VECTOR
#endif

namespace Type
{
	enum
	{
		INVALID = -1,
		NIL,
		BOOL,
		LIGHTUSERDATA,
		NUMBER,
		STRING,
		TABLE,
		FUNCTION,
		USERDATA,
		THREAD,

		// UserData
		ENTITY,
		VECTOR,
		ANGLE,
		PHYSOBJ,
		SAVE,
		RESTORE,
		DAMAGEINFO,
		EFFECTDATA,
		MOVEDATA,
		RECIPIENTFILTER,
		USERCMD,
		SCRIPTEDVEHICLE,

		// Client Only
		MATERIAL,
		PANEL,
		PARTICLE,
		PARTICLEEMITTER,
		TEXTURE,
		USERMSG,

		CONVAR,
		IMESH,
		MATRIX,
		SOUND,
		PIXELVISHANDLE,
		DLIGHT,
		VIDEO,
		FILE,

		COUNT
	};

	static const char* Name[] =
	{
		"nil",//0
		"bool",//1
		"lightuserdata",//2
		"number",//3
		"string",//4
		"table",//5
		"function",//6
		"userdata",//7
		"thread",//8
		"entity",//9
		"vector",//10
		"angle",
		"physobj",
		"save",
		"restore",
		"damageinfo",
		"effectdata",
		"movedata",
		"recipientfilter",
		"usercmd",
		"vehicle",
		"material",
		"panel",
		"particle",
		"particleemitter",
		"texture",
		"usermsg",
		"convar",
		"mesh",
		"matrix",
		"sound",
		"pixelvishandle",
		"dlight",
		"video",
		"file"


	};
};

class ILuaObject
{
public:

	virtual void			Set(ILuaObject* obj) = 0;
	virtual void			SetFromStack(int i) = 0;
	virtual void			UnReference() = 0;
	virtual int				GetType(void) = 0;
	virtual const char*		GetString(void) = 0;
	virtual float			GetFloat(void) = 0;
	virtual int				GetInt(void) = 0;
	virtual void*			GetUserData(void) = 0;
	virtual void			SetMember(const char* name) = 0;
	virtual void			SetMember(const char* name, ILuaObject* obj) = 0; // ( This is also used to set nil by passing NULL )
	virtual void			SetMember(const char* name, float val) = 0;
	virtual void			SetMember(const char* name, bool val) = 0;
	virtual void			SetMember(const char* name, const char* val) = 0;
	virtual void			SetMember(const char* name, CLuaFunction f) = 0;
	virtual bool			GetMemberBool(const char* name, bool b = true) = 0;
	virtual int				GetMemberInt(const char* name, int i = 0) = 0;
	virtual float	__thiscall		GetMemberFloat(const char* name, float f = 0.0f) = 0;
	virtual const char*		GetMemberStr(const char* name, const char* = "") = 0;
	virtual void*			GetMemberUserData(const char* name, void* = 0) = 0;
	virtual void*			GetMemberUserData(float name, void* = 0) = 0;
	virtual ILuaObject* 	GetMember(const char* name) = 0;
	virtual ILuaObject* 	GetMember(ILuaObject*) = 0;
	virtual void			SetMetaTable(ILuaObject* obj) = 0;
	virtual void			SetUserData(void* obj) = 0;
	virtual void			Push(void) = 0;
	virtual bool			isNil() = 0;
	virtual bool			isTable() = 0;
	virtual bool			isString() = 0;
	virtual bool			isNumber() = 0;
	virtual bool			isFunction() = 0;
	virtual bool			isUserData() = 0;
	virtual ILuaObject* 	GetMember(float fKey) = 0;
	virtual void*			Remove_Me_1(const char* name, void* = 0) = 0;
	virtual void			SetMember(float fKey) = 0;
	virtual void			SetMember(float fKey, ILuaObject* obj) = 0;
	virtual void			SetMember(float fKey, float val) = 0;
	virtual void			SetMember(float fKey, bool val) = 0;
	virtual void			SetMember(float fKey, const char* val) = 0;
	virtual void			SetMember(float fKey, CLuaFunction f) = 0;
	virtual const char*		GetMemberStr(float name, const char* = "") = 0;
	virtual void			SetMember(ILuaObject* k, ILuaObject* v) = 0;
	virtual bool			GetBool(void) = 0;
	virtual bool			PushMemberFast(int iStackPos) = 0;
	virtual void			SetMemberFast(int iKey, int iValue) = 0;
	virtual void			SetFloat(float val) = 0;
	virtual void			SetString(const char* val) = 0;
	virtual double			GetDouble(void) = 0;
	virtual void			SetMember_FixKey(char  const*, float) = 0;
	virtual void			SetMember_FixKey(char  const*, char  const*) = 0;
	virtual void			SetMember_FixKey(char  const*, ILuaObject *) = 0;
	virtual void			SetMember_FixKey1(char  const*, float) = 0;
	virtual void			SetMember_FixKey2(char  const*, char  const*) = 0;
	virtual bool			isBool(void) = 0;
	virtual void			SetMemberDouble(char  const*, double) = 0;
	virtual void			SetMemberNil(char  const*) = 0;
	virtual void			SetMemberNil(float) = 0;
	virtual bool			Debug_IsUnreferenced() = 0;
	virtual void			Init(void) = 0;
	virtual void			SetFromGlobal(char  const*) = 0;
	virtual void			GetStringLen(unsigned int * out) = 0;
	virtual void			GetMemberUInt(char  const*) = 0;
	virtual void			SetMember(char  const*, unsigned long long) = 0;
	virtual void			SetReference(int i) = 0;
	virtual void			RemoveMember(char  const*) = 0;
	virtual void			RemoveMember(float) = 0;
	virtual bool			MemberIsNil(char  const*) = 0;
	virtual void			SetMemberDouble(float, double) = 0;
	virtual double	__thiscall		GetMemberDouble(char  const*, double) = 0;

};

/*int __cdecl CLuaObject__CLuaObject(int a1)
{
int result; // eax@1

*(_DWORD *)a1 = &off_7C9548;
*(_BYTE *)(a1 + 4) = 0;
*(_DWORD *)(a1 + 12) = -1;
*(_DWORD *)(a1 + 8) = -1;
result = *g_Lua;
*(_DWORD *)(a1 + 16) = *g_Lua;
return result;
}
*/

class CLuaObject : public ILuaObject
{
public:
	int						_unk1;				//0004
	int						_unk2;				//0008
	int						_unk3;				//000C
	ILuaInterface*	lua;				//0010
};

class IThreadedCall
{
public:
};

class ILuaCallback
{
public:
};

namespace Bootil
{
	struct Buffer
	{
		unsigned char u[0x1000];
	};
};

class ILuaInterface
{
public:
	virtual int				Top(void) = 0;
	virtual void			Push(int iStackPos) = 0;
	virtual void			Pop(int iAmt = 1) = 0;
	virtual void			GetTable(int iStackPos) = 0;
	virtual void			GetField(int iStackPos, const char* strName) = 0;
	virtual void			SetField(int iStackPos, const char* strName) = 0;
	virtual void			CreateTable() = 0;
	virtual void			SetTable(int i) = 0;
	virtual void			SetMetaTable(int i) = 0;
	virtual bool			GetMetaTable(int i) = 0;
	virtual void			Call(int iArgs, int iResults) = 0;
	virtual int				PCall(int iArgs, int iResults, int iErrorFunc) = 0;
	virtual int				Equal(int iA, int iB) = 0;
	virtual int				RawEqual(int iA, int iB) = 0;
	virtual void			Insert(int iStackPos) = 0;
	virtual void			Remove(int iStackPos) = 0;
	virtual int				Next(int iStackPos) = 0;
	virtual void*			NewUserdata(unsigned int iSize) = 0;
	virtual void			ThrowError(const char* strError) = 0;
	virtual void			CheckType(int iStackPos, int iType) = 0;
	virtual void			ArgError(int iArgNum, const char* strMessage) = 0;
	virtual void			RawGet(int iStackPos) = 0;
	virtual void			RawSet(int iStackPos) = 0;
	virtual const char*		GetString(int iStackPos = -1, unsigned int* iOutLen = 0) = 0;
	virtual double			GetNumber(int iStackPos = -1) = 0;
	virtual bool			GetBool(int iStackPos = -1) = 0;
	virtual CLuaFunction	GetCFunction(int iStackPos = -1) = 0;
	virtual void*			GetUserdata(int iStackPos = -1) = 0;
	virtual void			PushNil() = 0;
	virtual void			PushString(const char* val, unsigned int iLen = 0) = 0;
	virtual void			PushNumber(double val) = 0;
	virtual void			PushBool(bool val) = 0;
	virtual void			PushCFunction(CLuaFunction val) = 0;
	virtual void			PushCClosure(CLuaFunction val, int iVars) = 0;
	virtual void			PushUserdata(void*) = 0;
	virtual int				ReferenceCreate() = 0;
	virtual void			ReferenceFree(int i) = 0;
	virtual void			ReferencePush(int i) = 0;
	virtual void			PushSpecial(int iType) = 0;
	virtual bool			IsType(int iStackPos, int iType) = 0;
	virtual int				GetType(int iStackPos) = 0;
	virtual const char*		GetTypeName(int iType) = 0;
	virtual void			CreateMetaTableType(const char* strName, int iType) = 0;
	virtual const char*		CheckString(int iStackPos = -1) = 0;
	virtual double			CheckNumber(int iStackPos = -1) = 0;
	virtual size_t			ObjLen(int index) = 0;
	virtual QAngle*			GetAngle(int index) = 0;
	virtual Vector*			GetVector(int index) = 0;
	virtual void			PushAngle(QAngle const& ang) = 0;
	virtual void			PushVector(Vector const& vec) = 0;
	virtual void			SetState(lua_State* state) = 0;
	virtual void			CreateMetaTable(const char* strName) = 0;
	virtual void			PushMetaTable(const char* strName) = 0;
	virtual void			PushUserType(const char* strName) = 0;
	virtual void			SetUserType(const char* strName) = 0;
	virtual bool			Init(ILuaCallback* pCallBack, bool bLoadJit) = 0;
	virtual void			Shutdown(void) = 0;
	virtual void*			Cycle(void) = 0;
	virtual ILuaObject*		Global() = 0;
	virtual ILuaObject*		GetObjectC(int index) = 0;
	virtual void			PushLuaObject(ILuaObject *obj) = 0;
	virtual void			PushLuaFunction(CLuaFunction func) = 0;
	virtual void			LuaError(const char *err, int index) = 0;
	virtual void			TypeError(const char *name, int index) = 0;
	virtual void			CallInternal(int args, int rets) = 0;
	virtual void			CallInternalNoReturns(int args) = 0;
	virtual bool			CallInternalGetBool(int args) = 0;
	virtual const char*		CallInternalGetString(int args) = 0;
	virtual bool			CallInternalGet(int args, ILuaObject *obj) = 0;
	virtual void			NewGlobalTable(const char *name) = 0;
	virtual ILuaObject*		NewTemporaryObject() = 0;
	virtual bool			isUserData(int index) = 0;
	virtual ILuaObject*		GetMetaTableObject(const char *name, int type) = 0;
	virtual ILuaObject*		GetMetaTableObject(int index) = 0;
	virtual ILuaObject*		GetReturn(int index) = 0;
	virtual bool			IsServer() = 0;
	virtual bool			IsClient() = 0;
	virtual bool			IsMenu() = 0;
	virtual void			DestroyObject(ILuaObject *obj) = 0;
	virtual  ILuaObject*	__thiscall	CreateObject() = 0;
	virtual void			SetMember(ILuaObject *table, ILuaObject *key, ILuaObject *value) = 0;
	virtual void			GetNewTable() = 0;
	virtual void			SetMember(ILuaObject *table, float key) = 0;
	virtual void			SetMember(ILuaObject *table, float key, ILuaObject *value) = 0;
	virtual void			SetMember(ILuaObject *table, const char *key) = 0;
	virtual void			SetMember(ILuaObject *table, const char *key, ILuaObject *value) = 0;
	virtual void			SetType(int type) = 0;
	virtual void			PushLong(long num) = 0;
	virtual int				GetFlags(int index) = 0;
	virtual bool			FindOnObjectsMetaTable(int objIndex, int keyIndex) = 0;
	virtual bool			FindObjectOnTable(int tableIndex, int keyIndex) = 0;
	virtual void			SetMemberFast(ILuaObject *table, int keyIndex, int valueIndex) = 0;
	virtual bool			RunString(const char *filename, const char *path, const char *stringToRun, bool run, bool showErrors) = 0;
	virtual bool			IsEqual(ILuaObject *objA, ILuaObject *objB) = 0;
	virtual void			Error(const char *err) = 0;
	virtual const char*		GetStringOrError(int index) = 0;
	virtual bool			RunLuaModule(const char *name) = 0;
	virtual bool			FindAndRunScript(const char *filename, bool run, bool showErrors, const char *) = 0;
	virtual void			SetPathID(const char *pathID) = 0;
	virtual const char*		GetPathID() = 0;
	virtual void			ErrorNoHalt(const char *fmt, ...) = 0;
	virtual void			Msg(const char *fmt, ...) = 0;
	virtual void			PushPath(const char *path) = 0;
	virtual void			PopPath() = 0;
	virtual const char*		GetPath() = 0;
	virtual int				GetColor(int index) = 0;
	virtual void			PushColor(int r, int g, int b, int a) = 0;
	virtual int				GetStack(int level, lua_Debug *dbg) = 0;
	virtual int				GetInfo(const char *what, lua_Debug *dbg) = 0;
	virtual const char*		GetLocal(lua_Debug *dbg, int n) = 0;
	virtual const char*		GetUpvalue(int funcIndex, int n) = 0;
	virtual void			RunStringEx(const char *filename, const char *path, const char *stringToRun, bool run, bool showErrors, bool) = 0;
	virtual size_t			GetDataString(int index, const char **str) = 0;
	virtual void			ErrorFromLua(const char *fmt, ...) = 0;
	virtual void			GetCurrentLocation() = 0;
	virtual void			MsgColour(void* col, const char *fmt, ...) = 0;
	virtual void			GetCurrentFile(std::string &outStr) = 0;
	virtual void			CompileString(Bootil::Buffer &dumper, const std::string &stringToCompile) = 0;
	virtual bool			CallFunctionProtected(int, int, bool) = 0;
	virtual void			Require(const char *name) = 0;
	virtual const char*		GetActualTypeName(int type) = 0;
	virtual void			SetupInfiniteLoopProtection(void) = 0;
};

enum LuaInterfaceType : unsigned char
{
	LUAINTERFACE_CLIENT = 0,
	LUAINTERFACE_SERVER = 1,
	LUAINTERFACE_MENU = 2
};

class Lua_Shared {
public:
	virtual void			Destructor() = 0;
	virtual void			Init(void*, bool, void* *, void*) = 0;
	virtual void			Shutdown(void) = 0;
	virtual void			DumpStats(void) = 0;
	virtual ILuaInterface*	CreateLuaInterface(LuaInterfaceType type, bool) = 0;
	virtual int				CloseLuaInterface(ILuaInterface*) = 0;
	virtual ILuaInterface*	GetLuaInterface(LuaInterfaceType type) = 0;
	virtual void*			LoadFile(std::string const&, const char*, bool from_datatable, bool create_chache) = 0;
	virtual void			GetCache(void*) = 0;
	virtual void			MountLua(char  const*) = 0;
	virtual void			MountLuaAdd(char  const*, char  const*) = 0;
	virtual void			UnMountLua(char  const*) = 0;
	virtual void			SetFileContents(char  const*, char  const*) = 0;
	virtual void			SetLuaFindHook(void *) = 0;
	virtual void			FindScripts(void*, void*, void*) = 0;

};


class CGameMode : public CLuaObject
{
public:
	unsigned char		gamemodeObject[0x14];	//0014
	unsigned char		obj2[0x14];				//0028
	unsigned char		hookCallObject[0x14];	//003C
	char				name[0x10];				//0050
	int					unk1;					//0060
	int					unk2;					//0064

	__forceinline ILuaObject* GetHookCall() {
		return (ILuaObject*)&hookCallObject;
	}

	//static CGameMode* Singleton();
};
//struct CUserCmd;
class cColor;
namespace Lua {
	enum Lua_Role {
		invalid = -1337,
		INNOCENT = 0,
		DETECTIVE,
		TRAITOR
	};
	enum KeypadStatus {
		Status_None,
		Status_Granted,
		Status_Denied
	};

	int GetTeam(int EntIdx);;
	cColor GetTeamColor(int TeamIdx);;
	const char* GetName(int EntIdx);;
	const char * GetTeamName(int TeamIdx);;
	Lua_Role GetRole(int EntIdx);;
	bool IsKeypad(int EntIdx);;
	const char * GetKeypadStaticCode(int EntIdx);;
	const char * GetKeypadTempCode(int EntIdx);;
	bool GetSecureKeypad(int EntIdx);;
	KeypadStatus GetStatusKeypad(int EntIdx);
	Vector * CalculateRenderPos(int EntIdx);
	QAngle * CalculateRenderAng(int EntIdx);
	double GetScale(int EntIdx);
	Vector* GetAimVector(int EntIdx);
	Vector* GetForward(int EntIdx);
	Vector * GetRight(int EntIdx);
	Vector * GetUp(int EntIdx);
	double GetWidth2D(int EntIdx);
	double GetHeight2D(int EntIdx);
	Vector* GetEyePos(int EntIdx);
	Vector* IntersectRayWithPlane(Vector rayOrigin, Vector rayDirection, Vector planePosition, Vector planeNormal);;
	Vector2D CalculateKeypadCursorPos(IClientEntity*Enemy, IClientEntity*Keypad);
	int GetHoveredElement(IClientEntity * Enemy, IClientEntity * Keypad);

	bool weapon_is_m9k(IClientEntity*weapon);


	const char * GetWeaponBase(int EntIdx);
	QAngle fas2_nospread(C_GMOD_Player* ply, C_BaseCombatWeapon*weap,CUserCmd* pCmd);
	QAngle cw2_nospread(C_GMOD_Player* ply, C_BaseCombatWeapon*weap, CUserCmd* pCmd);

	bool m9k_wallscan(const Vector &vecEyePos, const Vector &point, const C_BaseCombatWeapon * weap, C_GMOD_Player*& ply_out);


	bool weapon_is_cw2(IClientEntity*weapon);
	bool cw2_wallscan(const Vector &vecEyePos, const Vector &point, const C_BaseCombatWeapon * weap, C_GMOD_Player*& ply_out);
}