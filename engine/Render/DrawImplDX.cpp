#include "DrawImplDX.h"
namespace Draw {
	TTF::Triangulator2DII triangulator2DI;
	TTF::Triangulator2DII FontLib[0xFFFF] = {};

	D3DDEVICE_CREATION_PARAMETERS creation_params;
	D3DVIEWPORT9 ViewPort;
	LPDIRECT3DDEVICE9        Device = NULL;

	Scene * current_scene;
	/*
	unsigned int current_scene->g_pos_vertex;
	IDirect3DVertexBuffer9* g_vb = NULL;
	constexpr UINT VertexBuffCount = 0x3FFFFF;
	Vertex * current_scene->g_Vertex_buff = nullptr;*/
}

D3DVECTOR Draw::ScaleVertex(const Vertex& Vertices, float scale) {
	return { Vertices.x*scale ,Vertices.y*scale ,Vertices.z*scale };
}
D3DVECTOR Draw::ScaleVertex(const Vertex & Vertices, float x_scale, float y_scale, float z_scale)
{
	return { Vertices.x*x_scale ,Vertices.y*y_scale ,Vertices.z*z_scale };
}
D3DVECTOR Draw::ScaleVertex(const Vertex & Vertices, float x_scale, float y_scale, float z_scale, float x_offset, float y_offset, float z_offset)
{
	return { Vertices.x*x_scale + x_offset,Vertices.y*y_scale + y_offset,Vertices.z*z_scale + z_offset };
}

void Draw::BeginDraw(LPDIRECT3DDEVICE9 dev, Scene * cur_scene, bool create_vb) {
	Device = dev;
	Device->GetViewport(&ViewPort);
	current_scene = cur_scene;
	current_scene->g_pos_vertex = NULL;

	if (create_vb && !current_scene->g_vb) {
		Device->CreateVertexBuffer(sizeof(current_scene->g_Vertex_buff),
			D3DUSAGE_WRITEONLY| D3DUSAGE_DYNAMIC,
			D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT,
			&current_scene->g_vb, NULL);
	}
	Device->GetCreationParameters(&creation_params);
}

void Draw::DrawScene(Scene * cur_scene)
{
	PVOID pData = NULL;
	//sizeof(cur_scene->g_Vertex_buff)
	if (cur_scene->g_vb->Lock(0, cur_scene->g_pos_vertex * sizeof(Vertex), &pData, D3DLOCK_DISCARD) == D3D_OK) {

		//cur_scene->g_vb->Lock(0, sizeof(cur_scene->g_Vertex_buff), &pData, D3DLOCK_DISCARD);
		memcpy_fast(pData, cur_scene->g_Vertex_buff, cur_scene->g_pos_vertex * sizeof(Vertex));
		cur_scene->g_vb->Unlock();
		Device->SetStreamSource(0, cur_scene->g_vb, 0, sizeof(Vertex));
		Device->DrawPrimitive(D3DPT_TRIANGLELIST, 0, cur_scene->g_pos_vertex / 3);
	}
}

void Draw::Release()
{
	if (current_scene->g_vb)current_scene->g_vb->Release(); current_scene->g_vb = NULL;
}

D3DDEVICE_CREATION_PARAMETERS Draw::GetCreationParams()
{
	return creation_params;
}

void Draw::InitFontLib(TTFCore::Font & font)
{
	for (auto it = 0; it < font.GlyphCount(); it++) {
		TTFCore::CodePoint cp(it);
		font.TriangulateGlyph(cp, Draw::triangulator2DI);
		FontLib[it] = Draw::triangulator2DI;
	}
}
Draw::rect_t Draw::GetAABBLetter(TTFCore::Font & font, TTFCore::CodePoint & cp, float x, float y, float size_of_font)
{
	//	font.TriangulateGlyph(cp, FontLib[str[i]]);
	auto d_x = 0.01f* size_of_font;
	auto d_y = 0.01f* -size_of_font;
	rect_t ret = { (float)ViewPort.Width,(float)ViewPort.Height,-(float)ViewPort.Width ,-(float)ViewPort.Height };
	for (auto it : FontLib[cp.code]) {

		if (ret.x > (float)FontLib[cp.code][it.i0].x *d_x + x)
			ret.x = (float)FontLib[cp.code][it.i0].x *d_x + x;
		if (ret.y > (float)FontLib[cp.code][it.i0].y *d_y + y)
			ret.y = (float)FontLib[cp.code][it.i0].y *d_y + y;

		if (ret.w < (float)FontLib[cp.code][it.i0].x *d_x + x)
			ret.w = (float)FontLib[cp.code][it.i0].x *d_x + x;
		if (ret.h < (float)FontLib[cp.code][it.i0].y *d_y + y)
			ret.h = (float)FontLib[cp.code][it.i0].y *d_y + y;
		//
		if (ret.x > (float)FontLib[cp.code][it.i1].x *d_x + x)
			ret.x = (float)FontLib[cp.code][it.i1].x *d_x + x;
		if (ret.y > (float)FontLib[cp.code][it.i1].y *d_y + y)
			ret.y = (float)FontLib[cp.code][it.i1].y *d_y + y;

		if (ret.w < (float)FontLib[cp.code][it.i1].x *d_x + x)
			ret.w = (float)FontLib[cp.code][it.i1].x *d_x + x;
		if (ret.h < (float)FontLib[cp.code][it.i1].y *d_y + y)
			ret.h = (float)FontLib[cp.code][it.i1].y *d_y + y;
		//
		if (ret.x > (float)FontLib[cp.code][it.i2].x *d_x + x)
			ret.x = (float)FontLib[cp.code][it.i2].x *d_x + x;
		if (ret.y > (float)FontLib[cp.code][it.i2].y *d_y + y)
			ret.y = (float)FontLib[cp.code][it.i2].y *d_y + y;

		if (ret.w < (float)FontLib[cp.code][it.i2].x *d_x + x)
			ret.w = (float)FontLib[cp.code][it.i2].x *d_x + x;
		if (ret.h < (float)FontLib[cp.code][it.i2].y *d_y + y)
			ret.h = (float)FontLib[cp.code][it.i2].y *d_y + y;
	}
	ret.w -= ret.x;
	ret.h -= ret.y;

	return ret;
}

Vector2D Draw::GetMaxTextW(TTFCore::Font & font, const wchar_t * str, float x, float y, float size_of_font, float spacing)
{
	rect_t prev_rect = {};
	uint16_t new_line_i = 0u;
	float summ_w = 0.f;
	float summ_h = 0.f;
	auto d_x = 0.01f* size_of_font;
	auto d_y = 0.01f* -size_of_font;

	for (size_t i(0u); i < wcslen(str); i++) {

		if (str[i] == '\n') {
			new_line_i = 0;
			summ_h += size_of_font * 2.f + spacing;
			summ_w = 0.f;
			continue;
		}
		else if (str[i] == ' ') {
			summ_w += size_of_font / 2.f;
			continue;
		}
		else if (str[i] == '\t') {
			summ_w += size_of_font * 2.f;
			continue;
		}
		//rect_t temp_rect = { (float)ViewPort.Width,(float)ViewPort.Height,-(float)ViewPort.Width ,-(float)ViewPort.Height };
		rect_t temp_rect = { (float)FLT_MAX,(float)FLT_MAX,-(float)FLT_MAX ,-(float)FLT_MAX };

		TTFCore::CodePoint cp(str[i]);
		//	font.TriangulateGlyph(cp, FontLib[str[i]]);
		if (cp.code > font.GlyphCount())
			continue;
		for (auto it : FontLib[str[i]]) {

			if (temp_rect.x > (float)FontLib[str[i]][it.i0].x *d_x + x)
				temp_rect.x = (float)FontLib[str[i]][it.i0].x *d_x + x;
			if (temp_rect.y > (float)FontLib[str[i]][it.i0].y *d_y + y)
				temp_rect.y = (float)FontLib[str[i]][it.i0].y *d_y + y;

			if (temp_rect.w < (float)FontLib[str[i]][it.i0].x *d_x + x)
				temp_rect.w = (float)FontLib[str[i]][it.i0].x *d_x + x;
			if (temp_rect.h < (float)FontLib[str[i]][it.i0].y *d_y + y)
				temp_rect.h = (float)FontLib[str[i]][it.i0].y *d_y + y;

			if (temp_rect.x > (float)FontLib[str[i]][it.i1].x *d_x + x)
				temp_rect.x = (float)FontLib[str[i]][it.i1].x *d_x + x;
			if (temp_rect.y > (float)FontLib[str[i]][it.i1].y *d_y + y)
				temp_rect.y = (float)FontLib[str[i]][it.i1].y *d_y + y;

			if (temp_rect.w < (float)FontLib[str[i]][it.i1].x *d_x + x)
				temp_rect.w = (float)FontLib[str[i]][it.i1].x *d_x + x;
			if (temp_rect.h < (float)FontLib[str[i]][it.i1].y *d_y + y)
				temp_rect.h = (float)FontLib[str[i]][it.i1].y *d_y + y;

			if (temp_rect.x > (float)FontLib[str[i]][it.i2].x *d_x + x)
				temp_rect.x = (float)FontLib[str[i]][it.i2].x *d_x + x;
			if (temp_rect.y > (float)FontLib[str[i]][it.i2].y *d_y + y)
				temp_rect.y = (float)FontLib[str[i]][it.i2].y *d_y + y;

			if (temp_rect.w < (float)FontLib[str[i]][it.i2].x *d_x + x)
				temp_rect.w = (float)FontLib[str[i]][it.i2].x *d_x + x;
			if (temp_rect.h < (float)FontLib[str[i]][it.i2].y *d_y + y)
				temp_rect.h = (float)FontLib[str[i]][it.i2].y *d_y + y;
		}
		temp_rect.w -= temp_rect.x;
		temp_rect.h -= temp_rect.y;
		prev_rect = temp_rect;
		summ_w += prev_rect.w;
		++new_line_i;
	}
	return { x + summ_w + spacing * new_line_i, y + summ_h };
}
Vector2D Draw::GetMaxText(TTFCore::Font & font, const char * str, float x, float y, float size_of_font, float spacing) {
	wchar_t bufff[255];
	if (MultiByteToWideChar(CP_UTF8, NULL, &str[0], -1, &bufff[0], 255)) {
		return GetMaxTextW(font, bufff, x, y, size_of_font, spacing);
	}
	return Vector2D();
}
/*
Vector2D Draw::GetMaxText(TTFCore::Font & font, const char * str, float x, float y, float size_of_font, float spacing)
{
	rect_t prev_rect = {};
	uint16_t new_line_i = 0u;
	float summ_w = 0.f;
	float summ_h = 0.f;
	auto d_x = 0.01f* size_of_font;
	auto d_y = 0.01f* -size_of_font;

	for (size_t i(0u); i < strlen(str); i++) {
		if (str[i] > 0x100)
			continue;
		if (str[i] == '\n') {
			new_line_i = 0;
			summ_h += size_of_font * 2.f + spacing;
			summ_w = 0.f;
			continue;
		}
		else if (str[i] == ' ') {
			summ_w += size_of_font / 2.f;
			continue;
		}
		else if (str[i] == '\t') {
			summ_w += size_of_font * 2.f;
			continue;
		}
		//rect_t temp_rect = { (float)ViewPort.Width,(float)ViewPort.Height,-(float)ViewPort.Width ,-(float)ViewPort.Height };
		rect_t temp_rect = { (float)FLT_MAX,(float)FLT_MAX,-(float)FLT_MAX ,-(float)FLT_MAX };

		TTFCore::CodePoint cp(str[i]);
		//	font.TriangulateGlyph(cp, FontLib[str[i]]);
		if (cp.code > font.GlyphCount())
			continue;
		for (auto it : FontLib[str[i]]) {

			if (temp_rect.x > (float)FontLib[str[i]][it.i0].x *d_x + x)
				temp_rect.x = (float)FontLib[str[i]][it.i0].x *d_x + x;
			if (temp_rect.y > (float)FontLib[str[i]][it.i0].y *d_y + y)
				temp_rect.y = (float)FontLib[str[i]][it.i0].y *d_y + y;

			if (temp_rect.w < (float)FontLib[str[i]][it.i0].x *d_x + x)
				temp_rect.w = (float)FontLib[str[i]][it.i0].x *d_x + x;
			if (temp_rect.h < (float)FontLib[str[i]][it.i0].y *d_y + y)
				temp_rect.h = (float)FontLib[str[i]][it.i0].y *d_y + y;

			if (temp_rect.x > (float)FontLib[str[i]][it.i1].x *d_x + x)
				temp_rect.x = (float)FontLib[str[i]][it.i1].x *d_x + x;
			if (temp_rect.y > (float)FontLib[str[i]][it.i1].y *d_y + y)
				temp_rect.y = (float)FontLib[str[i]][it.i1].y *d_y + y;

			if (temp_rect.w < (float)FontLib[str[i]][it.i1].x *d_x + x)
				temp_rect.w = (float)FontLib[str[i]][it.i1].x *d_x + x;
			if (temp_rect.h < (float)FontLib[str[i]][it.i1].y *d_y + y)
				temp_rect.h = (float)FontLib[str[i]][it.i1].y *d_y + y;

			if (temp_rect.x > (float)FontLib[str[i]][it.i2].x *d_x + x)
				temp_rect.x = (float)FontLib[str[i]][it.i2].x *d_x + x;
			if (temp_rect.y > (float)FontLib[str[i]][it.i2].y *d_y + y)
				temp_rect.y = (float)FontLib[str[i]][it.i2].y *d_y + y;

			if (temp_rect.w < (float)FontLib[str[i]][it.i2].x *d_x + x)
				temp_rect.w = (float)FontLib[str[i]][it.i2].x *d_x + x;
			if (temp_rect.h < (float)FontLib[str[i]][it.i2].y *d_y + y)
				temp_rect.h = (float)FontLib[str[i]][it.i2].y *d_y + y;
		}
		temp_rect.w -= temp_rect.x;
		temp_rect.h -= temp_rect.y;
		prev_rect = temp_rect;
		summ_w += prev_rect.w;
		++new_line_i;
	}
	return { x + summ_w + spacing * new_line_i, y + summ_h };
}*/
/*
Vector2D Draw::TextEx(TTFCore::Font & font, const char * str, float x, float y, float size_of_font, float spacing, D3DCOLOR col)
{
	if (!current_scene->g_Vertex_buff)
		return {};
	rect_t prev_rect = {};
	uint16_t new_line_i = 0u;
	float summ_w = 0.f;
	float summ_h = 0.f;
	auto d_x = 0.01f* size_of_font;
	auto d_y = 0.01f* -size_of_font;

	for (size_t i(0u); i < strlen(str); i++) {
		if (str[i] > 0xFFFF)
			continue;
		if (str[i] == '\n') {
			new_line_i = 0;
			summ_h += size_of_font * 2.f + spacing;
			summ_w = 0.f;
			continue;
		}
		else if (str[i] == ' ') {
			summ_w += size_of_font / 2.f;
			continue;
		}
		else if (str[i] == '\t') {
			summ_w += size_of_font * 2.f;
			continue;
		}
		auto x_buff = x + summ_w + spacing * new_line_i;
		auto y_buff = y + summ_h;

		//rect_t temp_rect = { (float)ViewPort.Width,(float)ViewPort.Height,-(float)ViewPort.Width ,-(float)ViewPort.Height };
		rect_t temp_rect = { (float)FLT_MAX,(float)FLT_MAX,-(float)FLT_MAX ,-(float)FLT_MAX };
		TTFCore::CodePoint cp(str[i]);
		if (cp.code > 0xFFFF)
			continue;
		
		for (auto it : FontLib[cp.code]) {
			current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = {
				(float)FontLib[str[i]][it.i0].x *d_x + x_buff,
				(float)FontLib[str[i]][it.i0].y * d_y + y_buff,
				col }; current_scene->g_pos_vertex++;

			if (temp_rect.x > (float)FontLib[str[i]][it.i0].x *d_x + x)
				temp_rect.x = (float)FontLib[str[i]][it.i0].x *d_x + x;
			if (temp_rect.y > (float)FontLib[str[i]][it.i0].y *d_y + y)
				temp_rect.y = (float)FontLib[str[i]][it.i0].y *d_y + y;

			if (temp_rect.w < (float)FontLib[str[i]][it.i0].x *d_x + x)
				temp_rect.w = (float)FontLib[str[i]][it.i0].x *d_x + x;
			if (temp_rect.h < (float)FontLib[str[i]][it.i0].y *d_y + y)
				temp_rect.h = (float)FontLib[str[i]][it.i0].y *d_y + y;

			current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = {
				(float)FontLib[str[i]][it.i1].x *d_x + x_buff,
				(float)FontLib[str[i]][it.i1].y* d_y + y_buff,
				col }; current_scene->g_pos_vertex++;
			if (temp_rect.x > (float)FontLib[str[i]][it.i1].x *d_x + x)
				temp_rect.x = (float)FontLib[str[i]][it.i1].x *d_x + x;
			if (temp_rect.y > (float)FontLib[str[i]][it.i1].y *d_y + y)
				temp_rect.y = (float)FontLib[str[i]][it.i1].y *d_y + y;

			if (temp_rect.w < (float)FontLib[str[i]][it.i1].x *d_x + x)
				temp_rect.w = (float)FontLib[str[i]][it.i1].x *d_x + x;
			if (temp_rect.h < (float)FontLib[str[i]][it.i1].y *d_y + y)
				temp_rect.h = (float)FontLib[str[i]][it.i1].y *d_y + y;

			current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = {
				(float)FontLib[str[i]][it.i2].x*d_x + x_buff,
				(float)FontLib[str[i]][it.i2].y* d_y + y_buff,
				col }; current_scene->g_pos_vertex++;
			if (temp_rect.x > (float)FontLib[str[i]][it.i2].x *d_x + x)
				temp_rect.x = (float)FontLib[str[i]][it.i2].x *d_x + x;
			if (temp_rect.y > (float)FontLib[str[i]][it.i2].y *d_y + y)
				temp_rect.y = (float)FontLib[str[i]][it.i2].y *d_y + y;

			if (temp_rect.w < (float)FontLib[str[i]][it.i2].x *d_x + x)
				temp_rect.w = (float)FontLib[str[i]][it.i2].x *d_x + x;
			if (temp_rect.h < (float)FontLib[str[i]][it.i2].y *d_y + y)
				temp_rect.h = (float)FontLib[str[i]][it.i2].y *d_y + y;
		}

		temp_rect.w -= temp_rect.x;
		temp_rect.h -= temp_rect.y;
		prev_rect = temp_rect;
		summ_w += prev_rect.w;
		++new_line_i;
	}
	return { x + summ_w + spacing * new_line_i, y + summ_h };
}*/
Vector2D Draw::TextEx(TTFCore::Font & font, const char * str, float x, float y, float size_of_font, float spacing, D3DCOLOR col) {

	wchar_t bufff[255];
	if (MultiByteToWideChar(CP_UTF8, NULL, &str[0], -1, &bufff[0], 255)) {
		return Draw::TextExW(font, bufff, x, y, size_of_font, spacing, col);
	}
	return Vector2D();
}
Vector2D Draw::TextExW(TTFCore::Font & font, const wchar_t * str, float x, float y, float size_of_font, float spacing, D3DCOLOR col)
{
	rect_t prev_rect = {};
	uint16_t new_line_i = 0u;
	float summ_w = 0.f;
	float summ_h = 0.f;
	auto d_x = 0.01f* size_of_font;
	auto d_y = 0.01f* -size_of_font;

	for (size_t i(0u); i < wcslen(str); i++) {
		if (str[i] == '\n') {
			new_line_i = 0;
			summ_h += size_of_font * 2.f + spacing;
			summ_w = 0.f;
			continue;
		}
		else if (str[i] == ' ') {
			summ_w += size_of_font / 2.f;
			continue;
		}
		else if (str[i] == '\t') {
			summ_w += size_of_font * 2.f;
			continue;
		}
		auto x_buff = x + summ_w + spacing * new_line_i;
		auto y_buff = y + summ_h;

		//rect_t temp_rect = { (float)ViewPort.Width,(float)ViewPort.Height,-(float)ViewPort.Width ,-(float)ViewPort.Height };
		rect_t temp_rect = { (float)FLT_MAX,(float)FLT_MAX,-(float)FLT_MAX ,-(float)FLT_MAX };
		TTFCore::CodePoint cp(str[i]);
		if (cp.code > font.GlyphCount())
			continue;

		for (auto it : FontLib[cp.code]) {
			current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = {
				(float)FontLib[str[i]][it.i0].x *d_x + x_buff,
				(float)FontLib[str[i]][it.i0].y * d_y + y_buff,
				col }; current_scene->g_pos_vertex++;

			if (temp_rect.x > (float)FontLib[str[i]][it.i0].x *d_x + x)
				temp_rect.x = (float)FontLib[str[i]][it.i0].x *d_x + x;
			if (temp_rect.y > (float)FontLib[str[i]][it.i0].y *d_y + y)
				temp_rect.y = (float)FontLib[str[i]][it.i0].y *d_y + y;

			if (temp_rect.w < (float)FontLib[str[i]][it.i0].x *d_x + x)
				temp_rect.w = (float)FontLib[str[i]][it.i0].x *d_x + x;
			if (temp_rect.h < (float)FontLib[str[i]][it.i0].y *d_y + y)
				temp_rect.h = (float)FontLib[str[i]][it.i0].y *d_y + y;

			current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = {
				(float)FontLib[str[i]][it.i1].x *d_x + x_buff,
				(float)FontLib[str[i]][it.i1].y* d_y + y_buff,
				col }; current_scene->g_pos_vertex++;
			if (temp_rect.x > (float)FontLib[str[i]][it.i1].x *d_x + x)
				temp_rect.x = (float)FontLib[str[i]][it.i1].x *d_x + x;
			if (temp_rect.y > (float)FontLib[str[i]][it.i1].y *d_y + y)
				temp_rect.y = (float)FontLib[str[i]][it.i1].y *d_y + y;

			if (temp_rect.w < (float)FontLib[str[i]][it.i1].x *d_x + x)
				temp_rect.w = (float)FontLib[str[i]][it.i1].x *d_x + x;
			if (temp_rect.h < (float)FontLib[str[i]][it.i1].y *d_y + y)
				temp_rect.h = (float)FontLib[str[i]][it.i1].y *d_y + y;

			current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = {
				(float)FontLib[str[i]][it.i2].x*d_x + x_buff,
				(float)FontLib[str[i]][it.i2].y* d_y + y_buff,
				col }; current_scene->g_pos_vertex++;
			if (temp_rect.x > (float)FontLib[str[i]][it.i2].x *d_x + x)
				temp_rect.x = (float)FontLib[str[i]][it.i2].x *d_x + x;
			if (temp_rect.y > (float)FontLib[str[i]][it.i2].y *d_y + y)
				temp_rect.y = (float)FontLib[str[i]][it.i2].y *d_y + y;

			if (temp_rect.w < (float)FontLib[str[i]][it.i2].x *d_x + x)
				temp_rect.w = (float)FontLib[str[i]][it.i2].x *d_x + x;
			if (temp_rect.h < (float)FontLib[str[i]][it.i2].y *d_y + y)
				temp_rect.h = (float)FontLib[str[i]][it.i2].y *d_y + y;
		}

		temp_rect.w -= temp_rect.x;
		temp_rect.h -= temp_rect.y;
		prev_rect = temp_rect;
		summ_w += prev_rect.w;
		++new_line_i;
	}
	return { x + summ_w + spacing * new_line_i, y + summ_h };
}
void Draw::TextExRot(TTFCore::Font& font, Matrix3x3& rot_matrix, const char * str, float x, float y, float size_of_font, float spacing, D3DCOLOR col)
{
	if (!current_scene->g_Vertex_buff)
		return;
	auto d_x = 0.01f* size_of_font;
	auto d_y = 0.01f* -size_of_font;
	float summ_w = 0.f;
	float summ_h = 0.f;
	rect_t prev_rect = {};
	uint16_t new_line_i = 0u;
	for (size_t i(0u); i < strlen(str); i++) {
		if (str[i] > 0x100)
			continue;
		if (str[i] == ' ') {
			summ_w += size_of_font / 2.f;
			continue;
		}
		if (str[i] == '\n') {
			new_line_i = 0;
			summ_h += size_of_font * 2.f + spacing;
			summ_w = 0.f;
			continue;
		}
		TTFCore::CodePoint cp(str[i]);
		//font.TriangulateGlyph(cp, FontLib[str[i]]);

	//	rect_t temp_rect = { (float)ViewPort.Height,(float)ViewPort.Width,0.f ,0.f };
		rect_t temp_rect = { (float)FLT_MAX,(float)FLT_MAX,-(float)FLT_MAX ,-(float)FLT_MAX };

		for (auto it : FontLib[cp.code]) {

			auto vec_rot0 = rot_matrix.mul(Vector((float)FontLib[str[i]][it.i0].x *d_x + summ_w + spacing * new_line_i, (float)FontLib[str[i]][it.i0].y * d_y + summ_h, 0.f));

			current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = {
				vec_rot0.m_x + x,
				vec_rot0.m_y + y,
				col }; current_scene->g_pos_vertex++;

			if (temp_rect.x > (float)vec_rot0.m_x + x)
				temp_rect.x = (float)vec_rot0.m_x + x;
			if (temp_rect.y > (float)vec_rot0.m_y + y)
				temp_rect.y = (float)vec_rot0.m_y + y;

			if (temp_rect.w < (float)vec_rot0.m_x + x)
				temp_rect.w = (float)vec_rot0.m_x + x;
			if (temp_rect.h < (float)vec_rot0.m_y + y)
				temp_rect.h = (float)vec_rot0.m_y + y;

			auto vec_rot1 = rot_matrix.mul(Vector((float)FontLib[str[i]][it.i1].x *d_x + summ_w + spacing * new_line_i, (float)FontLib[str[i]][it.i1].y * d_y + summ_h, 0.f));
			current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = {
				vec_rot1.m_x + x,
				vec_rot1.m_y + y,
				col }; current_scene->g_pos_vertex++;
			if (temp_rect.x > (float)vec_rot1.m_x + x)
				temp_rect.x = (float)vec_rot1.m_x + x;
			if (temp_rect.y > (float)vec_rot1.m_y + y)
				temp_rect.y = (float)vec_rot1.m_y + y;

			if (temp_rect.w < (float)vec_rot1.m_x + x)
				temp_rect.w = (float)vec_rot1.m_x + x;
			if (temp_rect.h < (float)vec_rot1.m_y + y)
				temp_rect.h = (float)vec_rot1.m_y + y;

			auto vec_rot2 = rot_matrix.mul(Vector((float)FontLib[str[i]][it.i2].x *d_x + summ_w + spacing * new_line_i, (float)FontLib[str[i]][it.i2].y * d_y + summ_h, 0.f));
			current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = {
				vec_rot2.m_x + x,
				vec_rot2.m_y + y,
				col }; current_scene->g_pos_vertex++;
			if (temp_rect.x > (float)vec_rot2.m_x + x)
				temp_rect.x = (float)vec_rot2.m_x + x;
			if (temp_rect.y > (float)vec_rot2.m_y + y)
				temp_rect.y = (float)vec_rot2.m_y + y;

			if (temp_rect.w < (float)vec_rot2.m_x + x)
				temp_rect.w = (float)vec_rot2.m_x + x;
			if (temp_rect.h < (float)vec_rot2.m_y + y)
				temp_rect.h = (float)vec_rot2.m_y + y;
			current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = {
				vec_rot2.m_x + x,
				vec_rot2.m_y + y,
				col }; current_scene->g_pos_vertex++;
			current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = {
				vec_rot1.m_x + x,
				vec_rot1.m_y + y,
				col }; current_scene->g_pos_vertex++;
			current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = {
				vec_rot0.m_x + x,
				vec_rot0.m_y + y,
				col }; current_scene->g_pos_vertex++;
		}
		temp_rect.w -= temp_rect.x;
		temp_rect.h -= temp_rect.y;
		prev_rect = temp_rect;
		summ_w += prev_rect.w;
		++new_line_i;
	}

}

void Draw::TriangleEx(const Vertex & v1, const Vertex & v2, const Vertex & v3)
{
	if (!current_scene->g_Vertex_buff)
		return;
	current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = v1; current_scene->g_pos_vertex++;
	current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = v2; current_scene->g_pos_vertex++;
	current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = v3; current_scene->g_pos_vertex++;
}

void Draw::RectEx(const Vertex & v1, const Vertex & v2, const Vertex & v3, const Vertex & v4)
{
	if (!current_scene->g_Vertex_buff)
		return;
	current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = v1; current_scene->g_pos_vertex++;
	current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = v2; current_scene->g_pos_vertex++;
	current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = v3; current_scene->g_pos_vertex++;
	current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = v3; current_scene->g_pos_vertex++;
	current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = v2; current_scene->g_pos_vertex++;
	current_scene->g_Vertex_buff[current_scene->g_pos_vertex] = v4; current_scene->g_pos_vertex++;
}

void Draw::Rect(float x, float y, float h, float w, D3DCOLOR col)
{
	RectEx({ x,y,col }, { x + w,y ,col }, { x ,y + h ,col }, { x + w,y + h,col });
}

void Draw::Box(float x, float y, float w, float h, D3DCOLOR clr, float Thickness)
{
	Rect(x - Thickness, y - Thickness, Thickness, w + Thickness,  clr);//up
	Rect(x + w, y - Thickness, h + Thickness, Thickness, clr);//right
	Rect(x, y + h, Thickness, w + Thickness,  clr);//bottom
	Rect(x - Thickness, y, h + Thickness, Thickness,  clr);//left
}

void Draw::Box_border(float x, float y, float w, float h, D3DCOLOR clr, float Thickness, float o_Thickness)
{
	Box(x +(o_Thickness*0.5f), y + (o_Thickness*0.5f), w - o_Thickness, h - o_Thickness, D3DCOLOR_RGBA(0, 0, 0, 255), Thickness + o_Thickness);
	Box(x, y, w, h, clr, Thickness);
}

void Draw::Box_Outlined(float x, float y, float w, float h, D3DCOLOR clr, float Thickness)
{
	Rect(x - Thickness, y - Thickness, h + Thickness * 2, w + Thickness * 2, D3DCOLOR_RGBA(0, 0, 0, 255));
	Rect(x, y, h, w, clr);
}


Vector2D Draw::Text(TTFCore::Font & font, const char * str, float x, float y, float size_of_font, float spacing, D3DCOLOR col, bool center)
{
	if (!center)
		return Draw::TextEx(font, str, x, y, size_of_font, spacing, col);

	auto t = Draw::GetMaxText(font, str, x, y, size_of_font, spacing);

	return Draw::TextEx(font, str, x - (t.m_x - x)*0.5f, y, size_of_font, spacing, col);
}

Vector2D Draw::Text_Outline(TTFCore::Font & font, const char * str, float x, float y, float size_of_font, float spacing, D3DCOLOR col, bool center)
{
	Draw::Text(font, str, x + 1.f, y + 1.f, size_of_font, spacing, D3DCOLOR_ARGB(255, 0, 0, 0), center);
	Draw::Text(font, str, x - 1.f, y - 1.f, size_of_font, spacing, D3DCOLOR_ARGB(255, 0, 0, 0), center);
	Draw::Text(font, str, x + 1.f, y - 1.f, size_of_font, spacing, D3DCOLOR_ARGB(255, 0, 0, 0), center);
	Draw::Text(font, str, x - 1.f, y + 1.f, size_of_font, spacing, D3DCOLOR_ARGB(255, 0, 0, 0), center);

	Draw::Text(font, str, x - 1.f, y , size_of_font, spacing, D3DCOLOR_ARGB(255, 0, 0, 0), center);
	Draw::Text(font, str, x , y + 1.f, size_of_font, spacing, D3DCOLOR_ARGB(255, 0, 0, 0), center);
	Draw::Text(font, str, x + 1.f, y , size_of_font, spacing, D3DCOLOR_ARGB(255, 0, 0, 0), center);
	Draw::Text(font, str, x , y - 1.f, size_of_font, spacing, D3DCOLOR_ARGB(255, 0, 0, 0), center);

	Draw::Text(font, str, x, y, size_of_font, spacing, col, center);
	return Vector2D();
}

void Draw::LineEx(const Vertex & v1, const Vertex & v2, float Thickness)
{
	if (!current_scene->g_Vertex_buff)
		return;
	auto d_Thickness = Thickness / 2;

	float delta_y = v2.y - v1.y;
	float delta_x = v2.x - v1.x;
	float tg, atan, sin, cos;
	if (delta_x == 0) {
		if (delta_y > 0) {
			static constexpr float m_r90_ = ToRadians(-90.f);
			tg = std::tan(m_r90_);
		}
		else if (delta_y < 0) {
			static constexpr float m_r90_ = ToRadians(90.f);
			tg = abs(std::tan(m_r90_));
		}
		else {
			return;
		}
	}
	else {
		tg = abs(delta_y / delta_x);
	}
	atan = std::atan(tg);
	sin = std::sin(atan);
	cos = std::cos(atan);

	auto m_x = delta_x > 0 ? 1 : -1;
	auto m_y = delta_y > 0 ? 1 : -1;
	auto dx = sin * d_Thickness*m_x;
	auto dy = cos * d_Thickness*m_y;
	Vertex p1;
	Vertex p2;
	Vertex p3;
	p2 = { float(v1.x + dx),float(v1.y - dy) ,v1.col };
	if ((m_x == -1 && m_y == 1) || (m_x == 1 && m_y == -1)) {
		p1 = { float(v2.x - dx),float(v2.y + dy) ,float(v2.z),float(v2.ht),v2.col };
		p3 = { float(v1.x - dx),float(v1.y + dy) ,float(v1.z),float(v1.ht),v1.col };
	}
	else {
		p1 = { float(v1.x - dx),float(v1.y + dy),float(v1.z),float(v1.ht),v1.col };
		p3 = { float(v2.x - dx),float(v2.y + dy),float(v2.z),float(v2.ht),v2.col };
	}
	TriangleEx(p1, p2, p3);
	Vertex p4;
	Vertex p5;
	Vertex p6;
	if ((m_x == -1 && m_y == 1) || (m_x == 1 && m_y == -1)) {
		p4 = p2;
		p5 = p1;
		p6 = { float(v2.x + dx),float(v2.y - dy),float(v2.z),float(v2.ht),v2.col };
	}
	else {
		p4 = { float(v2.x + dx),float(v2.y - dy),float(v2.z),float(v2.ht),v2.col };
		p5 = p3;
		p6 = p2;
	}
	TriangleEx(p4, p5, p6);
}

void Draw::Line(float x0, float y0, float x1, float y1, float Thickness, D3DCOLOR col)
{
	Draw::LineEx(Vertex(x0, y0, col), Vertex(x1, y1, col), Thickness);
}

void  Draw::Line_Outline(float x0, float y0, float x1, float y1, float l_Thickness, float o_Thickness)
{
	Line(x0, y0, x1, y1, D3DCOLOR_ARGB(255, 1, 1, 1), l_Thickness + o_Thickness);
}

void Draw::Line_Outlined(float x0, float y0, float x1, float y1, float l_Thickness, float o_Thickness, D3DCOLOR col)
{
	Line_Outline(x0, y0, x1, y1, l_Thickness, o_Thickness);
	Line(x0, y0, x1, y1, col, l_Thickness);
}

void Draw::CircleEmpty(float x, float y, float radius, float deg_start, float deg_end, int Sides, D3DCOLOR Color, float Thickness)
{
	float Step = (D3DX_PI * 2.0 / Sides);

	for (auto temp(ToRadians(deg_start)); temp < ToRadians(deg_end); temp += Step) {
		float X1 = radius * cos(temp) + x;
		float Y1 = radius * sin(temp) + y;
		float X2 = radius * cos(temp + Step) + x;
		float Y2 = radius * sin(temp + Step) + y;
		LineEx({ X1,Y1,Color }, { X2,Y2,Color }, Thickness);
	}
}

void Draw::Circle(float x, float y, float radius, float deg_start, float deg_end, int Sides, D3DCOLOR Color)
{
	float Step = ((D3DX_PI * 2.0) / Sides);

	for (auto temp(ToRadians(deg_start)); temp <= ToRadians(deg_end) - Step / 10; temp += Step) {
		float X1 = radius * cos(temp) + x;
		float Y1 = radius * sin(temp) + y;
		float X2 = radius * cos(temp + Step) + x;
		float Y2 = radius * sin(temp + Step) + y;
		TriangleEx({ x,y,Color }, { X1,Y1,Color }, { X2,Y2,Color });
	}
}

void Draw::RoundedRect(float x, float y, float h, float w, D3DCOLOR col, float Round_size)
{
	Rect(x - Round_size, y, h, Round_size, col);
	Rect(x, y + h, Round_size, w, col);
	Rect(x + w, y, h, Round_size, col);
	Rect(x, y - Round_size, Round_size, w, col);
	Rect(x, y, h, w, col);
	Circle(x, y, Round_size, 180.f, 270.f, 64, col);
	Circle(x + w, y, Round_size, 270.f, 360.f, 64, col);
	Circle(x + w, y + h, Round_size, 0.f, 90.f, 64, col);
	Circle(x, y + h, Round_size, 90.f, 180.f, 64, col);
}

void Draw::CubeFilled(Matrix3x3& rot_matrix, float x, float y, float z, float w_s, float h_s, float z_s, D3DCOLOR col)
{
	for (auto it(0u); it < 6; it++) {
		D3DVECTOR ver_0 = rot_matrix.mul(ScaleVertex(Draw::CoubVertexes[it * 4], w_s, h_s, z_s));
		D3DVECTOR ver_1 = rot_matrix.mul(ScaleVertex(Draw::CoubVertexes[it * 4 + 1], w_s, h_s, z_s));
		D3DVECTOR ver_2 = rot_matrix.mul(ScaleVertex(Draw::CoubVertexes[it * 4 + 2], w_s, h_s, z_s));
		D3DVECTOR ver_3 = rot_matrix.mul(ScaleVertex(Draw::CoubVertexes[it * 4 + 3], w_s, h_s, z_s));

		Draw::RectEx({ ver_0.x + x ,ver_0.y + y ,ver_0.z + z,1.f,col },
			{ ver_1.x + x ,ver_1.y + y ,ver_1.z + z,1.f,col },
			{ ver_2.x + x,ver_2.y + y,ver_2.z + z,1.f,col },
			{ ver_3.x + x,ver_3.y + y ,ver_3.z + z,1.f,col });
		//Draw::RectEx({ ver_0.x + x ,ver_0.y + y ,ver_0.z + z,1.f,Draw::CoubVertexes[it * 4].col },
		//	{ ver_1.x + x ,ver_1.y + y ,ver_1.z + z,1.f,Draw::CoubVertexes[it * 4].col },
		//	{ ver_2.x + x,ver_2.y + y,ver_2.z + z,1.f,Draw::CoubVertexes[it * 4].col },
		//	{ ver_3.x + x,ver_3.y + y ,ver_3.z + z,1.f,Draw::CoubVertexes[it * 4].col });
	}
}
void Draw::CubeFilled(Matrix3x3& rot_matrix, float x, float y, float z, float w_s, float h_s, float z_s, float x_offset, float y_offset, float z_offset, D3DCOLOR col)
{
	for (auto it(0u); it < 6; it++) {
		D3DVECTOR ver_0 = rot_matrix.mul(ScaleVertex(Draw::CoubVertexes[it * 4], w_s, h_s, z_s, x_offset, y_offset, z_offset));
		D3DVECTOR ver_1 = rot_matrix.mul(ScaleVertex(Draw::CoubVertexes[it * 4 + 1], w_s, h_s, z_s, x_offset, y_offset, z_offset));
		D3DVECTOR ver_2 = rot_matrix.mul(ScaleVertex(Draw::CoubVertexes[it * 4 + 2], w_s, h_s, z_s, x_offset, y_offset, z_offset));
		D3DVECTOR ver_3 = rot_matrix.mul(ScaleVertex(Draw::CoubVertexes[it * 4 + 3], w_s, h_s, z_s, x_offset, y_offset, z_offset));

		Draw::RectEx({ ver_0.x + x ,ver_0.y + y ,ver_0.z + z,1.f,col },
			{ ver_1.x + x ,ver_1.y + y ,ver_1.z + z,1.f,col },
			{ ver_2.x + x,ver_2.y + y,ver_2.z + z,1.f,col },
			{ ver_3.x + x,ver_3.y + y ,ver_3.z + z,1.f,col });
		//Draw::RectEx({ ver_0.x + x ,ver_0.y + y ,ver_0.z + z,1.f,Draw::CoubVertexes[it * 4].col },
		//	{ ver_1.x + x ,ver_1.y + y ,ver_1.z + z,1.f,Draw::CoubVertexes[it * 4].col },
		//	{ ver_2.x + x,ver_2.y + y,ver_2.z + z,1.f,Draw::CoubVertexes[it * 4].col },
		//	{ ver_3.x + x,ver_3.y + y ,ver_3.z + z,1.f,Draw::CoubVertexes[it * 4].col });
	}
}
Matrix3x3 RotateMatrixByOrdinal(Vector o_ang_rot, QAngle ang_rot, float deg_rot_by_angle, float deg_rot_by_side) {
	Matrix3x3 temp = {};
	Matrix3x3 side = {};
	o_ang_rot.Normalize();
	temp.InitRotMatrix(ang_rot.ToVector(), deg_rot_by_angle);
	side.InitRotMatrix(temp.mul(o_ang_rot), deg_rot_by_side);
	temp = temp * side;
	return temp;
}

void Draw::Checkbox3DEmpty(Vector VecRot, float deg_rot, float x, float y, float z, float w_s, float h_s, float z_s, D3DCOLOR col, D3DCOLOR line_col, float l_Thickness)
{
	Matrix3x3 rot_matrix = {};
	Matrix3x3 m_temp = {};
	D3DVECTOR w2s;
	Vector side;

	D3DVECTOR topL, bottom, topR;
	VecRot.Normalize();
	rot_matrix.InitRotMatrix(VecRot, deg_rot);
	CubeFilled(rot_matrix, x, y, z, w_s, h_s, z_s, col);
	CubeFilled(rot_matrix, x, y, z, w_s, h_s, z_s, 0.f, 0.f, w_s * 2.f, col);
	w2s = rot_matrix.mul(ScaleVertex(Draw::CoubVertexes[0], w_s, h_s, z_s));

	Matrix3x3 tempa = {};
	tempa.InitRotMatrix_Y(90.f);
	m_temp = rot_matrix * tempa;
	CubeFilled(m_temp, x, y, z, w_s, h_s, z_s, w_s, 0.f, w_s + z_s, col);
	CubeFilled(m_temp, x, y, z, w_s, h_s, z_s, w_s, w_s * 2.f - z_s * 2.f, w_s + z_s, col);

	CubeFilled(m_temp, x, y, z, w_s, h_s, z_s, w_s, 0.f, -w_s - z_s, col);
	CubeFilled(m_temp, x, y, z, w_s, h_s, z_s, w_s, w_s * 2.f - z_s * 2.f, -w_s - z_s, col);


	topL = rot_matrix.mul(ScaleVertex(Draw::CoubVertexes[3], w_s, h_s, z_s, 0.f, -h_s, h_s));
	topR = rot_matrix.mul(ScaleVertex(Draw::CoubVertexes[15], w_s, h_s, z_s, 0.f, 0.f, -h_s));

	m_temp = RotateMatrixByOrdinal(VecRot, QAngle(90.f, 0.f, 0.f), 90.f, -deg_rot);

	CubeFilled(m_temp, x - w2s.x, y - w2s.y, z - w2s.z - w2s.z, w_s, h_s, z_s, w_s, h_s, -z_s, col);
	CubeFilled(m_temp, x - w2s.x, y - w2s.y, z - w2s.z - w2s.z, w_s, h_s, z_s, w_s, h_s, -z_s + w_s * 2.f, col);

	w2s = m_temp.mul(ScaleVertex(Draw::CoubVertexes[0], w_s, h_s, z_s));
	m_temp = RotateMatrixByOrdinal(VecRot, QAngle(-90.f, 0.f, 0.f), -90.f, -deg_rot);

	CubeFilled(m_temp, x - w2s.x, y - w2s.y, z - w2s.z - w2s.z, w_s, h_s, z_s, w_s * 2.f - h_s, -w_s, -z_s, col);
	CubeFilled(m_temp, x - w2s.x, y - w2s.y, z - w2s.z - w2s.z, w_s, h_s, z_s, w_s * 2.f - h_s, -w_s, -z_s + w_s * 2.f, col);

	w2s = m_temp.mul(ScaleVertex(Draw::CoubVertexes[0], w_s, h_s, z_s));
	m_temp = rot_matrix;

	CubeFilled(m_temp, x - w2s.x, y - w2s.y, z - w2s.z - w2s.z, w_s, h_s, z_s, -h_s, w_s *3.f - (h_s*2.f), -z_s, col);
	CubeFilled(m_temp, x - w2s.x, y - w2s.y, z - w2s.z - w2s.z, w_s, h_s, z_s, -h_s, w_s *3.f - (h_s*2.f), -z_s + w_s * 2.f, col);
	w2s = m_temp.mul(ScaleVertex(Draw::CoubVertexes[0], w_s, h_s, z_s));
	bottom = m_temp.mul(ScaleVertex(Draw::CoubVertexes[0], w_s, h_s, z_s, (-w_s / 2.f), (w_s *2.f - (h_s*4.f)), 0.f));

}

void Draw::Checkbox3DFilled(Vector VecRot, float deg_rot, float x, float y, float z, float w_s, float h_s, float z_s, D3DCOLOR col, D3DCOLOR line_col, D3DCOLOR fill_col, float l_Thickness)
{
	Matrix3x3 mat = {};
	mat.InitRotMatrix(VecRot, deg_rot);
	Draw::CubeFilled(mat, x, y, z, w_s, w_s - h_s, w_s - z_s, 0.f, w_s, w_s, fill_col);
	Draw::Checkbox3DEmpty(VecRot, deg_rot, x, y, z, w_s, h_s, z_s, col, line_col, l_Thickness);
}

void Draw::RotateRelPoint(float & x, float & y, const float Deg, const float point_x, const float point_y)
{
	float RadRot = ToRadians(Deg);

	x -= point_x; y -= point_y;
	float rotatedX = x * cos(RadRot) - y * sin(RadRot);
	float rotatedY = x * sin(RadRot) + y * cos(RadRot);
	x = rotatedX + point_x;
	y = rotatedY + point_y;
}

void Draw::RotateRelZero(float & x, float & y, const float Deg)
{
	float RadRot = ToRadians(Deg);

	x = x * cos(RadRot) - y * sin(RadRot);
	y = x * sin(RadRot) + y * cos(RadRot);
}







Vertex Draw::CoubVertexes[] =
{

{ 1.0f,	1.0f,-1.0f, 1.f,	D3DCOLOR_XRGB(255, 255, 255) },//back
{ 1.0f,-1.0f,-1.0f, 1.f,	D3DCOLOR_XRGB(255, 255, 255) },
{-1.0f,	1.0f,-1.0f, 1.f,	D3DCOLOR_XRGB(255, 255, 255) },
{-1.0f,-1.0f,-1.0f, 1.f,	D3DCOLOR_XRGB(255, 255, 255) },

{ -1.0f, 1.0f,-1.0f, 1.f,	D3DCOLOR_XRGB(255, 0, 0) },//left
{ -1.0f,-1.0f,-1.0f, 1.f,	D3DCOLOR_XRGB(255, 0, 0) },
{ -1.0f, 1.0f, 1.0f, 1.f,	D3DCOLOR_XRGB(255, 0, 0) },
{ -1.0f,-1.0f, 1.0f, 1.f,	D3DCOLOR_XRGB(255, 0, 0) },

{ 1.0f,-1.0f,-1.0f, 1.f,	D3DCOLOR_XRGB(0, 255, 0) },//right
{ 1.0f,	1.0f,-1.0f, 1.f,	D3DCOLOR_XRGB(0, 255, 0) },
{ 1.0f,-1.0f, 1.0f, 1.f,	D3DCOLOR_XRGB(0, 255, 0) },
{ 1.0f,	1.0f, 1.0f, 1.f,	D3DCOLOR_XRGB(0, 255, 0) },

{ 1.0f,	-1.0f,	1.0f, 1.f,	D3DCOLOR_XRGB(0, 0, 255) },//face
{ 1.0f,	 1.0f,	1.0f, 1.f,	D3DCOLOR_XRGB(0, 0,255) },
{-1.0f, -1.0f,	1.0f, 1.f,	D3DCOLOR_XRGB(0, 0, 255) },
{-1.0f,  1.0f,	1.0f, 1.f,	D3DCOLOR_XRGB(0, 0, 255) },

{ 1.0f,	1.0f, 1.0f,	1.f,	D3DCOLOR_XRGB(0, 255, 255) },//top
{ 1.0f,	1.0f,-1.0f, 1.f,	D3DCOLOR_XRGB(0, 255, 255) },
{-1.0f, 1.0f, 1.0f,	1.f,	D3DCOLOR_XRGB(0, 255, 255) },
{-1.0f, 1.0f,-1.0f, 1.f,	D3DCOLOR_XRGB(0, 255, 255) },

{ 1.0f,-1.0f,-1.0f,1.f,		D3DCOLOR_XRGB(255, 0, 255) },
{ 1.0f,-1.0f, 1.0f,1.f,		D3DCOLOR_XRGB(255, 0, 255) },
{-1.0f,-1.0f,-1.0f,1.f,		D3DCOLOR_XRGB(255, 0, 255) },
{-1.0f,-1.0f, 1.0f,1.f,		D3DCOLOR_XRGB(255, 0, 255) },
};