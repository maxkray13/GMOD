
#include "../SDK.h"


#include "Entity.h"

bool IClientEntity::IsValid()
{
	return ((DWORD)this) && !IsBadReadPtr((void*)this, 0x1000 ) && !IsDormant();
}
char * IClientEntity::GetScriptName()
{
	char * ret = nullptr;

	if (SDK::ClassId::CSENT_anim() == GetClientClass()->m_ClassID) {
		ret = SENT_anim_script_name();
		goto endl;
	}
	if (SDK::ClassId::CSENT_point() == GetClientClass()->m_ClassID) {
		ret = SENT_point_script_name();
		goto endl;
	}
	if (SDK::ClassId::CLuaNextBot() == GetClientClass()->m_ClassID) {
		ret = LuaNextBot_script_name();
		goto endl;
	}
	if (SDK::ClassId::CSENT_AI() == GetClientClass()->m_ClassID) {
		ret = SENT_AI_script_name();
		goto endl;
	}
	if (SDK::ClassId::CWeaponSWEP() == GetClientClass()->m_ClassID) {
		ret = WeaponSWEP_script_name();
		goto endl;
	}

	return ret;
endl:;
	//if (IsBadStringPtr(ret, 0x4) || IsBadReadPtr(ret, 0x4)) {
	//	ret = nullptr;
	//}

	return ret;
}
bool IClientEntity::GetPredictable()
{
	return *(bool*)(this + 0x17F);
}
int IClientEntity::RestoreData(const char * context, int slot, int type)
{
	using RestoreData_fn = int(__thiscall*)(void*, const char * context, int slot, int type);
	return Core::Memory::vfunc<RestoreData_fn>(this, 115)(this, context, slot, type);
}
QAngle* IClientEntity::GetRenderAngle()
{
	//EmitSound: pitch out of bounds = %i\n
	using renderang_fn = QAngle * (__thiscall*)(void*);
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Engine.GetStartAddr(), SDK::Engine.GetSize(),
			(const char *)"\x75\xCC\x8B\xCC\x8B\xCC\xFF\xCC\xCC\xF3\x0F",
			0xCC);
		offset += 0x6;
		offset = Core::Memory::get_disp(offset);
		offset /= 4;
	}
	return Core::Memory::vfunc<renderang_fn>(this, offset)(this);

	//return QAngle();
}
Vector* IClientEntity::GetRenderPos()
{
	using renderpos_fn = Vector * (__thiscall*)(void*);
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xFF\xCC\xCC\xCC\xCC\xCC\x8B\xCC\x8B\xCC\x8B\xCC\xFF\xCC\xCC\xF3",
			0xCC);
		offset += 12;
		offset = Core::Memory::get_disp(offset);//*(char*)offset; //Core::Memory::get_disp(offset);
		offset /= 4;
	}
	return Core::Memory::vfunc<renderpos_fn>(this, offset)(this);
}
void IClientEntity::GetVectors(Vector * pForward, Vector * pRight, Vector * pUp)
{
	const matrix3x4_t &entityToWorld = m_rgflCoordinateFrame();

	if (pForward != NULL)
	{
		MatrixGetColumn(entityToWorld, 0, *pForward);
	}

	if (pRight != NULL)
	{
		MatrixGetColumn(entityToWorld, 1, *pRight);
		*pRight *= -1.0f;
	}

	if (pUp != NULL)
	{
		MatrixGetColumn(entityToWorld, 2, *pUp);
	}

}
C_BaseEntity * C_BaseEntity::GetBaseEntity(int index)
{
	auto client_entity = g_pEntList->GetClientEntity(index);
	if (!client_entity->IsValid())
		return nullptr;
	if (!client_entity->GetBaseEntity())
		return nullptr;
	return (client_entity ? (C_BaseEntity *)client_entity : nullptr);
}
bool C_BaseEntity::IsWeapon()
{
	using IsWeapon_fn = bool(__thiscall*)(void*);
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x8B\x80\xCC\xCC\xCC\xCC\xFF\xD0\x84\xC0\x75\x15\x8B\x0D\xCC\xCC\xCC\xCC\x68\xCC\xCC\xCC\xCC\x8B\x01\xFF\x90\xCC\xCC\xCC\xCC\x33\xF6\x8B\x06\x8B\xCE\xFF\x90\xCC\xCC\xCC\xCC\x5E\x85\xC0\x74\x19",//xx????xxxxxxxx????x????xxxx????xxxxxxxx????xxxxx
			0xCC);
		offset = Core::Memory::get_disp(offset);
		offset /= 4;
	}
	return Core::Memory::vfunc<IsWeapon_fn>(this, offset)(this);
}
float C_BaseEntity::m_flSpawnTime()
{
	return *(float*)(DWORD(this) + 0x148);
}
const char * C_BaseEntity::GetTeamName()
{
	using t = const char * (__stdcall*)(int);
	static t fn = NULL;
	if (!fn) {
		fn = (t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\xFF\xCC\x08\xE8\xCC\xCC\xCC\xCC\x8B\xCC\x83\xC4\x04\x85\xCC\x75\xCC\xB8",//xxxxxxxxxx
			0xCC);
	}
	return fn(this->m_iTeamNum());
}
QAngle & C_BaseAnimating::GetAbsAngles()
{
	using GetAbsAngles_td = QAngle & (__thiscall*)(void*);
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xFF\xCC\xCC\x8B\xCC\xCC\xCC\xCC\xCC\xCC\x8D\xCC\xCC\xCC\x8D\xCC\xCC\xCC\xFF\xCC\xCC\xCC\xCC\xCC\x8B",
			0xCC);
		offset = Core::Memory::get_disp(offset);
		offset /= 4;
	}
	return Core::Memory::vfunc<GetAbsAngles_td>(this, 140)(this);
}
int & C_BaseAnimating::m_iPrevBoneMask()
{
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x83\xCC\xFF\x75\xCC\x8B\xCC\xCC\xCC\xCC\xCC\xA1",//xx????xxxxxxxx????x????xxxx????xxxxxxxx????xxxxx
			0xCC);
		offset += 5u;
		offset = Core::Memory::get_disp(offset);
		offset += 4;
	}

	return *(int*)(DWORD(this) + offset);
}
bool C_BaseAnimating::IsToolRecording()
{
	return *(bool *)(DWORD(this) + 181);
}
CThreadFastMutex & C_BaseAnimating::m_BoneSetupLock()
{
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x8D\xCC\xCC\xCC\xCC\xCC\x3B\xCC\x74\xCC\x33",
			0xCC);
		offset = Core::Memory::get_disp(offset);
		offset += 4;
	}

	return *(CThreadFastMutex*)(DWORD(this) + offset);
}
unsigned long & C_BaseAnimating::m_iMostRecentModelBoneCounter()
{
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x8B\xCC\xCC\xCC\xCC\xCC\x3B\xCC\xCC\xCC\xCC\xCC\x74\xCC\x8B\xCC\x8B",
			0xCC);
		offset = Core::Memory::get_disp(offset);
		offset += 4;
	}

	return *(unsigned long*)(DWORD(this) + offset);
}
float & C_BaseAnimating::m_flLastBoneSetupTime()
{
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xD9\xCC\xCC\xCC\xCC\xCC\xD9\xCC\xDF\xCC\xDD\xCC\x72\xCC\xF3\xCC\xCC\xCC\xCC\xC7",
			0xCC);
		offset = Core::Memory::get_disp(offset);
		offset += 4;
	}

	return *(float*)(DWORD(this) + offset);
}
CBoneAccessor & C_BaseAnimating::m_BoneAccessor()
{//CStudioHdr *C_BaseAnimating::OnNewModel()
	static auto offset = 0u;//LookupAttachment( "eyes" );
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x89\xCC\xCC\xCC\xCC\xCC\x89\xCC\xCC\xCC\xCC\xCC\x8B\xCC\xCC\xCC\xCC\xCC\x85\xCC\x74\xCC\x8B\xCC\xE8",
			0xCC);
		offset = Core::Memory::get_disp(offset);
	}

	return *(CBoneAccessor*)(DWORD(this) + offset);
}

int & C_BaseAnimating::m_iAccumulatedBoneMask()
{
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x8B\xCC\xCC\xCC\xCC\xCC\x8B\xCC\x89\xCC\xCC\xCC\xCC\xCC\xC7\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x85",
			0xCC);
		offset = Core::Memory::get_disp(offset);
		offset += 4;
	}

	return *(int*)(DWORD(this) + offset);
}

CIKContext ** C_BaseAnimating::m_pIk()
{
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xC7\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\xCC\x33\xCC\x85\xCC\x74\xCC\x8B\xCC\xE8",
			0xCC);
		offset = Core::Memory::get_disp(offset);
		offset += 4;
	}

	return (CIKContext**)(DWORD(this) + offset);
}

bool C_BaseAnimating::IsRagdoll()
{
	using IsRagdoll_td = bool(__thiscall*)(void*);
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x8B\xCC\xCC\xCC\xCC\xCC\xFF\xCC\x84\xCC\x75\xCC\x8B\xCC\xD9",
			0xCC);
		offset = Core::Memory::get_disp(offset);
		offset /= 4;
	}
	return Core::Memory::vfunc<IsRagdoll_td>(this, offset)(this);
}

void C_BaseAnimating::UpdateIKLocks(float currentTime)
{
	using UpdateIKLocks_td = void(__thiscall*)(void*, float);
	static UpdateIKLocks_td UpdateIKLocks_fn = NULL;

	if (!UpdateIKLocks_fn) {
		UpdateIKLocks_fn = (UpdateIKLocks_td)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x83\xEC\x0C\x8B\xCC\xCC\x89\xCC\xCC\x8B",
			0xCC)));
	}
	return UpdateIKLocks_fn(this, currentTime);

}

void C_BaseAnimating::CalculateIKLocks(float currentTime)
{
	using CalculateIKLocks_td = void(__thiscall*)(void*, float);
	static CalculateIKLocks_td CalculateIKLocks_fn = NULL;

	if (!CalculateIKLocks_fn) {
		CalculateIKLocks_fn = (CalculateIKLocks_td)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x81\xEC\x44\x04",
			0xCC)));
	}
	return CalculateIKLocks_fn(this, currentTime);
}

void C_BaseAnimating::BuildTransformations(CStudioHdr * hdr, Vector * pos, Quaternion * q, const matrix3x4_t & cameraTransform, int boneMask, CBoneBitList & boneComputed)
{//7 args
	using BuildTransformations_td = void(__thiscall*)(void*, CStudioHdr * hdr, Vector * pos, Quaternion * q, const matrix3x4_t & cameraTransform, int boneMask, CBoneBitList & boneComputed, void*);
	static BuildTransformations_td BuildTransformations_fn = NULL;

	if (!BuildTransformations_fn) {
		BuildTransformations_fn = (BuildTransformations_td)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x81\xEC\xB4\x01\xCC\xCC\xCC\x8B",
			0xCC)));
	}

	//return Core::Memory::vfunc<BuildTransformations_td>(this, 205)(this, hdr, pos, q, cameraTransform, boneMask, boneComputed,NULL);
	//205
	return BuildTransformations_fn(this, hdr, pos, q, cameraTransform, boneMask, boneComputed, NULL);

}

CUtlVector<matrix3x4_t>& C_BaseAnimating::m_CachedBoneData()
{

	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xFF\xCC\xCC\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\x83\xCC\xCC\xB3",
			0xCC);
		offset = Core::Memory::get_disp(offset);
		offset += 4;
	}

	return *(CUtlVector<matrix3x4_t>*)(DWORD(this) + offset);
}

bool C_BaseAnimating::SetupBones_AttachmentHelper(CStudioHdr * hdr)
{
	using SetupBones_AttachmentHelper_td = bool(__thiscall*)(void*, CStudioHdr * hdr);
	static SetupBones_AttachmentHelper_td SetupBones_AttachmentHelper_fn = NULL;

	if (!SetupBones_AttachmentHelper_fn) {
		SetupBones_AttachmentHelper_fn = (SetupBones_AttachmentHelper_td)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x83\xEC\x70\xCC\x8B\xCC\xCC\xCC\x8B\xCC\x89\xCC\xCC\x85\xCC\x75",
			0xCC)));
	}
	return SetupBones_AttachmentHelper_fn(this, hdr);
}

void C_BaseAnimating::ControlMouth(CStudioHdr * pstudiohdr)
{
	using ControlMouth_td = void(__thiscall*)(void*, CStudioHdr *);
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xFF\xCC\xCC\xCC\xCC\xCC\xF7\xCC\xCC\xCC\xCC\xCC\xCC\x75\xCC\xF7",
			0xCC);
		offset = Core::Memory::get_disp(offset);
		offset /= 4;
	}
	return Core::Memory::vfunc<ControlMouth_td>(this, offset)(this, pstudiohdr);
	//\xFF\xCC\xCC\xCC\xCC\xCC\xF7\xCC\xCC\xCC\xCC\xCC\xCC\x75\xCC\xF7
}

void C_BaseAnimating::StandardBlendingRules(CStudioHdr * hdr, Vector pos[], Quaternion q[], float currentTime, int boneMask)
{
	using StandardBlendingRules_td = void(__thiscall*)(void*, CStudioHdr * hdr, Vector pos[], Quaternion q[], float currentTime, int boneMask);
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xFF\xCC\xCC\xCC\xCC\xCC\x8B\xCC\xCC\xC7\xCC\xCC\xCC\xCC\xCC\xCC\x85",
			0xCC);
		offset = Core::Memory::get_disp(offset);
		offset /= 4;
	}
	return Core::Memory::vfunc<StandardBlendingRules_td>(this, offset)(this, hdr, pos, q, currentTime, boneMask);
}
#pragma optimize(off)
/*void C_BaseAnimating::AccumulatePose( CStudioHdr *pStudioHdr, CIKContext *pIKContext, Vector pos[], Quaternion q[], int sequence, float cycle,  float poseParameter[], int boneMask, float flWeight, float flTime)
{
	using AccumulatePose_td = void(__stdcall*)(const CStudioHdr * pStudioHdr, CIKContext * pIKContext, Vector pos[], Quaternion q[], int sequence, float cycle, const float poseParameter[], int boneMask, float flWeight, float flTime);
	static AccumulatePose_td AccumulatePose_fn = NULL;

	if (!AccumulatePose_fn) {
		auto off = (uintptr_t)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xE8\xCC\xCC\xCC\xCC\xD9\xCC\xCC\x8D\xCC\xCC\xCC\xFF\xCC\xCC\xCC\xD9\xCC\xCC\xCC\x8B",
			0xCC)));

		AccumulatePose_fn = (AccumulatePose_td)Core::Memory::follow_to(off);
	}

	auto fn = SDK::Client.GetStartAddr() + 0x10A7E0;
	__declspec (align(16)) struct memed {
		CStudioHdr * hdr;
		int boneMask;
		float* poseParameter;
	};
	auto kk = poseParameter;
	__declspec (align(16))memed userpurge = {};
	userpurge.hdr = pStudioHdr;
	userpurge.boneMask = boneMask;
	userpurge.poseParameter = poseParameter;



	__declspec (align(16))uint32_t buff[15] = {};
	*buff = (uint32_t)pStudioHdr;
	buff[1] = (uint32_t)boneMask;
	buff[2] = (uint32_t)poseParameter;
	buff[3] = NULL;



	char buff_ret[260] = {};

	using meme_fn = void(__thiscall*)(memed** mem, CIKContext *, Vector pos[], Quaternion q[], int sequence, float cycle, float flWeight, float flTime);
	_asm {
		push pIKContext;
		fld time;
		sub esp, 0xC;
		fstp[esp + 0x8];
		fld flWeight;
		fstp[esp + 0x4];
		fld cycle;
		fstp[esp];
		push sequence;
		push q;
		push pos;
		lea ecx, buff;
		call fn;
		add esp, 0x1C;
	}

	return;
}*/
#pragma optimize(on)
/*void C_BaseAnimating::CalcAutoplaySequences(const CStudioHdr * pStudioHdr, CIKContext * pIKContext, Vector pos[], Quaternion q[], const float poseParameters[], int boneMask, float realTime)
{
	using CalcAutoplaySequences_td = void(__thiscall*)(const CStudioHdr * pStudioHdr, CIKContext * pIKContext, Vector pos[], Quaternion q[], const float poseParameters[], int boneMask, float realTime);
	static CalcAutoplaySequences_td CalcAutoplaySequences_fn = NULL;

	if (!CalcAutoplaySequences_fn) {
		auto off = (uintptr_t)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xE8\xCC\xCC\xCC\xCC\x8B\xCC\xCC\x8B\xCC\x83\xCC\xCC\xCC\xCC\xCC\xCC\x74\xCC\x8B\xCC\x8D",
			0xCC)));
		CalcAutoplaySequences_fn = (CalcAutoplaySequences_td)Core::Memory::follow_to(off);
	}
	return CalcAutoplaySequences_fn( pStudioHdr, pIKContext, pos,  q, poseParameters,  boneMask, realTime);
}*/
void C_BaseAnimating::CalcBoneAdj(const CStudioHdr * pStudioHdr, Vector pos[], Quaternion q[], const float controllers[], int boneMask)
{
	using CalcBoneAdj_td = void(__cdecl*)(const CStudioHdr * pStudioHdr, Vector pos[], Quaternion q[], const float controllers[], int boneMask);
	static CalcBoneAdj_td CalcBoneAdj_fn;

	if (!CalcBoneAdj_fn) {
		CalcBoneAdj_fn = (CalcBoneAdj_td)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x8B\xCC\xCC\x81\xEC\x80\xCC\xCC\xCC\x8B\xCC\xCC\x33",
			0xCC)));
	}

	CalcBoneAdj_fn(pStudioHdr, pos, q, controllers, boneMask);
}




void C_BaseAnimating::MoveToLastReceivedPosition()
{
	using MoveToLastReceivedPosition_fn = void(__thiscall*)(void*, bool);

	static MoveToLastReceivedPosition_fn MoveToLastReceivedPosition_f;

	if (!MoveToLastReceivedPosition_f) {
		MoveToLastReceivedPosition_f = (MoveToLastReceivedPosition_fn)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x80\x7D\x08\x00\x56\x8B\xF1\x75\x0A",//xxxxxxxxxxxx
			0xCC)));
	}

	MoveToLastReceivedPosition_f(this, true);
}

bool C_BaseAnimating::GetBoneTransform(matrix3x4_t * output, float time)
{
	return SetupBones(output, MAXSTUDIOBONE, 0x0100, time);
}

bool C_BaseAnimating::GetBoneWorld(int index, matrix3x4_t * transform, Vector & output)
{
	if (transform)
	{
		output[0] = transform[index][0][3];
		output[1] = transform[index][1][3];
		output[2] = transform[index][2][3];
	}

	return !output.IsZero();
}

bool C_BaseAnimating::GetBoxBoundWorld(int index, matrix3x4_t * transform, Vector & min, Vector & max)
{
	if (transform)
	{
		auto model = GetModel();

		if (model)
		{
			auto studio = g_pVModelInfo->GetStudiomodel(model);

			if (studio)
			{
				auto box = studio->pHitbox(index, m_nHitboxSet());

				if (box)
				{
					min = box->bbmin.Transform(transform[box->bone]);
					max = box->bbmax.Transform(transform[box->bone]);
				}
			}
		}
	}

	return (!min.IsZero() && !max.IsZero());
}
bool C_BaseAnimating::GetBoxBoundWorld(const char * name, matrix3x4_t * transform, Vector & min, Vector & max)
{
	if (transform)
	{
		auto model = GetModel();

		if (model)
		{
			auto studio = g_pVModelInfo->GetStudiomodel(model);

			if (studio)
			{
				int Bone_Index = -1;
				for (int i = 0; i < studio->numbones; i++)
				{
					mstudiobone_t* t_pBone = studio->pBone(i);
					if (!t_pBone || !(t_pBone->flags & 256) || t_pBone->parent == -1 || !t_pBone->GetName())
						continue;
					if (strcmp(name, t_pBone->GetName()) == 0) {
						Bone_Index = i;
						break;
					}
				}
				if (Bone_Index == -1)
					return false;
				auto hitbox_set = studio->pHitboxSet(m_nHitboxSet());
				if (!hitbox_set)
					return false;
				mstudiobbox_t* target_hitbox = nullptr;
				for (int it = 0; it < hitbox_set->numhitboxes; it++)
				{
					mstudiobbox_t* t_hitbox = hitbox_set->pHitbox(it);
					if (!t_hitbox)
						continue;
					if (t_hitbox->bone = Bone_Index) {
						target_hitbox = t_hitbox;
						break;
					}
				}
				if (!target_hitbox)
					return false;
				min = target_hitbox->bbmin.Transform(transform[target_hitbox->bone]);
				max = target_hitbox->bbmax.Transform(transform[target_hitbox->bone]);
			}
		}
	}

	return (!min.IsZero() && !max.IsZero());
}
bool C_BaseAnimating::GetBoxWorld(const char * name, matrix3x4_t * transform, Vector & output)
{
	Vector min = {};
	Vector max = {};

	if (GetBoxBoundWorld(name, transform, min, max)) {
		if (Engine::Var::Instance().Randomize.Get()) {
			static float random_pos = 0.5f;
			static float random_upd = 0.f;
			float upd_Time = (3.f)*g_pGlobalVars->absoluteframetime;
			random_upd += upd_Time;

			if (random_upd >= 0.9f) {
				random_upd = 0;
				random_pos = SDK::Function::RandomFloat(0.f, 1.f);

			}
			auto delta = max - min;
			output = min + delta * random_pos;
		}
		else {
			output = (min + max) * 0.5f;
		}

	}


	return !output.IsZero();
}
bool C_BaseAnimating::GetBoxWorld(int index, matrix3x4_t * transform, Vector & output)
{
	Vector min = {};
	Vector max = {};

	if (GetBoxBoundWorld(index, transform, min, max)) {
		if (Engine::Var::Instance().Randomize.Get()) {
			static float random_pos = 0.5f;
			static float random_upd = 0.f;
			float upd_Time = (3.f)*g_pGlobalVars->absoluteframetime;
			random_upd += upd_Time;

			if (random_upd >= 0.9f) {
				random_upd = 0;
				random_pos = SDK::Function::RandomFloat(0.f, 1.f);

			}
			auto delta = max - min;
			output = min + delta * random_pos;
		}
		else {
			output = (min + max) * 0.5f;
		}

	}


	return !output.IsZero();
}



bool C_BaseAnimating::Interpolate(float time)
{
	//C_BaseEntity::InterpolateServerEntities
	using t = bool(__thiscall*)(void*, float);
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xD9\xCC\xCC\xD9\xCC\xCC\x8B\xCC\xCC\xCC\xCC\xCC\xFF\xCC\x8B",//D9 ? ? D9 ? ? 8B ? ? ? ? ? FF ? 8B
			0xCC);
		offset += 0x6;
		offset = Core::Memory::get_disp(offset);
		offset /= 4;
		//	Engine::msg_box("offset is 0x%p", offset);
	}

	return Core::Memory::vfunc<t>(this, offset)(this, time);
}

bool & C_BaseAnimating::m_bReadyToDraw()
{
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x75\x07\x33\xDB\xE9\xCC\xCC\xCC\xCC\xA1",//xxxxx????x
			0xCC);
		offset -= 0x4;
		offset = Core::Memory::get_disp(offset);
		offset += 0x4;
	}

	return *(bool*)(DWORD(this) + offset);
}

void C_BaseAnimating::UpdateClientSideAnimation()
{
	using UpdateClientSideAnimation_fn = void(__thiscall*)(void*);
	static UpdateClientSideAnimation_fn UpdateClientSideAnimation = nullptr;
	if (!UpdateClientSideAnimation) {

		UpdateClientSideAnimation = (UpdateClientSideAnimation_fn)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x51\x56\x8B\xF1\x8B\x96",//xxxxxxxxx
			0xCC));
	}
	UpdateClientSideAnimation(this);
}

CStudioHdr * C_BaseAnimating::GetModelPtr()
{
	return *(CStudioHdr **)(DWORD(this) + 0x16CC);;
}

int C_BaseAnimating::LookupPoseParameter(void * model_ptr, const char* name)
{
	using LookupPoseParameter_fn = int(__stdcall*)(void*, const char*);
	static LookupPoseParameter_fn LookupPoseParameter_f = nullptr;
	if (!LookupPoseParameter_f) {
		LookupPoseParameter_f = (LookupPoseParameter_fn)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x57\x8B\x7D\x08\x85\xFF\x75\x07",//xxxxxxxxxxx
			0xCC));
	}
	return LookupPoseParameter_f(model_ptr, name);
}

float C_BaseAnimating::SetPoseParameter(void * model_ptr, int iParameter, float flValue)
{
	using SetPoseParameter_fn = float(__thiscall*)(void*, void*, int, float);
	static SetPoseParameter_fn SetPoseParameter_f = nullptr;
	if (!SetPoseParameter_f) {
		SetPoseParameter_f = (SetPoseParameter_fn)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x8B\x45\x08\xD9\x45\x10",//xxxxxxxxx
			0xCC));

	}
	return SetPoseParameter_f(this, model_ptr, iParameter, flValue);
}

void C_BaseAnimating::InvalidateBoneCache()
{
	return SDK::Function::InvalidateBoneCache(this);
}

Vector C_BaseAnimating::GetNetworkOrigin()
{
	return *(Vector*)((DWORD)(this) + 0x338);
}

QAngle C_BaseAnimating::GetNetworkAngles()
{
	return *(QAngle*)((DWORD)(this) + 0x344);
}

float & C_BaseAnimating::m_flAnimTime()
{
	return *(float*)((DWORD)(this) + 0x60);
}

CUtlVector< C_AnimationLayer >& C_BaseAnimating::m_AnimOverlay()
{
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x8D\xCC\xCC\xCC\xCC\xCC\x8B\xCC\x8D\xCC\xCC\xCC\xCC\xCC\x8B\xCC\xCC\x2B",// 8D ? ? ? ? ? 8B ? 8D ? ? ? ? ? 8B ? ? 2B
			0xCC);
		offset = Core::Memory::get_disp(offset);
	}

	return *(CUtlVector<C_AnimationLayer>*)((DWORD)(this) + offset);
}

int& C_BaseAnimating::m_nSequence()
{
	return *(int*)((DWORD)(this) + 0x1630);
}

float& C_BaseAnimating::m_flCycle()
{
	return *(float*)((DWORD)(this) + 0x1638);
}

float * C_BaseAnimating::m_flPoseParameter()
{
	return (float*)((DWORD)(this) + 0x1494);
}

float * C_BaseAnimating::m_flEncodedController()
{
	return (float*)((DWORD)(this) + 0x15B0);
}

Vector C_BasePlayer::EyePosition()
{
	Vector vec = m_vecOrigin();

	vec.m_z += m_vecViewOffset();
	return vec;
}

void C_BasePlayer::m_pCurrentCommand(CUserCmd * pCmd)
{
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x89\xB7\xCC\xCC\xCC\xCC\xE8\xCC\xCC\xCC\xCC\xA1",//xx????x????x
			0xCC);
		offset = Core::Memory::get_disp(offset);
	}
	if (IsBadReadPtr(this, offset + 0x10) || IsBadWritePtr(this, offset + 0x10))
		return;
	*(CUserCmd**)(DWORD(this) + offset) = pCmd;
	//return *(CUserCmd**)(this + offset);
}

bool C_BasePlayer::IsLocal()
{
	if (!IsGood())
		return false;

	return (entindex() == g_pEngine->GetLocalPlayer());
}

bool C_BasePlayer::IsGood()
{
	return (IsValid() && m_lifeState() == LIFE_ALIVE);
}

bool C_BasePlayer::WeaponAbleToShot()
{
	auto weap = GetActiveWeapon();
	if (!weap->IsValid())
		return false;
	auto clip = weap->m_iClip1();
	auto fire_rate = weap->GetFireRate();

	auto next_attack = weap->m_flNextPrimaryAttack();

	if (fire_rate > 0.0) {

		if (fire_rate < g_pGlobalVars->interval_per_tick) {
			
			return (clip > 0 && SDK::Vars::g_tick % 2 == 0 ? true : false &&next_attack <= this->GetServerTime());
		}
		return (clip > 0 &&next_attack <= this->GetServerTime());
	}
	
	return (clip > 0 && SDK::Vars::g_tick % 2 == 0 ? true : false && next_attack <= this->GetServerTime());
}

MoveType_t C_BasePlayer::GetMoveType()
{
	//client.dll\\
	"PhysicsSimulate: %s bad movetype %d"
	return *(MoveType_t*)((DWORD)(this) + 0x178);
}

void C_BaseEntity::EstimateAbsVelocity(Vector & vel)
{
	using EstimateAbsVelocity_fn = void(__thiscall*)(void*, const Vector &);
	static EstimateAbsVelocity_fn EstimateAbsVelocity_;

	if (!EstimateAbsVelocity_) {
		EstimateAbsVelocity_ = (EstimateAbsVelocity_fn)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x83\xEC\x0C\xCC\x8B\xCC\xE8\xCC\xCC\xCC\xCC\x3B\xCC\x75",//xxxxxxxx????xxxxxxx
			0xCC)));
	}


	return EstimateAbsVelocity_(this, vel);
}

bool C_BaseEntity::Teleported()
{
	using Teleported_td = bool(__thiscall*)(void*);
	static Teleported_td Teleported_fn;

	if (!Teleported_fn) {
		auto off = (uintptr_t)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xE8\xCC\xCC\xCC\xCC\x84\xCC\x75\xCC\x8A\xCC\xCC\x3A",//xxxxxxxx????xxxxxxx
			0xCC)));
		Teleported_fn = (Teleported_td)Core::Memory::follow_to(off);

	}
	return Teleported_fn(this);
}
/*
int& C_BaseEntity::m_nTickBase()
{
	return *(int*)((DWORD)(this) + 0x2580);
}*/

CBasePlayerAnimState * C_BasePlayer::PlayerAnimState()
{
	//\x74\xCC\x83\xCC\xCC\xCC\xCC\xCC\xCC\x74\xCC\x8B\xCC\xCC\xCC\xCC\xCC\x6A\x03
	static DWORD p_anim_st = NULL;
	if (!p_anim_st)
	{
		auto off = (DWORD)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x74\xCC\x83\xCC\xCC\xCC\xCC\xCC\xCC\x74\xCC\x8B\xCC\xCC\xCC\xCC\xCC\x6A\x03",//xxxxxxxx????xxxxxxx
			0xCC)));
		off += 0x4;
		p_anim_st = *(DWORD*)off;
	}
	return *(CBasePlayerAnimState**)((DWORD)(this) + p_anim_st);
}

void C_BasePlayer::SetAbsOrigin(Vector& Origin)
{
	using SetAbsOrigin_fn = void(__thiscall*)(void*, const Vector &);

	static SetAbsOrigin_fn SetAbsOrigin;

	if (!SetAbsOrigin) {
		SetAbsOrigin = (SetAbsOrigin_fn)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x56\x57\x8B\xF1\xE8\xCC\xCC\xCC\xCC\x8B\x7D\x08\xF3\x0F\x10\x07",//xxxxxxxx????xxxxxxx
			0xCC)));
	}


	SetAbsOrigin(this, Origin);
}

void C_BasePlayer::SetAbsAngles(QAngle & Angle)
{
	using SetAbsAngles_fn = void(__thiscall*)(void*, const QAngle &);

	static SetAbsAngles_fn SetAbsAngles;

	if (!SetAbsAngles) {
		SetAbsAngles = (SetAbsAngles_fn)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x81\xEC\xCC\xCC\xCC\xCC\x56\x57\x8B\xF1\xE8\xCC\xCC\xCC\xCC\x8B\x7D\x08",//xxxxx????xxxxx????xxx
			0xCC)));
	}


	SetAbsAngles(this, Angle);
}

VarMapping_t * C_BasePlayer::GetVarMapping()
{
	return (VarMapping_t *)(DWORD(this) + 0x14);
}

void C_BasePlayer::DisableInterpolation()
{
	VarMapping_t* map = GetVarMapping();
	if (!map) return;

	for (int i = 0; i < map->m_nInterpolatedEntries; i++) {
		VarMapEntry_t *e = &map->m_Entries[i];
		e->m_bNeedsToInterpolate = false;
	}
}

void C_BasePlayer::EnableInterpolation()
{
	VarMapping_t* map = GetVarMapping();
	if (!map) return;

	for (int i = 0; i < map->m_nInterpolatedEntries; i++) {
		VarMapEntry_t *e = &map->m_Entries[i];
		e->m_bNeedsToInterpolate = true;
	}
}
/*
QAngle & C_BasePlayer::m_angEyeAngles()
{
	return *(QAngle*)((DWORD)(this) + 0x2bdc);//FRAME_RENDER_START
}
*/
float C_BasePlayer::m_vecViewOffset()
{
	static DWORD off_view = NULL;
	if (!off_view)
	{
		auto offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xF3\x0F\x11\xCC\xCC\xCC\xCC\xCC\xF3\x0F\x11\xCC\xCC\xCC\xCC\xCC\xFF\xCC\xCC\x8B\xCC\xE8",
			0xCC);
		offset += 0xC;
		off_view = *(DWORD*)offset;

	}
	return *(float*)((DWORD)this + off_view);
}
/*
QAngle C_BasePlayer::m_vecPunchAngle()
{
	return *(QAngle*)((DWORD)(this) + 0x22C8);
}

QAngle C_BasePlayer::m_vecPunchAngleVel()
{
	return *(QAngle*)((DWORD)(this) + 0x2304);
}*/

C_GMOD_Player * C_GMOD_Player::GetGPlayer(int index)
{
	auto ent = C_BaseEntity::GetBaseEntity(index);
	return ToGMOD_Player(ToBasePlayer(ent));
}

C_GMOD_Player * C_GMOD_Player::GetLocalPlayer()
{

	if (!((C_GMOD_Player *)g_pClientTools->GetLocalPlayer())->IsValid()) {

		return NULL;
	}

	return reinterpret_cast<C_GMOD_Player *>(g_pClientTools->GetLocalPlayer());
}

void C_GMOD_Player::UpdateButtonState(int Button)
{
	using UpdateButtonState_fn = void(__thiscall*)(void*, int);
	//static UpdateButtonState_fn UpdateButtonState_ = nullptr;
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xFF\xCC\xCC\xCC\xCC\xCC\x8B\xCC\x8D\xCC\xCC\xCC\x8B\xCC\xFF\xCC\xCC\xCC\xCC\xCC\x6A\xCC\x8B",//C_GMOD_Player__UpdateButtonState
			//(const char *)"\x55\x8B\xEC\x8B\xCC\xCC\xCC\x8B\xCC\xCC\xCC\xCC\xCC\x89\xCC\xCC\xCC\xCC\xCC\x33",// 55 8B EC 8B ? ? ? 8B ? ? ? ? ? 89 ? ? ? ? ? 33//C_BasePlayer__UpdateButtonState
			0xCC);
		offset = Core::Memory::get_disp(offset);
		//UpdateButtonState_ = (UpdateButtonState_fn)offset;
		offset /= 4;
	}
	return Core::Memory::vfunc<UpdateButtonState_fn>(this, offset)(this, Button);
}

void C_BaseEntity::RunPreThink()
{
	if (!this->PhysicsRunThink())
		return;
	this->PreThink();
}

void C_BaseEntity::RunThink()
{
	int thinktick = this->GetNextThinkTick();

	if (thinktick <= 0 || thinktick > this->m_nTickBase())
		return;

	SetNextThink(-1.f);

	this->Think();
}

void C_BaseEntity::RunPostThink()
{
	this->PostThink();
}

Vector C_GMOD_Player::GetAttackSpread()
{
	using GetAttackSpread_fn = int(__stdcall*)(Vector *, IClientEntity *, IClientEntity *);
	static GetAttackSpread_fn GetAttackSpread_ = 0;
	if (!GetAttackSpread_) {
		GetAttackSpread_ = (GetAttackSpread_fn)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x8B\x4D\x0C\x85\xC9\x74\x14",//xxxxxxxxxx//55 8B EC 8B 4D 0C 85 C9 74 14
			0xCC));
	}
	Vector out = {};

	auto weap = GetActiveWeapon();
	if (!weap->IsValid())
		return out;

	GetAttackSpread_(&out, weap, NULL);// -2 offset 

	static auto temp = 0x3E05A9A8;
	float def_out = *(float*)&temp;

	if (out[0] == def_out && out[1] == def_out && out[3] == def_out)
		return Vector(0, 0, 0);
	return out;
}

void C_BaseEntity::SetNextThink(float nextThinkTime, const char * szContext)
{
	using SetNextThink_fn = void(__thiscall*)(void*, float, const char *);
	static SetNextThink_fn SetNextThink_func = 0u;
	if (!SetNextThink_func) {
		SetNextThink_func = (SetNextThink_fn)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\xF3\x0F\x10\x45\xCC\x0F\x2E\x05\xCC\xCC\xCC\xCC\x53",//xxxxxxx?xxx????x
			0xCC);
	}
	return SetNextThink_func(this, nextThinkTime, szContext);

}

bool C_BaseEntity::PhysicsRunThink()
{
	using PhysicsRunThink_fn = bool(__thiscall*)(void*, int);
	static PhysicsRunThink_fn PhysicsRunThink_func = 0u;
	if (!PhysicsRunThink_func) {
		PhysicsRunThink_func = (PhysicsRunThink_fn)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x53\x8B\xD9\x56\x57\x8B\x83",//xxxxxxxxxx
			0xCC);
	}
	return PhysicsRunThink_func(this, 0);
}

void C_BaseEntity::PreThink()
{
	using PreThink_fn = void(__thiscall*)(void*);
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x74\x0A\x8B\x07\x8B\xCF\xFF\x90\xCC\xCC\xCC\xCC\x6A\x00",//xxxxxxxx????xx
			0xCC);
		offset += 0x6;
		offset = Core::Memory::get_disp(offset);
		offset /= 4;
	}
	return Core::Memory::vfunc<PreThink_fn>(this, offset)(this);
}

int C_BaseEntity::GetNextThinkTick()//
{
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x55\x8B\xEC\x8B\xCC\xCC\xCC\x8B\xCC\x85\xCC\x75\xCC\x8B\xCC\xCC\xCC\xCC\xCC\x83\xCC\xFF",// 55 8B EC 8B ? ? ? 8B ? 85 ? 75 ? 8B ? ? ? ? ? 83 ? FF
			0xCC);
	}
	using td = int(__thiscall*)(void*, const char*);

	return ((td)offset)(this, NULL);
}

void C_BaseEntity::Think()
{
	using RunThink_fn = void(__thiscall*)(void*);
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xFF\x90\xCC\xCC\xCC\xCC\xFF\x35",//xx????xx
			0xCC);
		offset = Core::Memory::get_disp(offset);
		offset /= 4;
	}
	return Core::Memory::vfunc<RunThink_fn>(this, offset)(this);
}

void C_BaseEntity::PostThink()
{
	using RunPostThink = void(__thiscall*)(void*);
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xFF\x90\xCC\xCC\xCC\xCC\x8B\x0D\xCC\xCC\xCC\xCC\x57\x8B\x01\xFF\x50\x0C",//xx????xx????xxxxxx
			0xCC);
		offset = Core::Memory::get_disp(offset);
		offset /= 4;
	}
	return Core::Memory::vfunc<RunPostThink>(this, offset)(this);
}

float C_BaseCombatWeapon::m_flAccuracyPenalty()
{
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x8B\x86\xCC\xCC\xCC\xCC\x5F\xF3\x0F\x11\x45",//xx????xxxxx
			0xCC);
		offset = Core::Memory::get_disp(offset);
	}

	return *(float*)((DWORD)(this) + offset);
}

double C_BaseCombatWeapon::GetFireRate()
{
	if (strcmp("CWeaponSWEP", GetClientClass()->GetName()) == 0) {
		g_pLuaShitInterface = g_pLua_Shared->GetLuaInterface(LuaInterfaceType::LUAINTERFACE_CLIENT);

		g_pLuaShitInterface->PushSpecial(SPECIAL_GLOB);
		g_pLuaShitInterface->GetField(-1, "FindMetaTable");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(2);
			return 0.0;
		}
		g_pLuaShitInterface->PushString("Player");
		g_pLuaShitInterface->Call(1, 1);
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(2);
			return 0.0;
		}
		g_pLuaShitInterface->GetField(-1, "GetActiveWeapon");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(3);
			return 0.0;
		}
		g_pLuaShitInterface->GetField(-3, "Entity");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return 0.0;
		}
		g_pLuaShitInterface->PushNumber(g_pEngine->GetLocalPlayer());
		g_pLuaShitInterface->Call(1, 1);
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(4);
			return 0.0;
		}
		g_pLuaShitInterface->Call(1, 1);
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop(3);
			return 0.0;
		}
		g_pLuaShitInterface->GetField(-1, "FireDelay");
		if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
			g_pLuaShitInterface->Pop();

			g_pLuaShitInterface->GetField(-1, "Primary");
			if (g_pLuaShitInterface->GetType(-1) == Type::NIL) {
				g_pLuaShitInterface->Pop(4);
				return 0.0;
			}
			g_pLuaShitInterface->GetField(-1, "RPM");
			auto ret = (float)g_pLuaShitInterface->GetNumber(-1);

			g_pLuaShitInterface->Pop(5);
			return 1.f / (ret / 60.f);
		}
		auto ret = (float)g_pLuaShitInterface->GetNumber(-1);
		g_pLuaShitInterface->Pop(4);
		return ret;
	}
	else if (strcmp("CWeapon357", GetClientClass()->GetName()) == 0 || strcmp("CWeaponCrossbow", GetClientClass()->GetName()) == 0) {
		return 0.75;
	}
	static auto offset = 0u;
	if (!offset) {
		offset = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x8B\x80\xCC\xCC\xCC\xCC\xFF\xD0\xA1\xCC\xCC\xCC\xCC\xF3\x0F\x10\x8F\xCC\xCC\xCC\xCC\xD9\x5D\xF8\x5E",//xx????xxx????xxxx????xxxx
			0xCC);
		offset = Core::Memory::get_disp(offset);
		offset /= 4;
	}
	using GetFireRate_fn = double(__thiscall*)(void*);
	return Core::Memory::vfunc<GetFireRate_fn>(this, offset)(this);
}
/*
float C_BaseCombatWeapon::m_flNextPrimaryAttack()
{
	return *(float*)((DWORD)(this) + 0x17CC);
}*/

void CIKContext::Construct()
{
	*(DWORD *)(DWORD(this) + 4080) = 0;
	*(DWORD *)(DWORD(this) + 4084) = DWORD(this);
	*(DWORD *)(DWORD(this) + 4092) = 0;
	*(DWORD *)(DWORD(this) + 4096) = 0;
	*(DWORD *)(DWORD(this) + 4100) = 0;
	*(DWORD *)(DWORD(this) + 4108) = *(DWORD *)(DWORD(this) + 4092);
	*(DWORD *)(DWORD(this) + 4104) = 0;
	*(DWORD *)(DWORD(this) + 4112) = 0;
	*(DWORD *)(DWORD(this) + 4116) = 0;
	*(DWORD *)(DWORD(this) + 4120) = 0;
	*(DWORD *)(DWORD(this) + 4128) = *(DWORD *)(DWORD(this) + 4112);
	*(DWORD *)(DWORD(this) + 4124) = 0;
	*(DWORD *)(DWORD(this) + 4084) = DWORD(this);
	*(DWORD *)(DWORD(this) + 4180) = -1;
	*(DWORD *)(DWORD(this) + 4088) = 0;
	*(DWORD *)(DWORD(this) + 4184) = 0xBF800000;
	*(DWORD *)(DWORD(this) + 4080) = 0;
}

void CIKContext::ClearTargets()
{
	using ClearTargets_td = void(__thiscall*)(void*);
	static ClearTargets_td ClearTargets_fn = NULL;

	if (!ClearTargets_fn) {
		auto off = (uintptr_t)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xE8\xCC\xCC\xCC\xCC\xA1\xCC\xCC\xCC\xCC\xFF\xCC\xCC\xD9\xCC\xCC\x8B",
			0xCC)));
		ClearTargets_fn = (ClearTargets_td)Core::Memory::follow_to(off);

	}
	return ClearTargets_fn(this);


}

void CIKContext::Init(const CStudioHdr * pStudioHdr, const QAngle & angles, const Vector & pos, float flTime, int iFramecounter, int boneMask)
{
	using Init_td = void(__thiscall*)(void*, const CStudioHdr *, const QAngle &, const Vector &, float, int, int);
	static Init_td Init_fn = NULL;

	if (!Init_fn) {
		auto off = (uintptr_t)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xE8\xCC\xCC\xCC\xCC\x8D\xCC\xCC\x8B\xCC\xCC\xCC\xCC\xCC\x8D\xCC\xCC\xFF",
			0xCC)));
		Init_fn = (Init_td)Core::Memory::follow_to(off);

	}
	return Init_fn(this, pStudioHdr, angles, pos, flTime, iFramecounter, boneMask);
}

void CIKContext::UpdateTargets(Vector pos[], Quaternion q[], matrix3x4_t boneToWorld[], CBoneBitList & boneComputed)
{
	using UpdateTargets_td = void(__thiscall*)(void*, Vector pos[], Quaternion q[], matrix3x4_t boneToWorld[], CBoneBitList & boneComputed);
	static UpdateTargets_td UpdateTargets_fn = NULL;

	if (!UpdateTargets_fn) {
		auto off = (uintptr_t)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xE8\xCC\xCC\xCC\xCC\x8B\xCC\xD9\xCC\xCC\xCC\x8B\xCC\xD9",
			0xCC)));
		UpdateTargets_fn = (UpdateTargets_td)Core::Memory::follow_to(off);

	}
	return UpdateTargets_fn(this, pos, q, boneToWorld, boneComputed);
}

void CIKContext::SolveDependencies(Vector pos[], Quaternion q[], matrix3x4_t boneToWorld[], CBoneBitList & boneComputed)
{
	using SolveDependencies_td = void(__thiscall*)(void*, Vector pos[], Quaternion q[], matrix3x4_t boneToWorld[], CBoneBitList & boneComputed);
	static SolveDependencies_td SolveDependencies_fn = NULL;

	if (!SolveDependencies_fn) {
		auto off = (uintptr_t)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xE8\xCC\xCC\xCC\xCC\xFF\xCC\xCC\x8B\xCC\x8D\xCC\xCC\xCC\xFF",
			0xCC)));
		SolveDependencies_fn = (SolveDependencies_td)Core::Memory::follow_to(off);
	}
	return SolveDependencies_fn(this, pos, q, boneToWorld, boneComputed);
}

void CBoneSetup::InitPose(Vector pos[], Quaternion q[])
{
	if (!m_pStudioHdr->pLinearBones())
	{
		for (int i = 0; i < m_pStudioHdr->numbones(); i++)
		{
			if (m_pStudioHdr->boneFlags(i) & m_boneMask)
			{
				mstudiobone_t *pbone = m_pStudioHdr->pBone(i);
				pos[i] = pbone->pos;
				q[i] = pbone->quat;
			}
		}
	}
	else
	{
		mstudiolinearbone_t *pLinearBones = m_pStudioHdr->pLinearBones();
		for (int i = 0; i < m_pStudioHdr->numbones(); i++)
		{
			if (m_pStudioHdr->boneFlags(i) & m_boneMask)
			{
				pos[i] = pLinearBones->pos(i);
				q[i] = pLinearBones->quat(i);
			}
		}
	}
}

void CBoneSetup::AccumulatePose(Vector pos[], Quaternion q[], int sequence, float cycle, float flWeight, float flTime, CIKContext * pIKContext)
{
	auto fn = SDK::Client.GetStartAddr() + 0x10A7E0;

	using meme_fn = void(__thiscall*)(void*, Vector pos[], Quaternion q[], int sequence, float cycle, float flWeight, float flTime, CIKContext * pIKContext);

	(meme_fn(fn))(this, pos, q, sequence, cycle, flWeight, flTime, pIKContext);
}

void CBoneSetup::CalcAutoplaySequences(Vector pos[], Quaternion q[], float flRealTime, CIKContext * pIKContext)
{
	using CalcAutoplaySequences_td = void(__thiscall*)(void*, Vector pos[], Quaternion q[], float flRealTime, CIKContext * pIKContext);
	static CalcAutoplaySequences_td CalcAutoplaySequences_fn = NULL;

	if (!CalcAutoplaySequences_fn) {
		auto off = (uintptr_t)((DWORD)(Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\xE8\xCC\xCC\xCC\xCC\x8B\xCC\xCC\x8B\xCC\x83\xCC\xCC\xCC\xCC\xCC\xCC\x74\xCC\x8B\xCC\x8D",
			0xCC)));
		CalcAutoplaySequences_fn = (CalcAutoplaySequences_td)Core::Memory::follow_to(off);
	}
	void * class_ptr = (void *)this;
	return CalcAutoplaySequences_fn(&class_ptr, pos, q, flRealTime, pIKContext);

}

inline C_BasePlayer * ToBasePlayer(C_BaseEntity * base_entity)
{
	if (!base_entity)
		return nullptr;
	if (!base_entity->IsValid())
		return nullptr;
	if (!base_entity->GetClientClass())
		return nullptr;
	if (!base_entity->GetClientClass()->GetName())
		return nullptr;
	if (base_entity->GetClientClass()->m_ClassID != SDK::ClassId::CGMOD_Player())
		return nullptr;

	//	if (!base_entity || !base_entity ->IsValid()||!base_entity->GetClientClass()->GetName() || strcmp(base_entity->GetClientClass()->GetName(),"CGMOD_Player"))// || !g_pClientTools->IsPlayer(base_entity)
	//		return nullptr;

	return reinterpret_cast<C_BasePlayer*>(base_entity);
}
