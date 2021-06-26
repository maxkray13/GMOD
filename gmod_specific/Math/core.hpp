#pragma once
#include "../../include/stdafx.h"

constexpr auto g_pi_radians = static_cast< float >( 3.14159265358979323846 );
constexpr auto g_pi_degrees = static_cast< float >( 180.0 );
class QAngle;
class Vector;
class Vector2D;
class Vector4D;

struct matrix3x4_t
{
public:
	matrix3x4_t() = default;
	matrix3x4_t(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23)
	{
		m_data[0][0] = m00;	m_data[0][1] = m01; m_data[0][2] = m02; m_data[0][3] = m03;
		m_data[1][0] = m10;	m_data[1][1] = m11; m_data[1][2] = m12; m_data[1][3] = m13;
		m_data[2][0] = m20;	m_data[2][1] = m21; m_data[2][2] = m22; m_data[2][3] = m23;
	}
	auto At( unsigned int index ) -> float*;
	auto At( unsigned int index ) const -> const float*;

public:
	auto operator [] ( unsigned int index ) -> float*;
	auto operator [] ( unsigned int index ) const -> const float*;

private:
	float m_data[ 3 ][ 4 ] = { };
};

void SinCos(float angle, float* sine, float*cose);
class VMatrix
{
public:
	auto At( unsigned int index ) -> float*;
	auto At( unsigned int index ) const -> const float*;

	void SetupMatrixOrgAngles(const Vector &origin, const QAngle &vAngles);
	VMatrix InverseTR();
	const matrix3x4_t& VMatrix::As3x4() const;
	matrix3x4_t& VMatrix::As3x4();
public:
	auto operator [] ( unsigned int index ) -> float*;
	auto operator [] ( unsigned int index ) const -> const float*;

	float m_data[ 4 ][ 4 ] = { };
};
class Quaternion				// same data-layout as engine's vec4_t,
{								//		which is a vec_t[4]
public:
	inline Quaternion(void) {

		// Initialize to NAN to catch errors
#ifdef _DEBUG
#ifdef VECTOR_PARANOIA
		x = y = z = w = VEC_T_NAN;
#endif
#endif
	}
	inline Quaternion(float ix, float iy, float iz, float iw) : x(ix), y(iy), z(iz), w(iw) { }
	inline void Init(float ix = 0.0f, float iy = 0.0f, float iz = 0.0f, float iw = 0.0f) { x = ix; y = iy; z = iz; w = iw; }

	bool IsValid() const {
		return x != NAN && y != NAN && z != NAN && w != NAN;
	};
	void Invalidate() {
		x = y = z = w = NAN;
	};


	bool operator==(const Quaternion &src) const {
		return (x == src.x) && (y == src.y) && (z == src.z) && (w == src.w);
	};
	bool operator!=(const Quaternion &src) const {
		return !operator==(src);
	};

	float* Base() { return (float*)this; }
	const float* Base() const { return (float*)this; }

	// array access...
	float operator[](int i) const {
		return ((float*)this)[i];
	};
	float& operator[](int i) {
		return ((float*)this)[i];
	};

	float x, y, z, w;
};


#define PIU 3.14159265358979323846f

#define DEG2RAD( x ) ( ( float )( x ) * ( float )( ( float )( PIU ) / 180.0f ) )
#define RAD2DEG( x ) ( ( float )( x ) * ( float )( 180.0f / ( float )( PIU ) ) )

void QuaternionSlerp(const Quaternion &p, const Quaternion &q, float t, Quaternion &qt);
void QuaternionSlerpNoAlign(const Quaternion &p, const Quaternion &q, float t, Quaternion &qt);
void MatrixSetColumn(const Vector &in, int column, matrix3x4_t& out);
inline void MatrixSetColumn(VMatrix &src, int nCol, const Vector &column);
void MatrixInvert(const matrix3x4_t& in, matrix3x4_t& out);
void AngleVectors(const QAngle &angles, Vector *forward, Vector *right, Vector *up);
void MatrixGetColumn(const matrix3x4_t& in, int column, Vector &out);
void MatrixAngles(const matrix3x4_t& matrix, float *angles);
void ConcatTransforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out);
void QuaternionMatrix(const Quaternion &q, const Vector &pos, matrix3x4_t& matrix);
void QuaternionMatrix(const Quaternion &q, matrix3x4_t& matrix);
void AxisAngleQuaternion(const Vector &axis, float angle, Quaternion &q);
void VectorIRotate(const float *in1, const matrix3x4_t& in2, float *out);
void AngleMatrix(const QAngle &angles, matrix3x4_t& matrix);
QAngle TransformAnglesToWorldSpace(const QAngle &angles, const matrix3x4_t &parentMatrix);
Vector WorldToLocal(Vector position, QAngle angle, Vector newSystemOrigin, QAngle newSystemAngles);
void VectorITransform(const float *in1, const matrix3x4_t& in2, float *out);
void AngleMatrix(const QAngle &angles, const Vector &position, matrix3x4_t& matrix);
void MatrixCopy(const matrix3x4_t& in, matrix3x4_t& out);

constexpr auto ToRadians( float degrees ) -> float
{
	return ( degrees * static_cast< float >( g_pi_radians / g_pi_degrees ) );
}

constexpr auto ToDegrees( float radians ) -> float
{
	return ( radians * static_cast< float >( g_pi_degrees / g_pi_radians ) );
}

auto AngleNormalize( float angle ) -> float;
auto AngleDistance( const QAngle& angles, const Vector& start, const Vector& end ) -> float;


struct Vertex {
	Vertex() = default;
	Vertex(float x, float y, D3DCOLOR col);
	Vertex(float x, float y, float z, float ht, D3DCOLOR col);
	float x, y, z, ht;
	D3DCOLOR col;
};
struct Matrix3x3 {
	Matrix3x3() = default;
	inline Matrix3x3 operator-() const;
	inline const Matrix3x3& operator+=(const Matrix3x3 &other);
	inline Vector GetForward() const;
	inline Vector GetLeft() const;
	inline Vector GetUp() const;
	Matrix3x3		operator*(const Matrix3x3 &other) const;
	inline Matrix3x3 Matrix3x3::operator+(const Matrix3x3 &other) const;
	D3DVECTOR mul(Vertex& scr);
	D3DVECTOR mul(D3DVECTOR& scr);
	Vector mul(Vector& scr);
	void InitRotMatrix(D3DVECTOR VecRot, float deg_rot);
	void InitRotMatrix(Vector VecRot, float deg_rot);
	inline Vector VMul4x3Transpose(const Vector &vVec) const;
	void InitRotMatrix_X(float deg_rot);
	void InitRotMatrix_Y(float deg_rot);
	void InitRotMatrix_Z(float deg_rot);
	inline float* operator[](int i);
	inline const float* operator[](int i) const;

	float m[3][3];
};