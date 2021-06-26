
#include "Engine.h"
#include "Render\Render.h"
#include "../gmod_specific/SDK.h"

inline float  ImSaturate(float f) { return (f < 0.0f) ? 0.0f : (f > 1.0f) ? 1.0f : f; };
#define IM_F32_TO_INT8_SAT(_VAL)        ((int)(ImSaturate(_VAL) * 255.0f + 0.5f))   

#define IM_COL32_R_SHIFT    0
#define IM_COL32_G_SHIFT    8
#define IM_COL32_B_SHIFT    16
#define IM_COL32_A_SHIFT    24
uint32_t ColorConvertFloat4ToU32(const ImVec4& in)
{
	uint32_t out = NULL;
	out = ((uint32_t)IM_F32_TO_INT8_SAT(in.x)) << IM_COL32_R_SHIFT;
	out |= ((uint32_t)IM_F32_TO_INT8_SAT(in.y)) << IM_COL32_G_SHIFT;
	out |= ((uint32_t)IM_F32_TO_INT8_SAT(in.z)) << IM_COL32_B_SHIFT;
	out |= ((uint32_t)IM_F32_TO_INT8_SAT(in.w)) << IM_COL32_A_SHIFT;
	return out;
}
namespace Engine {
	HMODULE h_image_base = nullptr;
	bool Initialize(HMODULE hDll) {
		if (Engine::h_image_base != nullptr) {
			return false;
		}
		Engine::h_image_base = hDll;
		Engine::s_disc_name = []()->std::string {
			char buff[MAX_PATH];
			ZeroMemory(buff, sizeof(buff));
			GetCurrentDirectory(MAX_PATH, buff);
			std::string str(buff);
			return str.substr(0, str.find('\\') + 1);
		}();

		[]() {
			auto& var =  Engine::Var::Instance();
			var.EngineVars.RegisterVar(var.Menu_Key, VK_INSERT, "Menu_Key");
			var.EngineVars.RegisterVar(var.Hide_Cheat_Key, VK_HOME, "Hide_Cheat_Key");

			var.EngineVars.BeginGroup("Aimbot");
			{
				var.EngineVars.RegisterVar(var.Aim_Enable, false, "Aim_Enable");
				var.EngineVars.RegisterVar(var.Aim_Silent, false, "Aim_Silent");
				var.EngineVars.RegisterVar(var.Aim_pSilent, false, "Aim_pSilent");
				var.EngineVars.RegisterVar(var.Aim_If_Can_Shot, false, "Aim_If_Can_Shot");
				var.EngineVars.RegisterVar(var.Randomize, false, "Randomize");
				var.EngineVars.RegisterVar(var.Aim_Hitscan, false, "Aim_Hitscan");
				var.EngineVars.RegisterVar(var.Aim_WallScan, false, "Aim_WallScan");

				//Engine::Var::Instance().Aim_LagCompensation.Get() = true;
				//var.EngineVars.RegisterVar(var.Aim_LagCompensation, false, "Aim_LagCompensation");
				var.EngineVars.RegisterVar(var.Aim_NoRecoil, false, "Aim_NoRecoil");
				var.EngineVars.RegisterVar(var.Aim_NoSpread, false, "Aim_NoSpread");
				var.EngineVars.RegisterVar(var.Aim_AutoPistol, false, "Aim_AutoPistol");
				var.EngineVars.RegisterVar(var.Aim_Auto_Shot, false, "Aim_Auto_Shot");
				var.EngineVars.RegisterVar(var.Aim_Auto_Reload, false, "Aim_Auto_Reload");
				var.EngineVars.RegisterVar(var.Aim_Bot_Check, false, "Aim_Bot_Check");
				var.EngineVars.RegisterVar(var.Aim_Team_Check, false, "Aim_Team_Check");
				var.EngineVars.RegisterVar(var.Aim_Key, 0, "Aim_Key");
				var.EngineVars.RegisterVar(var.Aim_Bone, 0, "Aim_Bone");
				var.EngineVars.RegisterVar(var.Aim_Fov, 20.f, "Aim_Fov");
				var.EngineVars.RegisterVar(var.Aim_Draw_Fov, false, "Aim_Draw_Fov");
				var.EngineVars.RegisterVar(var.Aim_Smooth, 8.f, "Aim_Smooth");
				var.EngineVars.RegisterVar(var.Aim_Priority, 2, "Aim_Priority");
				var.EngineVars.RegisterVar(var.AntiAim_Enable, false, "AntiAim_Enable");
				var.EngineVars.RegisterVar(var.AntiAim_CAC_Safe, true, "AntiAim_CAC_Safe");
				var.EngineVars.RegisterVar(var.AntiAim_Pitch, 2, "AntiAim_Pitch");
				var.EngineVars.RegisterVar(var.AntiAim_Yaw, 2, "AntiAim_Yaw");
				var.EngineVars.RegisterVar(var.AntiAim_Fake_Yaw, 0, "AntiAim_Fake_Yaw");
				var.EngineVars.RegisterVar(var.AntiAim_Fake_Yaw_Add, 180.f, "AntiAim_Fake_Yaw_Add");
			}var.EngineVars.EndGroup();

			
			var.EngineVars.BeginGroup("Esp");
			{
				var.EngineVars.RegisterVar(var.Esp_Halo, false, "Esp_Halo");
				var.EngineVars.RegisterVar(var.Halo_Passes, 1, "Halo_Passes");
				var.EngineVars.RegisterVar(var.Halo_Bloom, 3.f, "Halo_Bloom");
				var.EngineVars.RegisterVar(var.Chams, false, "Chams");
				var.EngineVars.RegisterVar(var.Chams_Type, 0, "Chams_Type");
				var.EngineVars.RegisterVar(var.Chams_Blend, 1.f, "Chams_Blend");
				var.EngineVars.RegisterVar(var.Keypad_Logger, false, "Keypad_Logger");
				var.EngineVars.RegisterVar(var.Esp_Box2D, false, "Esp_Box2D");
				var.EngineVars.RegisterVar(var.Esp_Box3D, false, "Esp_Box3D");
				var.EngineVars.RegisterVar(var.Esp_Skeleton, false, "Esp_Skeleton");
				var.EngineVars.RegisterVar(var.Esp_Head_Circle, false, "Esp_Head_Circle");
				var.EngineVars.RegisterVar(var.Esp_HP, false, "Esp_HP");
				var.EngineVars.RegisterVar(var.Esp_Distance, false, "Esp_Distance");
				var.EngineVars.RegisterVar(var.Esp_Active_Weapon, false, "Esp_Active_Weapon");
				var.EngineVars.RegisterVar(var.Esp_Steam_Name, false, "Esp_Steam_Name");
				var.EngineVars.RegisterVar(var.Esp_RP_Name, false, "Esp_RP_Name");
				var.EngineVars.RegisterVar(var.Esp_Class, false, "Esp_Class");
				var.EngineVars.RegisterVar(var.Esp_Role, false, "Esp_Role");
				var.EngineVars.RegisterVar(var.Esp_Colors_By_Class, false, "Esp_Colors_By_Class");
				var.EngineVars.RegisterVar(var.Esp_Bot_Check, false, "Esp_Bot_Check");
				var.EngineVars.RegisterVar(var.Esp_Team_Check, false, "Esp_Team_Check");
				var.EngineVars.RegisterVar(var.Esp_Max_Distance, 5000.f, "Esp_Max_Distance");
				var.EngineVars.RegisterVar(var.Esp_Thickness, 2.f, "Esp_Thickness");
				var.EngineVars.RegisterVar(var.Esp_Thickness_Outline, 2.f, "Esp_Thickness_Outline");
				var.EngineVars.RegisterVar(var.Esp_Text_Size, 16.f, "Esp_Text_Size");
				var.EngineVars.RegisterVar(var.Esp_Tracer, false, "Esp_Tracer");
				var.EngineVars.RegisterVar(var.Only_Visible_Traces, false, "Only_Visible_Traces");
				var.EngineVars.RegisterVar(var.Tracer_Color, D3DCOLOR(ColorConvertFloat4ToU32(ImVec4(0.f, 1.f, 0.f, 1.00f))), "Tracer_Color");
				var.EngineVars.RegisterVar(var.Tracer_holdtime, 1.f, "Tracer_holdtime");
				var.EngineVars.RegisterVar(var.Esp_Halo_Color, D3DCOLOR(ColorConvertFloat4ToU32(ImVec4(0.f, 1.f, 0.437086f, 1.00f))), "Esp_Halo_Color");
				var.EngineVars.RegisterVar(var.Chams_Visible, D3DCOLOR(ColorConvertFloat4ToU32(ImVec4(0.f, 1.f, 0.646288f, 1.00f))), "Chams_Visible");
				var.EngineVars.RegisterVar(var.Chams_Invisible, D3DCOLOR(ColorConvertFloat4ToU32(ImVec4(0.536424f, 1.f, 0.f, 1.00f))), "Chams_Invisible");
				var.EngineVars.RegisterVar(var.Esp_Box2D_Color, D3DCOLOR(ColorConvertFloat4ToU32(ImVec4(0.19214f, 0.f, 1.f, 1.00f))), "Esp_Box2D_color");
				var.EngineVars.RegisterVar(var.Esp_Box3D_Color, D3DCOLOR(ColorConvertFloat4ToU32(ImVec4(0.f, 0.462882f, 1.f, 1.00f))), "Esp_Box3D_color");
				var.EngineVars.RegisterVar(var.Esp_Skeleton_Color, D3DCOLOR(ColorConvertFloat4ToU32(ImVec4(0.f, 1.f, 0.253275f, 1.00f))), "Esp_Skeleton_color");
				var.EngineVars.RegisterVar(var.Esp_Head_Color, D3DCOLOR(ColorConvertFloat4ToU32(ImVec4(1.f, 0.445415f, 0.f, 1.00f))), "Esp_Head_color");
				var.EngineVars.RegisterVar(var.Esp_Text_Color, D3DCOLOR(ColorConvertFloat4ToU32(ImVec4(0.f, 0.854312f, 1.f, 1.00f))), "Esp_Text_color");
				
				var.EngineVars.RegisterVar(var.Fake_angle_Chams, false, "Fake_angle_Chams");
				var.EngineVars.RegisterVar(var.Fake_angle_Chams_Type, 2, "Fake_angle_Chams_Type");
				var.EngineVars.RegisterVar(var.Fake_angle_Chams_Blend, 1.f, "Chams_Blend");
				var.EngineVars.RegisterVar(var.Fake_Angle_Color, D3DCOLOR(ColorConvertFloat4ToU32(ImVec4(0.f, 1.f, 0.646288f, 1.00f))), "Fake_Angle_Color");
				var.EngineVars.RegisterVar(var.Friend_Color, D3DCOLOR(ColorConvertFloat4ToU32(ImVec4(0.f, 1.f, 0.646288f, 1.00f))), "Friend_Color");

			}var.EngineVars.EndGroup();
			
			
			var.EngineVars.BeginGroup("Misc");
			{
				var.EngineVars.RegisterVar(var.Bhop, false, "Bhop");
				var.EngineVars.RegisterVar(var.Bhop_Safe_Mod, true, "Bhop_Safe_Mod");
				var.EngineVars.RegisterVar(var.Ground_Strafe, false, "Ground_Strafe");
				var.EngineVars.RegisterVar(var.Strafe_Helper, false, "Strafe_Helper");
				var.EngineVars.RegisterVar(var.Auto_Strafe, false, "Auto_Strafe");
				var.EngineVars.RegisterVar(var.Fake_Lag, 0, "Fake_Lag");
				//Engine::Var::Instance().Fake_Lag.Get() = 14;
				var.EngineVars.RegisterVar(var.Achievement_Spam, false, "Achievement_Spam");
				var.EngineVars.RegisterVar(var.NoName, false, "NoName");
				var.EngineVars.RegisterVar(var.Spectator_List, false, "Spectator_List");
				var.EngineVars.RegisterVar(var.No_Visible_Recoil, false, "No_Visible_Recoil");
				var.EngineVars.RegisterVar(var.Debug_Camera, false, "Debug_Camera");
				var.EngineVars.RegisterVar(var.Debug_Camera_Speed, 5.f, "Debug_Camera_Speed");
				var.EngineVars.RegisterVar(var.Debug_Camera_Key, 0x48, "Debug_Camera_Key");
				var.EngineVars.RegisterVar(var.Freeze_Players, false, "Freeze_Players");
				var.EngineVars.RegisterVar(var.Third_Person, false, "Third_Person");
				var.EngineVars.RegisterVar(var.Third_Person_Distance, 5.f, "Third_Person_Distance");
				var.EngineVars.RegisterVar(var.Third_Person_Key, 0x56, "Third_Person_Key");
				var.EngineVars.RegisterVar(var.Fov_changer, 0.f, "Fov_changer");
				var.EngineVars.RegisterVar(var.Viewmodel_changer, 0.f, "Viewmodel_changer");
				var.EngineVars.RegisterVar(var.Custom_Disconnect, false, "Custom_Disconnect");
				var.EngineVars.RegisterVar(var.Crosshair, false, "Crosshair");
				var.EngineVars.RegisterVar(var.Crosshair_Size, 2.f, "Crosshair_Size");
				var.EngineVars.RegisterVar(var.Crosshair_Thickness, 1.5f, "Crosshair_Thickness");
				var.EngineVars.RegisterVar(var.Crosshair_Color, D3DCOLOR(ColorConvertFloat4ToU32(ImVec4(0.53f, 0.53f, 0.87f, 0.80f))), "Crosshair_Color");
				var.EngineVars.RegisterVar(var.SV_Lag, false, "SV_Lag");
				var.EngineVars.RegisterVar(var.SV_Lag_Type, 0, "SV_Lag_Type");
				var.EngineVars.RegisterVar(var.SV_Lag_Intensity, 200.f, "SV_Lag_Intensity");
				var.EngineVars.RegisterVar(var.SV_Lag_Key, 0x6A, "SV_Lag_Key");
				var.EngineVars.RegisterVar(var.Slow_Motion, false, "Slow_Motion");
				var.EngineVars.RegisterVar(var.Slow_Motion_Scale, 0.5f, "Slow_Motion_Scale");
				var.EngineVars.RegisterVar(var.Slow_Motion_Key, 0x45, "Slow_Motion_Key");
				var.EngineVars.RegisterVar(var.Special_movefix, true, "Special_movefix");
				var.EngineVars.RegisterVar(var.CurrStateMovementRecorder, 0, "CurrStateMovementRecorder");
				var.EngineVars.RegisterVar(var.LoopMovementRecorder, false, "LoopMovementRecorder");
				var.EngineVars.RegisterVar(var.Fullbright, false, "Fullbright");
				//var.EngineVars.RegisterVar(var.AirStuck, false, "AirStuck");
				//var.EngineVars.RegisterVar(var.AirStuck_Key, (int)'H' , "AirStuck_Key");
			}var.EngineVars.EndGroup();

			var.EngineVars.BeginGroup("EntityList");
			{
				var.EngineVars.RegisterVar(var.PropList_Show_Props, false, "Show_Props");
				var.EngineVars.RegisterVar(var.PropList_Toggle, true, "Toggle");
				var.EngineVars.RegisterVar(var.PropList_Render_Name, false, "Render_Name");
				var.EngineVars.RegisterVar(var.PropList_Fast_Del_Key, 0x42, "Fast_Del_Key");
				var.EngineVars.RegisterVar(var.PropList_Fast_Add_Key, 0x56, "Fast_Add_Key");
				var.EngineVars.RegisterVar(var.PropList_HoldTime, 0.1f, "HoldTime");
				var.EngineVars.RegisterVar(var.PropList_Distance, 1337.f, "Distance");
			}var.EngineVars.EndGroup();
			var.EngineVars.BeginGroup("NetLog");
			{
				var.EngineVars.RegisterVar(var.NSend, false, "NSend");
				var.EngineVars.RegisterVar(var.NSend_Intensity, 10, "NSend_Intensity");
				var.EngineVars.RegisterVar(var.NSend_Key, VK_DIVIDE, "NSend_Key");

			}var.EngineVars.EndGroup();
			var.EngineVars.BeginGroup("GuiVar");
			{
			/*	var.EngineVars.RegisterVar(var.ImGuiCol_Text, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.90f, 0.90f, 0.90f, 1.00f))), "Text");
				var.EngineVars.RegisterVar(var.ImGuiCol_TextDisabled, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.60f, 0.60f, 0.60f, 1.00f))), "TextDisabled");
				var.EngineVars.RegisterVar(var.ImGuiCol_WindowBg, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.00f, 0.00f, 0.00f, 0.70f))), "WindowBg");
				var.EngineVars.RegisterVar(var.ImGuiCol_ChildWindowBg, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.00f, 0.00f, 0.00f, 0.00f))), "ChildWindowBg");
				var.EngineVars.RegisterVar(var.ImGuiCol_PopupBg, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.05f, 0.05f, 0.10f, 0.90f))), "PopupBg");
				var.EngineVars.RegisterVar(var.ImGuiCol_Border, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.70f, 0.70f, 0.70f, 0.65f))), "Border");
				var.EngineVars.RegisterVar(var.ImGuiCol_BorderShadow, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.00f, 0.00f, 0.00f, 0.00f))), "BorderShadow");
				var.EngineVars.RegisterVar(var.ImGuiCol_FrameBg, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.80f, 0.80f, 0.80f, 0.30f))), "FrameBg");
				var.EngineVars.RegisterVar(var.ImGuiCol_FrameBgHovered, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.90f, 0.80f, 0.80f, 0.40f))), "FrameBgHovered");
				var.EngineVars.RegisterVar(var.ImGuiCol_FrameBgActive, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.90f, 0.65f, 0.65f, 0.45f))), "FrameBgActive");
				var.EngineVars.RegisterVar(var.ImGuiCol_TitleBg, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.27f, 0.27f, 0.54f, 0.83f))), "TitleBg");
				var.EngineVars.RegisterVar(var.ImGuiCol_TitleBgCollapsed, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.40f, 0.40f, 0.80f, 0.20f))), "TitleBgCollapsed");
				var.EngineVars.RegisterVar(var.ImGuiCol_TitleBgActive, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.32f, 0.32f, 0.63f, 0.87f))), "TitleBgActive");
				var.EngineVars.RegisterVar(var.ImGuiCol_MenuBarBg, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.40f, 0.40f, 0.55f, 0.80f))), "MenuBarBg");
				var.EngineVars.RegisterVar(var.ImGuiCol_ScrollbarBg, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.20f, 0.25f, 0.30f, 0.60f))), "ScrollbarBg");
				var.EngineVars.RegisterVar(var.ImGuiCol_ScrollbarGrab, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.40f, 0.40f, 0.80f, 0.30f))), "ScrollbarGrab");
				var.EngineVars.RegisterVar(var.ImGuiCol_ScrollbarGrabHovered, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.40f, 0.40f, 0.80f, 0.40f))), "ScrollbarGrabHovered");
				var.EngineVars.RegisterVar(var.ImGuiCol_ScrollbarGrabActive, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.80f, 0.50f, 0.50f, 0.40f))), "ScrollbarGrabActive");
				var.EngineVars.RegisterVar(var.ImGuiCol_ComboBg, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.20f, 0.20f, 0.20f, 0.99f))), "ComboBg");
				var.EngineVars.RegisterVar(var.ImGuiCol_CheckMark, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.90f, 0.90f, 0.90f, 0.50f))), "CheckMark");
				var.EngineVars.RegisterVar(var.ImGuiCol_SliderGrab, D3DCOLOR(ImGui::GetColorU32(ImVec4(1.00f, 1.00f, 1.00f, 0.30f))), "SliderGrab");
				var.EngineVars.RegisterVar(var.ImGuiCol_SliderGrabActive, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.80f, 0.50f, 0.50f, 1.00f))), "SliderGrabActive");
				var.EngineVars.RegisterVar(var.ImGuiCol_Button, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.67f, 0.40f, 0.40f, 0.60f))), "Button");
				var.EngineVars.RegisterVar(var.ImGuiCol_ButtonHovered, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.67f, 0.40f, 0.40f, 1.00f))), "ButtonHovered");
				var.EngineVars.RegisterVar(var.ImGuiCol_ButtonActive, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.80f, 0.50f, 0.50f, 1.00f))), "ButtonActive");
				var.EngineVars.RegisterVar(var.ImGuiCol_Header, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.40f, 0.40f, 0.90f, 0.45f))), "Header");
				var.EngineVars.RegisterVar(var.ImGuiCol_HeaderHovered, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.45f, 0.45f, 0.90f, 0.80f))), "HeaderHovered");
				var.EngineVars.RegisterVar(var.ImGuiCol_HeaderActive, D3DCOLOR(ImGui::GetColorU32(ImVec4(0.53f, 0.53f, 0.87f, 0.80f))), "HeaderActive");*/
			}var.EngineVars.EndGroup();
			var.EngineVars.RegisterVar(var.Temp_str, std::string(), "Temp_str");
		}();
		
		if (SDK::Initialize()) {
			Render::Attach();
		}
		return true;
	}

	void Finalize() {
		//Render::CGui::Instance().Finalize();
		Engine::Var::Instance().EngineVars.DeleteAllVar();
	}


	void msg_box(const char * fmt, ...) {
		if (!fmt) return;
		if (strlen(fmt) < 1) return;

		va_list va_alist;
		char logBuf[256] = { 0 };

		va_start(va_alist, fmt);
		_vsnprintf(logBuf + strlen(logBuf), sizeof(logBuf) - strlen(logBuf), fmt, va_alist);
		va_end(va_alist);

		if (logBuf[0] != '\0') {
			MessageBox(0, logBuf, "DEBUG", MB_OK);
		}

	}

	static std::ofstream out;
	std::mutex syn;

	void WriteFile(const char * fmt, ...) {
		
		syn.lock();


		if (!out.is_open()) {

			remove("hello.txt");

			out = std::ofstream("hello.txt");
		}

		if (!fmt) return;
		if (strlen(fmt) < 1) return;

		va_list va_alist;
		char logBuf[256] = { 0 };

		va_start(va_alist, fmt);
		_vsnprintf(logBuf + strlen(logBuf), sizeof(logBuf) - strlen(logBuf), fmt, va_alist);
		va_end(va_alist);

		if (logBuf[0] != '\0') {


			out << logBuf << std::endl;
		}
		

		syn.unlock();
	}

}

