#include "Esp.h"
#include "../../engine/Engine.h"
//#include "../Util/EngineDraw.h"
#include "../Hooks/Hooks.h"
#include "../Functional/FriendList.h"
void cEsp::Esp_Tick()
{
	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal)
		return;
	this->pLocal = pLocal;

	if (Engine::Var::Instance().PropList_Toggle.Get()) {
		EntEsp::Instance().Update_Ent_vec();
		EntEsp::Instance().Update_Temp_Ent();
	}
	for (auto i_Num = 0; i_Num <= g_pEntList->GetMaxEntities(); i_Num++) {
		auto pEntity = C_BaseEntity::GetBaseEntity(i_Num);

		if (!pEntity || !pEntity->IsValid() || pEntity == pLocal)
			continue;
		if (!SDK::W2S(pEntity->m_vecOrigin(), Vector())) {
			continue;
		}
		
		this->pEntity = pEntity;
		auto pGPlayer = ToGMOD_Player(ToBasePlayer(pEntity));

		pGPlayer
			? GPlayer_Tick(pGPlayer)
			: BaseEnt_Tick(pEntity);
	}
}

void cEsp::GPlayer_Tick(C_GMOD_Player * ply)
{
	if (!ply->IsGood())
		return;
	player_info_t info = {};
	if (!g_pEngine->GetPlayerInfo(ply->entindex(), &info))
		return;

	if (Engine::Var::Instance().Esp_Bot_Check.Get())
		if (info.fakeplayer)
			return;
	Vector Distance = pLocal->m_vecOrigin() - ply->m_vecOrigin();

	if (Distance.Length() >= Engine::Var::Instance().Esp_Max_Distance.Get())
		return;

	if (!ply->GetBoneTransform(pBoneToWorldOut))
		return;

	auto transform = ply->m_rgflCoordinateFrame();
	auto mins = ply->OBBMins();
	auto maxs = ply->OBBMaxs();

	Vector points[] = {
	{ mins[0], mins[1], mins[2] },
	{ mins[0], maxs[1], mins[2] },
	{ maxs[0], maxs[1], mins[2] },
	{ maxs[0], mins[1], mins[2] },
	{ maxs[0], maxs[1], maxs[2] },
	{ mins[0], maxs[1], maxs[2] },
	{ mins[0], mins[1], maxs[2] },
	{ maxs[0], mins[1], maxs[2] },
	};
	for (auto i = 0; i < 8; i++)
		pointsTransformed[i] = points[i].Transform(transform);

	if (!SDK::W2S(pointsTransformed[3], flb) || !SDK::W2S(pointsTransformed[5], brt)
		|| !SDK::W2S(pointsTransformed[0], blb) || !SDK::W2S(pointsTransformed[4], frt)
		|| !SDK::W2S(pointsTransformed[2], frb) || !SDK::W2S(pointsTransformed[1], brb)
		|| !SDK::W2S(pointsTransformed[6], blt) || !SDK::W2S(pointsTransformed[7], flt))
		return;



	Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };
	left = flb[0];
	top = flb[1];
	right = flb[0];
	bottom = flb[1];

	for (int i = 1; i < 8; i++)
	{
		if (left > arr[i][0])
			left = arr[i][0];
		if (top < arr[i][1])
			top = arr[i][1];
		if (right < arr[i][0])
			right = arr[i][0];
		if (bottom > arr[i][1])
			bottom = arr[i][1];
	}
	x = std::round(left);
	y = std::round(bottom);

	w = std::round(right - left);
	h = std::round(top - bottom);


	this->Team = Lua::GetTeam(ply->entindex());
	if (this->Team == -1337)
		return;
	this->TeamName = Lua::GetTeamName(this->Team);
	if (!this->TeamName || strcmp(this->TeamName, "") == 0)
		return;
	this->TeamColor = Lua::GetTeamColor(this->Team);

	auto l_Team = Lua::GetTeam(pLocal->entindex());
	if (l_Team == -1337)
		return;
	auto l_Team_col = Lua::GetTeamColor(l_Team);
	if (Engine::Var::Instance().Esp_Team_Check.Get()) {
		if (this->TeamColor.GetRawColor() == l_Team_col.GetRawColor()) {
			return;
		}
	}

	b_use_team_col = Engine::Var::Instance().Esp_Colors_By_Class.Get();
	b_is_friend = FriendList::Instance().IsFriend(ply);





	if (Engine::Var::Instance().Esp_Box2D.Get())
		DrawBox2D(ply);
	if (Engine::Var::Instance().Esp_Box3D.Get())
		DrawBox3D(ply);
	if (Engine::Var::Instance().Esp_Skeleton.Get())
		DrawSkeleton(ply);
	if (Engine::Var::Instance().Esp_Head_Circle.Get())
		DrawHeadCircle(ply);


	DrawInfo(ply, info);
}

void cEsp::BaseEnt_Tick(C_BaseEntity * ent)
{

	if (!ent->GetScriptName() || IsBadReadPtr(ent->GetScriptName(), 0x4))
		return;

	auto script_name = ent->GetScriptName();

	if (Engine::Var::Instance().Keypad_Logger.Get()) {

		bool only_update = false;
		int iter = 0;
		if (strcmp("keypad", script_name))
			goto key_end;

		for (auto it : KeypadList) {
			auto ent_k = C_BaseEntity::GetBaseEntity(it.EntityIdx);
			if (!ent || !ent->IsValid())
				continue;
			if (ent_k == ent) {
				only_update = true;
				break;
			}
			iter++;
		}

		if (!only_update) {
			KeypadRender KeyRend = KeypadRender();

			KeyRend.EntityIdx = ent->entindex();
			KeyRend.secured = Lua::GetSecureKeypad(ent->entindex());
			KeypadList.push_back(KeyRend);
		}
		else {
			KeypadList[iter].EntityIdx = pEntity->entindex();

			if (!Lua::GetSecureKeypad(pEntity->entindex())) {
				if (!Lua::GetKeypadTempCode(pEntity->entindex()))
					goto key_end;
				KeypadList[iter].TempCode = std::string();
				KeypadList[iter].TempCode = std::string(Lua::GetKeypadTempCode(pEntity->entindex()));
				if (Lua::GetStatusKeypad(pEntity->entindex()) == Lua::KeypadStatus::Status_Granted &&
					(!KeypadList[iter].StaticCode.size() || strcmp(KeypadList[iter].TempCode.c_str(), KeypadList[iter].StaticCode.c_str()))) {
					KeypadList[iter].StaticCode = std::string();
					KeypadList[iter].StaticCode = std::string(Lua::GetKeypadTempCode(pEntity->entindex()));
				}


			}
			else {
				if (!Lua::GetKeypadTempCode(pEntity->entindex()))
					goto key_end;
				auto status = Lua::GetStatusKeypad(ent->entindex());
				for (auto i_Num = 0; i_Num <= g_pEntList->GetMaxEntities(); i_Num++) {
					auto pEntity = C_BaseEntity::GetBaseEntity(i_Num);

					if (!pEntity || !pEntity->IsValid())
						continue;

					auto pGPlayer = ToGMOD_Player(ToBasePlayer(pEntity));
					if (!pGPlayer)
						continue;

					Vector Dist = (ent->m_vecOrigin() - pGPlayer->m_vecOrigin());
					if (abs(Dist.Length()) >= 300.f)
						continue;

					if (!Lua::GetKeypadTempCode(ent->entindex())|| IsBadReadPtr(Lua::GetKeypadTempCode(ent->entindex()), 0x4))
						break;
					if (KeypadList[iter].TempCode.size() != std::string(Lua::GetKeypadTempCode(ent->entindex())).size() &&
						status != Lua::KeypadStatus::Status_Granted&&status != Lua::KeypadStatus::Status_Denied) {
						if (Lua::GetHoveredElement(pGPlayer, ent) > 0) {
							KeypadList[iter].TempCode += std::to_string(Lua::GetHoveredElement(pGPlayer, ent));
						}
					}
				}
				if (status == Lua::KeypadStatus::Status_Granted && (!KeypadList[iter].StaticCode.size() || KeypadList[iter].TempCode.size() && KeypadList[iter].StaticCode != KeypadList[iter].TempCode)) {
					KeypadList[iter].StaticCode = KeypadList[iter].TempCode;
					KeypadList[iter].TempCode.clear();
				}
				if (status == Lua::KeypadStatus::Status_Denied) {
					KeypadList[iter].TempCode.clear();
				}
				if (status == Lua::KeypadStatus::Status_None//crash
					&&KeypadList[iter].TempCode.size() != std::string(Lua::GetKeypadTempCode(ent->entindex())).size()) {
					KeypadList[iter].TempCode.clear();
				}
			}

			auto transform = ent->m_rgflCoordinateFrame();
			auto mins = ent->OBBMins();
			auto maxs = ent->OBBMaxs();

			Vector points[] = {
				{ mins[0], mins[1], mins[2] },
			{ mins[0], maxs[1], mins[2] },
			{ maxs[0], maxs[1], mins[2] },
			{ maxs[0], mins[1], mins[2] },
			{ maxs[0], maxs[1], maxs[2] },
			{ mins[0], maxs[1], maxs[2] },
			{ mins[0], mins[1], maxs[2] },
			{ maxs[0], mins[1], maxs[2] },
			};
			for (auto i = 0; i < 8; i++)
				pointsTransformed[i] = points[i].Transform(transform);

			if (!SDK::W2S(pointsTransformed[3], flb) || !SDK::W2S(pointsTransformed[5], brt)
				|| !SDK::W2S(pointsTransformed[0], blb) || !SDK::W2S(pointsTransformed[4], frt)
				|| !SDK::W2S(pointsTransformed[2], frb) || !SDK::W2S(pointsTransformed[1], brb)
				|| !SDK::W2S(pointsTransformed[6], blt) || !SDK::W2S(pointsTransformed[7], flt))
				goto key_end;



			Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };
			left = flb[0];
			top = flb[1];
			right = flb[0];
			bottom = flb[1];

			for (int i = 1; i < 8; i++)
			{
				if (left > arr[i][0])
					left = arr[i][0];
				if (top < arr[i][1])
					top = arr[i][1];
				if (right < arr[i][0])
					right = arr[i][0];
				if (bottom > arr[i][1])
					bottom = arr[i][1];
			}


			auto& draw = SDK::CDraw::Instance();
			wchar_t Temp[256];
			float Delta = g_pSurface->GetFontTall(draw.font_esp) / 0.7272727272727273f;
			int idx = 1;

			MultiByteToWideChar(CP_UTF8, NULL, &KeypadList[iter].TempCode[0], -1, &Temp[0], 255);
			draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, Color(Engine::Var::Instance().Esp_Text_Color.Get()).to_cColor(), true);
			idx++;
			MultiByteToWideChar(CP_UTF8, NULL, &KeypadList[iter].StaticCode[0], -1, &Temp[0], 255);
			draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, Color(Engine::Var::Instance().Esp_Text_Color.Get()).to_cColor(), true);
		}


	}


key_end:;

	if (Engine::Var::Instance().PropList_Toggle.Get()) {
		//EntEsp::Instance().Update_Ent_vec();
		//EntEsp::Instance().Update_Temp_Ent();
		EntEsp::Instance().EntEsp_tick(ent);
	}


}

void cEsp::DrawBox2D(C_BaseEntity * ent)
{
	auto& draw = SDK::CDraw::Instance();
	if (!b_is_friend) {
		draw.Outline(x, y, w, h, b_use_team_col ? this->TeamColor : Color(Engine::Var::Instance().Esp_Box2D_Color.Get()).to_cColor(), Engine::Var::Instance().Esp_Thickness.Get());
	}
	else {
		draw.Outline(x, y, w, h, Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor(), Engine::Var::Instance().Esp_Thickness.Get());

	}
}

void cEsp::DrawBox3D(C_BaseEntity * ent)
{
	auto& draw = SDK::CDraw::Instance();
	auto col = Color(Engine::Var::Instance().Esp_Box3D_Color.Get()).to_cColor();
	auto l_thick = Engine::Var::Instance().Esp_Thickness.Get();
	auto o_thick = Engine::Var::Instance().Esp_Thickness_Outline.Get();
	draw.Line_outline(flb.m_x, flb.m_y, frb.m_x, frb.m_y, l_thick, o_thick);
	draw.Line_outline(frb.m_x, frb.m_y, brb.m_x, brb.m_y, l_thick, o_thick);
	draw.Line_outline(brb.m_x, brb.m_y, blb.m_x, blb.m_y, l_thick, o_thick);
	draw.Line_outline(blb.m_x, blb.m_y, flb.m_x, flb.m_y, l_thick, o_thick);

	draw.Line_outline(flt.m_x, flt.m_y, frt.m_x, frt.m_y, l_thick, o_thick);
	draw.Line_outline(frt.m_x, frt.m_y, brt.m_x, brt.m_y, l_thick, o_thick);
	draw.Line_outline(brt.m_x, brt.m_y, blt.m_x, blt.m_y, l_thick, o_thick);
	draw.Line_outline(blt.m_x, blt.m_y, flt.m_x, flt.m_y, l_thick, o_thick);

	draw.Line_outline(flb.m_x, flb.m_y, flt.m_x, flt.m_y, l_thick, o_thick);
	draw.Line_outline(frb.m_x, frb.m_y, frt.m_x, frt.m_y, l_thick, o_thick);
	draw.Line_outline(brb.m_x, brb.m_y, brt.m_x, brt.m_y, l_thick, o_thick);
	draw.Line_outline(blb.m_x, blb.m_y, blt.m_x, blt.m_y, l_thick, o_thick);

	////////////////////
	if (!b_is_friend) {

		draw.Line(flb.m_x, flb.m_y, frb.m_x, frb.m_y, b_use_team_col ? this->TeamColor : col, l_thick);
		draw.Line(frb.m_x, frb.m_y, brb.m_x, brb.m_y, b_use_team_col ? this->TeamColor : col, l_thick);
		draw.Line(brb.m_x, brb.m_y, blb.m_x, blb.m_y, b_use_team_col ? this->TeamColor : col, l_thick);
		draw.Line(blb.m_x, blb.m_y, flb.m_x, flb.m_y, b_use_team_col ? this->TeamColor : col, l_thick);

		draw.Line(flt.m_x, flt.m_y, frt.m_x, frt.m_y, b_use_team_col ? this->TeamColor : col, l_thick);
		draw.Line(frt.m_x, frt.m_y, brt.m_x, brt.m_y, b_use_team_col ? this->TeamColor : col, l_thick);
		draw.Line(brt.m_x, brt.m_y, blt.m_x, blt.m_y, b_use_team_col ? this->TeamColor : col, l_thick);
		draw.Line(blt.m_x, blt.m_y, flt.m_x, flt.m_y, b_use_team_col ? this->TeamColor : col, l_thick);

		draw.Line(flb.m_x, flb.m_y, flt.m_x, flt.m_y, b_use_team_col ? this->TeamColor : col, l_thick);
		draw.Line(frb.m_x, frb.m_y, frt.m_x, frt.m_y, b_use_team_col ? this->TeamColor : col, l_thick);
		draw.Line(brb.m_x, brb.m_y, brt.m_x, brt.m_y, b_use_team_col ? this->TeamColor : col, l_thick);
		draw.Line(blb.m_x, blb.m_y, blt.m_x, blt.m_y, b_use_team_col ? this->TeamColor : col, l_thick);
	}
	else {
		col = Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor();
		draw.Line(flb.m_x, flb.m_y, frb.m_x, frb.m_y, col, l_thick);
		draw.Line(frb.m_x, frb.m_y, brb.m_x, brb.m_y, col, l_thick);
		draw.Line(brb.m_x, brb.m_y, blb.m_x, blb.m_y, col, l_thick);
		draw.Line(blb.m_x, blb.m_y, flb.m_x, flb.m_y, col, l_thick);

		draw.Line(flt.m_x, flt.m_y, frt.m_x, frt.m_y, col, l_thick);
		draw.Line(frt.m_x, frt.m_y, brt.m_x, brt.m_y, col, l_thick);
		draw.Line(brt.m_x, brt.m_y, blt.m_x, blt.m_y, col, l_thick);
		draw.Line(blt.m_x, blt.m_y, flt.m_x, flt.m_y, col, l_thick);

		draw.Line(flb.m_x, flb.m_y, flt.m_x, flt.m_y, col, l_thick);
		draw.Line(frb.m_x, frb.m_y, frt.m_x, frt.m_y, col, l_thick);
		draw.Line(brb.m_x, brb.m_y, brt.m_x, brt.m_y, col, l_thick);
		draw.Line(blb.m_x, blb.m_y, blt.m_x, blt.m_y, col, l_thick);
	}
}

void cEsp::DrawSkeleton(C_GMOD_Player * ply)
{
	auto pStudioModel = g_pVModelInfo->GetStudiomodel(ply->GetModel());

	if (!pStudioModel)
		return;
	auto& draw = SDK::CDraw::Instance();
	auto col = Color(Engine::Var::Instance().Esp_Skeleton_Color.Get()).to_cColor();
	wchar_t Temp[256];
	for (int i = 0; i < pStudioModel->numbones; i++) {

		mstudiobone_t* pBone = pStudioModel->GetBone(i);


		if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
			continue;



		Vector vBonePos1;
		if (!SDK::W2S(Vector(pBoneToWorldOut[i][0][3], pBoneToWorldOut[i][1][3], pBoneToWorldOut[i][2][3]), vBonePos1))
			continue;
		//		MultiByteToWideChar(CP_UTF8, NULL, &pBone->GetName()[0], -1, &Temp[0], 255);
		//		draw.text(vBonePos1.m_x, vBonePos1.m_y, Temp, draw.font_esp, Color(Engine::Var::Instance().Esp_Text_Color.Get()).to_cColor(), true);


		Vector vBonePos2;
		if (!SDK::W2S(Vector(pBoneToWorldOut[pBone->parent][0][3], pBoneToWorldOut[pBone->parent][1][3], pBoneToWorldOut[pBone->parent][2][3]), vBonePos2))
			continue;

		draw.Line_outline(int(vBonePos1.m_x), int(vBonePos1.m_y), int(vBonePos2.m_x), int(vBonePos2.m_y), Engine::Var::Instance().Esp_Thickness.Get(), Engine::Var::Instance().Esp_Thickness_Outline.Get());

	}

	for (int i = 0; i < pStudioModel->numbones; i++) {

		mstudiobone_t* pBone = pStudioModel->GetBone(i);


		if (!pBone || !(pBone->flags & 256) || pBone->parent == -1)
			continue;

		Vector vBonePos1;
		if (!SDK::W2S(Vector(pBoneToWorldOut[i][0][3], pBoneToWorldOut[i][1][3], pBoneToWorldOut[i][2][3]), vBonePos1))
			continue;

		Vector vBonePos2;
		if (!SDK::W2S(Vector(pBoneToWorldOut[pBone->parent][0][3], pBoneToWorldOut[pBone->parent][1][3], pBoneToWorldOut[pBone->parent][2][3]), vBonePos2))
			continue;
		if (!b_is_friend) {
			draw.Line(int(vBonePos1.m_x), int(vBonePos1.m_y), int(vBonePos2.m_x), int(vBonePos2.m_y), b_use_team_col ? this->TeamColor : col, Engine::Var::Instance().Esp_Thickness.Get());
		}
		else {
			draw.Line(int(vBonePos1.m_x), int(vBonePos1.m_y), int(vBonePos2.m_x), int(vBonePos2.m_y), Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor(), Engine::Var::Instance().Esp_Thickness.Get());

		}
	}
}

void cEsp::DrawHeadCircle(C_GMOD_Player * ply)
{
	Vector min = {};
	Vector max = {};
	matrix3x4_t bone_data[MAXSTUDIOBONE] = {};

	if (!ply->SetupBones(bone_data, MAXSTUDIOBONE, 0x0100, 0))
		return;

	if (!ply->GetBoxBoundWorld("ValveBiped.Bip01_Head1", bone_data, min, max))
		return;
	auto center_coord = (min + max)*0.5f;
	Vector screen_center;
	if (!SDK::W2S(center_coord, screen_center))
		return;
	Vector screen_max;
	if (!SDK::W2S(max, screen_max))
		return;
	Vector screen_min;
	if (!SDK::W2S(min, screen_min))
		return;

	float Radius = (screen_max - screen_min).Length() / 2;

	auto& draw = SDK::CDraw::Instance();
	auto col = Color(Engine::Var::Instance().Esp_Head_Color.Get()).to_cColor();
	if (!b_is_friend) {
		draw.ColoredCircle(int(screen_center[0]), int(screen_center[1]), Radius + 1, cColor(0, 0, 0, 255));
		draw.ColoredCircle(int(screen_center[0]), int(screen_center[1]), Radius - 1, cColor(0, 0, 0, 255));
		draw.ColoredCircle(int(screen_center[0]), int(screen_center[1]), Radius, b_use_team_col ? this->TeamColor : col);
	}
	else {
		draw.ColoredCircle(int(screen_center[0]), int(screen_center[1]), Radius + 1, cColor(0, 0, 0, 255));
		draw.ColoredCircle(int(screen_center[0]), int(screen_center[1]), Radius - 1, cColor(0, 0, 0, 255));
		draw.ColoredCircle(int(screen_center[0]), int(screen_center[1]), Radius, Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor());

	}
}

void cEsp::DrawInfo(C_GMOD_Player * ply, player_info_t& info)
{
	auto& draw = SDK::CDraw::Instance();
	wchar_t Temp[256];

	auto Delta = g_pSurface->GetFontTall(draw.font_esp) / 0.7272727272727273f;
	auto text_col = Color(Engine::Var::Instance().Esp_Text_Color.Get()).to_cColor();
	int idx = 1;
	if (Engine::Var::Instance().Esp_Active_Weapon.Get()) {
		char* weap_name = nullptr;
		if (ply->m_hActiveWeapon()) {
			auto Weapon = ply->GetActiveWeapon();
			if (Weapon && !IsBadReadPtr(Weapon, 0x4)) {
				if (Weapon->GetScriptName() && !IsBadReadPtr(Weapon->GetScriptName(), 0x4)) {
					weap_name = Weapon->GetScriptName();
				}
				else {
					weap_name = Weapon->GetClientClass()->GetName();
				}
			}
		}
		if (!weap_name) {
			weap_name = "None";
		}
		MultiByteToWideChar(CP_UTF8, NULL, &weap_name[0], -1, &Temp[0], 255);
		if (!b_is_friend) {
			draw.text((left + right) / 2, bottom - Delta, Temp, draw.font_esp, b_use_team_col ? this->TeamColor : text_col, true);
		}
		else {
			draw.text((left + right) / 2, bottom - Delta, Temp, draw.font_esp, Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor(), true);

		}
	}
	if (Engine::Var::Instance().Esp_HP.Get()) {

		draw.Line_outline(left - 4, bottom - 1, left - 4, top +1, 4, 2);
		auto procent_ = float(ply->m_iHealth()) / float(ply->m_iMaxHealth());
		auto height_delta = bottom - top;

		procent_ <= 1.f
			? draw.Line(left - 4, top, left - 4.f, top + (height_delta*procent_), cColor(0, 255, 0, 255), 4)
			: draw.Line(left - 4, bottom, left - 4, top, cColor(0, 255, 0, 255), 4);

	}
	if (Engine::Var::Instance().Esp_Distance.Get()) {
		//	swprintf_s(Temp, L"%f", ply->m_flSimulationTime());
		swprintf_s(Temp, L"%.1f", (pLocal->m_vecOrigin() - ply->m_vecOrigin()).Length());
		if (!b_is_friend) {
			draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, b_use_team_col ? this->TeamColor : text_col, true);
		}
		else {
			draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor(), true);

		}
		idx++;
	}

	if (Engine::Var::Instance().Esp_Steam_Name.Get()) {
		MultiByteToWideChar(CP_UTF8, NULL, &info.name[0], -1, &Temp[0], 255);
		if (!b_is_friend) {
			draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, b_use_team_col ? this->TeamColor : text_col, true);
		}
		else {
			draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor(), true);

		}
		idx++;
	}
	if (Engine::Var::Instance().Esp_RP_Name.Get()) {
		auto rp_name = Lua::GetName(ply->entindex());
		if (!rp_name)
			rp_name = "None";
		MultiByteToWideChar(CP_UTF8, NULL, &rp_name[0], -1, &Temp[0], 255);
		if (!b_is_friend) {
			draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, b_use_team_col ? this->TeamColor : text_col, true);
		}
		else {
			draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor(), true);

		}
		idx++;
	}
	if (Engine::Var::Instance().Esp_Class.Get()) {
		MultiByteToWideChar(CP_UTF8, NULL, &TeamName[0], -1, &Temp[0], 255);
		if (!b_is_friend) {
			draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, b_use_team_col ? this->TeamColor : text_col, true);
		}
		else {
			draw.text((left + right)*0.5f, top + idx * Delta, Temp, draw.font_esp, Color(Engine::Var::Instance().Friend_Color.Get()).to_cColor(), true);

		}
		idx++;
	}

}

void EntEsp::EntEsp_tick(C_BaseEntity * ent)
{
	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal)
		return;
	auto script_name = ent->GetScriptName();

	std::string ent_name = script_name;
	if (!ent_name.size())
		return;

	if (ent_name.find("gmodz") != -1) {
		ent_name = get_model_name(ent);
	}

	if (!ent_name.size())return;


	if (Ent_vec.size()) {
		auto iter = 0u;
		for (auto it : Ent_vec) {
			iter++;
			auto var_bool = it->ExtractAs<bool>();
			if (!var_bool)
				continue;

			if (var_bool->s_Name == ent_name) {
				if (var_bool->Get()) {
					if (Ent_vec.size() < iter + 1)
						break;
					auto var_col = Ent_vec[iter]->ExtractAs<D3DCOLOR>();
					if (!var_col)
						return;

					Vector Dist = pLocal->m_vecOrigin() - ent->m_vecOrigin();
					if (Dist.Length() > Engine::Var::Instance().PropList_Distance.Get())
						return;
					auto transform = ent->m_rgflCoordinateFrame();
					auto mins = ent->OBBMins();
					auto maxs = ent->OBBMaxs();

					Vector points[] = {
						{ mins[0], mins[1], mins[2] },
					{ mins[0], maxs[1], mins[2] },
					{ maxs[0], maxs[1], mins[2] },
					{ maxs[0], mins[1], mins[2] },
					{ maxs[0], maxs[1], maxs[2] },
					{ mins[0], maxs[1], maxs[2] },
					{ mins[0], mins[1], maxs[2] },
					{ maxs[0], mins[1], maxs[2] },
					};
					for (auto i = 0; i < 8; i++)
						pointsTransformed[i] = points[i].Transform(transform);

					if (!SDK::W2S(pointsTransformed[3], flb) || !SDK::W2S(pointsTransformed[5], brt)
						|| !SDK::W2S(pointsTransformed[0], blb) || !SDK::W2S(pointsTransformed[4], frt)
						|| !SDK::W2S(pointsTransformed[2], frb) || !SDK::W2S(pointsTransformed[1], brb)
						|| !SDK::W2S(pointsTransformed[6], blt) || !SDK::W2S(pointsTransformed[7], flt))
						return;

					auto& draw = SDK::CDraw::Instance();
					auto col = Color(var_col->Get()).to_cColor();
					auto l_thick = Engine::Var::Instance().Esp_Thickness.Get();
					auto o_thick = Engine::Var::Instance().Esp_Thickness_Outline.Get();
					draw.Line_outline(flb.m_x, flb.m_y, frb.m_x, frb.m_y, l_thick, o_thick);
					draw.Line_outline(frb.m_x, frb.m_y, brb.m_x, brb.m_y, l_thick, o_thick);
					draw.Line_outline(brb.m_x, brb.m_y, blb.m_x, blb.m_y, l_thick, o_thick);
					draw.Line_outline(blb.m_x, blb.m_y, flb.m_x, flb.m_y, l_thick, o_thick);

					draw.Line_outline(flt.m_x, flt.m_y, frt.m_x, frt.m_y, l_thick, o_thick);
					draw.Line_outline(frt.m_x, frt.m_y, brt.m_x, brt.m_y, l_thick, o_thick);
					draw.Line_outline(brt.m_x, brt.m_y, blt.m_x, blt.m_y, l_thick, o_thick);
					draw.Line_outline(blt.m_x, blt.m_y, flt.m_x, flt.m_y, l_thick, o_thick);

					draw.Line_outline(flb.m_x, flb.m_y, flt.m_x, flt.m_y, l_thick, o_thick);
					draw.Line_outline(frb.m_x, frb.m_y, frt.m_x, frt.m_y, l_thick, o_thick);
					draw.Line_outline(brb.m_x, brb.m_y, brt.m_x, brt.m_y, l_thick, o_thick);
					draw.Line_outline(blb.m_x, blb.m_y, blt.m_x, blt.m_y, l_thick, o_thick);

					////////////////////

					draw.Line(flb.m_x, flb.m_y, frb.m_x, frb.m_y, col, l_thick);
					draw.Line(frb.m_x, frb.m_y, brb.m_x, brb.m_y, col, l_thick);
					draw.Line(brb.m_x, brb.m_y, blb.m_x, blb.m_y, col, l_thick);
					draw.Line(blb.m_x, blb.m_y, flb.m_x, flb.m_y, col, l_thick);

					draw.Line(flt.m_x, flt.m_y, frt.m_x, frt.m_y, col, l_thick);
					draw.Line(frt.m_x, frt.m_y, brt.m_x, brt.m_y, col, l_thick);
					draw.Line(brt.m_x, brt.m_y, blt.m_x, blt.m_y, col, l_thick);
					draw.Line(blt.m_x, blt.m_y, flt.m_x, flt.m_y, col, l_thick);

					draw.Line(flb.m_x, flb.m_y, flt.m_x, flt.m_y, col, l_thick);
					draw.Line(frb.m_x, frb.m_y, frt.m_x, frt.m_y, col, l_thick);
					draw.Line(brb.m_x, brb.m_y, brt.m_x, brt.m_y, col, l_thick);
					draw.Line(blb.m_x, blb.m_y, blt.m_x, blt.m_y, col, l_thick);

					if (Engine::Var::Instance().PropList_Render_Name.Get()) {
						Vector arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };
						float left = flb.m_x;
						float top = flb.m_y;
						float right = flb.m_x;
						float bottom = flb.m_y;

						for (int i = 1; i < 8; i++) {
							if (left > arr[i].m_x)
								left = arr[i].m_x;
							if (top < arr[i].m_y)
								top = arr[i].m_y;
							if (right < arr[i].m_x)
								right = arr[i].m_x;
							if (bottom > arr[i].m_y)
								bottom = arr[i].m_y;
						}
						wchar_t Temp[256];
						//	auto text_col = Color(Engine::Var::Instance().Esp_Text_Color.Get()).to_cColor();
						MultiByteToWideChar(CP_UTF8, NULL, &ent_name[0], -1, &Temp[0], 255);
						draw.text((left + right)*0.5f, top + 0.5f, Temp, draw.font_esp, col, true);
					}

				}

				else {
					break;
				}
			}
		}
	}


}

void EntEsp::Update_Temp_Ent()
{
	if (!g_pEngine->IsInGame() || g_pEngine->IsDrawingLoadingImage())
		return;
	static float Show;
	float upd_Time = 0.1f*g_pGlobalVars->absoluteframetime;
	Show += upd_Time;
	if (Show <= 1.9f) {
		return;
	}
	Show = 0.f;

	if (Temp_Ent.size()) {
		Temp_Ent.clear();
	}
	for (auto i_Num = 0u; i_Num <= g_pEntList->GetMaxEntities(); i_Num++) {
		auto ent = C_BaseEntity::GetBaseEntity(i_Num);
		if (!ent || !ent->IsValid() || !ent->GetScriptName() || IsBadReadPtr(ent->GetScriptName(), 0x4))
			continue;
		std::string ent_name = ent->GetScriptName();
		if (!ent_name.size())
			continue;
		if (ent_name.find("gmodz") != -1) {
			ent_name = get_model_name(ent);
		}
		if (exist_in_temp(ent_name) || exist_in_list(ent_name))
			continue;

		Temp_Ent.push_back(ent_name);
	}


}

void EntEsp::Force_Update_Temp_Ent()
{
	if (Temp_Ent.size()) {
		Temp_Ent.clear();
	}
	for (auto i_Num = 0u; i_Num <= g_pEntList->GetMaxEntities(); i_Num++) {
		auto ent = C_BaseEntity::GetBaseEntity(i_Num);
		if (!ent || !ent->IsValid() || !ent->GetScriptName() || IsBadReadPtr(ent->GetScriptName(), 0x4))
			continue;
		std::string ent_name = ent->GetScriptName();
		if (!ent_name.size())
			continue;
		if (ent_name.find("gmodz") != -1) {
			ent_name = get_model_name(ent);
		}
		if (exist_in_temp(ent_name) || exist_in_list(ent_name))
			continue;

		Temp_Ent.push_back(ent_name);
	}
}

void EntEsp::Update_Ent_vec()
{
	if (!is_vec_change&&Engine::Var::Instance().EngineVars.ParseVarGroup("Ent_vec").size() == Ent_vec.size())
		return;
	Ent_vec = Engine::Var::Instance().EngineVars.ParseVarGroup("Ent_vec");
	is_vec_change = false;
}

void EntEsp::Vis_PropEsp_tick()
{
	static C_BaseEntity* LastSelected;
	static float Show = 0.f;


	auto pLocal = C_GMOD_Player::GetLocalPlayer();
	if (!pLocal)
		return;
	Vector EyePos = pLocal->EyePosition();
	Vector Forward;
	Hooks::Variables::PureAngle.ToVector(&Forward);
	Vector start = EyePos;
	Vector end = EyePos + Forward * Engine::Var::Instance().PropList_Distance.Get();
	trace_t Trace;

	static float Step;
	Step = (1.f / Engine::Var::Instance().PropList_HoldTime.Get())*g_pGlobalVars->absoluteframetime;
	SDK::Function::UTIL_TraceLine(start, end, MASK_SOLID, pLocal, 0, &Trace);

	if (Trace.m_pEnt&&Trace.m_pEnt->GetScriptName() && !IsBadReadPtr(Trace.m_pEnt->GetScriptName(), 0x4)) {
		if (!Trace.m_pEnt || Trace.m_pEnt->entindex() == -1)
			return;
		auto ent = C_BaseEntity::GetBaseEntity(Trace.m_pEnt->entindex());
		if (!ent || !ent->IsValid())
			return;
		if (LastSelected == ent) {
			Show += Step;
			Show = clamp(Show, 0.f, 1.f);
			std::string ent_name = ent->GetScriptName();
			if (!ent_name.size())
				return;
			if (ent_name.find("gmodz") != -1) {
				ent_name = get_model_name(ent);
			}
			if (Show >= 0.9) {
				auto text_col = Color(Engine::Var::Instance().Esp_Text_Color.Get()).to_cColor();
				auto& draw = SDK::CDraw::Instance();
				wchar_t Temp[256];
				int w, h;
				g_pEngine->GetScreenSize(w, h);

				MultiByteToWideChar(CP_UTF8, NULL, &ent_name[0], -1, &Temp[0], 255);
				draw.text(w / 2, h / 2, Temp, draw.font_esp, text_col, true);

				if (GetAsyncKeyState(Engine::Var::Instance().PropList_Fast_Add_Key.Get()) && !exist_in_list(ent_name)) {

					Engine::Var::Instance().EngineVars.BeginGroup("Ent_vec");
					Engine::Var::Instance().EngineVars.RegisterVar(*new Core::VarUtil::Var<bool>, true, ent_name.c_str(), Core::VarUtil::v_dynamic);
					Engine::Var::Instance().EngineVars.RegisterVar(*new Core::VarUtil::Var<D3DCOLOR>, D3DCOLOR_ARGB(250, 250, 250, 255), std::string(ent_name + "_color"), Core::VarUtil::v_dynamic);
					Engine::Var::Instance().EngineVars.EndGroup();
					is_vec_change = true;
					EntEsp::Instance().Update_Ent_vec();
					EntEsp::Instance().Force_Update_Temp_Ent();
				}
				if (GetAsyncKeyState(Engine::Var::Instance().PropList_Fast_Del_Key.Get())) {


					std::string name = ent_name;
					std::string name_col = std::string(std::string(ent_name) + "_color");
					auto var = Engine::Var::Instance().EngineVars.FindVar(name, "Ent_vec");
					while (var) {
						Engine::Var::Instance().EngineVars.DeleteVar(*var);
						var = Engine::Var::Instance().EngineVars.FindVar(name.c_str(), "Ent_vec");
					}
					auto iter = 0u;
					for (auto it : EntEsp::Instance().Ent_vec) {
						if (!it || !it->s_Name) {
							iter++;
							continue;
						}
						if (strcmp(it->s_Name, name.c_str()) == 0) {
							EntEsp::Instance().Ent_vec.erase(EntEsp::Instance().Ent_vec.begin() + iter);
							iter--;
						}
						iter++;
					}

					auto var_col = Engine::Var::Instance().EngineVars.FindVar(name_col, "Ent_vec");
					while (var_col) {
						Engine::Var::Instance().EngineVars.DeleteVar(*var_col);
						var_col = Engine::Var::Instance().EngineVars.FindVar(name_col, "Ent_vec");
					}
					iter = 0u;
					for (auto it : EntEsp::Instance().Ent_vec) {
						if (!it || !it->s_Name) {
							iter++;
							continue;
						}
						if (strcmp(it->s_Name, name_col.c_str()) == 0) {
							EntEsp::Instance().Ent_vec.erase(EntEsp::Instance().Ent_vec.begin() + iter);
							iter--;
						}
						iter++;
					}
					EntEsp::Instance().is_vec_change = true;
					EntEsp::Instance().Update_Ent_vec();
					EntEsp::Instance().Force_Update_Temp_Ent();
				}
			}
		}
		else {
			Show = 0.f;
			LastSelected = ent;
		}
	}
	else {
		LastSelected = nullptr;
	}

}

bool EntEsp::exist_in_temp(std::string & name)
{
	for (auto a_name : Temp_Ent) {
		if (a_name == name) {
			return true;
		}
	}
	return false;
}

bool EntEsp::exist_in_list(std::string & name)
{
	for (auto it : Ent_vec) {
		auto b_var = it->ExtractAs<bool>();
		if (!b_var)
			continue;
		if (b_var->s_Name == name) {
			return true;
		}
	}
	return false;
}

std::string EntEsp::get_model_name(C_BaseEntity * ent)
{

	std::string ret = std::string();
	auto Model = ent->GetModel();
	if (!Model) {
		return ret;
	}
	auto pStudioModel = g_pVModelInfo->GetStudiomodel(Model);
	if (!pStudioModel) {
		return ret;
	}

	ret = pStudioModel->name;
	if (!ret.size()) {
		return ret;
	}
	if (ret.find_last_of('/') != -1) {
		ret = (ret.substr(ret.find_last_of('/') + 1));
	}
	if (ret.find_last_of('\\') != -1) {
		ret = (ret.substr(ret.find_last_of('\\') + 1));
	}
	if (ret.find('.') != -1) {
		ret = (ret.substr(0, ret.find('.')));
	}

	return ret;
}
