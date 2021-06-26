#include "../SDK.h"
#include "LagCompensation.h"
bool IsTickValid(CUserCmd* pCmd,int tick )
{

	INetChannelInfo *nci = g_pEngine->GetNetChannelInfo();

	static ConVar* sv_maxunlag = nullptr;
	if (!sv_maxunlag) sv_maxunlag = g_pCvar->FindVar("sv_maxunlag");

	if (!nci || !sv_maxunlag)
		return false;

	float correct = 0.f;
	correct += nci->GetLatency(FLOW_OUTGOING);
	int lerpTicks = TIME_TO_TICKS(SDK::GetLerpTime());
	correct += TICKS_TO_TIME(lerpTicks);
	correct = clamp(correct, 0, sv_maxunlag->flVal);

	int targettick = tick - lerpTicks;

	float deltaTime = correct - TICKS_TO_TIME(pCmd->tick_count - targettick);
	if (fabsf(deltaTime) <= 0.2f)
	{
		return true;
	}
	return false;
}

LagCompensation g_LagCompensation;

bool SetupPoseParameters(C_GMOD_Player* ply, void *pStudioHdr)
{
	auto anim = ply->PlayerAnimState();
	if (!anim)
		return false;

	if (anim->m_bPoseParameterInit)
		return true;

	// Save off the pose parameter indices.
	if (!pStudioHdr)
		return false;

	anim->m_bPoseParameterInit = true;

	// Look for the movement blenders.
	anim->m_PoseParameterData.m_iMoveX = ply->LookupPoseParameter(pStudioHdr, "move_x");
	anim->m_PoseParameterData.m_iMoveY = ply->LookupPoseParameter(pStudioHdr, "move_y");
	/*
	if ( ( m_PoseParameterData.m_iMoveX < 0 ) || ( m_PoseParameterData.m_iMoveY < 0 ) )
		return false;
	*/

	// Look for the aim pitch blender.
	anim->m_PoseParameterData.m_iAimPitch = ply->LookupPoseParameter(pStudioHdr, "body_pitch");
	/*
	if ( m_PoseParameterData.m_iAimPitch < 0 )
		return false;
	*/

	// Look for aim yaw blender.
	anim->m_PoseParameterData.m_iAimYaw = ply->LookupPoseParameter(pStudioHdr, "body_yaw");
	/*
	if ( m_PoseParameterData.m_iAimYaw < 0 )
		return false;
	*/

	anim->m_PoseParameterData.m_iMoveYaw = ply->LookupPoseParameter(pStudioHdr, "move_yaw");
	anim->m_PoseParameterData.m_iMoveScale = ply->LookupPoseParameter(pStudioHdr, "move_scale");
	/*
	if ( ( m_PoseParameterData.m_iMoveYaw < 0 ) || ( m_PoseParameterData.m_iMoveScale < 0 ) )
		return false;
	*/

	return true;
}

void update_animstate(C_GMOD_Player* ply, float eyeYaw, float eyePitch) {
	void *pStudioHdr = ply->GetModelPtr();
	if (!pStudioHdr)
		return;
	auto anim = ply->PlayerAnimState();
	if (!anim)
		return;

	if (!anim->ShouldUpdateAnimState())
	{
		anim->ClearAnimationState();
		return;
	}
	anim->m_flEyeYaw = AngleNormalize(eyeYaw);
	anim->m_flEyePitch = AngleNormalize(eyePitch);

	anim->ComputeSequences(pStudioHdr);
	//anim->ComputeSequences(pStudioHdr);

	if (SetupPoseParameters(ply,pStudioHdr))
	{
		// Pose parameter - what direction are the player's legs running in.
		anim->ComputePoseParam_MoveYaw(pStudioHdr);

		// Pose parameter - Torso aiming (up/down).
		anim->ComputePoseParam_AimPitch(pStudioHdr);

		// Pose parameter - Torso aiming (rotation).
		anim->ComputePoseParam_AimYaw(pStudioHdr);
	}
}




void LagCompensation::update_animation(C_GMOD_Player* ply, float sim_time)
{
	static ConVar * host_timescale = nullptr;
	if (!host_timescale) {
		host_timescale = g_pCvar->FindVar(("host_timescale"));
	}
	float OldCurTime = g_pGlobalVars->curtime;
	float OldRealTime = g_pGlobalVars->realtime;
	float OldFrameTime = g_pGlobalVars->frametime;
	float OldAbsFrameTime = g_pGlobalVars->absoluteframetime;
	float OldInterpAmount = g_pGlobalVars->interpolation_amount;
	int OldFrameCount = g_pGlobalVars->framecount;
	int OldTickCount = g_pGlobalVars->tickcount;


	g_pGlobalVars->curtime = sim_time;
	g_pGlobalVars->realtime = sim_time;
	g_pGlobalVars->frametime = g_pGlobalVars->interval_per_tick * host_timescale->flVal;
	g_pGlobalVars->absoluteframetime = g_pGlobalVars->interval_per_tick * host_timescale->flVal;
	g_pGlobalVars->interpolation_amount = 0;
	g_pGlobalVars->framecount = TIME_TO_TICKS(sim_time);
	g_pGlobalVars->tickcount = TIME_TO_TICKS(sim_time);

	auto anim = ply->PlayerAnimState();
	if (anim) {
//		ply->m_bClientSideAnimation() = true;
		ply->UpdateClientSideAnimation();
//		ply->m_bClientSideAnimation() = false;
	}

	g_pGlobalVars->curtime = OldCurTime;
	g_pGlobalVars->realtime = OldRealTime;
	g_pGlobalVars->frametime = OldFrameTime;
	g_pGlobalVars->absoluteframetime = OldAbsFrameTime;
	g_pGlobalVars->interpolation_amount = OldInterpAmount;
	g_pGlobalVars->framecount = OldFrameCount;
	g_pGlobalVars->tickcount = OldTickCount;
}

void LagCompensation::update_animation_ex(C_GMOD_Player * ply, QAngle angle, float sim_time)
{
	static ConVar * host_timescale = nullptr;
	if (!host_timescale) {
		host_timescale = g_pCvar->FindVar(("host_timescale"));
	}
	float OldCurTime = g_pGlobalVars->curtime;
	float OldRealTime = g_pGlobalVars->realtime;
	float OldFrameTime = g_pGlobalVars->frametime;
	float OldAbsFrameTime = g_pGlobalVars->absoluteframetime;
	float OldInterpAmount = g_pGlobalVars->interpolation_amount;
	int OldFrameCount = g_pGlobalVars->framecount;
	int OldTickCount = g_pGlobalVars->tickcount;


	g_pGlobalVars->curtime = sim_time;
	g_pGlobalVars->realtime = sim_time;
	g_pGlobalVars->frametime = g_pGlobalVars->interval_per_tick * host_timescale->flVal;
	g_pGlobalVars->absoluteframetime = g_pGlobalVars->interval_per_tick * host_timescale->flVal;
	g_pGlobalVars->interpolation_amount = 0;
	g_pGlobalVars->framecount = TIME_TO_TICKS(sim_time);
	g_pGlobalVars->tickcount = TIME_TO_TICKS(sim_time);

	auto anim = ply->PlayerAnimState();
	if (anim) {
//		ply->m_bClientSideAnimation() = true;
		auto saved = ply->m_angEyeAngles();
		ply->m_angEyeAngles() = angle;
		ply->UpdateClientSideAnimation();
		ply->m_angEyeAngles() = saved;
//		ply->m_bClientSideAnimation() = false;
	}
	g_pGlobalVars->curtime = OldCurTime;
	g_pGlobalVars->realtime = OldRealTime;
	g_pGlobalVars->frametime = OldFrameTime;
	g_pGlobalVars->absoluteframetime = OldAbsFrameTime;
	g_pGlobalVars->interpolation_amount = OldInterpAmount;
	g_pGlobalVars->framecount = OldFrameCount;
	g_pGlobalVars->tickcount = OldTickCount;
}

void LagCompensation::clear_data()
{
	recived_data.clear();
}

void LagCompensation::PostDataUpdate(C_GMOD_Player* ply)
{
	if (!ply->IsGood()) {
		auto& data = recived_data.find(ply->entindex());
		if (data != recived_data.end()) {
			data->second.clear();
			return;
		}
	}
	auto& data = recived_data.find(ply->entindex());
	if (data != recived_data.end()) {
		tick info = build_tick(ply);
		data->second.push_back(info);
	}
	else {
		recived_data[ply->entindex()] = std::vector< tick>();
		tick info = build_tick(ply);
		recived_data[ply->entindex()].push_back(info);
	}
}

void LagCompensation::remove_bad_ticks(CUserCmd* pCmd)
{
	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal || !pLocal->IsGood()) {
		recived_data.clear();
		return;
	}
	

	for (auto& it : recived_data) {
		auto ply = C_GMOD_Player::GetGPlayer(it.first);
		if (!ply || !ply->IsGood() || ply == pLocal)
		{
			it.second.clear();
		}
		int pos = 0;
		for (auto& tick : it.second) {
			if (!IsTickValid(pCmd,TIME_TO_TICKS(tick.m_flSimulationTime))) {
				it.second.erase(it.second.begin() + pos);
				--pos;
			}
			++pos;
		}
	}

}

bool LagCompensation::get_last_tick(C_GMOD_Player* ply, tick & tick_out)
{
	tick ret = {};
	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal || !pLocal->IsGood())
		return false;
	for (auto& find : recived_data) {
		auto nerd = C_GMOD_Player::GetGPlayer(find.first);
		if (!nerd || !nerd->IsGood() || nerd == pLocal || nerd != ply)
			continue;
		if (!find.second.size()) {
			return false;
		}
		ret = find.second[find.second.size() - 1];
		break;
	}
	tick_out = ret;

	return true;
}

tick __forceinline LagCompensation::build_tick(C_GMOD_Player * ply)
{
	tick info = {};
	info.m_priority = tick_normal;
	info.m_flSpawnTime = ply->m_flSpawnTime();
	info.m_clientReciveTick = SDK::Vars::g_tick > 0 ? SDK::Vars::g_tick : 0;
	info.m_vecAngles = ply->GetAbsAngles();
	info.m_vecAngles.Normalize();
	info.m_vecAngles.Clamp();
	info.m_vecOrigin = ply->GetNetworkOrigin();
	info.m_flSimulationTime = ply->m_flSimulationTime();
	info.m_flSimulationTime_temp = ply->m_flSimulationTime();
	info.m_vecMins = ply->OBBMins();
	info.m_vecMaxs = ply->OBBMaxs();
	info.m_masterCycle = ply->m_flCycle();
	info.m_masterSequence = ply->m_nSequence();
	auto& anim_overlay = ply->m_AnimOverlay();
	for (auto it = 0; it < anim_overlay.m_Size; it++) {
		auto layer = &anim_overlay.m_Memory.m_pMemory[it];
		memcpy(&info.m_layerRecords[it], layer, sizeof(C_AnimationLayer));
		//info.m_layerRecords[it].m_flCycle = layer->m_flCycle;
		//info.m_layerRecords[it].m_nOrder = layer->m_nOrder;
		//info.m_layerRecords[it].m_nSequence = layer->m_nSequence;
		//info.m_layerRecords[it].m_flWeight = layer->m_flWeight;

	}
	memcpy(info.m_flPoseParameter, ply->m_flPoseParameter(), sizeof(float[24]));
	auto anim = ply->PlayerAnimState();

	info.m_AnimationRecord.m_bForceAimYaw = anim->m_bForceAimYaw;//0x0004

	for (auto it = 0; it < anim->m_aGestureSlots.m_Size; it++) {
		auto gustere = &anim->m_aGestureSlots.m_Memory.m_pMemory[it];
		info.m_AnimationRecord.m_gustereRecords[it].m_bActive = gustere->m_bActive;
		info.m_AnimationRecord.m_gustereRecords[it].m_bAutoKill = gustere->m_bAutoKill;
		info.m_AnimationRecord.m_gustereRecords[it].m_iActivity = gustere->m_iActivity;
		info.m_AnimationRecord.m_gustereRecords[it].m_iGestureSlot = gustere->m_iGestureSlot;
	}
	
	info.m_AnimationRecord.m_angRender = anim->m_angRender;//0x0020
	info.m_AnimationRecord.m_bPoseParameterInit = anim->m_bPoseParameterInit;//0x002C
	info.m_AnimationRecord.m_PoseParameterData = anim->m_PoseParameterData;//0x0030
	info.m_AnimationRecord.m_DebugAnimData = anim->m_DebugAnimData;//0x0054
	info.m_AnimationRecord.m_bCurrentFeetYawInitialized = anim->m_bCurrentFeetYawInitialized;//0x006C
	info.m_AnimationRecord.m_flLastAnimationStateClearTime = anim->m_flLastAnimationStateClearTime;//0x0070
	info.m_AnimationRecord.m_flEyeYaw = anim->m_flEyeYaw;//0x0074
	info.m_AnimationRecord.m_flEyePitch = anim->m_flEyePitch;//0x0078
	info.m_AnimationRecord.m_flGoalFeetYaw = anim->m_flGoalFeetYaw;//0x007C
	info.m_AnimationRecord.m_flCurrentFeetYaw = anim->m_flCurrentFeetYaw;//0x0080
	info.m_AnimationRecord.m_flLastAimTurnTime = anim->m_flLastAimTurnTime;//0x0084
	info.m_AnimationRecord.m_MovementData = anim->m_MovementData;//0x0088
	info.m_AnimationRecord.m_bJumping = anim->m_bJumping;//0x0098
	info.m_AnimationRecord.m_flJumpStartTime = anim->m_flJumpStartTime;//0x009C
	info.m_AnimationRecord.m_bFirstJumpFrame = anim->m_bFirstJumpFrame;//0x00A0
	info.m_AnimationRecord.m_bInSwim = anim->m_bInSwim;//0x00A1
	info.m_AnimationRecord.m_bFirstSwimFrame = anim->m_bFirstSwimFrame;//0x00A2
	info.m_AnimationRecord.m_bDying = anim->m_bDying;//0x00A3
	info.m_AnimationRecord.m_bFirstDyingFrame = anim->m_bFirstDyingFrame;//0x00A4
	info.m_AnimationRecord.m_eCurrentMainSequenceActivity = anim->m_eCurrentMainSequenceActivity;//0x00A8
	info.m_AnimationRecord.m_nSpecificMainSequence = anim->m_nSpecificMainSequence;//0x00AC
	info.m_AnimationRecord.m_flLastGroundSpeedUpdateTime = anim->m_flLastGroundSpeedUpdateTime;//0x00B4
	info.m_AnimationRecord.m_flMaxGroundSpeed = anim->m_flMaxGroundSpeed;//0x00E8
	info.m_AnimationRecord.m_nMovementSequence = anim->m_nMovementSequence;//0x00EC
	info.m_AnimationRecord.m_LegAnimType = anim->m_LegAnimType;//0x00F0

	return info;
}

void LagCompensation::set_tick(C_GMOD_Player * ply, tick target_tick)
{
	ply->InvalidateBoneCache();

	//ply->m_nSequence() = target_tick.m_masterSequence;
	//ply->m_flCycle() = target_tick.m_masterCycle;
	//ply->OBBMaxs() = target_tick.m_vecMaxs;
	//ply->OBBMins() = target_tick.m_vecMins;
	//ply->m_flSimulationTime() = target_tick.m_flSimulationTime;

	ply->SetAbsOrigin(target_tick.m_vecOrigin);
	ply->SetAbsAngles(target_tick.m_vecAngles);
	ply->m_angEyeAngles() = target_tick.m_vecAngles;
	auto& anim_overlay = ply->m_AnimOverlay();

	for (auto it = 0; it < anim_overlay.m_Size; it++) {
		auto layer = &anim_overlay.m_Memory.m_pMemory[it];
		memcpy(layer, &target_tick.m_layerRecords[it], sizeof(C_AnimationLayer));
		/*layer->m_flCycle = target_tick.m_layerRecords[it].m_flCycle;
		layer->m_nOrder = target_tick.m_layerRecords[it].m_nOrder;
		layer->m_nSequence = target_tick.m_layerRecords[it].m_nSequence;
		layer->m_flWeight = target_tick.m_layerRecords[it].m_flWeight;*/
	}
	auto anim = ply->PlayerAnimState();
	for (auto it = 0; it < anim->m_aGestureSlots.m_Size; it++) {
		auto gustere = &anim->m_aGestureSlots.m_Memory.m_pMemory[it];
		gustere->m_bActive = target_tick.m_AnimationRecord.m_gustereRecords[it].m_bActive ;
		gustere->m_bAutoKill = target_tick.m_AnimationRecord.m_gustereRecords[it].m_bAutoKill;
		gustere->m_iActivity = target_tick.m_AnimationRecord.m_gustereRecords[it].m_iActivity;
		gustere->m_iGestureSlot = target_tick.m_AnimationRecord.m_gustereRecords[it].m_iGestureSlot;
	}
	update_animation_ex(ply, target_tick.m_vecAngles, target_tick.m_flSimulationTime);
}

void LagCompensation::set_tick_force(C_GMOD_Player * ply, tick target_tick)
{
	ply->InvalidateBoneCache();

	//ply->m_nSequence() = target_tick.m_masterSequence;
	//ply->m_flCycle() = target_tick.m_masterCycle;
	//ply->OBBMaxs() = target_tick.m_vecMaxs;
	//ply->OBBMins() = target_tick.m_vecMins;
	//ply->m_flSimulationTime() = target_tick.m_flSimulationTime;

	ply->SetAbsOrigin(target_tick.m_vecOrigin);
	ply->SetAbsAngles(target_tick.m_vecAngles);
	ply->m_angEyeAngles() = target_tick.m_vecAngles;
	auto& anim_overlay = ply->m_AnimOverlay();

	for (auto it = 0; it < anim_overlay.m_Size; it++) {
		auto layer = &anim_overlay.m_Memory.m_pMemory[it];
		memcpy(layer, &target_tick.m_layerRecords[it], sizeof(C_AnimationLayer));
		/*layer->m_flCycle = target_tick.m_layerRecords[it].m_flCycle;
		layer->m_nOrder = target_tick.m_layerRecords[it].m_nOrder;
		layer->m_nSequence = target_tick.m_layerRecords[it].m_nSequence;
		layer->m_flWeight = target_tick.m_layerRecords[it].m_flWeight;*/
	}
	
	auto anim = ply->PlayerAnimState();
	if (anim) {
		anim->m_bForceAimYaw = target_tick.m_AnimationRecord.m_bForceAimYaw;//0x0004
		for (auto it = 0; it < anim->m_aGestureSlots.m_Size; it++) {
			auto gustere = &anim->m_aGestureSlots.m_Memory.m_pMemory[it];
			gustere->m_bActive = target_tick.m_AnimationRecord.m_gustereRecords[it].m_bActive;
			gustere->m_bAutoKill = target_tick.m_AnimationRecord.m_gustereRecords[it].m_bAutoKill;
			gustere->m_iActivity = target_tick.m_AnimationRecord.m_gustereRecords[it].m_iActivity;
			gustere->m_iGestureSlot = target_tick.m_AnimationRecord.m_gustereRecords[it].m_iGestureSlot;
		}
		//anim->m_aGestureSlots = target_tick.m_AnimationRecord.m_aGestureSlots;//0x0008
		anim->m_angRender = target_tick.m_AnimationRecord.m_angRender;//0x0020
		anim->m_bPoseParameterInit = target_tick.m_AnimationRecord.m_bPoseParameterInit;//0x002C
		anim->m_PoseParameterData = target_tick.m_AnimationRecord.m_PoseParameterData;//0x0030
		anim->m_DebugAnimData = target_tick.m_AnimationRecord.m_DebugAnimData;//0x0054
		anim->m_bCurrentFeetYawInitialized = target_tick.m_AnimationRecord.m_bCurrentFeetYawInitialized;//0x006C
		anim->m_flLastAnimationStateClearTime = target_tick.m_AnimationRecord.m_flLastAnimationStateClearTime;//0x0070
		anim->m_flEyeYaw = target_tick.m_AnimationRecord.m_flEyeYaw;//0x0074
		anim->m_flEyePitch = target_tick.m_AnimationRecord.m_flEyePitch;//0x0078
		anim->m_flGoalFeetYaw = target_tick.m_AnimationRecord.m_flGoalFeetYaw;//0x007C
		anim->m_flCurrentFeetYaw = target_tick.m_AnimationRecord.m_flCurrentFeetYaw;//0x0080
		anim->m_flLastAimTurnTime = target_tick.m_AnimationRecord.m_flLastAimTurnTime;//0x0084
		anim->m_MovementData = target_tick.m_AnimationRecord.m_MovementData;//0x0088
		anim->m_bJumping = target_tick.m_AnimationRecord.m_bJumping;//0x0098
		anim->m_flJumpStartTime = target_tick.m_AnimationRecord.m_flJumpStartTime;//0x009C
		anim->m_bFirstJumpFrame = target_tick.m_AnimationRecord.m_bFirstJumpFrame;//0x00A0
		anim->m_bInSwim = target_tick.m_AnimationRecord.m_bInSwim;//0x00A1
		anim->m_bFirstSwimFrame = target_tick.m_AnimationRecord.m_bFirstSwimFrame;//0x00A2
		anim->m_bDying = target_tick.m_AnimationRecord.m_bDying;//0x00A3
		anim->m_bFirstDyingFrame = target_tick.m_AnimationRecord.m_bFirstDyingFrame;//0x00A4
		anim->m_eCurrentMainSequenceActivity = target_tick.m_AnimationRecord.m_eCurrentMainSequenceActivity;//0x00A8
		anim->m_nSpecificMainSequence = target_tick.m_AnimationRecord.m_nSpecificMainSequence;//0x00AC
		anim->m_flLastGroundSpeedUpdateTime = target_tick.m_AnimationRecord.m_flLastGroundSpeedUpdateTime;//0x00B4
		anim->m_flMaxGroundSpeed = target_tick.m_AnimationRecord.m_flMaxGroundSpeed;//0x00E8
		anim->m_nMovementSequence = target_tick.m_AnimationRecord.m_nMovementSequence;//0x00EC
		anim->m_LegAnimType = target_tick.m_AnimationRecord.m_LegAnimType;//0x00F0

		auto model_ptr = ply->GetModelPtr();
		if (model_ptr) {
			ply->SetPoseParameter(model_ptr, target_tick.m_AnimationRecord.m_PoseParameterData.m_iMoveX, target_tick.m_AnimationRecord.m_DebugAnimData.m_vecMoveYaw.m_x);
			ply->SetPoseParameter(model_ptr, target_tick.m_AnimationRecord.m_PoseParameterData.m_iMoveY, target_tick.m_AnimationRecord.m_DebugAnimData.m_vecMoveYaw.m_y);

			ply->SetPoseParameter(model_ptr, target_tick.m_AnimationRecord.m_PoseParameterData.m_iAimPitch, target_tick.m_AnimationRecord.m_DebugAnimData.m_flAimPitch);

			ply->SetPoseParameter(model_ptr, target_tick.m_AnimationRecord.m_PoseParameterData.m_iAimYaw, target_tick.m_AnimationRecord.m_DebugAnimData.m_flAimYaw);
		}
		memcpy(ply->m_flPoseParameter(), target_tick.m_flPoseParameter, sizeof(float[24]));
	}
}

void LagCompensation::push_tick(C_GMOD_Player * ply, tick target_tick)
{
	if (!ply->IsGood()) {
		auto& data = recived_data.find(ply->entindex());
		if (data != recived_data.end()) {
			data->second.clear();
			return;
		}
	}
	auto& data = recived_data.find(ply->entindex());
	if (data != recived_data.end()) {
		data->second.push_back(target_tick);
	}
	else {
		recived_data[ply->entindex()] = std::vector< tick>();
		recived_data[ply->entindex()].push_back(target_tick);
	}
}

tick old_tick;

bool LagCompensation::StartLagCompensation(C_GMOD_Player * ply, tick & tick_out)
{
	tick ret = {};
	tick_out = ret;
	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal || !pLocal->IsGood())
		return false;
	bool select = false;
	for (auto& find : recived_data) {
		auto nerd = C_GMOD_Player::GetGPlayer(find.first);
		if (!nerd || !nerd->IsGood() || nerd == pLocal || nerd != ply)
			continue;
		select = true;
		float newest_sim_time = -100.f;

		if (!find.second.size()) {
			return false;
		}
		bool have_trusted_record = false;
		for (auto it : find.second) {
			if (it.m_priority == tick_trusted) {
				ret = it;
				have_trusted_record = true;
				break;
			}
		}
		if (have_trusted_record)break;

		for (auto it : find.second) {
			if (it.m_flSimulationTime > newest_sim_time) {
				ret = it;
			}
		}
		break;
	}
	if (!select)return false;
	tick_out = ret;
	old_tick = build_tick(ply);

	set_tick_force(ply, ret);

	return true;
}

void LagCompensation::FinishLagCompensation(C_GMOD_Player * ply)
{
	set_tick_force(ply, old_tick);
}
