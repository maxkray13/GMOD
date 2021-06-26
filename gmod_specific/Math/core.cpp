#include "core.hpp"
#include "qangle.hpp"
#include "vector.hpp"

auto matrix3x4_t::At( unsigned int index ) -> float*
{
	return m_data[ index ];
}

auto matrix3x4_t::At( unsigned int index ) const -> const float*
{
	return m_data[ index ];
}

auto matrix3x4_t::operator [] ( unsigned int index ) -> float*
{
	return At( index );
}

auto matrix3x4_t::operator [] ( unsigned int index ) const -> const float*
{
	return At( index );
}


auto VMatrix::At( unsigned int index ) -> float*
{
	return m_data[ index ];
}

auto VMatrix::At( unsigned int index ) const -> const float*
{
	return m_data[ index ];
}
void SinCos(float angle, float* sine, float*cose) {
	*sine = sin(angle);
	*cose = cos(angle);
}
static void SetupMatrixAnglesInternal(float m[4][4], const QAngle & vAngles)
{
	float		sr, sp, sy, cr, cp, cy;

	SinCos(ToRadians(vAngles[1]), &sy, &cy);
	SinCos(ToRadians(vAngles[0]), &sp, &cp);
	SinCos(ToRadians(vAngles[2]), &sr, &cr);

	// matrix = (YAW * PITCH) * ROLL
	m[0][0] = cp * cy;
	m[1][0] = cp * sy;
	m[2][0] = -sp;
	m[0][1] = sr * sp*cy + cr * -sy;
	m[1][1] = sr * sp*sy + cr * cy;
	m[2][1] = sr * cp;
	m[0][2] = (cr*sp*cy + -sr * -sy);
	m[1][2] = (cr*sp*sy + -sr * cy);
	m[2][2] = cr * cp;
	m[0][3] = 0.f;
	m[1][3] = 0.f;
	m[2][3] = 0.f;
}
void VMatrix::SetupMatrixOrgAngles(const Vector &origin, const QAngle &vAngles)
{
	SetupMatrixAnglesInternal(m_data, vAngles);

	// Add translation
	m_data[0][3] = origin.m_x;
	m_data[1][3] = origin.m_y;
	m_data[2][3] = origin.m_z;
	m_data[3][0] = 0.0f;
	m_data[3][1] = 0.0f;
	m_data[3][2] = 0.0f;
	m_data[3][3] = 1.0f;
}



FORCEINLINE void VectorCopy(const Vector& src, Vector& dst)
{
	dst.m_x = src.m_x;
	dst.m_y = src.m_y;
	dst.m_z = src.m_z;
}
void Vector3DMultiply(const VMatrix &src1, const Vector &src2, Vector &dst)
{
	// Make sure it works if src2 == dst
	Vector tmp;
	const Vector &v = (&src2 == &dst) ? static_cast<const Vector>(tmp) : src2;

	if (&src2 == &dst)
	{
		VectorCopy(src2, tmp);
	}

	dst[0] = src1[0][0] * v[0] + src1[0][1] * v[1] + src1[0][2] * v[2];
	dst[1] = src1[1][0] * v[0] + src1[1][1] * v[1] + src1[1][2] * v[2];
	dst[2] = src1[2][0] * v[0] + src1[2][1] * v[1] + src1[2][2] * v[2];
}
inline void MatrixSetColumn(VMatrix &src, int nCol, const Vector &column)
{

	src.m_data[0][nCol] = column.m_x;
	src.m_data[1][nCol] = column.m_y;
	src.m_data[2][nCol] = column.m_z;
}
void MatrixInverseTR(const VMatrix& src, VMatrix &dst)
{
	Vector vTrans, vNewTrans;

	// Transpose the upper 3x3.
	dst.m_data[0][0] = src.m_data[0][0];  dst.m_data[0][1] = src.m_data[1][0]; dst.m_data[0][2] = src.m_data[2][0];
	dst.m_data[1][0] = src.m_data[0][1];  dst.m_data[1][1] = src.m_data[1][1]; dst.m_data[1][2] = src.m_data[2][1];
	dst.m_data[2][0] = src.m_data[0][2];  dst.m_data[2][1] = src.m_data[1][2]; dst.m_data[2][2] = src.m_data[2][2];

	// Transform the translation.
	vTrans.Init(-src.m_data[0][3], -src.m_data[1][3], -src.m_data[2][3]);
	Vector3DMultiply(dst, vTrans, vNewTrans);
	MatrixSetColumn(dst, 3, vNewTrans);

	// Fill in the bottom row.
	dst.m_data[3][0] = dst.m_data[3][1] = dst.m_data[3][2] = 0.0f;
	dst.m_data[3][3] = 1.0f;
}
VMatrix VMatrix::InverseTR()
{
	VMatrix ret;
	MatrixInverseTR(*this, ret);
	return ret;
}

const matrix3x4_t & VMatrix::As3x4() const
{
	return *((const matrix3x4_t*)this);
}

matrix3x4_t & VMatrix::As3x4()
{
	return *((matrix3x4_t*)this);
}

auto VMatrix::operator [] ( unsigned int index ) -> float*
{
	return At( index );
}

auto VMatrix::operator [] ( unsigned int index ) const -> const float*
{
	return At( index );
}
template <class T>
FORCEINLINE void swap(T& x, T& y)
{
	T temp = x;
	x = y;
	y = temp;
}
FORCEINLINE float DotProduct(const float *v1, const float *v2)
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}
void MatrixInvert(const matrix3x4_t& in, matrix3x4_t& out)
{
	if (&in == &out)
	{
		swap(out[0][1], out[1][0]);
		swap(out[0][2], out[2][0]);
		swap(out[1][2], out[2][1]);
	}
	else
	{
		// transpose the matrix
		out[0][0] = in[0][0];
		out[0][1] = in[1][0];
		out[0][2] = in[2][0];

		out[1][0] = in[0][1];
		out[1][1] = in[1][1];
		out[1][2] = in[2][1];

		out[2][0] = in[0][2];
		out[2][1] = in[1][2];
		out[2][2] = in[2][2];
	}

	// now fix up the translation to be in the other space
	float tmp[3];
	tmp[0] = in[0][3];
	tmp[1] = in[1][3];
	tmp[2] = in[2][3];

	out[0][3] = -DotProduct(tmp, out[0]);
	out[1][3] = -DotProduct(tmp, out[1]);
	out[2][3] = -DotProduct(tmp, out[2]);
}
inline void MatrixAngles(const matrix3x4_t &matrix, QAngle &angles)
{
	MatrixAngles(matrix, &angles.m_x);
}

void AngleVectors(const QAngle &angles, Vector *forward, Vector *right, Vector *up)
{

	float sr, sp, sy, cr, cp, cy;

	SinCos(ToRadians(angles[1]), &sy, &cy);
	SinCos(ToRadians(angles[0]), &sp, &cp);
	SinCos(ToRadians(angles[2]), &sr, &cr);

	if (forward)
	{
		forward->m_x = cp * cy;
		forward->m_y = cp * sy;
		forward->m_z = -sp;
	}

	if (right)
	{
		right->m_x = (-1 * sr*sp*cy + -1 * cr*-sy);
		right->m_y = (-1 * sr*sp*sy + -1 * cr*cy);
		right->m_z = -1 * sr*cp;
	}

	if (up)
	{
		up->m_x = (cr*sp*cy + -sr * -sy);
		up->m_y = (cr*sp*sy + -sr * cy);
		up->m_z = cr * cp;
	}
}

void QuaternionMatrix(const Quaternion &q, const Vector &pos, matrix3x4_t& matrix)
{

	QuaternionMatrix(q, matrix);

	matrix[0][3] = pos.m_x;
	matrix[1][3] = pos.m_y;
	matrix[2][3] = pos.m_z;
}
void QuaternionMatrix(const Quaternion &q, matrix3x4_t& matrix)
{
	// Original code
	// This should produce the same code as below with optimization, but looking at the assmebly,
	// it doesn't.  There are 7 extra multiplies in the release build of this, go figure.
#if 1
	matrix[0][0] = 1.0 - 2.0 * q.y * q.y - 2.0 * q.z * q.z;
	matrix[1][0] = 2.0 * q.x * q.y + 2.0 * q.w * q.z;
	matrix[2][0] = 2.0 * q.x * q.z - 2.0 * q.w * q.y;

	matrix[0][1] = 2.0f * q.x * q.y - 2.0f * q.w * q.z;
	matrix[1][1] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.z * q.z;
	matrix[2][1] = 2.0f * q.y * q.z + 2.0f * q.w * q.x;

	matrix[0][2] = 2.0f * q.x * q.z + 2.0f * q.w * q.y;
	matrix[1][2] = 2.0f * q.y * q.z - 2.0f * q.w * q.x;
	matrix[2][2] = 1.0f - 2.0f * q.x * q.x - 2.0f * q.y * q.y;

	matrix[0][3] = 0.0f;
	matrix[1][3] = 0.0f;
	matrix[2][3] = 0.0f;
#else
	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;

	// precalculate common multiplitcations
	x2 = q.x + q.x;
	y2 = q.y + q.y;
	z2 = q.z + q.z;
	xx = q.x * x2;
	xy = q.x * y2;
	xz = q.x * z2;
	yy = q.y * y2;
	yz = q.y * z2;
	zz = q.z * z2;
	wx = q.w * x2;
	wy = q.w * y2;
	wz = q.w * z2;

	matrix[0][0] = 1.0 - (yy + zz);
	matrix[0][1] = xy - wz;
	matrix[0][2] = xz + wy;
	matrix[0][3] = 0.0f;

	matrix[1][0] = xy + wz;
	matrix[1][1] = 1.0 - (xx + zz);
	matrix[1][2] = yz - wx;
	matrix[1][3] = 0.0f;

	matrix[2][0] = xz - wy;
	matrix[2][1] = yz + wx;
	matrix[2][2] = 1.0 - (xx + yy);
	matrix[2][3] = 0.0f;
#endif
}
void AxisAngleQuaternion(const Vector &axis, float angle, Quaternion &q)
{
	float sa, ca;

	SinCos(DEG2RAD(angle) * 0.5f, &sa, &ca);

	q.x = axis.m_x * sa;
	q.y = axis.m_y * sa;
	q.z = axis.m_z * sa;
	q.w = ca;
}
void VectorIRotate(const float *in1, const matrix3x4_t& in2, float *out)
{
	out[0] = in1[0] * in2[0][0] + in1[1] * in2[1][0] + in1[2] * in2[2][0];
	out[1] = in1[0] * in2[0][1] + in1[1] * in2[1][1] + in1[2] * in2[2][1];
	out[2] = in1[0] * in2[0][2] + in1[1] * in2[1][2] + in1[2] * in2[2][2];
}
void AngleMatrix(const QAngle &angles, matrix3x4_t& matrix)
{
#ifdef _VPROF_MATHLIB
	VPROF_BUDGET("AngleMatrix", "Mathlib");
#endif

	float sr, sp, sy, cr, cp, cy;

#ifdef _X360
	fltx4 radians, scale, sine, cosine;
	radians = LoadUnaligned3SIMD(angles.Base());
	scale = ReplicateX4(M_PI_F / 180.f);
	radians = MulSIMD(radians, scale);
	SinCos3SIMD(sine, cosine, radians);

	sp = SubFloat(sine, 0);	sy = SubFloat(sine, 1);	sr = SubFloat(sine, 2);
	cp = SubFloat(cosine, 0);	cy = SubFloat(cosine, 1);	cr = SubFloat(cosine, 2);
#else
	SinCos(DEG2RAD(angles[1]), &sy, &cy);
	SinCos(DEG2RAD(angles[0]), &sp, &cp);
	SinCos(DEG2RAD(angles[2]), &sr, &cr);

#endif

	// matrix = (YAW * PITCH) * ROLL
	matrix[0][0] = cp * cy;
	matrix[1][0] = cp * sy;
	matrix[2][0] = -sp;

	float crcy = cr * cy;
	float crsy = cr * sy;
	float srcy = sr * cy;
	float srsy = sr * sy;
	matrix[0][1] = sp * srcy - crsy;
	matrix[1][1] = sp * srsy + crcy;
	matrix[2][1] = sr * cp;

	matrix[0][2] = (sp*crcy + srsy);
	matrix[1][2] = (sp*crsy - srcy);
	matrix[2][2] = cr * cp;

	matrix[0][3] = 0.0f;
	matrix[1][3] = 0.0f;
	matrix[2][3] = 0.0f;
}
void MatrixSetColumn(const Vector &in, int column, matrix3x4_t& out)
{
	out[0][column] = in.m_x;
	out[1][column] = in.m_y;
	out[2][column] = in.m_z;
}
void MatrixCopy(const matrix3x4_t& in, matrix3x4_t& out)
{
	memcpy((void*)&out, (void*)&in, sizeof(float) * 3 * 4);
}
void AngleMatrix(const QAngle &angles, const Vector &position, matrix3x4_t& matrix)
{
	AngleMatrix(angles, matrix);
	MatrixSetColumn(position, 3, matrix);
}

void ConcatTransforms(const matrix3x4_t& in1, const matrix3x4_t& in2, matrix3x4_t& out)
{
	if (&in1 == &out)
	{
		matrix3x4_t in1b;
		MatrixCopy(in1, in1b);
		ConcatTransforms(in1b, in2, out);
		return;
	}
	if (&in2 == &out)
	{
		matrix3x4_t in2b;
		MatrixCopy(in2, in2b);
		ConcatTransforms(in1, in2b, out);
		return;
	}
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
		in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
		in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
		in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
		in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
		in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
		in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
		in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
		in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
		in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
		in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
		in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
		in1[2][2] * in2[2][3] + in1[2][3];
}
void MatrixGetColumn(const matrix3x4_t& in, int column, Vector &out)
{
	out.m_x = in[0][column];
	out.m_y = in[1][column];
	out.m_z = in[2][column];
}
void MatrixAngles(const matrix3x4_t& matrix, float *angles)
{
#ifdef _VPROF_MATHLIB
	VPROF_BUDGET("MatrixAngles", "Mathlib");
#endif
	float forward[3];
	float left[3];
	float up[3];

	//
	// Extract the basis vectors from the matrix. Since we only need the Z
	// component of the up vector, we don't get X and Y.
	//
	forward[0] = matrix[0][0];
	forward[1] = matrix[1][0];
	forward[2] = matrix[2][0];
	left[0] = matrix[0][1];
	left[1] = matrix[1][1];
	left[2] = matrix[2][1];
	up[2] = matrix[2][2];

	float xyDist = sqrtf(forward[0] * forward[0] + forward[1] * forward[1]);

	// enough here to get angles?
	if (xyDist > 0.001f)
	{
		// (yaw)	y = ATAN( forward.y, forward.x );		-- in our space, forward is the X axis
		angles[1] = RAD2DEG(atan2f(forward[1], forward[0]));

		// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
		angles[0] = RAD2DEG(atan2f(-forward[2], xyDist));

		// (roll)	z = ATAN( left.z, up.z );
		angles[2] = RAD2DEG(atan2f(left[2], up[2]));
	}
	else	// forward is mostly Z, gimbal lock-
	{
		// (yaw)	y = ATAN( -left.x, left.y );			-- forward is mostly z, so use right for yaw
		angles[1] = RAD2DEG(atan2f(-left[0], left[1]));

		// (pitch)	x = ATAN( -forward.z, sqrt(forward.x*forward.x+forward.y*forward.y) );
		angles[0] = RAD2DEG(atan2f(-forward[2], xyDist));

		// Assume no roll in this case as one degree of freedom has been lost (i.e. yaw == roll)
		angles[2] = 0;
	}
}

QAngle TransformAnglesToWorldSpace(const QAngle & angles, const matrix3x4_t & parentMatrix)
{
	matrix3x4_t angToParent, angToWorld;
	AngleMatrix(angles, angToParent);
	ConcatTransforms(parentMatrix, angToParent, angToWorld);
	QAngle out;
	MatrixAngles(angToWorld, out);
	return out;
}


void VectorITransform(const float * in1, const matrix3x4_t & in2, float * out)
{
	float in1t[3];

	in1t[0] = in1[0] - in2[0][3];
	in1t[1] = in1[1] - in2[1][3];
	in1t[2] = in1[2] - in2[2][3];

	out[0] = in1t[0] * in2[0][0] + in1t[1] * in2[1][0] + in1t[2] * in2[2][0];
	out[1] = in1t[0] * in2[0][1] + in1t[1] * in2[1][1] + in1t[2] * in2[2][1];
	out[2] = in1t[0] * in2[0][2] + in1t[1] * in2[1][2] + in1t[2] * in2[2][2];
}

auto AngleNormalize( float angle ) -> float
{
	angle = std::fmod( angle, 360.f );

	if( angle > 180.f )
		angle -= 360.f;
	else if( angle < -180.f )
		angle += 360.f;

	return angle;
}

auto AngleDistance( const QAngle& angles, const Vector& start, const Vector& end ) -> float
{
	auto direction = end - start;
	direction.Normalize();

	auto forward = angles.ToVector();
	auto dot = forward.Dot( direction );

	return ToRadians( std::acos( dot ) );
}


inline Matrix3x3 Matrix3x3::operator-() const
{
	Matrix3x3 ret;
	for (int i = 0; i < 16; i++)
	{
		((float*)ret.m)[i] = ((float*)m)[i];
	}
	return ret;
}

inline const Matrix3x3 & Matrix3x3::operator+=(const Matrix3x3 & other)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			m[i][j] += other.m[i][j];
		}
	}

	return *this;
}

inline Vector Matrix3x3::GetForward() const
{
	return Vector(m[0][0], m[1][0], m[2][0]);
}

inline Vector Matrix3x3::GetLeft() const
{
	return Vector(m[0][1], m[1][1], m[2][1]);
}

inline Vector Matrix3x3::GetUp() const
{
	return Vector(m[0][2], m[1][2], m[2][2]);
}

Matrix3x3 Matrix3x3::operator*(const Matrix3x3 & other) const
{
	Matrix3x3 out = {};
	for (auto i = 0; i < 3; i++)
	{
		for (auto j = 0; j < 3; j++)
		{
			out[i][j] = 0;
			for (auto k = 0; k < 3; k++)
				out[i][j] += m[i][k] * other.m[k][j];
		}
	}
	return out;
}

inline Matrix3x3 Matrix3x3::operator+(const Matrix3x3 & other) const
{
	Matrix3x3 ret;
	for (int i = 0; i < 16; i++)
	{
		((float*)ret.m)[i] = ((float*)m)[i] + ((float*)other.m)[i];
	}
	return ret;
}

D3DVECTOR Matrix3x3::mul(Vertex & scr)
{
	return { m[0][0] * scr.x + m[0][1] * scr.y + m[0][2] * scr.z ,
		m[1][0] * scr.x + m[1][1] * scr.y + m[1][2] * scr.z ,
		m[2][0] * scr.x + m[2][1] * scr.y + m[2][2] * scr.z };
}

D3DVECTOR Matrix3x3::mul(D3DVECTOR & scr)
{
	return { m[0][0] * scr.x + m[0][1] * scr.y + m[0][2] * scr.z ,
		m[1][0] * scr.x + m[1][1] * scr.y + m[1][2] * scr.z ,
		m[2][0] * scr.x + m[2][1] * scr.y + m[2][2] * scr.z };
}

Vector Matrix3x3::mul(Vector & scr)
{
	return { m[0][0] * scr.m_x + m[0][1] * scr.m_y + m[0][2] * scr.m_z ,
		m[1][0] * scr.m_x + m[1][1] * scr.m_y + m[1][2] * scr.m_z ,
		m[2][0] * scr.m_x + m[2][1] * scr.m_y + m[2][2] * scr.m_z };
}

void Matrix3x3::InitRotMatrix(D3DVECTOR VecRot, float deg_rot)
{
	float RadRot = ToRadians(deg_rot);
	auto co = cos(RadRot);
	auto si = sin(RadRot);
	auto leight = std::sqrt(VecRot.x* VecRot.x + VecRot.y * VecRot.y + VecRot.z * VecRot.z);
	VecRot.x /= leight;
	VecRot.y /= leight;
	VecRot.z /= leight;
	m[0][0] = co + ((1.f - co)*(VecRot.x*VecRot.x));
	m[0][1] = ((1.f - co)*VecRot.y*VecRot.x) + (si * VecRot.z);
	m[0][2] = ((1.f - co)*VecRot.z*VecRot.x) - (si* VecRot.y);

	m[1][0] = ((1.f - co)*VecRot.x*VecRot.y) - (si*VecRot.z);
	m[1][1] = co + ((1.f - co)*VecRot.y*VecRot.y);
	m[1][2] = ((1.f - co)*VecRot.z*VecRot.y) + (si*VecRot.x);

	m[2][0] = ((1.f - co)*VecRot.x*VecRot.z) + (si*VecRot.y);
	m[2][1] = ((1.f - co)*VecRot.y*VecRot.z) + (si*VecRot.x);
	m[2][2] = co + ((1.f - co)*VecRot.z*VecRot.z);
}

void Matrix3x3::InitRotMatrix(Vector VecRot, float deg_rot)
{
	float RadRot = ToRadians(deg_rot);
	auto co = cos(RadRot);
	auto si = sin(RadRot);
	VecRot.Normalize();

	m[0][0] = co + ((1.f - co)*(VecRot.m_x*VecRot.m_x));
	m[0][1] = ((1.f - co)*VecRot.m_y*VecRot.m_x) + (si * VecRot.m_z);
	m[0][2] = ((1.f - co)*VecRot.m_z*VecRot.m_x) - (si* VecRot.m_y);

	m[1][0] = ((1.f - co)*VecRot.m_x*VecRot.m_y) - (si*VecRot.m_z);
	m[1][1] = co + ((1.f - co)*VecRot.m_y*VecRot.m_y);
	m[1][2] = ((1.f - co)*VecRot.m_z*VecRot.m_y) + (si*VecRot.m_x);

	m[2][0] = ((1.f - co)*VecRot.m_x*VecRot.m_z) + (si*VecRot.m_y);
	m[2][1] = ((1.f - co)*VecRot.m_y*VecRot.m_z) + (si*VecRot.m_x);
	m[2][2] = co + ((1.f - co)*VecRot.m_z*VecRot.m_z);
}

inline Vector Matrix3x3::VMul4x3Transpose(const Vector & vVec) const
{
	Vector tmp = vVec;
	return Vector(
		m[0][0] * tmp.m_x + m[1][0] * tmp.m_y + m[2][0] * tmp.m_z,
		m[0][1] * tmp.m_x + m[1][1] * tmp.m_y + m[2][1] * tmp.m_z,
		m[0][2] * tmp.m_x + m[1][2] * tmp.m_y + m[2][2] * tmp.m_z
	);
}

void Matrix3x3::InitRotMatrix_X(float deg_rot)
{
	float RadRot = ToRadians(deg_rot);
	auto co = cos(RadRot);
	auto si = sin(RadRot);

	m[0][0] = 1.f;
	m[0][1] = 0.f;
	m[0][2] = 0.f;

	m[1][0] = 0.f;
	m[1][1] = co;
	m[1][2] = si;

	m[2][0] = 0.f;
	m[2][1] = -si;
	m[2][2] = co;
}

void Matrix3x3::InitRotMatrix_Y(float deg_rot)
{
	float RadRot = ToRadians(deg_rot);
	auto co = cos(RadRot);
	auto si = sin(RadRot);

	m[0][0] = co;
	m[0][1] = 0.f;
	m[0][2] = -si;

	m[1][0] = 0.f;
	m[1][1] = 1.f;
	m[1][2] = 0.f;

	m[2][0] = si;
	m[2][1] = 0.f;
	m[2][2] = co;
}

void Matrix3x3::InitRotMatrix_Z(float deg_rot)
{
	float RadRot = ToRadians(deg_rot);
	auto co = cos(RadRot);
	auto si = sin(RadRot);

	m[0][0] = co;
	m[0][1] = si;
	m[0][2] = 0.f;

	m[1][0] = -si;
	m[1][1] = co;
	m[1][2] = 0.f;

	m[2][0] = 0.f;
	m[2][1] = 0.f;
	m[2][2] = 1.f;
}

inline float * Matrix3x3::operator[](int i)
{
	return m[i];
}

inline const float * Matrix3x3::operator[](int i) const
{
	return m[i];
}

Vertex::Vertex(float x, float y, D3DCOLOR col)
{
	this->x = x;
	this->y = y;
	this->col = col;
	z = 1.f;
	ht = 1.f;
}

Vertex::Vertex(float x, float y, float z, float ht, D3DCOLOR col)
{
	this->x = x;
	this->y = y;
	this->col = col;
	this->z = z;
	this->ht = ht;
}
