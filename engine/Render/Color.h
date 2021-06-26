#pragma once
struct ImVec4 {
	float x, y, z, w;
	ImVec4() { x = y = z = w = 0.0f; }
	ImVec4(float _x, float _y, float _z, float _w) { x = _x; y = _y; z = _z; w = _w; }
};
class cColor
{
public:

	cColor()
	{
		*((int *)this) = 0;
	}
	cColor(DWORD raw)
	{
		SetRawColor(raw);
	}
	cColor(int _r, int _g, int _b)
	{
		SetColor(_r, _g, _b, 255);
	}

	cColor(int _r, int _g, int _b, int _a)
	{
		SetColor(_r, _g, _b, _a);
	}

	void SetColor(int _r, int _g, int _b, int _a = 0)
	{
		_color[0] = (unsigned char)_r;
		_color[1] = (unsigned char)_g;
		_color[2] = (unsigned char)_b;
		_color[3] = (unsigned char)_a;
	}

	void GetColor(int &_r, int &_g, int &_b, int &_a) const
	{
		_r = _color[0];
		_g = _color[1];
		_b = _color[2];
		_a = _color[3];
	}

	void SetRawColor(int color32)
	{
		*((int *)this) = color32;
	}

	int GetRawColor() const
	{
		return *((int *)this);
	}

	inline int r() const { return _color[0]; }
	inline int g() const { return _color[1]; }
	inline int b() const { return _color[2]; }
	inline int a() const { return _color[3]; }

	unsigned char &operator[](int index)
	{
		return _color[index];
	}

	const unsigned char &operator[](int index) const
	{
		return _color[index];
	}

	bool operator == (const cColor &rhs) const
	{
		return (*((int *)this) == *((int *)&rhs));
	}

	bool operator != (const cColor &rhs) const
	{
		return !(operator==(rhs));
	}

	cColor &operator=(const cColor &rhs)
	{
		SetRawColor(rhs.GetRawColor());
		return *this;
	}

private:
	unsigned char _color[4];
};
class Color
{
public:

	Color(D3DCOLOR& color):
		_color(color)
	{
	}

	void SetColor(int _r, int _g, int _b, int _a = 0)
	{
		this->_color = D3DCOLOR_ARGB(_a, _r, _g, _b);
		
	}
	

	void GetColor(int &_r, int &_g, int &_b, int &_a) const
	{
		
		auto ptr = (unsigned char*)this;
		_r = ptr[0];
		_g = ptr[1];
		_b = ptr[2];
		_a = ptr[3];
	}
	ImVec4 ToImVec4() {

		auto constexpr s = 1.0f / 255.0f;
		__m128 clr = _mm_cvtepi32_ps(_mm_cvtepu8_epi32((__m128i&)_color));
		auto mulled = _mm_mul_ps(clr, _mm_load_ps1(&s));
		return { mulled.m128_f32[0],mulled.m128_f32[1],mulled.m128_f32[2],mulled.m128_f32[3] };
	}

	void FromImVec4(ImVec4& From) {
		const float m_const = 255.f;
		auto fl_result = _mm_mul_ps(_mm_load_ps((float*)&From), _mm_load_ps1(&m_const));
		auto shuffled = _mm_shuffle_epi8(_mm_cvtps_epi32(fl_result), shuffle_color_mask);

		this->_color = _mm_cvtsi128_si32(shuffled);
	}
	cColor to_cColor() {
		return cColor(_color);
	}

	void from_cColor(cColor From) {
		SetColor(From.r(), From.g(), From.b(), From.a());
	}


	void SetRawColor(D3DCOLOR color)
	{
		_color = color;
	}

	D3DCOLOR GetRawColor() const
	{
		return _color;
	}
	unsigned char &operator[](int index)
	{
		auto ptr = (unsigned char*)this;
		return ptr[index];
	}

	const unsigned char &operator[](int index) const
	{
		auto ptr = (unsigned char*)this;
		return ptr[index];
	}

	bool operator == (const Color &rhs) const
	{
		return (*((D3DCOLOR *)this) == *((D3DCOLOR *)&rhs));
	}

	bool operator != (const Color &rhs) const
	{
		return !(operator==(rhs));
	}

	Color &operator=(const Color &rhs)
	{
		SetRawColor(rhs.GetRawColor());
		return *this;
	}
private:
	const __m128i shuffle_color_mask = _mm_setr_epi8(0, 4, 8, 12, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1);

	D3DCOLOR& _color;
};

