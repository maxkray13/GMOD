#pragma once
#include "../../../include/stdafx.h"
#include "../DrawImplDX.h"
extern TTFCore::Font g_font;
#define ARRAYSIZE(_ARR)          ((int)(sizeof(_ARR)/sizeof(*_ARR)))
#define TRESH_HOLD_DEALY 0.88f
#define TRESH_HOLD (g_TicksPerSecond * TRESH_HOLD_DEALY);
namespace Gui {
	struct _Keys
	{
		bool        bPressed;
		DWORD       dwStartTime;
	};
	struct combo_lr {
		combo_lr() = default;
		std::function<void(int* val, const char ** mnemonic, int max_results, int mnemonic_size, Vector2D pos, float t_x, float rect_size)> func;
		int* val; const char ** mnemonic; int max_results; int mnemonic_size; Vector2D pos; float t_x; float rect_size;
	};
	struct Tab {
	public:
		float x, y, w, h, t_y, t_x, t_w;
		UINT graph;
		std::vector<combo_lr> combo_cb;
		const char * name;
	};
	struct Mouse_Input {
		Vector2D mouse_pos;
		bool	 m_button_down[5];
		int		mouse_wheel;
	};
	extern float next_response_time;
	extern Mouse_Input mMouse_Input;
	extern Tab * current_tab;
	extern _Keys kPressingKeys[256];
	extern INT64 g_TicksPerSecond;
	extern float g_Time;
	extern float DeltaTime;
	void ProcessInput();

	void ProcessTime();
	void BeginScene(LPDIRECT3DDEVICE9 dev, Draw::Scene * cur_scene);
	void Process_Draw(Draw::Scene * cur_scene);

	bool BeginWindow(const char * name, const Draw::rect_t& first_pos,D3DCOLOR backside_col, D3DCOLOR cube_col);
	void EndWindow();

	bool BeginTab(const char * name,UINT graphs);
	void EndTab();

	bool IsInBox(float x, float y, float w, float h);
	void Text(const char * format_str,...);
	bool CheckBox(const char * name, bool & val);
	bool FloatSlider(const char * name, float & val,float min,float max);
	bool IntSlider(const char * name, int & val, int min, int max);

	template<typename T>
	bool Slider(const char * name, const char * format, T & val, T min, T max) {
		T prev_val = val;

		char Format_Buff[256];
		ZeroMemory(Format_Buff, sizeof(Format_Buff));

		sprintf_s(Format_Buff, format, name, val);

		auto slider_text_pos = Draw::TextEx(g_font, Format_Buff, current_tab->t_x, current_tab->t_y + FontSize, FontSize, 2.f, D3DCOLOR_ARGB(255, 255, 255, 255));
		Draw::Rect(current_tab->t_x, slider_text_pos.m_y + FontSize, FontSize + 2.f, current_tab->t_w, D3DCOLOR_ARGB(255, 255, 255, 255));
		float delta = max - min;
		Draw::Rect(current_tab->t_x, slider_text_pos.m_y + FontSize, FontSize + 2.f, ((val - min) / delta)*current_tab->t_w, D3DCOLOR_ARGB(255, 57, 177, 117));

		if (IsInBox(current_tab->t_x - 3.f, slider_text_pos.m_y + FontSize + 2.f, current_tab->t_w + 6.f, FontSize) && mMouse_Input.m_button_down[0] && g_Time > next_response_time) {
			val = ((mMouse_Input.mouse_pos.m_x - current_tab->t_x) / current_tab->t_w)*delta + min;
			if (val > max)
				val = max;
			if (val < min)
				val = min;
		}
		current_tab->t_y += (slider_text_pos.m_y + FontSize * 2.f) - current_tab->t_y + FontSize * 1.5f;

		if (current_tab->t_y + FontSize * 1.5f + 20.f > current_tab->y + current_tab->h) {
			Gui::NewGraph();

			return prev_val != val;
		}
		return prev_val != val;

	}

	bool InputBuffer(const char * name, wchar_t * buff, size_t size);
	template<typename T>
	void InputBufferDecremental(const char * name, T& val, T minimum, T maximum) {
		static T* ptr_val = nullptr;
		static bool is_nill = false;
		static bool is_neg = false;
		if (ptr_val != &val) {
			ptr_val = &val;
			is_nill = false;
			is_neg = false;
		}

		wchar_t buff[260];
		long long temp = (long long)val;
		if (is_nill) {
			ZeroMemory(buff, sizeof(buff));
		}
		else if (is_neg) {
			swprintf_s(buff, L"-");
		}
		else {
			swprintf_s(buff, L"%I64d", temp);
		}
		
		InputBuffer(name, buff, ARRAYSIZE(buff));
		auto len = wcslen(buff);
		if (!len ) {
			is_nill = true;
		}
		else if (buff[0] == L'-'&&buff[1] == NULL) {
			is_neg = true;
			is_nill = false;
		}
		else {
			is_nill = false;
			is_neg = false;
		}
		if (!swscanf(buff, L"%I64d", &temp)) {
			val = NULL;
		}
		else {
			val = std::clamp((T)temp, minimum, maximum);
		}
		
	}

	template<typename T>
	void InputBufferDot(const char * name, T* val, T minimum, T maximum) {
		static T* ptr_val = nullptr;
		static bool activ = false;

		static wchar_t buff[260];

		double temp = (double)*val;
		if (ptr_val != val) {
			ptr_val = val;
			activ = false;
			swprintf_s(buff, L"%f", temp);
		}
		
		
		if (InputBuffer(name, buff, ARRAYSIZE(buff))) {
		}
		*val = (T)wcstod(buff,NULL);
	}


	bool* Combo(const char* name, int & val, const char ** mnemonic, int max_results, int mnemonic_size);
	struct List_st {
		std::string name;
		bool* value;
	};
	void List(const char* name,std::vector<List_st>& list, int max_results);
	void ColorPickerButton(const char* name, D3DCOLOR& val);
	bool HotKey(const char* name, int& val);
	bool Button(const char * name);
	void NewGraph();
	bool State_Key(int Key, DWORD dwTimeOut);
}