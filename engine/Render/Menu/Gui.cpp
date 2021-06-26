#include "Gui.h"
#include "../Color.h"

namespace Gui {
	float g_Time;
	INT64 g_TicksPerSecond = 0.f;
	float DeltaTime;
	static bool g_hover = false;

	/*struct _Keys
	{
		bool        bPressed;
		DWORD       dwStartTime;
	}kPressingKeys[256];*/
	_Keys kPressingKeys[256];
	int last_key = 0;
	wchar_t input_buff[260];
	void Resetinput_buff() {
		ZeroMemory(input_buff, sizeof(input_buff));
	}

	Mouse_Input mMouse_Input;



	float next_response_time;
	Tab * current_tab = nullptr;

	class Window {
	public:
		Window() = default;
		Window(const char * name, float f_x, float f_y, float f_w, float f_h, D3DCOLOR c_b, D3DCOLOR c_c) : name(name), x(f_x), y(f_y), w(f_w), h(f_h),
			back_side_col(c_b), cube_col(c_c) {};
		uint16_t target_tab;
		const char * name;
		std::vector < Tab > tabs;
		float x, y, w, h;
		D3DCOLOR back_side_col;
		D3DCOLOR cube_col;
	};
	Window * target_wnd = nullptr;
	//Window window;

	std::vector<Window> windows;


}

bool Gui::IsInBox(float x, float y, float w, float h) {
	return(mMouse_Input.mouse_pos.m_x >= x && mMouse_Input.mouse_pos.m_x <= x + w && mMouse_Input.mouse_pos.m_y >= y && mMouse_Input.mouse_pos.m_y <= y + h);
}

bool Gui::State_Key(int Key, DWORD dwTimeOut) {
	if (HIWORD(GetAsyncKeyState(Key))) {
		if (!kPressingKeys[Key].bPressed || (kPressingKeys[Key].dwStartTime && (kPressingKeys[Key].dwStartTime + dwTimeOut) <= GetTickCount64())) {
			kPressingKeys[Key].bPressed = TRUE;
			if (dwTimeOut > NULL)
				kPressingKeys[Key].dwStartTime = GetTickCount64();
			return TRUE;
		}
	}
	else
		kPressingKeys[Key].bPressed = FALSE;
	return FALSE;
}

void Gui::ProcessInput() {

	BYTE key[256];
	ZeroMemory(key, sizeof(key));
	static BYTE old_key[256];

	GetKeyState(0);
	GetKeyboardState(key);
	for (int i = 0; i < 256; i++) {
		if (key[i] & 0x80) {
			last_key = i;
		}
		if (i < VK_BACK) {
			int Temp = i - 1;
			if (Temp > 2) {
				Temp -= 1;
			}
			if (key[i] & 0x80) {
				mMouse_Input.m_button_down[Temp] = true;
			}
			else {
				mMouse_Input.m_button_down[Temp] = false;
			}
		}
		else {
			if (key[i] & 0x80) {
				kPressingKeys[i].bPressed = true;
				if (!(old_key[i] & 0x80)) {

					UINT uScanCode = MapVirtualKey(i, MAPVK_VK_TO_VSC);
					WORD cresult = NULL;
					int lresult = ToAsciiEx(i, uScanCode, key, &cresult, 0, (HKL)(ULONG_PTR)0x04090409);

					auto off = wcslen(input_buff);
					if (off + 1 < sizeof(input_buff) / sizeof(wchar_t))
					{
						//wprintf_s(L"%wc \n", (wchar_t)cresult);//cresult
						//printf_s("cresult %p \n", cresult);
						//auto b = (char)cresult;
						input_buff[off] = (wchar_t)cresult;
						input_buff[off + 1] = NULL;
					}
				}
			}
			else {
				kPressingKeys[i].bPressed = false;
			}
		}
	}
	memcpy(old_key, key, sizeof(old_key));

	POINT Mouse;
	GetCursorPos(&Mouse);
	auto hwnd = Draw::GetCreationParams().hFocusWindow;
	if (hwnd != INVALID_HANDLE_VALUE) {
		ScreenToClient(hwnd, &Mouse);
	}
	mMouse_Input.mouse_pos.m_x = (float)Mouse.x;
	mMouse_Input.mouse_pos.m_y = (float)Mouse.y;
}

void Gui::ProcessTime()
{
	static INT64 current_time = 1.f;
	if (!g_TicksPerSecond) {
		QueryPerformanceFrequency((LARGE_INTEGER *)&g_TicksPerSecond);
	}
	QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
	DeltaTime = (float)(current_time - g_Time) / g_TicksPerSecond;
	g_Time = current_time;
}

void Gui::BeginScene(LPDIRECT3DDEVICE9 dev, Draw::Scene * cur_scene) {
	Draw::BeginDraw(dev, cur_scene,true);
	//ProcessInput();
}

void Gui::Process_Draw(Draw::Scene * cur_scene) {
	Draw::DrawScene(cur_scene);
}

bool Gui::BeginWindow(const char * name, const Draw::rect_t & first_pos, D3DCOLOR backside_col, D3DCOLOR cube_col)
{
	Window * target_window = nullptr;
	for (auto& it : windows) {
		if (std::hash<std::string>()(name) == std::hash<std::string>()(it.name)) {
			target_window = &it;
		}
	}
	if (!target_window) {
		Window wnd = {};
		wnd.tabs = {};
		wnd.name = name;
		wnd.target_tab = 0u;
		wnd.x = first_pos.x;
		wnd.y = first_pos.y;
		wnd.h = first_pos.h;
		wnd.w = first_pos.w;
		wnd.back_side_col = backside_col;
		wnd.cube_col = cube_col;
		windows.push_back(wnd);
		return false;
	}
	target_wnd = target_window;
	Draw::RoundedRect(target_wnd->x, target_wnd->y, target_wnd->h, target_wnd->w, target_wnd->back_side_col, 10.f);



	Draw::Rect(target_wnd->x + 32.f, target_wnd->y + 32.f, target_wnd->h - 64.f, target_wnd->w - 64.f, D3DCOLOR_ARGB(255, 26, 26, 26));

	float t_y = FontSize;
	float largest_x = 0.f;

	uint16_t i = 0u;
	for (auto it : target_wnd->tabs) {

		auto temp = Draw::TextEx(g_font, it.name, target_wnd->x + 42.f, target_wnd->y + 32.f + t_y + FontSize + (FontSize / 2.f), FontSize, 2.f,
			i == target_wnd->target_tab ? target_wnd->cube_col : D3DCOLOR_ARGB(255, 255, 255, 255));
		t_y += (FontSize + 15.f) * 1.5f;
		if (!largest_x)
			largest_x = temp.m_x;
		if (temp.m_x > largest_x)
			largest_x = temp.m_x;

		i++;
	}
	t_y = FontSize;
	i = 0u;
	for (auto it : target_wnd->tabs) {
		t_y += (FontSize + 15.f) * 1.5f;
		if (t_y != FontSize + (FontSize + 15.f) * 1.5f)
			Draw::LineEx({ target_wnd->x + 32.f, target_wnd->y + t_y - 15.f ,target_wnd->back_side_col }, { largest_x + 10.f,target_wnd->y + t_y - 15.f ,target_wnd->back_side_col }, 2.f);

		if (IsInBox(target_wnd->x + 32.f, target_wnd->y + 32.f + t_y - (FontSize + 15.f) * 1.5f - FontSize, largest_x - (target_wnd->x + 21.f), (FontSize + 15.f) * 1.5f) && mMouse_Input.m_button_down[0])
			target_wnd->target_tab = i;

		i++;
	}

	t_y += (FontSize + 15.f) * 1.5f;
	Draw::LineEx({ target_wnd->x + 32.f, target_wnd->y + t_y - 15.f ,target_wnd->back_side_col }, { largest_x + 10.f,target_wnd->y + t_y - 15.f ,target_wnd->back_side_col }, 2.f);
	Draw::LineEx({ largest_x + 10.f,target_wnd->y + 32.f ,target_wnd->back_side_col }, { largest_x + 10.f,target_wnd->y + target_wnd->h - 32.f ,target_wnd->back_side_col }, 2.f);

	if (target_wnd->tabs.size() && target_wnd->tabs.size() >= target_wnd->target_tab && target_wnd->tabs[target_wnd->target_tab].name) {
		target_wnd->tabs[target_wnd->target_tab].x = largest_x + 10.f;
		target_wnd->tabs[target_wnd->target_tab].y = target_wnd->y + 32.f;
		target_wnd->tabs[target_wnd->target_tab].w = (target_wnd->w + target_wnd->x) - largest_x - 32.f;
		target_wnd->tabs[target_wnd->target_tab].h = target_wnd->h - 64.f;
	}


	g_hover = false;

	return true;
}

void Gui::EndWindow()
{
	static float rotation = 0.f;
	rotation += 34.5f*DeltaTime;
	if (rotation > 360.f)
		rotation -= 360.f;

	Vector lt = Vector(mMouse_Input.mouse_pos.m_x - (target_wnd->x + 16), mMouse_Input.mouse_pos.m_y - (target_wnd->y + 16), 0.f);
	lt.Normalize();
	Vector rt = Vector(mMouse_Input.mouse_pos.m_x - (target_wnd->x + target_wnd->w - 16), mMouse_Input.mouse_pos.m_y - (target_wnd->y + 16), 0.f);
	rt.Normalize();
	Vector lb = Vector(mMouse_Input.mouse_pos.m_x - (target_wnd->x + 16), mMouse_Input.mouse_pos.m_y - (target_wnd->y + target_wnd->h - 16), 0.f);
	lb.Normalize();
	Vector rb = Vector(mMouse_Input.mouse_pos.m_x - (target_wnd->x + target_wnd->w - 16), mMouse_Input.mouse_pos.m_y - (target_wnd->y + target_wnd->h - 16), 0.f);
	rb.Normalize();

	Draw::Checkbox3DEmpty(lt, rotation, target_wnd->x + 16, target_wnd->y + 16, -0, 8.f, 1.f, 1.f, target_wnd->cube_col, D3DCOLOR_ARGB(255, 0, 255, 0), 2.f);
	Draw::Checkbox3DEmpty(rt, rotation + 90.f, target_wnd->x + target_wnd->w - 16, target_wnd->y + 16, -0, 8.f, 1.f, 1.f, target_wnd->cube_col, D3DCOLOR_ARGB(255, 0, 255, 0), 2.f);
	Draw::Checkbox3DEmpty(rb, rotation + 180.f, target_wnd->x + target_wnd->w - 16, target_wnd->y + target_wnd->h - 16, -0, 8.f, 1.f, 1.f, target_wnd->cube_col, D3DCOLOR_ARGB(255, 0, 255, 0), 2.f);
	Draw::Checkbox3DEmpty(lb, rotation + 270.f, target_wnd->x + 16, target_wnd->y + target_wnd->h - 16, -0, 8.f, 1.f, 1.f, target_wnd->cube_col, D3DCOLOR_ARGB(255, 0, 255, 0), 2.f);


	static bool is_hovered = false;
	static Vector2D last_pos;
	if (IsInBox(target_wnd->x, target_wnd->y, 32.f, target_wnd->h) || IsInBox(target_wnd->x + 32.f, target_wnd->y, target_wnd->w - 32.f, 32.f) || IsInBox(target_wnd->x + target_wnd->w - 32.f, target_wnd->y + 32.f, 32.f, target_wnd->h - 32.f) || IsInBox(target_wnd->x + 32.f, target_wnd->y + target_wnd->h - 32.f, target_wnd->w - 32.f, 32.f) || is_hovered) {
		if (mMouse_Input.m_button_down[0]) {
			is_hovered = true;
			if (!last_pos.Length())
				last_pos = mMouse_Input.mouse_pos;
		}
		else
			last_pos.Init(),
			is_hovered = false;
	}
	else
		last_pos.Init(),
		is_hovered = false;

	if (is_hovered && !g_hover) {

		if (last_pos != mMouse_Input.mouse_pos) {
			target_wnd->x += (mMouse_Input.mouse_pos.m_x - last_pos.m_x);
			target_wnd->y += (mMouse_Input.mouse_pos.m_y - last_pos.m_y);
		}
		last_pos = mMouse_Input.mouse_pos;
	}

	target_wnd = nullptr;
}

bool Gui::BeginTab(const char * name, UINT graphs) {
	for (auto &it : target_wnd->tabs) {
		if (it.name && strcmp(it.name, name) == NULL)
			current_tab = &it;
	}
	if (!current_tab) {
		Tab tb = {};
		tb.name = name;
		tb.x = tb.y = tb.w = tb.h = tb.t_x = tb.t_y = tb.t_w = 0.f;
		tb.graph = graphs;
		target_wnd->tabs.push_back(tb);
		return false;
	}
	if (std::hash<std::string>()(name) != std::hash<std::string>()(target_wnd->tabs[target_wnd->target_tab].name))
		return false;
	current_tab->graph = graphs;
	current_tab->t_x = current_tab->x + FontSize;
	current_tab->t_y = current_tab->y + FontSize + (FontSize / 2.f);
	current_tab->t_w = (current_tab->w / (float)current_tab->graph) - FontSize * 2.f;
	return true;
}

void Gui::EndTab() {
	if (current_tab->combo_cb.size()) {
		current_tab->combo_cb[0].func(current_tab->combo_cb[0].val, current_tab->combo_cb[0].mnemonic, current_tab->combo_cb[0].max_results, current_tab->combo_cb[0].mnemonic_size, current_tab->combo_cb[0].pos, current_tab->combo_cb[0].t_x, current_tab->combo_cb[0].rect_size);
	}

//	for (auto cb : current_tab->combo_cb) {
	//	cb.func(cb.val, cb.mnemonic, cb.max_results, cb.mnemonic_size, cb.pos, cb.t_x, cb.rect_size);
	//}
	current_tab->combo_cb.clear();
	current_tab->t_y = current_tab->t_x = 0.f;
	current_tab = nullptr;
}

void Gui::Text(const char * format_str, ...)
{
	va_list va;
	va_start(va, format_str);

	char temp_buff[0x200];
	ZeroMemory(temp_buff, sizeof(temp_buff));
	const int ret = _vsnprintf(temp_buff, sizeof(temp_buff), format_str, va);

	auto text_pos = Draw::TextEx(g_font, temp_buff, current_tab->t_x, current_tab->t_y + FontSize, FontSize, 2.f, D3DCOLOR_ARGB(255, 255, 255, 255));
	current_tab->t_y += text_pos.m_y - current_tab->t_y + FontSize * 1.5f;



	if (current_tab->t_y + FontSize * 1.5f + 20.f > current_tab->y + current_tab->h) {
		Gui::NewGraph();
	}


	va_end(va);
}

bool Gui::CheckBox(const char * name, bool & val) {
	auto prev_val = val;


	auto checkbox_text_pos = Draw::TextEx(g_font, name, current_tab->t_x, current_tab->t_y + FontSize, FontSize, 2.f, D3DCOLOR_ARGB(255, 255, 255, 255));
	Draw::Rect(current_tab->t_x + current_tab->t_w - 10.f, checkbox_text_pos.m_y - FontSize, FontSize, FontSize, val ? D3DCOLOR_ARGB(255, 57, 177, 117) : D3DCOLOR_ARGB(255, 255, 255, 255));

	static bool is_pressed = false;
	if (IsInBox(current_tab->t_x + current_tab->t_w - 10.f, checkbox_text_pos.m_y - FontSize, FontSize, FontSize)) {
		if (mMouse_Input.m_button_down[0] && g_Time > next_response_time) {
			is_pressed = true;
		}
		else if (!mMouse_Input.m_button_down[0] && is_pressed) {
			val ^= 1;
			is_pressed = false;
		}
	}
	current_tab->t_y += checkbox_text_pos.m_y - current_tab->t_y + FontSize * 1.5f;



	if (current_tab->t_y + FontSize * 1.5f + 20.f > current_tab->y + current_tab->h) {
		Gui::NewGraph();
		return prev_val != val;
	}
	return prev_val != val;
}

bool Gui::FloatSlider(const char * name, float & val, float min, float max) {
	auto prev_val = val;


	char Format_Buff[256];
	ZeroMemory(Format_Buff, sizeof(Format_Buff));

	sprintf_s(Format_Buff, "%s %.1f", name, val);

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

bool Gui::IntSlider(const char * name, int & val, int min, int max)
{
	auto prev_val = val;


	char Format_Buff[256];
	ZeroMemory(Format_Buff, sizeof(Format_Buff));

	sprintf_s(Format_Buff, "%s %d", name, val);

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

bool Gui::InputBuffer(const char * name, wchar_t * buff, size_t size) {

	char Format_Buff[260];


	auto inputbuff_text_pos = Draw::TextEx(g_font, name, current_tab->t_x, current_tab->t_y + FontSize, FontSize, 2.f, D3DCOLOR_ARGB(255, 255, 255, 255));
	Draw::Rect(current_tab->t_x, inputbuff_text_pos.m_y + FontSize, FontSize + 2.f, current_tab->t_w, D3DCOLOR_ARGB(255, 255, 255, 255));

	ZeroMemory(Format_Buff, sizeof(Format_Buff));

	static bool is_hovered = false;
	if (!is_hovered) {
		sprintf_s(Format_Buff, "%ws", buff);

		char * ptr = Format_Buff;
		auto t_pos = Draw::GetMaxText(g_font, ptr, current_tab->t_x + 2.f, inputbuff_text_pos.m_y + FontSize + FontSize, FontSize, 2.f);

		if (t_pos.m_x - current_tab->t_x > current_tab->t_w) {
			for (auto it = 0; it < strlen(Format_Buff); it++) {
				ptr = &Format_Buff[it];
				auto t_pos = Draw::GetMaxText(g_font, ptr, current_tab->t_x + 2.f, inputbuff_text_pos.m_y + FontSize + FontSize, FontSize, 2.f);
				if (t_pos.m_x - current_tab->t_x < current_tab->t_w) {
					break;
				}
			}
		}
		Draw::TextEx(g_font, ptr, current_tab->t_x + 2.f, inputbuff_text_pos.m_y + FontSize + FontSize, FontSize, 2.f, D3DCOLOR_ARGB(255, 0, 0, 0));

		if (IsInBox(current_tab->t_x, inputbuff_text_pos.m_y + FontSize, current_tab->t_w, FontSize) && mMouse_Input.m_button_down[0]) {
			is_hovered = true;
			Resetinput_buff();
		}
	}
	else {
		auto str_to_cpy = wcslen(input_buff);
		auto curr_off = wcslen(buff);
		if (curr_off + str_to_cpy + 1 > size) {
			str_to_cpy = size - curr_off - 1;
		}

		for (auto i = curr_off; i < curr_off + str_to_cpy; i++) {
			if (input_buff[i - curr_off] == 0x8) {
				i -= 2;
				--str_to_cpy;
				continue;
			}
			else if (input_buff[i - curr_off] > 0x100) {
				--str_to_cpy;
				continue;
			}
			else if (input_buff[i - curr_off] == 0xD) {
				is_hovered = false;
				goto done;
			}
			else if (input_buff[i - curr_off] == 0x1B) {
				is_hovered = false;
				goto done;
			}
			buff[i] = input_buff[i - curr_off];
		}
		if (!str_to_cpy && wcslen(input_buff)) {
			for (auto i = 0; i < wcslen(input_buff); i++) {
				if (input_buff[i] == 0x8 && curr_off) {
					buff[curr_off - 1] = '\0';
				}
			}
		}

		buff[curr_off + str_to_cpy] = '\0';
		sprintf_s(Format_Buff, "%ws", buff);

		char * ptr = Format_Buff;
		auto t_pos = Draw::GetMaxText(g_font, ptr, current_tab->t_x + 2.f, inputbuff_text_pos.m_y + FontSize + FontSize, FontSize, 2.f);

		if (t_pos.m_x - current_tab->t_x > current_tab->t_w) {
			for (auto it = 0; it < strlen(Format_Buff); it++) {
				ptr = &Format_Buff[it];
				auto t_pos = Draw::GetMaxText(g_font, ptr, current_tab->t_x + 2.f, inputbuff_text_pos.m_y + FontSize + FontSize, FontSize, 2.f);
				if (t_pos.m_x - current_tab->t_x < current_tab->t_w) {
					break;
				}
			}
		}
		t_pos = Draw::TextEx(g_font, ptr, current_tab->t_x + 2.f, inputbuff_text_pos.m_y + FontSize + FontSize, FontSize, 2.f, D3DCOLOR_ARGB(255, 0, 0, 0));
		Draw::LineEx(Vertex(t_pos.m_x + 2.f, inputbuff_text_pos.m_y + FontSize, D3DCOLOR_ARGB(255, 57, 177, 117)), Vertex(t_pos.m_x + 2.f, inputbuff_text_pos.m_y + FontSize + FontSize + 2.f, D3DCOLOR_ARGB(255, 57, 177, 117)), 2.f);
		Resetinput_buff();
		if (!IsInBox(current_tab->t_x, inputbuff_text_pos.m_y + FontSize, current_tab->t_w, FontSize) && mMouse_Input.m_button_down[0]) {
			is_hovered = false;
		}
	}
	if (current_tab->t_y + FontSize * 1.5f + 20.f > current_tab->y + current_tab->h) {
		Gui::NewGraph();
		return is_hovered;
	}

done:;
	current_tab->t_y += (inputbuff_text_pos.m_y + FontSize - 5.f + FontSize + 5.f) - current_tab->t_y + FontSize * 1.5f;

	return is_hovered;
}

bool* Gui::Combo(const char * name, int& val, const char ** mnemonic, int max_results, int mnemonic_size)
{
	if (val >= mnemonic_size)val = mnemonic_size - 1;

	auto combo_text_pos = Draw::TextEx(g_font, name, current_tab->t_x, current_tab->t_y + FontSize, FontSize, 2.f, D3DCOLOR_ARGB(255, 255, 255, 255));
	static bool is_hovered = false;
	static int * prt_val = nullptr;
	static bool is_clicked = false;
	static bool is_pressed = false;
	is_clicked = false;
	if ((!is_hovered || prt_val != &val)) {
		Draw::Rect(current_tab->t_x, combo_text_pos.m_y + FontSize, FontSize + 3.f, current_tab->t_w, D3DCOLOR_ARGB(255, 57, 177, 117));
		if (mnemonic[val]) {
			char * ptr = (char*)mnemonic[val];
			auto t_pos = Draw::GetMaxText(g_font, ptr, current_tab->t_x + 2.f, combo_text_pos.m_y + FontSize + FontSize, FontSize, 2.f);
			auto it = 0;

			if (t_pos.m_x - current_tab->t_x > current_tab->t_w - (FontSize + 2.f)) {
				for (; it < strlen(mnemonic[val]); it++) {
					ptr = (char*)&mnemonic[val][it];
					auto t_pos = Draw::GetMaxText(g_font, ptr, current_tab->t_x + 2.f, combo_text_pos.m_y + FontSize + FontSize, FontSize, 2.f);
					if (t_pos.m_x - current_tab->t_x < current_tab->t_w - (FontSize + 2.f)) {
						break;
					}
				}
			}
			std::string curr = std::string(mnemonic[val]);
			if (it) {
				curr = curr.substr(0u, curr.length() - it);
			}
			Draw::TextEx(g_font, curr.c_str(), current_tab->t_x + 2.f, combo_text_pos.m_y + FontSize + FontSize, FontSize, 2.f, D3DCOLOR_ARGB(255, 0, 0, 0));
		}
		Draw::Rect(current_tab->t_x + current_tab->t_w - (FontSize + 3.f) , combo_text_pos.m_y + FontSize, FontSize + 3.f, FontSize + 3.f, D3DCOLOR_XRGB(255, 255, 255));

		Draw::TriangleEx(
			Vertex(current_tab->t_x + current_tab->t_w - (FontSize + 3.f) + 2.f, combo_text_pos.m_y + FontSize + 2.f, target_wnd->cube_col),
			Vertex(current_tab->t_x + current_tab->t_w - 2.5f , combo_text_pos.m_y + FontSize + 2.f, target_wnd->cube_col),
			Vertex(current_tab->t_x + current_tab->t_w - ((FontSize + 3.f) / 2.f) -0.5f, combo_text_pos.m_y + FontSize + FontSize + 1.f, target_wnd->cube_col)
		);//((FontSize + 4.f ) / 2.f) - 0.5f
		
		if (IsInBox(current_tab->t_x, combo_text_pos.m_y + FontSize, current_tab->t_w, FontSize + 2.f)) {
			if (mMouse_Input.m_button_down[0] && mnemonic_size > 1 && g_Time > next_response_time) {
				is_pressed = true;
			}
			else if (!mMouse_Input.m_button_down[0] && is_pressed &&!is_hovered) {
				is_hovered = true;
				prt_val = &val;
				is_pressed = false;
			}
		}
	}
	else {

		std::function tt = [](int* val, const char ** mnemonic, int max_results, int mnemonic_size, Vector2D pos, float t_x, float rect_size) {
		//	next_response_time = g_Time + TRESH_HOLD;
			static float curr_offset = 0.f;
			if (curr_offset < 0.f)curr_offset = 0.f;

			auto target_size = curr_offset + max_results;
			if (target_size > mnemonic_size) {
				target_size = max_results;
				curr_offset = mnemonic_size - max_results;
			}
			const auto r_off = 3.f;
			auto y_max = pos.m_y + FontSize + (FontSize + 3.f + r_off)*max_results - (pos.m_y + FontSize) - r_off;
			Draw::Rect(t_x, pos.m_y + FontSize, y_max, rect_size, D3DCOLOR_ARGB(255, 0, 0, 0));
			auto ttt = 0u;
			for (auto i = (int)lround(curr_offset); i < target_size; i++) {
				if (ttt >= max_results)
					break;
				auto curr_y = pos.m_y + FontSize + (FontSize + 3.f + r_off)*(i - lround(curr_offset));
				Draw::Rect(t_x, curr_y, FontSize + 3.f, rect_size - FontSize - 2.f, *val == (i) ? D3DCOLOR_ARGB(255, 57, 177, 117) : D3DCOLOR_ARGB(255, 255, 255, 255));
				if (IsInBox(t_x, curr_y, rect_size - FontSize - 2.f, FontSize + 3.f) && mMouse_Input.m_button_down[0] && g_Time > next_response_time) {
					*val = i;
					is_clicked = true;
					next_response_time = g_Time + TRESH_HOLD;
				}
				if (mnemonic[i]) {
					char * ptr = (char*)mnemonic[i];
					auto t_pos = Draw::GetMaxText(g_font, ptr, t_x + 2.f, curr_y + FontSize, FontSize, 2.f);
					auto it = 0;

					if (t_pos.m_x - (t_x + 2.f) > current_tab->t_w - (FontSize + 3.f)) {
						for (; it < strlen(mnemonic[i]); it++) {
							ptr = (char*)&mnemonic[i][it];
							auto t_pos = Draw::GetMaxText(g_font, ptr, current_tab->t_x + 2.f, curr_y + FontSize, FontSize, 2.f);
							if (t_pos.m_x - current_tab->t_x < current_tab->t_w - (FontSize + 3.f)) {
								break;
							}
						}
					}
					std::string curr = std::string(mnemonic[i]);
					if (it) {
						curr = curr.substr(0u, curr.length() - it);
					}
					Draw::TextEx(g_font, curr.c_str(), t_x + 2.f, curr_y + FontSize, FontSize, 2.f, D3DCOLOR_ARGB(255, 0, 0, 0));
				}

				ttt++;
			}

			Draw::Rect(t_x + rect_size - (FontSize + 3.f), pos.m_y + FontSize, y_max, FontSize + 3.f, D3DCOLOR_ARGB(255, 255, 255, 255));
			Draw::TriangleEx(
				Vertex(t_x + rect_size - (FontSize + 3.f) + 1.f, pos.m_y + FontSize + FontSize + 1.f, target_wnd->cube_col),
				Vertex(t_x + rect_size - ((FontSize + 3.f) / 2.f) - 0.5f, pos.m_y + FontSize + 1.f, target_wnd->cube_col),
				Vertex(t_x + rect_size - 2.f, pos.m_y + FontSize + FontSize + 1.f, target_wnd->cube_col)
			);
			if (IsInBox(t_x + rect_size - (FontSize + 2.f), pos.m_y + FontSize, FontSize + 2.f, FontSize + 2.f) && mMouse_Input.m_button_down[0]) {
				if (curr_offset > 0.f)curr_offset -= std::clamp(10.f * DeltaTime, 0.f, 1.f);
			}

			Draw::TriangleEx(
				Vertex(t_x + rect_size - (FontSize + 3.f) + 2.f, pos.m_y + y_max - 1.f, target_wnd->cube_col),
				Vertex(t_x + rect_size - 2.5f, pos.m_y + y_max - 1.f, target_wnd->cube_col),
				Vertex(t_x + rect_size - ((FontSize + 3.f) / 2.f) - 0.5f, pos.m_y + FontSize + y_max - 2.f, target_wnd->cube_col)
			);

			if (IsInBox(t_x + rect_size - (FontSize + 2.f), pos.m_y + y_max, FontSize + 2.f, FontSize + 2.f) && mMouse_Input.m_button_down[0]) {
				curr_offset += std::clamp(10.f * DeltaTime, 0.f, 1.f);
				target_size = curr_offset + max_results;
				if (target_size > mnemonic_size) {
					target_size = max_results;
					curr_offset = mnemonic_size - max_results;
				}
			}

			if (!IsInBox(t_x, pos.m_y, rect_size, y_max + FontSize + 2.f) && mMouse_Input.m_button_down[0]) {
				next_response_time = g_Time + TRESH_HOLD;
				is_hovered = false;
				is_pressed = false;
				prt_val = nullptr;
				curr_offset = 0;
			}

		};
		combo_lr lt = {};
		lt.func = tt;
		lt.val = &val;
		lt.mnemonic = mnemonic;
		lt.max_results = max_results;
		lt.mnemonic_size = mnemonic_size;
		lt.pos = combo_text_pos;
		lt.rect_size = current_tab->t_w;
		lt.t_x = current_tab->t_x;
		if (!current_tab->combo_cb.size()) {
			current_tab->combo_cb.push_back(lt);
		}
	}


	g_hover |= is_hovered;

	if (current_tab->t_y + FontSize * 1.5f + 20.f > current_tab->y + current_tab->h) {
		Gui::NewGraph();
		return &is_clicked;
	}
	current_tab->t_y += (combo_text_pos.m_y + FontSize + FontSize) - current_tab->t_y + FontSize * 1.5f;

	return &is_clicked;
}
void Gui::List(const char * name, std::vector<List_st>& list, int max_results)
{
	auto list_text_pos = Draw::TextEx(g_font, name, current_tab->t_x, current_tab->t_y + FontSize, FontSize, 2.f, D3DCOLOR_ARGB(255, 255, 255, 255));

	static float offset = 0.f;
	if (offset < 0.f)offset = 0.f;

	auto target_size = offset + max_results;
	if (target_size > list.size()) {
		target_size = max_results;
		offset = list.size() - max_results;
	}

	auto save_y_text = list_text_pos.m_y;
	int iter = 0;
	for (auto it = list.begin() + (int)lround(offset); it != list.end(); it++) {
		current_tab->t_w;
		auto curr_y = list_text_pos.m_y + FontSize;
		Draw::Rect(current_tab->t_x, curr_y, FontSize + 3.f, current_tab->t_w - FontSize - 2.f, *(*it).value ? D3DCOLOR_ARGB(255, 57, 177, 117) : D3DCOLOR_ARGB(255, 255, 255, 255));
		list_text_pos = Draw::TextEx(g_font, (*it).name.c_str(), current_tab->t_x, list_text_pos.m_y + FontSize + FontSize, FontSize, 2.f, D3DCOLOR_ARGB(255, 0, 0, 0));
		list_text_pos.m_y -= 4.f;
		static bool is_pressed = false;
		if (IsInBox(current_tab->t_x, curr_y, current_tab->t_w - FontSize - 2.f, FontSize + 1.f)) {
			if (mMouse_Input.m_button_down[0]) {
				is_pressed = true;
			}
			else if (!mMouse_Input.m_button_down[0] && is_pressed) {
				*(*it).value ^= 1;
				is_pressed = false;
			}
		}
		iter++;
		if (iter >= max_results)
			break;
	}

	auto y_max = list_text_pos.m_y - save_y_text - FontSize + 7.f;
	Draw::Rect(current_tab->t_x + current_tab->t_w - FontSize - 3.f, save_y_text + FontSize, y_max, FontSize + 3.f, D3DCOLOR_ARGB(255, 255, 255, 255));
	Draw::TriangleEx(
		Vertex(current_tab->t_x + current_tab->t_w - (FontSize + 3.f) + 1.5f, save_y_text + FontSize + FontSize + 1.f, target_wnd->cube_col),
		Vertex(current_tab->t_x + current_tab->t_w - ((FontSize + 3.f) / 2.f) - 0.5f, save_y_text + FontSize + 1.f, target_wnd->cube_col),
		Vertex(current_tab->t_x + current_tab->t_w - 2.f, save_y_text + FontSize + FontSize + 1.f, target_wnd->cube_col)
	);
	if (IsInBox(current_tab->t_x + current_tab->t_w - (FontSize + 2.f), save_y_text + FontSize, FontSize + 2.f, FontSize + 2.f) && mMouse_Input.m_button_down[0]) {
		if (offset > 0.f)offset -= std::clamp(10.f * DeltaTime, 0.f, 1.f);
	}

	Draw::TriangleEx(
		Vertex(current_tab->t_x + current_tab->t_w - (FontSize + 2.f) + 1.f, save_y_text + y_max - 1.f, target_wnd->cube_col),
		Vertex(current_tab->t_x + current_tab->t_w - 2.5f, save_y_text + y_max - 1.f, target_wnd->cube_col),
		Vertex(current_tab->t_x + current_tab->t_w - ((FontSize + 3.f) / 2.f) - 0.5f, save_y_text + FontSize + y_max - 2.f, target_wnd->cube_col)
	);
	if (IsInBox(current_tab->t_x + current_tab->t_w - (FontSize + 2.f), save_y_text + y_max, FontSize + 2.f, FontSize + 2.f) && mMouse_Input.m_button_down[0]) {
		offset += std::clamp(10.f * DeltaTime, 0.f, 1.f);
		target_size = offset + max_results;
		if (target_size >= list.size()) {
			target_size = max_results;
			offset = list.size() - max_results;
		}
	}

	if ((list_text_pos.m_y) + FontSize * 1.5f + 20.f > current_tab->y + current_tab->h) {
		Gui::NewGraph();
		return;
	}
	current_tab->t_y += (list_text_pos.m_y) - current_tab->t_y + FontSize * 1.5f;
}
void ColorConvertHSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b)
{
	if (s == 0.0f) {
		out_r = out_g = out_b = v;
		return;
	}

	h = fmodf(h, 1.0f) / (60.0f / 360.0f);
	int   i = (int)h;
	float f = h - (float)i;
	float p = v * (1.0f - s);
	float q = v * (1.0f - s * f);
	float t = v * (1.0f - s * (1.0f - f));

	switch (i) {
	case 0: out_r = v; out_g = t; out_b = p; break;
	case 1: out_r = q; out_g = v; out_b = p; break;
	case 2: out_r = p; out_g = v; out_b = t; break;
	case 3: out_r = p; out_g = q; out_b = v; break;
	case 4: out_r = t; out_g = p; out_b = v; break;
	case 5: default: out_r = v; out_g = p; out_b = q; break;
	}
}

void Gui::ColorPickerButton(const char * name, D3DCOLOR & val)
{
	static D3DCOLOR * prt_val = nullptr;
	auto colorpicker_text_pos = Draw::TextEx(g_font, name, current_tab->t_x, current_tab->t_y + FontSize, FontSize, 2.f, D3DCOLOR_ARGB(255, 255, 255, 255));

	auto meme = cColor(val);
	cColor curr = {};
	curr.SetColor(meme.b(), meme.g(), meme.r(), 255);
	Draw::Rect(current_tab->t_x + current_tab->t_w - FontSize, colorpicker_text_pos.m_y - FontSize, FontSize, FontSize, curr.GetRawColor());

	static bool is_hovered = false;
	static bool is_pressed = false;
	if (!is_hovered || prt_val != &val) {
		if (IsInBox(current_tab->t_x + current_tab->t_w - FontSize, colorpicker_text_pos.m_y - FontSize, FontSize, FontSize)) {
			if (mMouse_Input.m_button_down[0] && g_Time > next_response_time) {
				is_pressed = true;
			}
			else if (!mMouse_Input.m_button_down[0] && is_pressed && !g_hover) {
				is_hovered = true;
				is_pressed = false;
				prt_val = &val;
			}
		}
	}
	else {
		std::function tt = [](int* val, const char ** mnemonic, int max_results, int mnemonic_size, Vector2D pos, float t_x, float rect_size) {
			next_response_time = g_Time + TRESH_HOLD;
			Draw::RectEx(Vertex(t_x + rect_size - 10.f + FontSize, pos.m_y + 100.f, D3DCOLOR_ARGB(255, 0, 0, 0)),
				Vertex(t_x + rect_size - 10.f + FontSize + 100.f, pos.m_y + 100.f, D3DCOLOR_ARGB(255, 255, 255, 255)),
				Vertex(t_x + rect_size - 10.f + FontSize, pos.m_y + 120.f, D3DCOLOR_ARGB(255, 0, 0, 0)),
				Vertex(t_x + rect_size - 10.f + FontSize + 100.f, pos.m_y + 120.f, D3DCOLOR_ARGB(255, 255, 255, 255))
			);

			static D3DCOLOR cpicker_col = D3DCOLOR_ARGB(255, 255, 255, 255);
			static float bright = .5f, off;
			for (int i = 0; i < 50; i++)
			{
				float off = (1.f / 50.f) * i;
				float r, g, b;
				ColorConvertHSVtoRGB(off, 1.f, 1.f, r, g, b);
				Draw::Rect(t_x + rect_size - 10.f + FontSize + 100, pos.m_y + i * 2, 2.f, 20.f, D3DCOLOR_ARGB(255, int(r * 255), int(g * 255), int(b * 255)));
			}
			if (IsInBox(t_x + rect_size - 10.f + FontSize + 100, pos.m_y, 20.f, 100.f) && mMouse_Input.m_button_down[0]) {
				auto delta_h = mMouse_Input.mouse_pos.m_y - pos.m_y;
				off = delta_h * (1.f / 100.f);
				float r, g, b;
				float s, v;
				if (bright < .5f) {
					s = 1.f;
					v = bright * 2.f;
				}
				else {
					s = 1.f - ((bright - .5f) * 2.f);
					v = 1.f;
				}
				ColorConvertHSVtoRGB(off, s, v, r, g, b);
				cpicker_col = D3DCOLOR_ARGB(255, int(b * 255), int(g * 255), int(r * 255));
				*(D3DCOLOR*)val = cpicker_col;
			}
			if (IsInBox(t_x + rect_size - 10.f + FontSize, pos.m_y + 100.f, 100.f, 20.f) && mMouse_Input.m_button_down[0]) {
				auto delta_w = mMouse_Input.mouse_pos.m_x - (t_x + rect_size - 10.f + FontSize);
				bright = delta_w * (1.f / 100.f);

				float s, v;
				if (bright < .5f) {
					s = 1.f;
					v = bright * 2.f;
				}
				else {
					s = 1.f - ((bright - .5f) * 2.f);
					v = 1.f;
				}
				float r, g, b;
				ColorConvertHSVtoRGB(off, s, v, r, g, b);
				cpicker_col = D3DCOLOR_ARGB(255, int(r * 255), int(g * 255), int(b * 255));
				*(D3DCOLOR*)val = cpicker_col;
			}
			auto meme = cColor(cpicker_col);
			cColor curr = {};
			curr.SetColor(meme.b(), meme.g(), meme.r(), 255);
			Draw::Rect(t_x + rect_size - 10.f + FontSize, pos.m_y, 100.f, 100.f, curr.GetRawColor());
			if (!IsInBox(t_x + rect_size - 10.f + FontSize, pos.m_y, 120, 120) && mMouse_Input.m_button_down[0]) {
				next_response_time = g_Time + TRESH_HOLD;
				is_hovered = false;
				prt_val = nullptr;
			}
		};
		combo_lr lt = {};
		lt.func = tt;
		lt.val = (int*)&val;
		lt.mnemonic = NULL;
		lt.max_results = NULL;
		lt.mnemonic_size = NULL;
		lt.pos = colorpicker_text_pos;
		lt.rect_size = current_tab->t_w;
		lt.t_x = current_tab->t_x;
		current_tab->combo_cb.push_back(lt);
	}

	if (current_tab->t_y + FontSize * 1.5f + 20.f > current_tab->y + current_tab->h) {
		Gui::NewGraph();
		return;
	}

	g_hover |= is_hovered;
	current_tab->t_y += colorpicker_text_pos.m_y - current_tab->t_y + FontSize * 1.5f;
}



const char* const KeyNames[] = {
	"Unknown",
	"VK_LBUTTON",
	"VK_RBUTTON",
	"VK_CANCEL",
	"VK_MBUTTON",
	"VK_XBUTTON1",
	"VK_XBUTTON2",
	"Unknown",
	"VK_BACK",
	"VK_TAB",
	"Unknown",
	"Unknown",
	"VK_CLEAR",
	"VK_RETURN",
	"Unknown",
	"Unknown",
	"VK_SHIFT",
	"VK_CONTROL",
	"VK_MENU",
	"VK_PAUSE",
	"VK_CAPITAL",
	"VK_KANA",
	"Unknown",
	"VK_JUNJA",
	"VK_FINAL",
	"VK_KANJI",
	"Unknown",
	"VK_ESCAPE",
	"VK_CONVERT",
	"VK_NONCONVERT",
	"VK_ACCEPT",
	"VK_MODECHANGE",
	"VK_SPACE",
	"VK_PRIOR",
	"VK_NEXT",
	"VK_END",
	"VK_HOME",
	"VK_LEFT",
	"VK_UP",
	"VK_RIGHT",
	"VK_DOWN",
	"VK_SELECT",
	"VK_PRINT",
	"VK_EXECUTE",
	"VK_SNAPSHOT",
	"VK_INSERT",
	"VK_DELETE",
	"VK_HELP",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"VK_LWIN",
	"VK_RWIN",
	"VK_APPS",
	"Unknown",
	"VK_SLEEP",
	"VK_NUMPAD0",
	"VK_NUMPAD1",
	"VK_NUMPAD2",
	"VK_NUMPAD3",
	"VK_NUMPAD4",
	"VK_NUMPAD5",
	"VK_NUMPAD6",
	"VK_NUMPAD7",
	"VK_NUMPAD8",
	"VK_NUMPAD9",
	"VK_MULTIPLY",
	"VK_ADD",
	"VK_SEPARATOR",
	"VK_SUBTRACT",
	"VK_DECIMAL",
	"VK_DIVIDE",
	"VK_F1",
	"VK_F2",
	"VK_F3",
	"VK_F4",
	"VK_F5",
	"VK_F6",
	"VK_F7",
	"VK_F8",
	"VK_F9",
	"VK_F10",
	"VK_F11",
	"VK_F12",
	"VK_F13",
	"VK_F14",
	"VK_F15",
	"VK_F16",
	"VK_F17",
	"VK_F18",
	"VK_F19",
	"VK_F20",
	"VK_F21",
	"VK_F22",
	"VK_F23",
	"VK_F24",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_NUMLOCK",
	"VK_SCROLL",
	"VK_OEM_NEC_EQUAL",
	"VK_OEM_FJ_MASSHOU",
	"VK_OEM_FJ_TOUROKU",
	"VK_OEM_FJ_LOYA",
	"VK_OEM_FJ_ROYA",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"VK_LSHIFT",
	"VK_RSHIFT",
	"VK_LCONTROL",
	"VK_RCONTROL",
	"VK_LMENU",
	"VK_RMENU"
};

bool Gui::HotKey(const char * name, int & val)
{

	auto old_val = val;
	auto hotkey_text_pos = Draw::TextEx(g_font, name, current_tab->t_x, current_tab->t_y + FontSize, FontSize, 2.f, D3DCOLOR_ARGB(255, 255, 255, 255));
	Draw::Rect(current_tab->t_x, hotkey_text_pos.m_y + FontSize, FontSize + 2.f, current_tab->t_w, D3DCOLOR_ARGB(255, 255, 255, 255));


	static int * prt_val = nullptr;

	static bool is_hovered = false;
	static bool is_pressed = false;
	static int countt = 0;
	if (!is_hovered || prt_val != &val) {
		Draw::TextEx(g_font, KeyNames[val], current_tab->t_x, hotkey_text_pos.m_y + FontSize + FontSize, FontSize, 2.f, D3DCOLOR_ARGB(255, 0, 0, 0));
		if (IsInBox(current_tab->t_x, hotkey_text_pos.m_y + FontSize, current_tab->t_w, FontSize + 2.f)) {
			if (mMouse_Input.m_button_down[0]) {
				is_pressed = true;
			}
			else if (!mMouse_Input.m_button_down[0] && is_pressed) {
				is_hovered = true;
				is_pressed = false;
				prt_val = &val;
				countt = 0;
				last_key = -1;
			}
		}
	}
	else {
		Draw::TextEx(g_font, "Press a key", current_tab->t_x, hotkey_text_pos.m_y + FontSize + FontSize, FontSize, 2.f, D3DCOLOR_ARGB(255, 0, 0, 0));
		if (countt > 100) {
			int type = last_key;

			if (type != -1 && type < sizeof(KeyNames) / sizeof(char*)) {
				val = type;
				is_hovered = false;
				prt_val = nullptr;
			}
		}
		else {
			countt++;
		}
	}
	if (current_tab->t_y + FontSize * 1.5f + 20.f > current_tab->y + current_tab->h) {
		Gui::NewGraph();
		return old_val != val;
	}
	current_tab->t_y += (hotkey_text_pos.m_y + FontSize - 5.f + FontSize + 5.f) - current_tab->t_y + FontSize * 1.5f;
	return old_val != val;
}

bool Gui::Button(const char * name)
{
	static const char* ptr_name = nullptr;
	auto ret = false;
	static bool is_pressed = false;

	auto t_pos = Draw::GetMaxText(g_font, name, current_tab->t_x + 2.f, current_tab->t_y + FontSize, FontSize, 2.f);
	Draw::Rect(current_tab->t_x + 2.f, current_tab->t_y, FontSize + 3.f, t_pos.m_x - (current_tab->t_x + 2.f), is_pressed && ptr_name == name ? target_wnd->cube_col : D3DCOLOR_ARGB(255, 255, 255, 255));
	Draw::TextEx(g_font, name, current_tab->t_x + 2.f, current_tab->t_y + FontSize, FontSize, 2.f, D3DCOLOR_ARGB(255, 0, 0, 0));



	if (IsInBox(current_tab->t_x + 2.f, current_tab->t_y, t_pos.m_x - (current_tab->t_x + 2.f), FontSize + 2.f)) {
		if (mMouse_Input.m_button_down[0] && g_Time > next_response_time) {
			is_pressed = true;
			ptr_name = name;
		}
		else if (!mMouse_Input.m_button_down[0] && is_pressed &&ptr_name == name) {
			//next_response_time = g_Time + TRESH_HOLD;
			ret ^= 1;
			is_pressed = false;
			ptr_name = nullptr;
		}
	}

	if (current_tab->t_y + FontSize * 1.5f + 20.f > current_tab->y + current_tab->h) {
		Gui::NewGraph();
		return ret;
	}
	current_tab->t_y += t_pos.m_y - current_tab->t_y + FontSize * 1.5f;
	return ret;
}

void Gui::NewGraph()
{
	current_tab->t_y = current_tab->y + FontSize + (FontSize / 2.f);
	current_tab->t_x += (current_tab->w / (float)current_tab->graph) - (FontSize / 2.f);

}
