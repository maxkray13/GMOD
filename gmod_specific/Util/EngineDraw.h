#pragma once
#include "../../engine/Render/Render.h"
#include "..\SDK.h"
namespace SDK {
	constexpr float to_deg = float(180.0 / D3DX_PI);
	constexpr float to_rad = float(D3DX_PI / 180.0);

	void CDraw::Init()
	{
		
		font_esp = g_pSurface->CreateFont();
		g_pSurface->SetFontGlyphSet(font_esp, "Verdana", 16, FW_SEMIBOLD, 0, 0, FONTFLAG_GAUSSIANBLUR| FONTFLAG_DROPSHADOW);

	//	char buff[200];
	//	sprintf_s(buff, "echo font_esp %d", font_esp);
	//	g_pEngine->ClientCmd(buff);
	}

	void CDraw::Reset()
	{
		//char buff[200];
		//sprintf_s(buff, "echo font_esp %d", font_esp);
	//	g_pEngine->ClientCmd(buff);
		g_pSurface->SetFontGlyphSet(font_esp, "Verdana", 16, FW_SEMIBOLD, 0, 0, FONTFLAG_GAUSSIANBLUR | FONTFLAG_DROPSHADOW);
	}

	void CDraw::Triangle(Vector2D p1, Vector2D p2, Vector2D p3, cColor clr)
	{
		static int t_Texture = -1;
		if (t_Texture == -1) {
			t_Texture = g_pSurface->CreateNewTextureID(true);
		}
		unsigned char buffer[4] = { 255,255,255,255 };
		g_pSurface->DrawSetTextureRGBA(t_Texture, buffer, 1, 1, false, false);

		g_pSurface->DrawSetColor(clr.r(), clr.g(), clr.b(), clr.a());
		g_pSurface->DrawSetTexture(t_Texture);
		Vertex_t verts[3];
		verts[0].Init(p1);
		verts[1].Init(p2);
		verts[2].Init(p3);
		g_pSurface->DrawTexturedPolygon(3, verts);
	}

	void CDraw::BoxEx(Vector2D p1, Vector2D p2, Vector2D p3, Vector2D p4, cColor clr)
	{
		Triangle(p1, p2, p3, clr);
		Triangle(p4, p3, p2, clr);
	}

	void CDraw::Line(int x1, int y1, int x2, int y2, cColor clr, int Thickness)
	{
		auto d_Thickness = Thickness / 2;

		float delta_y = y2 - y1;
		float delta_x = x2 - x1;
		float tg, atan, sin, cos;
		if (delta_x == 0) {
			if (delta_y > 0) {
				static constexpr float m_r90_ = -90.f* to_rad;
				tg = std::tan(m_r90_);
			}
			else if (delta_y < 0) {
				static constexpr float m_r90_ = 90.f* to_rad;
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

		Vector2D p1;
		Vector2D p2;
		Vector2D p3;

		p2 = { float(x1 + dx),float(y1 - dy) };
		if ((m_x == -1 && m_y == 1) || (m_x == 1 && m_y == -1)) {
			p1 = { float(x2 - dx),float(y2 + dy) };
			p3 = { float(x1 - dx),float(y1 + dy) };
		}
		else {
			p1 = { float(x1 - dx),float(y1 + dy) };
			p3 = { float(x2 - dx),float(y2 + dy) };
		}
		Triangle(p1, p2, p3, clr);
		Vector2D p4;
		Vector2D p5;
		Vector2D p6;
		if ((m_x == -1 && m_y == 1)|| (m_x == 1 && m_y == -1)) {
			p4 = p2;
			p5 = p1;
			p6 = { float(x2 + dx),float(y2 - dy) };
		}
		else {
			p4 = { float(x2 + dx),float(y2 - dy) };
			p5 = p3;
			p6 = p2;
		}
		Triangle(p4, p5, p6, clr);
	}
	void CDraw::Line_outline(int x1, int y1, int x2, int y2, int l_Thickness, int o_Thickness)
	{
		auto col = cColor(0, 0, 0);
		Line(x1 , y1 , x2 , y2 , col, l_Thickness + o_Thickness);
	}
	void CDraw::Line_Outlined(int x1, int y1, int x2, int y2, cColor clr, int l_Thickness, int o_Thickness)
	{
		Line_outline(x1, y1, x2, y2, l_Thickness, o_Thickness);
		Line(x1, y1, x2, y2, clr, l_Thickness);
	}

	void CDraw::Box(int x, int y, int w, int h, cColor clr)
	{
		Vector2D p1 = { float(x), float(y) };
		Vector2D p2 = { float(x + w), float(y) };
		Vector2D p3 = { float(x), float(y + h) };
		Vector2D p4 = { float(x + w), float(y + h) };

		BoxEx(p1, p2, p3, p4, clr);
	}

	void CDraw::Outline(int x, int y, int w, int h, cColor clr, int Thickness)
	{
		Box(x - Thickness, y - Thickness, w + Thickness, Thickness, clr);//up
		Box(x + w, y - Thickness, Thickness, h + Thickness, clr);//right
		Box(x, y + h, w + Thickness, Thickness, clr);//bottom
		Box(x - Thickness, y, Thickness, h + Thickness, clr);//left
	}

	void CDraw::Box_Outlined(int x, int y, int w, int h, cColor clr, int Thickness)
	{
		Box(x - Thickness, y - Thickness, w + Thickness * 2, h + Thickness * 2, cColor(0, 0, 0));
		Box(x, y, w, h, clr);
	}


	void CDraw::text(int x, int y, const wchar_t * string, HFont font, cColor color, bool centeredx, bool centeredy)
	{
		if (centeredx || centeredy)
		{
			int wide, tall;
			g_pSurface->GetTextSize(font, string, wide, tall);
			if (centeredx)
				x -= wide / 2;
			if (centeredy)
				y -= tall / 2;
		}
		//g_pSurface->DrawFlushText();
		g_pSurface->DrawSetTextPos(x, y);
		g_pSurface->DrawSetTextFont(font);//, FONT_DRAW_NONADDITIVE
		g_pSurface->DrawSetTextColor(color);
		g_pSurface->DrawPrintText(string, wcslen(string));
	}

	void CDraw::text(int x, int y, const wchar_t * string, HFont font, cColor color, bool centered)
	{
		text(x, y, string, font, color, centered, centered);
	}

	Vector2D CDraw::get_text_size(const wchar_t * string, HFont font)
	{
		int x_res, y_res;
		g_pSurface->GetTextSize(font, string, x_res, y_res);
		return Vector2D(x_res, y_res);
	}

	void CDraw::ColoredCircle(int x, int y, int radius, cColor clr)
	{
		g_pSurface->DrawSetColor(clr.r(), clr.g(), clr.b(), clr.a());
		g_pSurface->DrawOutlinedCircle(x, y, radius, 64);
	}
	void CDraw::LinePoint::Push(int x1 , int y1 , int x2, int y2, cColor clr, int l_Thickness, bool is_end)
	{
		float delta_y = y2 - y1;
		float delta_x = x2 - x1;
		float tg;
		if (delta_x == 0) {
			if (delta_y > 0) {
				static constexpr float m_r90_ = -90.f* to_rad;
				tg = std::tan(m_r90_);
			}
			else if (delta_y < 0) {
				static constexpr float m_r90_ = 90.f* to_rad;
				tg = std::tan(m_r90_);
			}
			else {
				return;
			}
		}
		else {
			tg = delta_y / delta_x;
		}
		this->points.push_back({ x1, y1, x2, y2,l_Thickness, clr, tg });
	}
	void CDraw::LinePoint::Push(int x2, int y2, cColor clr, int l_Thickness, bool is_end)
	{
		auto x1 = points[points.size() - 1].x2;
		auto y1 = points[points.size() - 1].y2;

		float delta_y = y2 - y1;
		float delta_x = x2 - x1;
		float curr_tg,diff, atan, sin, cos;;
		if (delta_x == 0) {
			if (delta_y > 0) {
				static constexpr float m_r90_ = -90.f* to_rad;
				curr_tg = std::tan(m_r90_);
			}
			else if (delta_y < 0) {
				static constexpr float m_r90_ = 90.f* to_rad;
				curr_tg =std::tan(m_r90_);
			}
			else {
				return;
			}
		}
		else {
			curr_tg = delta_y / delta_x;
		}

		atan = std::atan(curr_tg);
		sin = std::sin(atan);
		cos = std::cos(atan);
		auto d_Thickness = l_Thickness / 2;
		auto dx = sin * d_Thickness;
		auto dy = cos * d_Thickness;

		auto denominator = (1 + (curr_tg * points[points.size() - 1].tg));
	//	if (denominator == 0) {
			//+-90
	//	}
		auto f_x1 = x1 > x2 ? x1 + dx : x1 - dx;
		auto f_y1 = y1 > y2 ? y1 + dy : y1 - dy;

		if (y1 > y2) {
			f_y1 = y1 + dy;
			f_x1 = x1 + dx;
		}
		else {
			f_y1 = y1 - dy;
			f_x1 = x1 - dx;
		}

		this->points.push_back({ int(f_x1) ,int(f_y1 ),int(x2), int(y2),int(l_Thickness), clr });

	//	diff = std::atan(curr_tg - points[points.size() - 1].tg) / denominator;
		//*to_deg
	//	diff *= to_deg;
	//	if ((diff > -45.f&& diff <= 0)|| (diff>=0 && diff<45)) {
	//		this->points.push_back({ int(x1 - dx), int(y1 - dy), int(x2), int(y2),int(l_Thickness), clr });//для 4 четверти перфект
	//	}
	//	else {
	//		this->points.push_back({ int(x1 + dx), int(y1 + dy), int(x2), int(y2),int(l_Thickness), clr });//для 4 четверти перфект
	//	}
		

	}
	void CDraw::LinePoint::Draw()
	{
		auto& EngineRender = SDK::CDraw::Instance();
		for (auto it : points) {
			EngineRender.Line(it.x1, it.y1, it.x2, it.y2, it.clr, it.l_Thickness);
		}
	}
	/*
	void LinePoint::Push(int x1, int y1, int x2, int y2, cColor clr, int l_Thickness, int o_Thickness = 0) {
		float delta_y = y2 - y1;
		float delta_x = x2 - x1;
		float curr_tg, prev_atan, prev_sin, prev_cos;
		if (delta_x == 0) {
			if (delta_y > 0) {
				static constexpr float m_r90_ = -90.f* to_rad;
				curr_tg = std::tan(m_r90_);
			}
			else if (delta_y < 0) {
				static constexpr float m_r90_ = 90.f* to_rad;
				curr_tg = abs(std::tan(m_r90_));
			}
			else {
				return;
			}
		}
		else {
			curr_tg = abs(delta_y / delta_x);
		}


		if (points.size()) {
			points.push_back({ x1 , y1 , x2 , y2 , l_Thickness, o_Thickness , clr , curr_tg });
			return;
		}
		if (points[points.size()].x2 != x1 || points[points.size()].y2 != y1)
			return;
		
		prev_atan = std::atan(points[points.size()].tg);
		prev_sin = std::sin(prev_atan);
		prev_cos = std::cos(prev_atan);

		auto m_x = points[points.size()].x2- points[points.size()].x1 > 0 ? 1 : -1;
		auto m_y = points[points.size()].y2 - points[points.size()].y1 > 0 ? 1 : -1;
		auto dx = prev_sin * (points[points.size()].l_Thickness + points[points.size()].o_Thickness)  *m_x;
		auto dy = prev_cos * (points[points.size()].l_Thickness + points[points.size()].o_Thickness) * m_y;

		Vector2D prev_max = { points[points.size()].x2 + dx,points[points.size()].y2 + dx };
		Vector2D prev_min = { points[points.size()].x2 - dx,points[points.size()].y2 - dx };

		auto diff = std::atan(curr_tg - points[points.size()].tg) / (1 + (curr_tg * points[points.size()].tg));

		if (diff > 180) {
			
		}



	}*/
}

