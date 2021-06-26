#pragma once
#include "../include/stdafx.h"
#include "Core\Basic\Basic.h"
#include "../engine/Core/Var/decl_var.h"

namespace Engine {
	extern HMODULE h_image_base ;
	static std::string s_disc_name;

	bool Initialize(HMODULE hDll);

	void Finalize();

	void msg_box(const char *fmt, ...);


	void WriteFile(const char *fmt, ...);

	class Var : public Basic::Singleton<Var>
	{
	public:
		 Core::VarUtil::sysVar EngineVars;

		 Core::VarUtil::Var<int>   Menu_Key;
		 Core::VarUtil::Var<int>   Hide_Cheat_Key;

#pragma region Aimbot
		 Core::VarUtil::Var<bool>  Aim_Enable;
		 Core::VarUtil::Var<bool>  Aim_Silent;
		 Core::VarUtil::Var<bool>  Aim_pSilent;
		 Core::VarUtil::Var<bool>  Aim_If_Can_Shot;
		 Core::VarUtil::Var<bool>  Randomize;
		 Core::VarUtil::Var<bool>  Aim_NoRecoil;
		 Core::VarUtil::Var<bool>  Aim_Hitscan;
		 Core::VarUtil::Var<bool>  Aim_NoSpread;
		 Core::VarUtil::Var<bool>  Aim_AutoPistol;
		 Core::VarUtil::Var<bool>  Aim_Auto_Shot;
		 Core::VarUtil::Var<bool>  Aim_Auto_Reload;
		 Core::VarUtil::Var<bool>  Aim_Bot_Check;
		 Core::VarUtil::Var<bool>  Aim_Team_Check;
		 Core::VarUtil::Var<int>   Aim_Key;
		 Core::VarUtil::Var<int>   Aim_Bone;
		 Core::VarUtil::Var<int>   Aim_Priority;//0 random //1 hp //2 Dist
		 Core::VarUtil::Var<float> Aim_Fov;
		 Core::VarUtil::Var<bool>  Instant_Swap;
		 Core::VarUtil::Var<bool>  Instant_Reload;
		 Core::VarUtil::Var<float> Instant_Reload_Scale;
		 Core::VarUtil::Var<bool>  Rapid_Fire;
		 Core::VarUtil::Var<int>   Rapid_Fire_Key;
		 Core::VarUtil::Var<bool>  Aim_WallScan;
		 Core::VarUtil::Var<bool>  Aim_LagCompensation;


		 Core::VarUtil::Var<bool>  Aim_Draw_Fov;
		 Core::VarUtil::Var<float> Aim_Smooth;
		 Core::VarUtil::Var<bool>  AntiAim_Enable;
		 Core::VarUtil::Var<bool>  AntiAim_CAC_Safe;
		 Core::VarUtil::Var<int>   AntiAim_Pitch;
		 Core::VarUtil::Var<int>   AntiAim_Yaw;
		 Core::VarUtil::Var<int>   AntiAim_Fake_Yaw;
		 Core::VarUtil::Var<float> AntiAim_Fake_Yaw_Add;
#pragma endregion
#pragma region Esp
		 Core::VarUtil::Var<bool>  Esp_Halo;
		 Core::VarUtil::Var<int>   Halo_Passes;
		 Core::VarUtil::Var<float> Halo_Bloom;
		 Core::VarUtil::Var<bool>  Chams;
		 Core::VarUtil::Var<int>   Chams_Type;
		 Core::VarUtil::Var<float> Chams_Blend;

		 Core::VarUtil::Var<bool>  Keypad_Logger;
		 Core::VarUtil::Var<bool>  Esp_Box2D;
		 Core::VarUtil::Var<bool>  Esp_Box3D;
		 Core::VarUtil::Var<bool>  Esp_Skeleton;
		 Core::VarUtil::Var<bool>  Esp_Head_Circle;
		 Core::VarUtil::Var<bool>  Esp_HP;
		 Core::VarUtil::Var<bool>  Esp_Distance;
		 Core::VarUtil::Var<bool>  Esp_Active_Weapon;
		 Core::VarUtil::Var<bool>  Esp_Steam_Name;
		 Core::VarUtil::Var<bool>  Esp_RP_Name;
		 Core::VarUtil::Var<bool>  Esp_Class;
		 Core::VarUtil::Var<bool>  Esp_Role;
		 Core::VarUtil::Var<bool>  Esp_Colors_By_Class;
		 Core::VarUtil::Var<bool>  Esp_Bot_Check;
		 Core::VarUtil::Var<bool>  Esp_Team_Check;
		 Core::VarUtil::Var<float> Esp_Max_Distance;
		 Core::VarUtil::Var<float> Esp_Thickness;
		 Core::VarUtil::Var<float> Esp_Thickness_Outline;
		 Core::VarUtil::Var<float> Esp_Text_Size;
		 Core::VarUtil::Var<bool>  Esp_Tracer;
		 Core::VarUtil::Var<bool>  Only_Visible_Traces;
		 Core::VarUtil::Var<D3DCOLOR>Tracer_Color;
		 Core::VarUtil::Var<float> Tracer_holdtime;
		 Core::VarUtil::Var<D3DCOLOR>Chams_Visible;
		 Core::VarUtil::Var<D3DCOLOR>Chams_Invisible;
		 Core::VarUtil::Var<D3DCOLOR>Esp_Halo_Color;

		 Core::VarUtil::Var<D3DCOLOR>Esp_Box2D_Color;
		 Core::VarUtil::Var<D3DCOLOR>Esp_Box3D_Color;
		 Core::VarUtil::Var<D3DCOLOR>Esp_Skeleton_Color;
		 Core::VarUtil::Var<D3DCOLOR>Esp_Head_Color;
		 Core::VarUtil::Var<D3DCOLOR>Esp_Text_Color;
		 Core::VarUtil::Var<bool>  Fake_angle_Chams;
		 Core::VarUtil::Var<float> Fake_angle_Chams_Blend;
		 Core::VarUtil::Var<int>   Fake_angle_Chams_Type;
		 Core::VarUtil::Var<D3DCOLOR>Fake_Angle_Color;
		 Core::VarUtil::Var<D3DCOLOR>Friend_Color;
#pragma endregion

#pragma region Misc
		 Core::VarUtil::Var<bool>  Bhop;
		 Core::VarUtil::Var<bool>  Bhop_Safe_Mod;
		 Core::VarUtil::Var<bool>  Ground_Strafe;
		 Core::VarUtil::Var<bool>  Strafe_Helper;
		 Core::VarUtil::Var<bool>  Auto_Strafe;
		 Core::VarUtil::Var<int>   Fake_Lag;
		 Core::VarUtil::Var<bool>  Achievement_Spam;
		 Core::VarUtil::Var<bool>  NoName;
		 Core::VarUtil::Var<bool>  Spectator_List;
		 Core::VarUtil::Var<bool>  No_Visible_Recoil;
		 Core::VarUtil::Var<bool>  Debug_Camera;
		 Core::VarUtil::Var<float> Debug_Camera_Speed;
		 Core::VarUtil::Var<int>   Debug_Camera_Key;
		 Core::VarUtil::Var<bool>  Freeze_Players;
		 Core::VarUtil::Var<bool>  Custom_Disconnect;

		 Core::VarUtil::Var<bool>  Third_Person;
		 Core::VarUtil::Var<float> Third_Person_Distance;
		 Core::VarUtil::Var<int>   Third_Person_Key;

		 Core::VarUtil::Var<float> Fov_changer;
		 Core::VarUtil::Var<float> Viewmodel_changer;

		 Core::VarUtil::Var<bool>  Crosshair;
		 Core::VarUtil::Var<float> Crosshair_Size;
		 Core::VarUtil::Var<float> Crosshair_Thickness;
		 Core::VarUtil::Var<D3DCOLOR>Crosshair_Color;
		 Core::VarUtil::Var<bool>  SV_Lag;
		 Core::VarUtil::Var<int>   SV_Lag_Type;
		 Core::VarUtil::Var<float> SV_Lag_Intensity;
		 Core::VarUtil::Var<int>   SV_Lag_Key;

		 Core::VarUtil::Var<bool>  Special_movefix;
		 Core::VarUtil::Var<int> CurrStateMovementRecorder;
		 Core::VarUtil::Var<bool>  LoopMovementRecorder;


		// Core::VarUtil::Var<bool>	AirStuck;
		// Core::VarUtil::Var<int>	AirStuck_Key;

		 Core::VarUtil::Var<bool>  Fullbright;
#pragma endregion
#pragma region EntityList
		 Core::VarUtil::Var<bool>  PropList_Show_Props;
		 Core::VarUtil::Var<bool>  PropList_Toggle;
		 Core::VarUtil::Var<bool>  PropList_Render_Name;
		 Core::VarUtil::Var<int>   PropList_Fast_Del_Key;
		 Core::VarUtil::Var<int>   PropList_Fast_Add_Key;
		 Core::VarUtil::Var<float> PropList_HoldTime;
		 Core::VarUtil::Var<float> PropList_Distance;


		 Core::VarUtil::Var<bool>  Slow_Motion;
		 Core::VarUtil::Var<float> Slow_Motion_Scale;
		 Core::VarUtil::Var<int>   Slow_Motion_Key;
#pragma endregion


		 Core::VarUtil::Var<bool>	NSend;
		 Core::VarUtil::Var<int>	NSend_Intensity;
		 Core::VarUtil::Var<int>	NSend_Key;



		 Core::VarUtil::Var<std::string>  Temp_str;
		 /*
#pragma region GuiVar
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_Text;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_TextDisabled;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_WindowBg;            // Background of normal windows
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_ChildWindowBg;       // Background of child windows
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_PopupBg;           // Background of popups, menus, tooltips windows
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_Border;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_BorderShadow;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_FrameBg;            // Background of checkbox, radio button, plot, slider, text input
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_FrameBgHovered;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_FrameBgActive;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_TitleBg;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_TitleBgCollapsed;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_TitleBgActive;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_MenuBarBg;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_ScrollbarBg;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_ScrollbarGrab;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_ScrollbarGrabHovered;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_ScrollbarGrabActive;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_ComboBg;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_CheckMark;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_SliderGrab;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_SliderGrabActive;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_Button;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_ButtonHovered;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_ButtonActive;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_Header;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_HeaderHovered;
		 Core::VarUtil::Var<D3DCOLOR>ImGuiCol_HeaderActive;
#pragma endregion*/
	};


}