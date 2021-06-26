#include "qangle.hpp"
#include "vector.hpp"

#include "core.hpp"

//#include <boost/algorithm/clamp.hpp>


QAngle::QAngle( float x, float y, float z )
	: m_x( x )
	, m_y( y )
	, m_z( z )
{ }

QAngle::QAngle( const QAngle& angles )
	: m_x( angles[ 0 ] )
	, m_y( angles[ 1 ] )
	, m_z( angles[ 2 ] )
{ }

QAngle::QAngle( const float* angles )
	: m_x( angles[ 0 ] )
	, m_y( angles[ 1 ] )
	, m_z( angles[ 2 ] )
{ }

auto QAngle::Init( float x /* = 0.f */, float y /* = 0.f */, float z /* = 0.f */ ) -> void
{
	m_x = x;
	m_y = y;
	m_z = z;
}

auto QAngle::ToArray() -> float*
{
	return reinterpret_cast< float* >( this );
}

auto QAngle::ToArray() const -> const float*
{
	return reinterpret_cast< const float* >( this );
}

auto QAngle::At( unsigned int index ) -> float&
{
	auto data = ToArray();
	return data[ index ];
}

auto QAngle::At( unsigned int index ) const -> const float
{
	auto data = ToArray();
	return data[ index ];
}

auto QAngle::IsEqual( const QAngle& angles ) const -> bool
{
	return ( m_x == angles[ 0 ] &&
			 m_y == angles[ 1 ] &&
			 m_z == angles[ 2 ] );
}

auto QAngle::Negate() -> void
{
	m_x = -m_x;
	m_y = -m_y;
	m_z = -m_z;
}

auto QAngle::Dot( const QAngle& angles ) const -> float
{
	return ( m_x * angles[ 0 ] +
			 m_y * angles[ 1 ] +
			 m_z * angles[ 2 ] );
}

auto QAngle::LengthSquared() const -> float
{
	return Dot( *this );
}

auto QAngle::Length() const -> float
{
	return std::sqrt( LengthSquared() );
}

auto QAngle::Add( const QAngle& angles ) -> void
{
	m_x += angles[ 0 ];
	m_y += angles[ 1 ];
	m_z += angles[ 2 ];
}

auto QAngle::Subtract( const QAngle& angles ) -> void
{
	m_x -= angles[ 0 ];
	m_y -= angles[ 1 ];
	m_z -= angles[ 2 ];
}

auto QAngle::Multiply( const QAngle& angles ) -> void
{
	m_x *= angles[ 0 ];
	m_y *= angles[ 1 ];
	m_z *= angles[ 2 ];
}

auto QAngle::Divide( const QAngle& angles ) -> void
{
	m_x /= angles[ 0 ];
	m_y /= angles[ 1 ];
	m_z /= angles[ 2 ];
}

auto QAngle::Add( float value ) -> void
{
	m_x += value;
	m_y += value;
	m_z += value;
}

auto QAngle::Subtract( float value ) -> void
{
	m_x -= value;
	m_y -= value;
	m_z -= value;
}

auto QAngle::Multiply( float value ) -> void
{
	m_x *= value;
	m_y *= value;
	m_z *= value;
}

auto QAngle::Divide( float value ) -> void
{
	m_x /= value;
	m_y /= value;
	m_z /= value;
}

auto QAngle::DistanceSquared( const QAngle& angles ) const -> float
{
	auto data = *this;
	data.Subtract( angles );
	return data.LengthSquared();
}

auto QAngle::Distance( const QAngle& angles ) const -> float
{
	auto data = *this;
	data.Subtract( angles );
	return data.Length();
}
// up / down
#define	PITCH	0
// left / right
#define	YAW		1
// tangage
#define	ROLL	2
auto QAngle::Normalize() -> void
{
	/*m_y = AngleNormalize(m_y);
	auto AngleNormalize90 = [](float angle) -> float
	{
		angle = std::fmod(angle, 180.f);

		if (angle > 90.f)
			angle -= 180.f;
		else if (angle < -90.f)
			angle += 180.f;

		return angle;
	};
	m_x = AngleNormalize90(m_x);
	m_x = std::clamp(m_x, -89.f, 89.f);
	m_z = 0.f;*/

	while (m_x > 89.0f)
		m_x -= 180.f;

	while (m_x < -89.0f)
		m_x += 180.f;

	while (m_y > 180.f)
		m_y -= 360.f;

	while (m_y < -180.f)
		m_y += 360.f;

	m_z = 0.f;

	/*int RemainderYaw = (int)m_y / 360;
	float ExcessYaw = RemainderYaw * 360.f;
	float Yaw = m_y - ExcessYaw;
	if (Yaw > 180.f) {
		Yaw -= 360.f;
	}
	if (Yaw < -180.f) {
		Yaw += 360.f;
	}

	int RemainderP = (int)m_x / 360;
	float ExcessP = RemainderP * 360.f;
	float Pitch = m_x - ExcessP;

	if (Pitch > 180.f) {
		Pitch -= 360.f;
	}
	else if (Pitch < -180.f) {
		Pitch += 360.f;
	}
	if (Pitch > 90.f) {
		Pitch -= 180.f;
	}
	else if (Pitch < -90.f) {
		Pitch += 180.f;
	}
	if (Pitch > 89.f) {
		Pitch = 89.f;
	}
	else if (Pitch < -89.f) {
		Pitch = -89.f;
	}
	m_y = Yaw;
	m_x = Pitch;
	m_z = 0.f;

	return;*/
}

auto QAngle::Clamp() -> void
{
	m_x = std::clamp( m_x, -89.f, 89.f );
	m_y = std::clamp( m_y, -180.f, 180.f );
	m_z = 0.f;
}

auto QAngle::ToVector(Vector* forward, Vector* side , Vector* up ) const -> Vector
{
	auto rad_pitch = ToRadians( m_x );
	auto rad_yaw = ToRadians( m_y );
	auto rad_roll = ToRadians( m_z );

	auto sin_pitch = std::sin( rad_pitch );
	auto sin_yaw = std::sin( rad_yaw );
	auto sin_roll = std::sin( rad_roll );

	auto cos_pitch = std::cos( rad_pitch );
	auto cos_yaw = std::cos( rad_yaw );
	auto cos_roll = std::cos( rad_roll );
	if ( forward ) 
	{
		 forward->At(0) = cos_pitch * cos_yaw;
		 forward->At(1) = cos_pitch * sin_yaw;
		 forward->At(2) = -sin_pitch;
	}

	if( side )
	{
		side->At( 0 ) = -1.0f * sin_roll * sin_pitch * cos_yaw + -1.0f * cos_roll * -sin_yaw;
		side->At( 1 ) = -1.0f * sin_roll * sin_pitch * sin_yaw + -1.0f * cos_roll * cos_yaw;
		side->At( 2 ) = -1.0f * sin_roll * cos_pitch;
	}

	if( up )
	{
		up->At( 0 ) = cos_roll * sin_pitch * cos_yaw + -sin_roll * -sin_yaw;
		up->At( 1 ) = cos_roll * sin_pitch * sin_yaw + -sin_roll * cos_yaw;
		up->At( 2 ) = cos_roll * cos_pitch;
	}

	return { cos_pitch * cos_yaw, cos_pitch * sin_yaw, -sin_pitch };
}

auto QAngle::ToVectorTranspose( Vector* side /*= nullptr*/, Vector* up /*= nullptr*/ ) const -> Vector
{
	auto rad_pitch = ToRadians( m_x );
	auto rad_yaw = ToRadians( m_y );
	auto rad_roll = ToRadians( m_z );

	auto sin_pitch = std::sin( rad_pitch );
	auto sin_yaw = std::sin( rad_yaw );
	auto sin_roll = std::sin( rad_roll );

	auto cos_pitch = std::cos( rad_pitch );
	auto cos_yaw = std::cos( rad_yaw );
	auto cos_roll = std::cos( rad_roll );

	if( side )
	{
		side->At( 0 ) = cos_pitch * sin_yaw;
		side->At( 1 ) = sin_roll * sin_pitch * sin_yaw + cos_roll * cos_yaw;
		side->At( 2 ) = cos_roll * sin_pitch * sin_yaw + -sin_roll * cos_yaw;
	}

	if( up )
	{
		up->At( 0 ) = -sin_pitch;
		up->At( 1 ) = sin_roll * cos_pitch;
		up->At( 2 ) = cos_roll * cos_pitch;
	}

	return { cos_pitch * cos_yaw,
			 sin_roll * sin_pitch * cos_yaw + cos_roll * -sin_yaw,
			 cos_roll * sin_pitch * cos_yaw + -sin_roll * -sin_yaw };
}


auto QAngle::operator = ( const QAngle& angles ) -> QAngle&
{
	Init( angles[ 0 ], angles[ 1 ], angles[ 2 ] );
	return *this;
}

auto QAngle::operator [] ( unsigned int index ) -> float&
{
	return At( index );
}

auto QAngle::operator [] ( unsigned int index ) const -> const float
{
	return At( index );
}

auto QAngle::operator == ( const QAngle& angles ) const -> bool
{
	return IsEqual( angles );
}

auto QAngle::operator != ( const QAngle& angles ) const -> bool
{
	return !IsEqual( angles );
}

auto QAngle::operator += ( const QAngle& angles ) -> QAngle&
{
	Add( angles );
	return *this;
}

auto QAngle::operator -= ( const QAngle& angles ) -> QAngle&
{
	Subtract( angles );
	return *this;
}

auto QAngle::operator *= ( const QAngle& angles ) -> QAngle&
{
	Multiply( angles );
	return *this;
}

auto QAngle::operator /= ( const QAngle& angles ) -> QAngle&
{
	Divide( angles );
	return *this;
}

auto QAngle::operator += ( float value ) -> QAngle&
{
	Add( value );
	return *this;
}

auto QAngle::operator -= ( float value ) -> QAngle&
{
	Subtract( value );
	return *this;
}

auto QAngle::operator *= ( float value ) -> QAngle&
{
	Multiply( value );
	return *this;
}

auto QAngle::operator /= ( float value ) -> QAngle&
{
	Divide( value );
	return *this;
}

auto QAngle::operator + ( const QAngle& angles ) const -> QAngle
{
	auto data = *this;
	data.Add( angles );
	return data;
}

auto QAngle::operator - ( const QAngle& angles ) const -> QAngle
{
	auto data = *this;
	data.Subtract( angles );
	return data;
}

auto QAngle::operator * ( const QAngle& angles ) const -> QAngle
{
	auto data = *this;
	data.Multiply( angles );
	return data;
}

auto QAngle::operator / ( const QAngle& angles ) const -> QAngle
{
	auto data = *this;
	data.Divide( angles );
	return data;
}

auto QAngle::operator + ( float value ) const -> QAngle
{
	auto data = *this;
	data.Add( value );
	return data;
}

auto QAngle::operator - ( float value ) const -> QAngle
{
	auto data = *this;
	data.Subtract( value );
	return data;
}

auto QAngle::operator * ( float value ) const -> QAngle
{
	auto data = *this;
	data.Multiply( value );
	return data;
}

auto QAngle::operator / ( float value ) const -> QAngle
{
	auto data = *this;
	data.Divide( value );
	return data;
}

QAngle QAngle::operator-(void) const
{
	return QAngle(-m_x, -m_y, -m_z);
}


QAngle QAngle::Zero = { 0.f, 0.f, 0.f };