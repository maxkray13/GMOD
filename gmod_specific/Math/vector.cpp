#include "vector.hpp"
#include "qangle.hpp"
#include "vector4d.hpp"

#include "core.hpp"



Vector::Vector( float x, float y, float z )
	: m_x( x )
	, m_y( y )
	, m_z( z )
{ }

Vector::Vector( const Vector& vector )
	: m_x( vector[ 0 ] )
	, m_y( vector[ 1 ] )
	, m_z( vector[ 2 ] )
{ }

Vector::Vector( const float* vector )
	: m_x( vector[ 0 ] )
	, m_y( vector[ 1 ] )
	, m_z( vector[ 2 ] )
{ }

auto Vector::Init( float x /* = 0.f */, float y /* = 0.f */, float z /* = 0.f */ ) -> void
{
	m_x = x;
	m_y = y;
	m_z = z;
}

auto Vector::ToArray() -> float*
{
	return reinterpret_cast< float* >( this );
}

auto Vector::ToArray() const -> const float*
{
	return reinterpret_cast< const float* >( this );
}

auto Vector::At( unsigned int index ) -> float&
{
	auto data = ToArray();
	return data[ index ];
}

auto Vector::At( unsigned int index ) const -> const float
{
	auto data = ToArray();
	return data[ index ];
}

auto Vector::IsEqual( const Vector& vector ) const -> bool
{
	return ( m_x == vector[ 0 ] &&
			 m_y == vector[ 1 ] &&
			 m_z == vector[ 2 ] );
}

auto Vector::Dot( const Vector& vector ) const -> float
{
	return ( m_x * vector[ 0 ] +
			 m_y * vector[ 1 ] +
			 m_z * vector[ 2 ] );
}

auto Vector::LengthSquared() const -> float
{
	return Dot( *this );
}

auto Vector::Length() const -> float
{
	return std::sqrt( LengthSquared() );
}

auto Vector::Length2DSquared() const -> float
{
	return ( m_x * m_x +
			 m_y * m_y );
}

auto Vector::Length2D() const -> float
{
	return std::sqrt( Length2DSquared() );
}

auto Vector::Add( const Vector& vector ) -> void
{
	m_x += vector[ 0 ];
	m_y += vector[ 1 ];
	m_z += vector[ 2 ];
}

auto Vector::Subtract( const Vector& vector ) -> void
{
	m_x -= vector[ 0 ];
	m_y -= vector[ 1 ];
	m_z -= vector[ 2 ];
}

auto Vector::Multiply( const Vector& vector ) -> void
{
	m_x *= vector[ 0 ];
	m_y *= vector[ 1 ];
	m_z *= vector[ 2 ];
}

auto Vector::Divide( const Vector& vector ) -> void
{
	m_x /= vector[ 0 ];
	m_y /= vector[ 1 ];
	m_z /= vector[ 2 ];
}

auto Vector::Add( float value ) -> void
{
	m_x += value;
	m_y += value;
	m_z += value;
}

auto Vector::Subtract( float value ) -> void
{
	m_x -= value;
	m_y -= value;
	m_z -= value;
}

auto Vector::Multiply( float value ) -> void
{
	m_x *= value;
	m_y *= value;
	m_z *= value;
}

auto Vector::Divide( float value ) -> void
{
	m_x /= value;
	m_y /= value;
	m_z /= value;
}

auto Vector::DistanceSquared( const Vector& vector ) const -> float
{
	auto data = *this;
	data.Subtract( vector );
	return data.LengthSquared();
}

auto Vector::Distance( const Vector& vector ) const -> float
{
	auto data = *this;
	data.Subtract( vector );
	return data.Length();
}

auto Vector::Normalize() -> float
{
	auto length = Length();
	auto length_normal = 1.f / length;

	m_x *= length_normal;
	m_y *= length_normal;
	m_z *= length_normal;

	return length;
}

auto Vector::NormalizeFast() -> void
{
	auto length = Length();
	auto length_normal = 1.f / length;

	m_x *= length_normal;
	m_y *= length_normal;
	m_z *= length_normal;
}

auto Vector::Transform( const matrix3x4_t& transform ) const -> Vector
{
	return { Dot( transform[ 0 ] ) + transform[ 0 ][ 3 ],
			 Dot( transform[ 1 ] ) + transform[ 1 ][ 3 ],
			 Dot( transform[ 2 ] ) + transform[ 2 ][ 3 ] };
}

auto Vector::IsZero() const -> bool
{
	return IsEqual( Vector::Zero );
}

auto Vector::Cross( const Vector& vector ) -> Vector
{
	return { m_y * vector[ 2 ] - m_z * vector[ 1 ],
			 m_z * vector[ 0 ] - m_x * vector[ 2 ],
			 m_x * vector[ 1 ] - m_y * vector[ 0 ] };
}

auto Vector::ToEulerAngles( Vector* pseudoup /*= nullptr*/ ) -> QAngle
{
	auto forward = *this;

	auto pitch = 0.f;
	auto yaw = 0.f;
	auto roll = 0.f;

	auto length = forward.Length2D();

	if( pseudoup )
	{
		auto left = pseudoup->Cross( *this );

		left.Normalize();

		pitch = ToDegrees( std::atan2( -forward[ 2 ], length ) );

		if( pitch < 0.f )
			pitch += 360.f;

		if( length > 0.001f )
		{
			yaw = ToDegrees( std::atan2( forward[ 1 ], forward[ 0 ] ) );

			if( yaw < 0.f )
				yaw += 360.f;

			auto up_z = ( forward[ 0 ] * left[ 1 ] ) - ( forward[ 1 ] * left[ 0 ] );

			roll = ToDegrees( std::atan2( left[ 2 ], up_z ) );

			if( roll < 0.f )
				roll += 360.f;
		}
		else
		{
			yaw = ToDegrees( std::atan2( -left[ 0 ], left[ 1 ] ) );

			if( yaw < 0.f )
				yaw += 360.f;
		}
	}
	else
	{
		if( forward[ 0 ] == 0.f && forward[ 1 ] == 0.f )
		{
			if( forward[ 2 ] > 0.f )
				pitch = 270.f;
			else
				pitch = 90.f;
		}
		else
		{
			pitch = ToDegrees( std::atan2( -forward[ 2 ], length ) );

			if( pitch < 0.f )
				pitch += 360.f;

			yaw = ToDegrees( std::atan2( forward[ 1 ], forward[ 0 ] ) );

			if( yaw < 0.f )
				yaw += 360.f;
		}
	}

	return { pitch, yaw, roll };
}

auto Vector::ToVector4D( float w /*= 0.f*/ ) const -> Vector4D
{
	return { m_x, m_y, m_z, w };
}


auto Vector::operator = ( const Vector& vector ) -> Vector&
{
	Init( vector[ 0 ], vector[ 1 ], vector[ 2 ] );
	return *this;
}

auto Vector::operator [] ( unsigned int index ) -> float&
{
	return At( index );
}

auto Vector::operator [] ( unsigned int index ) const -> const float
{
	return At( index );
}

auto Vector::operator == ( const Vector& vector ) const -> bool
{
	return IsEqual( vector );
}

auto Vector::operator != ( const Vector& vector ) const -> bool
{
	return !IsEqual( vector );
}

auto Vector::operator += ( const Vector& vector ) -> Vector&
{
	Add( vector );
	return *this;
}

auto Vector::operator -= ( const Vector& vector ) -> Vector&
{
	Subtract( vector );
	return *this;
}

auto Vector::operator *= ( const Vector& vector ) -> Vector&
{
	Multiply( vector );
	return *this;
}

auto Vector::operator /= ( const Vector& vector ) -> Vector&
{
	Divide( vector );
	return *this;
}

auto Vector::operator += ( float value ) -> Vector&
{
	Add( value );
	return *this;
}

auto Vector::operator -= ( float value ) -> Vector&
{
	Subtract( value );
	return *this;
}

auto Vector::operator *= ( float value ) -> Vector&
{
	Multiply( value );
	return *this;
}

auto Vector::operator /= ( float value ) -> Vector&
{
	Divide( value );
	return *this;
}

auto Vector::operator + ( const Vector& vector ) const -> Vector
{
	auto data = *this;
	data.Add( vector );
	return data;
}

auto Vector::operator - ( const Vector& vector ) const -> Vector
{
	auto data = *this;
	data.Subtract( vector );
	return data;
}

auto Vector::operator * ( const Vector& vector ) const -> Vector
{
	auto data = *this;
	data.Multiply( vector );
	return data;
}

auto Vector::operator / ( const Vector& vector ) const -> Vector
{
	auto data = *this;
	data.Divide( vector );
	return data;
}

auto Vector::operator + ( float value ) const -> Vector
{
	auto data = *this;
	data.Add( value );
	return data;
}

auto Vector::operator - ( float value ) const -> Vector
{
	auto data = *this;
	data.Subtract( value );
	return data;
}

auto Vector::operator * ( float value ) const -> Vector
{
	auto data = *this;
	data.Multiply( value );
	return data;
}

auto Vector::operator / ( float value ) const -> Vector
{
	auto data = *this;
	data.Divide( value );
	return data;
}

Vector Vector::operator-(void) const
{
	return Vector(-m_x, -m_y, -m_z);
}


Vector Vector::Zero = { 0.f, 0.f, 0.f };