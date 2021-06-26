#pragma once

using SetupBones_fn = bool(__fastcall*)(IClientRenderable*, void*, matrix3x4_t *, int, int, float);
static SetupBones_fn oSetupBones_;
static bool* g_bInThreadedBoneSetup = nullptr;
static unsigned long*	g_iModelBoneCounter = nullptr;


void InitPose(
	const CStudioHdr *pStudioHdr,
	Vector pos[],
	Quaternion q[],
	int boneMask
)
{
	if (!pStudioHdr->pLinearBones())
	{
		for (int i = 0; i < pStudioHdr->numbones(); i++)
		{
			if (pStudioHdr->boneFlags(i) & boneMask)
			{
				mstudiobone_t *pbone = pStudioHdr->pBone(i);
				pos[i] = pbone->pos;
				q[i] = pbone->quat;
			}
		}
	}
	else
	{
		mstudiolinearbone_t *pLinearBones = pStudioHdr->pLinearBones();
		for (int i = 0; i < pStudioHdr->numbones(); i++)
		{
			if (pStudioHdr->boneFlags(i) & boneMask)
			{
				pos[i] = pLinearBones->pos(i);
				q[i] = pLinearBones->quat(i);
			}
		}
	}
}
const Vector vec3_origin(0, 0, 0);
const QAngle vec3_angle(0, 0, 0);
const Vector vec3_invalid(FLT_MAX, FLT_MAX, FLT_MAX);
/*
void AccumulatePose(
	 CStudioHdr *pStudioHdr,
	CIKContext *pIKContext,
	Vector pos[],
	Quaternion q[],
	int sequence,
	float cycle,
	const float poseParameter[],
	int boneMask,
	float flWeight,
	float flTime
)
{
	Vector		pos2[MAXSTUDIOBONES];
	__declspec (align(16))Quaternion	q2[MAXSTUDIOBONES];

	// This shouldn't be necessary, but the Assert should help us catch whoever is screwing this up
	flWeight = clamp(flWeight, 0.0f, 1.0f);

	if (sequence < 0)
		return;

	mstudioseqdesc_t	&seqdesc = pStudioHdr->pSeqdesc(sequence);

	// add any IK locks to prevent extremities from moving
	CIKContext seq_ik;
	if (seqdesc.numiklocks)
	{
		seq_ik.Init(pStudioHdr, vec3_angle, vec3_origin, 0.0, 0, boneMask);  // local space relative so absolute position doesn't mater
		seq_ik.AddSequenceLocks(seqdesc, pos, q);
	}

	if (seqdesc.flags & STUDIO_LOCAL)
	{
		InitPose(pStudioHdr, pos2, q2, boneMask);
	}

	if (CalcPoseSingle(pStudioHdr, pos2, q2, seqdesc, sequence, cycle, poseParameter, boneMask, flTime))
	{
		// this weight is wrong, the IK rules won't composite at the correct intensity
		AddLocalLayers(pStudioHdr, pIKContext, pos2, q2, seqdesc, sequence, cycle, poseParameter, boneMask, 1.0, flTime);
		SlerpBones(pStudioHdr, q, pos, seqdesc, sequence, q2, pos2, flWeight, boneMask);
	}

	if (pIKContext)
	{
		pIKContext->AddDependencies(seqdesc, sequence, cycle, poseParameter, flWeight);
	}

	AddSequenceLayers(pStudioHdr, pIKContext, pos, q, seqdesc, sequence, cycle, poseParameter, boneMask, flWeight, flTime);

	if (seqdesc.numiklocks)
	{
		seq_ik.SolveSequenceLocks(seqdesc, pos, q);
	}
}*/


void Studio_BuildMatrices(
	const CStudioHdr *pStudioHdr,
	const QAngle& angles,
	const Vector& origin,
	const Vector pos[],
	const Quaternion q[],
	int iBone,
	matrix3x4_t bonetoworld[MAXSTUDIOBONES],
	int boneMask
)
{
	int i, j;

	int					chain[MAXSTUDIOBONES];
	int					chainlength = 0;

	if (iBone < -1 || iBone >= pStudioHdr->numbones())
		iBone = 0;

	// build list of what bones to use
	if (iBone == -1)
	{
		// all bones
		chainlength = pStudioHdr->numbones();
		for (i = 0; i < pStudioHdr->numbones(); i++)
		{
			chain[chainlength - i - 1] = i;
		}
	}
	else
	{
		// only the parent bones
		i = iBone;
		while (i != -1)
		{
			chain[chainlength++] = i;
			i = pStudioHdr->boneParent(i);
		}
	}

	matrix3x4_t bonematrix;
	matrix3x4_t rotationmatrix; // model to world transformation
	AngleMatrix(angles, origin, rotationmatrix);

	for (j = chainlength - 1; j >= 0; j--)
	{
		i = chain[j];
		if (pStudioHdr->boneFlags(i) & boneMask)
		{
			QuaternionMatrix(q[i], pos[i], bonematrix);

			if (pStudioHdr->boneParent(i) == -1)
			{
				ConcatTransforms(rotationmatrix, bonematrix, bonetoworld[i]);
			}
			else
			{
				ConcatTransforms(bonetoworld[pStudioHdr->boneParent(i)], bonematrix, bonetoworld[i]);
			}
		}
	}
}


void GetSkeleton_impl(C_GMOD_Player * ply, CStudioHdr *pStudioHdr, Vector pos[], Quaternion q[], int boneMask) {
	if (!pStudioHdr)
		return;

	//InitPose(pStudioHdr, pos, q, boneMask);

	CBoneSetup Setup = {};
	Setup.m_boneMask = boneMask;
	Setup.m_flPoseParameter = ply->m_flPoseParameter();
	Setup.m_pPoseDebugger = NULL;
	Setup.m_pStudioHdr = pStudioHdr;


	Setup.InitPose(pos, q);
	Setup.AccumulatePose(pos, q, ply->m_nSequence(), ply->m_flCycle(), 1.0, g_pGlobalVars->curtime, *ply->m_pIk());

	//ply->AccumulatePose(pStudioHdr, *ply->m_pIk(), pos, q, ply->m_nSequence(), ply->m_flCycle(), ply->m_flPoseParameter(), boneMask, 1.0, g_pGlobalVars->curtime);
	//Engine::msg_box("gud1");
	if (ply->m_pIk())
	{
		CIKContext auto_ik;
		auto_ik.Construct();
		auto_ik.Init(pStudioHdr, ply->m_angEyeAngles(), ply->GetNetworkOrigin(), g_pGlobalVars->curtime, 0, boneMask);
		Setup.CalcAutoplaySequences(pos, q, g_pGlobalVars->curtime, &auto_ik);
		//ply->CalcAutoplaySequences(pStudioHdr, &auto_ik, pos, q, ply->m_flPoseParameter(), boneMask, g_pGlobalVars->curtime);
	}
	else
	{
		Setup.CalcAutoplaySequences(pos, q, g_pGlobalVars->curtime, NULL);
		//ply->CalcAutoplaySequences(pStudioHdr, NULL, pos, q, ply->m_flPoseParameter(), boneMask, g_pGlobalVars->curtime);
	}
	//Engine::msg_box("gud2");
	ply->CalcBoneAdj(pStudioHdr, pos, q, ply->m_flEncodedController(), boneMask);
	//Engine::msg_box("gud3");
}


void SetupBones_server(C_GMOD_Player * ply, matrix3x4_t *pBoneToWorld, int boneMask) {

	//CStudioHdr *hdr = ply->GetModelPtr();
	using lock_td = CStudioHdr * (__thiscall*)(C_GMOD_Player * ply);
	CStudioHdr *hdr = (lock_td(SDK::Client.GetStartAddr() + 0x9C190))(ply);

	if (!hdr || !hdr->SequencesAvailable())
		return;

//	ply->m_fFlags() |= EFL_SETTING_UP_BONES;
	if (!*ply->m_pIk() && hdr->numikchains() > 0 && !(*(BYTE *)(DWORD(ply) + 4 + 82) & ENTCLIENTFLAG_DONTUSEIK)) {
		*ply->m_pIk() = (CIKContext*)malloc(sizeof(CIKContext));
		(*ply->m_pIk())->Construct();
	}

	Vector		pos[MAXSTUDIOBONES];
	__declspec (align(16))Quaternion	q[MAXSTUDIOBONES];
	memset(pos, 0xFF, sizeof(pos));
	memset(q, 0xFF, sizeof(q));


	if (*ply->m_pIk()) {
		CBoneBitList boneComputed;
		static int m_iIKCounter = 0;
		m_iIKCounter++;
		(*ply->m_pIk())->Init(hdr, ply->GetRenderAngles(), ply->GetRenderOrigin(), g_pGlobalVars->curtime, m_iIKCounter, boneMask);
		GetSkeleton_impl(ply, hdr, pos, q, boneMask);
		ply->UpdateIKLocks(g_pGlobalVars->curtime);
		(*ply->m_pIk())->UpdateTargets(pos, q, pBoneToWorld, boneComputed);
		ply->CalculateIKLocks(g_pGlobalVars->curtime);
		(*ply->m_pIk())->SolveDependencies(pos, q, pBoneToWorld, boneComputed);
	}
	else {
		GetSkeleton_impl(ply, hdr, pos, q, boneMask);

	}

	Studio_BuildMatrices(
		hdr,
		ply->GetRenderAngles(), ply->GetRenderOrigin(),
		pos,
		q,
		-1,
		pBoneToWorld,
		boneMask);

//	ply->m_fFlags() &= ~EFL_SETTING_UP_BONES;
}


bool SetupBonesRebuild(C_GMOD_Player * ply, matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) {

	if (!g_iModelBoneCounter) {
		auto off = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x3B\xCC\xCC\xCC\xCC\xCC\x74\xCC\x8B\xCC\x8B\xCC\x8B\xCC\xCC\xCC\xCC\xCC\xFF",
			0xCC);
		off += 2;
		g_iModelBoneCounter = *(unsigned long**)off;
	}

	if (!g_bInThreadedBoneSetup) {
		auto off = (uintptr_t)Core::Memory::FindPattern((PBYTE)SDK::Client.GetStartAddr(), SDK::Client.GetSize(),
			(const char *)"\x80\x3D\xCC\xCC\xCC\xCC\xCC\x74\xCC\xFF\x15",
			0xCC);
		off += 2;
		g_bInThreadedBoneSetup = *(bool**)off;
	}
	static ConVar * cl_SetupAllBones = nullptr;
	if (!cl_SetupAllBones) {
		cl_SetupAllBones = g_pCvar->FindVar("cl_SetupAllBones");
	}


	if (ply->m_nSequence() == -1)
		return false;


	if (boneMask == -1)
	{
		boneMask = ply->m_iPrevBoneMask();
	}

	if (cl_SetupAllBones->intVal)
	{
		boneMask |= BONE_USED_BY_ANYTHING;
	}

	if (ply->IsToolRecording())
	{
		boneMask |= BONE_USED_BY_ANYTHING;
	}


	if (*g_bInThreadedBoneSetup)
	{
		if (!ply->m_BoneSetupLock().TryLock())
		{
			return false;
		}
	}

	AUTO_LOCK(ply->m_BoneSetupLock());

	if (*g_bInThreadedBoneSetup)
	{
		ply->m_BoneSetupLock().Unlock();
	}

	if (ply->m_iMostRecentModelBoneCounter() != *g_iModelBoneCounter) {

		if (currentTime >= ply->m_flLastBoneSetupTime()) {
			ply->m_BoneAccessor().SetReadableBones(0);
			ply->m_BoneAccessor().SetWritableBones(0);
			ply->m_flLastBoneSetupTime() = currentTime;
		}

		ply->m_iPrevBoneMask() = ply->m_iAccumulatedBoneMask();
		ply->m_iAccumulatedBoneMask() = 0;

		CStudioHdr *hdr = ply->GetModelPtr();
		if (hdr) {
			hdr->ClearPerfCounters();
		}
	}

	//MarkForThreadedBoneSetup();

	// Keep track of everthing asked for over the entire frame
	ply->m_iAccumulatedBoneMask() |= boneMask;

	// Make sure that we know that we've already calculated some bone stuff this time around.
	ply->m_iMostRecentModelBoneCounter() = *g_iModelBoneCounter;



	CStudioHdr *hdr = ply->GetModelPtr();
	if (!hdr || !hdr->SequencesAvailable())
		return false;

	__declspec (align(16))matrix3x4_t parentTransform;

	auto anim = ply->PlayerAnimState();
	if (!anim)
		return false;

	QAngle Angle = anim->m_angRender;
	Angle[PITCH] = Angle[ROLL] = 0.f;
	AngleMatrix(Angle, ply->GetNetworkOrigin(), parentTransform);

	// Load the boneMask with the total of what was asked for last frame.
	boneMask |= ply->m_iPrevBoneMask();

	int oldReadableBones = ply->m_BoneAccessor().GetReadableBones();
	ply->m_BoneAccessor().SetWritableBones(ply->m_BoneAccessor().GetReadableBones() | boneMask);
	ply->m_BoneAccessor().SetReadableBones(ply->m_BoneAccessor().GetWritableBones());


	/*
		if (C_BaseAnimating::GetBoneManipulator) {
			if (hdr + 156 > 0)
			{
				v50 = &v77;
				v51 = &v78;
				v106 = (int)&v77;
				v52 = 0;
				do
				{
					C_BoneManipulate::ManipulateBone(v49, v52, (int)v51, (int)v50);
					v53 = *(_DWORD *)LODWORD(boneMaskb);
					++v52;
					v51 += 12;
					v49 = v97;
					v50 = (char *)(v106 + 16);
					v106 += 16;
				} while (v52 < *(_DWORD *)(v53 + 156));
				v9 = v95;
				v28 = v96;
				Ply = v95 - 4;

			}
			v106 = (int)(v49 + 2018);
		}*/
	if (hdr->flags() & STUDIOHDR_FLAGS_STATIC_PROP)
	{
		MatrixCopy(parentTransform, ply->m_BoneAccessor().GetBoneForWrite(0));
	}
	else {

//		ply->m_fFlags() |= EFL_SETTING_UP_BONES;
		if (!*ply->m_pIk() && hdr->numikchains() > 0 && !(*(BYTE *)(DWORD(ply) + 4 + 82) & ENTCLIENTFLAG_DONTUSEIK)) {
			*ply->m_pIk() = (CIKContext*)malloc(sizeof(CIKContext));
			(*ply->m_pIk())->Construct();
		}

		Vector		pos[MAXSTUDIOBONES];
		__declspec (align(16))Quaternion	q[MAXSTUDIOBONES];
		memset(pos, 0xFF, sizeof(pos));
		memset(q, 0xFF, sizeof(q));

		int bonesMaskNeedRecalc = boneMask | oldReadableBones;

		if (*ply->m_pIk())
		{
			//if (ply->Teleported() || ply->m_fEffects() & EF_NOINTERP)
			(*ply->m_pIk())->ClearTargets();


			
			(*ply->m_pIk())->Init(hdr, Angle, ply->GetNetworkOrigin(), currentTime, g_pGlobalVars->framecount, bonesMaskNeedRecalc);
		}
		//GetSkeleton_impl(ply, hdr, pos, q, boneMask);
		ply->StandardBlendingRules(hdr, pos, q, currentTime, bonesMaskNeedRecalc);

		CBoneBitList boneComputed;
		if (*ply->m_pIk() && !ply->IsRagdoll()) {

			ply->UpdateIKLocks(currentTime);
			(*ply->m_pIk())->UpdateTargets(pos, q, ply->m_BoneAccessor().GetBoneArrayForWrite(), boneComputed);

			ply->CalculateIKLocks(currentTime);
			(*ply->m_pIk())->SolveDependencies(pos, q, ply->m_BoneAccessor().GetBoneArrayForWrite(), boneComputed);
		}


		ply->BuildTransformations(hdr, pos, q, parentTransform, bonesMaskNeedRecalc, boneComputed);//?


		ply->ControlMouth(hdr);
//		ply->m_fFlags() &= ~EFL_SETTING_UP_BONES;

	}
	if (!(oldReadableBones & BONE_USED_BY_ATTACHMENT) && (boneMask & BONE_USED_BY_ATTACHMENT))
	{
		ply->SetupBones_AttachmentHelper(hdr);
	}


	if (pBoneToWorldOut)
	{
		if (nMaxBones >= ply->m_CachedBoneData().Count())
		{
			memcpy(pBoneToWorldOut, ply->m_CachedBoneData().Base(), sizeof(matrix3x4_t) * ply->m_CachedBoneData().Count());
		}
		else
		{
			return false;
		}
	}

	return true;
}




bool __fastcall hkSetupBones(IClientRenderable*ecx, void*edx, matrix3x4_t *pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) {
	auto ent = (IClientEntity*)(DWORD(ecx) - 0x4);



	
	if (ecx && ent && ent->IsValid() && ent->GetBaseEntity() && ToGMOD_Player(ToBasePlayer((C_BaseEntity*)ent)))
		//&& Engine::Var::Instance().Aim_LagCompensation.Get())
	{
		auto GPly = ToGMOD_Player(ToBasePlayer((C_BaseEntity*)ent));

		static ConVar * host_timescale = nullptr;
		if (!host_timescale) {
			host_timescale = g_pCvar->FindVar(("host_timescale"));
		}
		//GPly->InvalidateBoneCache();

		auto serv_time = SDK::Vars::g_tick*g_pGlobalVars->interval_per_tick;
		float OldCurTime = g_pGlobalVars->curtime;
		float OldRealTime = g_pGlobalVars->realtime;
		float OldFrameTime = g_pGlobalVars->frametime;
		float OldAbsFrameTime = g_pGlobalVars->absoluteframetime;
		float OldInterpAmount = g_pGlobalVars->interpolation_amount;
		int OldFrameCount = g_pGlobalVars->framecount;
		int OldTickCount = g_pGlobalVars->tickcount;


		float fl_lerp = SDK::GetLerpTime();

		g_pGlobalVars->curtime = TICKS_TO_TIME(SDK::Vars::g_tick) - fl_lerp;
		g_pGlobalVars->realtime = TICKS_TO_TIME(SDK::Vars::g_tick) - fl_lerp;

		//g_pGlobalVars->frametime = g_pGlobalVars->interval_per_tick * host_timescale->flVal;
		//g_pGlobalVars->absoluteframetime = g_pGlobalVars->interval_per_tick * host_timescale->flVal;

		g_pGlobalVars->interpolation_amount = 0;
		g_pGlobalVars->framecount = SDK::Vars::g_tick - TIME_TO_TICKS(fl_lerp);
		g_pGlobalVars->tickcount = SDK::Vars::g_tick - TIME_TO_TICKS(fl_lerp);


//		GPly->m_fEffects() |= EF_NOINTERP;
		//SetupBones_server(GPly, pBoneToWorldOut, boneMask);
		//GPly->m_bClientSideAnimation() = true;
		auto ret = oSetupBones_(ecx, edx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
		//auto ret = SetupBonesRebuild(GPly, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
		//GPly->m_bClientSideAnimation() = false;
		//auto ret = SetupBonesRebuild(GPly, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
		

		//GPly->m_bClientSideAnimation() = false;
		//*(BYTE*)((DWORD)GPly + 113) = save_17;
		//*(BYTE*)((DWORD)GPly + 112) = save_16;

		g_pGlobalVars->curtime = OldCurTime;
		g_pGlobalVars->realtime = OldRealTime;
		g_pGlobalVars->frametime = OldFrameTime;
		g_pGlobalVars->absoluteframetime = OldAbsFrameTime;
		g_pGlobalVars->interpolation_amount = OldInterpAmount;
		g_pGlobalVars->framecount = OldFrameCount;
		g_pGlobalVars->tickcount = OldTickCount;

		return ret;
	}

	

	return oSetupBones_(ecx, edx, pBoneToWorldOut, nMaxBones, boneMask, currentTime);
}