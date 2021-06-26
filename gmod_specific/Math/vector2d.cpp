#include "vector2d.hpp"

#include "core.hpp"

Vector2D::Vector2D()
	: m_x( 0.f )
	, m_y( 0.f )
{ }

Vector2D::Vector2D( float x, float y )
	: m_x( x )
	, m_y( y )
{ }

Vector2D::Vector2D( const Vector2D& vector )
	: m_x( vector[ 0 ] )
	, m_y( vector[ 1 ] )
{ }

Vector2D::Vector2D( const float* vector )
	: m_x( vector[ 0 ] )
	, m_y( vector[ 1 ] )
{ }

auto Vector2D::Init( float x /* = 0.f */, float y /* = 0.f */ ) -> void
{
	m_x = x;
	m_y = y;
}

auto Vector2D::ToArray() -> float*
{
	return reinterpret_cast< float* >( this );
}

auto Vector2D::ToArray() const -> const float*
{
	return reinterpret_cast< const float* >( this );
}

auto Vector2D::At( unsigned int index ) -> float&
{
	auto data = ToArray();
	return data[ index ];
}

auto Vector2D::At( unsigned int index ) const -> const float
{
	auto data = ToArray();
	return data[ index ];
}

auto Vector2D::IsEqual( const Vector2D& vector ) const -> bool
{
	return ( m_x == vector[ 0 ] &&
			 m_y == vector[ 1 ] );
}

auto Vector2D::Negate() -> void
{
	m_x = -m_x;
	m_y = -m_y;
}

auto Vector2D::Dot( const Vector2D& vector ) const -> float
{
	return ( m_x * vector[ 0 ] +
			 m_y * vector[ 1 ] );
}

auto Vector2D::LengthSquared() const -> float
{
	return Dot( *this );
}

auto Vector2D::Length() const -> float
{
	return std::sqrt( LengthSquared() );
}

auto Vector2D::Add( const Vector2D& vector ) -> void
{
	m_x += vector[ 0 ];
	m_y += vector[ 1 ];
}

auto Vector2D::Subtract( const Vector2D& vector ) -> void
{
	m_x -= vector[ 0 ];
	m_y -= vector[ 1 ];
}

auto Vector2D::Multiply( const Vector2D& vector ) -> void
{
	m_x *= vector[ 0 ];
	m_y *= vector[ 1 ];
}

auto Vector2D::Divide( const Vector2D& vector ) -> void
{
	m_x /= vector[ 0 ];
	m_y /= vector[ 1 ];
}

auto Vector2D::Add( float value ) -> void
{
	m_x += value;
	m_y += value;
}

auto Vector2D::Subtract( float value ) -> void
{
	m_x -= value;
	m_y -= value;
}

auto Vector2D::Multiply( float value ) -> void
{
	m_x *= value;
	m_y *= value;
}

auto Vector2D::Divide( float value ) -> void
{
	m_x /= value;
	m_y /= value;
}

auto Vector2D::DistanceSquared( const Vector2D& vector ) const -> float
{
	auto data = *this;
	data.Subtract( vector );
	return data.LengthSquared();
}

auto Vector2D::Distance( const Vector2D& vector ) const -> float
{
	auto data = *this;
	data.Subtract( vector );
	return data.Length();
}

auto Vector2D::Normalize() -> float
{
	auto length = Length();
	auto length_normal = 1.f / length;

	m_x *= length_normal;
	m_y *= length_normal;

	return length;
}

auto Vector2D::NormalizeFast() -> void
{
	auto length = Length();
	auto length_normal = 1.f / length;

	m_x *= length_normal;
	m_y *= length_normal;
}


auto Vector2D::operator = ( const Vector2D& vector ) -> Vector2D&
{
	Init( vector[ 0 ], vector[ 1 ] );
	return *this;
}

auto Vector2D::operator [] ( unsigned int index ) -> float&
{
	return At( index );
}

auto Vector2D::operator [] ( unsigned int index ) const -> const float
{
	return At( index );
}

auto Vector2D::operator == ( const Vector2D& vector ) const -> bool
{
	return IsEqual( vector );
}

auto Vector2D::operator != ( const Vector2D& vector ) const -> bool
{
	return !IsEqual( vector );
}

auto Vector2D::operator += ( const Vector2D& vector ) -> Vector2D&
{
	Add( vector );
	return *this;
}

auto Vector2D::operator -= ( const Vector2D& vector ) -> Vector2D&
{
	Subtract( vector );
	return *this;
}

auto Vector2D::operator *= ( const Vector2D& vector ) -> Vector2D&
{
	Multiply( vector );
	return *this;
}

auto Vector2D::operator /= ( const Vector2D& vector ) -> Vector2D&
{
	Divide( vector );
	return *this;
}

auto Vector2D::operator += ( float value ) -> Vector2D&
{
	Add( value );
	return *this;
}

auto Vector2D::operator -= ( float value ) -> Vector2D&
{
	Subtract( value );
	return *this;
}

auto Vector2D::operator *= ( float value ) -> Vector2D&
{
	Multiply( value );
	return *this;
}

auto Vector2D::operator /= ( float value ) -> Vector2D&
{
	Divide( value );
	return *this;
}

auto Vector2D::operator + ( const Vector2D& vector ) const -> Vector2D
{
	auto data = *this;
	data.Add( vector );
	return data;
}

auto Vector2D::operator - ( const Vector2D& vector ) const -> Vector2D
{
	auto data = *this;
	data.Subtract( vector );
	return data;
}

auto Vector2D::operator * ( const Vector2D& vector ) const -> Vector2D
{
	auto data = *this;
	data.Multiply( vector );
	return data;
}

auto Vector2D::operator / ( const Vector2D& vector ) const -> Vector2D
{
	auto data = *this;
	data.Divide( vector );
	return data;
}

auto Vector2D::operator + ( float value ) const -> Vector2D
{
	auto data = *this;
	data.Add( value );
	return data;
}

auto Vector2D::operator - ( float value ) const -> Vector2D
{
	auto data = *this;
	data.Subtract( value );
	return data;
}

auto Vector2D::operator * ( float value ) const -> Vector2D
{
	auto data = *this;
	data.Multiply( value );
	return data;
}

auto Vector2D::operator / ( float value ) const -> Vector2D
{
	auto data = *this;
	data.Divide( value );
	return data;
}


Vector2D Vector2D::Zero = { 0.f, 0.f };