#pragma once
#include "../../gmod_specific/Math/core.hpp"
#include "../../gmod_specific/Math/qangle.hpp"
#include "../../gmod_specific/Math/vector.hpp"
#include "../../gmod_specific/Math/vector2d.hpp"
#include "../../gmod_specific/Math/vector4d.hpp"
#include "../../include/stdafx.h"
#include "font/TTF.h"



#define FontSize 10.f
namespace Draw {

	struct rect_t {
		rect_t() = default;
		float x, y, h, w;
	};
	
	__declspec(align(16)) struct Scene {
		IDirect3DVertexBuffer9* g_vb = NULL;
		unsigned int g_pos_vertex;
		Vertex g_Vertex_buff[0x800000];
	};
	void BeginDraw(LPDIRECT3DDEVICE9 dev, Scene * cur_scene,bool create_vb);
	void DrawScene(Scene * cur_scene);
	void Release();
	D3DDEVICE_CREATION_PARAMETERS GetCreationParams();
	extern TTF::Triangulator2DII triangulator2DI;
	extern Vertex CoubVertexes[];
	extern TTF::Triangulator2DII FontLib[0xFFFF];
	void InitFontLib(TTFCore::Font& font);
	rect_t GetAABBLetter(TTFCore::Font& font, TTFCore::CodePoint& cp, float x, float y, float size_of_font) ;
	Vector2D GetMaxTextW(TTFCore::Font & font, const wchar_t * str, float x, float y, float size_of_font, float spacing);
	Vector2D GetMaxText(TTFCore::Font& font, const char * str, float x, float y, float size_of_font, float spacing);
	Vector2D TextEx(TTFCore::Font& font, const char * str, float x, float y, float size_of_font, float spacing, D3DCOLOR col) ;
	Vector2D TextExW(TTFCore::Font & font, const wchar_t * str, float x, float y, float size_of_font, float spacing, D3DCOLOR col);
	Vector2D Text(TTFCore::Font& font, const char * str, float x, float y, float size_of_font, float spacing, D3DCOLOR col,bool center);
	Vector2D Text_Outline(TTFCore::Font& font, const char * str, float x, float y, float size_of_font, float spacing, D3DCOLOR col, bool center);
	void TextExRot(TTFCore::Font& font, Matrix3x3& rot_matrix, const char * str, float x, float y, float size_of_font, float spacing, D3DCOLOR col);
	void TriangleEx(const Vertex& v1, const Vertex& v2, const Vertex& v3);
	void RectEx(const Vertex& v1, const Vertex& v2, const Vertex& v3, const Vertex& v4);
	void Rect(float x, float y, float h, float w, D3DCOLOR col);
	void Box(float x, float y, float w, float h, D3DCOLOR clr, float Thickness);
	void Box_border(float x, float y, float w, float h, D3DCOLOR clr, float Thickness, float o_Thickness);
	void Box_Outlined(float x, float y, float w, float h, D3DCOLOR clr, float Thickness);
	void LineEx(const Vertex& v1, const Vertex& v2, float Thickness);
	void Line(float x0, float y0, float x1, float y1, float Thickness, D3DCOLOR col);
	void Line_Outline(float x0, float y0, float x1, float y1, float l_Thickness, float o_Thickness);
	void Line_Outlined(float x0, float y0, float x1, float y1, float l_Thickness, float o_Thickness, D3DCOLOR col);
	void CircleEmpty(float x, float y, float radius, float deg_start, float deg_end, int Sides, D3DCOLOR Color, float Thickness);
	void Circle(float x, float y, float radius, float deg_start, float deg_end, int Sides, D3DCOLOR Color);
	void RoundedRect(float x, float y, float h, float w, D3DCOLOR col, float Round_size);
	void CubeFilled(Matrix3x3& rot_matrix,float x,float y, float z,float w_s,float h_s,float z_s, D3DCOLOR col);
	void CubeFilled(Matrix3x3& rot_matrix, float x, float y, float z, float w_s, float h_s, float z_s, float x_offset, float y_offset, float z_offset, D3DCOLOR col);
	void Checkbox3DEmpty(Vector VecRot, float deg_rot, float x, float y, float z, float w_s, float h_s, float z_s, D3DCOLOR col, D3DCOLOR line_col,float l_Thickness);
	void Checkbox3DFilled(Vector VecRot, float deg_rot, float x, float y, float z, float w_s, float h_s, float z_s, D3DCOLOR col, D3DCOLOR line_col, D3DCOLOR fill_col, float l_Thickness);
	inline void RotateRelPoint(float& x, float& y, const float Deg, const float point_x, const float point_y);
	inline void RotateRelZero(float& x, float& y, const float Deg);
	D3DVECTOR ScaleVertex(const Vertex& Vertices, float scale);
	D3DVECTOR ScaleVertex(const Vertex& Vertices, float x_scale, float y_scale, float z_scale);
	D3DVECTOR ScaleVertex(const Vertex& Vertices, float x_scale, float y_scale, float z_scale, float x_offset, float y_offset, float z_offset);
}