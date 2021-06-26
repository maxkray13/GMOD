#include "vector4d.hpp"

#include "core.hpp"

Vector4D::Vector4D()
	: m_x( 0.f )
	, m_y( 0.f )
	, m_z( 0.f )
	, m_w( 0.f )
{ }

Vector4D::Vector4D( float x, float y, float z, float w )
	: m_x( x )
	, m_y( y )
	, m_z( z )
	, m_w( w )
{ }

Vector4D::Vector4D( const Vector4D& vector )
	: m_x( vector[ 0 ] )
	, m_y( vector[ 1 ] )
	, m_z( vector[ 2 ] )
	, m_w( vector[ 3 ] )
{ }

Vector4D::Vector4D( const float* vector )
	: m_x( vector[ 0 ] )
	, m_y( vector[ 1 ] )
	, m_z( vector[ 2 ] )
	, m_w( vector[ 3 ] )
{ }

auto Vector4D::Init( float x /* = 0.f */, float y /* = 0.f */, float z /* = 0.f */, float w /*= 0.f*/ ) -> void
{
	m_x = x;
	m_y = y;
	m_z = z;
	m_w = w;
}

auto Vector4D::ToArray() -> float*
{
	return reinterpret_cast< float* >( this );
}

auto Vector4D::ToArray() const -> const float*
{
	return reinterpret_cast< const float* >( this );
}

auto Vector4D::At( unsigned int index ) -> float&
{
	auto data = ToArray();
	return data[ index ];
}

auto Vector4D::At( unsigned int index ) const -> const float
{
	auto data = ToArray();
	return data[ index ];
}

auto Vector4D::IsEqual( const Vector4D& vector ) const -> bool
{
	return ( m_x == vector[ 0 ] &&
			 m_y == vector[ 1 ] &&
			 m_z == vector[ 2 ] &&
			 m_w == vector[ 3 ] );
}

auto Vector4D::Dot( const Vector4D& vector ) const -> float
{
	return ( m_x * vector[ 0 ] +
			 m_y * vector[ 1 ] +
			 m_z * vector[ 2 ] +
			 m_w * vector[ 3 ] );
}

auto Vector4D::LengthSquared() const -> float
{
	return Dot( *this );
}

auto Vector4D::Length() const -> float
{
	return std::sqrt( LengthSquared() );
}

auto Vector4D::Add( const Vector4D& vector ) -> void
{
	m_x += vector[ 0 ];
	m_y += vector[ 1 ];
	m_z += vector[ 2 ];
	m_w += vector[ 3 ];
}

auto Vector4D::Subtract( const Vector4D& vector ) -> void
{
	m_x -= vector[ 0 ];
	m_y -= vector[ 1 ];
	m_z -= vector[ 2 ];
	m_w -= vector[ 3 ];
}

auto Vector4D::Multiply( const Vector4D& vector ) -> void
{
	m_x *= vector[ 0 ];
	m_y *= vector[ 1 ];
	m_z *= vector[ 2 ];
	m_w *= vector[ 3 ];
}

auto Vector4D::Divide( const Vector4D& vector ) -> void
{
	m_x /= vector[ 0 ];
	m_y /= vector[ 1 ];
	m_z /= vector[ 2 ];
	m_w /= vector[ 3 ];
}

auto Vector4D::Add( float value ) -> void
{
	m_x += value;
	m_y += value;
	m_z += value;
	m_w += value;
}

auto Vector4D::Subtract( float value ) -> void
{
	m_x -= value;
	m_y -= value;
	m_z -= value;
	m_w -= value;
}

auto Vector4D::Multiply( float value ) -> void
{
	m_x *= value;
	m_y *= value;
	m_z *= value;
	m_w *= value;
}

auto Vector4D::Divide( float value ) -> void
{
	m_x /= value;
	m_y /= value;
	m_z /= value;
	m_w /= value;
}

auto Vector4D::DistanceSquared( const Vector4D& vector ) const -> float
{
	auto data = *this;
	data.Subtract( vector );
	return data.LengthSquared();
}

auto Vector4D::Distance( const Vector4D& vector ) const -> float
{
	auto data = *this;
	data.Subtract( vector );
	return data.Length();
}

auto Vector4D::Normalize() -> float
{
	auto length = Length();
	auto length_normal = 1.f / length;

	m_x *= length_normal;
	m_y *= length_normal;
	m_z *= length_normal;
	m_w *= length_normal;

	return length;
}

auto Vector4D::NormalizeFast() -> void
{
	auto length = Length();
	auto length_normal = 1.f / length;

	m_x *= length_normal;
	m_y *= length_normal;
	m_z *= length_normal;
	m_w *= length_normal;
}


auto Vector4D::operator = ( const Vector4D& vector ) -> Vector4D&
{
	Init( vector[ 0 ], vector[ 1 ], vector[ 2 ], vector[ 3 ] );
	return *this;
}

auto Vector4D::operator [] ( unsigned int index ) -> float&
{
	return At( index );
}

auto Vector4D::operator [] ( unsigned int index ) const -> const float
{
	return At( index );
}

auto Vector4D::operator == ( const Vector4D& vector ) const -> bool
{
	return IsEqual( vector );
}

auto Vector4D::operator != ( const Vector4D& vector ) const -> bool
{
	return !IsEqual( vector );
}

auto Vector4D::operator += ( const Vector4D& vector ) -> Vector4D&
{
	Add( vector );
	return *this;
}

auto Vector4D::operator -= ( const Vector4D& vector ) -> Vector4D&
{
	Subtract( vector );
	return *this;
}

auto Vector4D::operator *= ( const Vector4D& vector ) -> Vector4D&
{
	Multiply( vector );
	return *this;
}

auto Vector4D::operator /= ( const Vector4D& vector ) -> Vector4D&
{
	Divide( vector );
	return *this;
}

auto Vector4D::operator += ( float value ) -> Vector4D&
{
	Add( value );
	return *this;
}

auto Vector4D::operator -= ( float value ) -> Vector4D&
{
	Subtract( value );
	return *this;
}

auto Vector4D::operator *= ( float value ) -> Vector4D&
{
	Multiply( value );
	return *this;
}

auto Vector4D::operator /= ( float value ) -> Vector4D&
{
	Divide( value );
	return *this;
}

auto Vector4D::operator + ( const Vector4D& vector ) const -> Vector4D
{
	auto data = *this;
	data.Add( vector );
	return data;
}

auto Vector4D::operator - ( const Vector4D& vector ) const -> Vector4D
{
	auto data = *this;
	data.Subtract( vector );
	return data;
}

auto Vector4D::operator * ( const Vector4D& vector ) const -> Vector4D
{
	auto data = *this;
	data.Multiply( vector );
	return data;
}

auto Vector4D::operator / ( const Vector4D& vector ) const -> Vector4D
{
	auto data = *this;
	data.Divide( vector );
	return data;
}

auto Vector4D::operator + ( float value ) const -> Vector4D
{
	auto data = *this;
	data.Add( value );
	return data;
}

auto Vector4D::operator - ( float value ) const -> Vector4D
{
	auto data = *this;
	data.Subtract( value );
	return data;
}

auto Vector4D::operator * ( float value ) const -> Vector4D
{
	auto data = *this;
	data.Multiply( value );
	return data;
}

auto Vector4D::operator / ( float value ) const -> Vector4D
{
	auto data = *this;
	data.Divide( value );
	return data;
}


Vector4D Vector4D::Zero = { 0.f, 0.f, 0.f, 0.f };