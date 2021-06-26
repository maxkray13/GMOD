#pragma once


class Vector2D
{
public:
	Vector2D();
	Vector2D( float x, float y );
	Vector2D( const Vector2D& vector );
	Vector2D( const float* vector );

	auto Init( float x = 0.f, float y = 0.f ) -> void;

	auto ToArray() -> float*;
	auto ToArray() const -> const float*;

	auto At( unsigned int index ) -> float&;
	auto At( unsigned int index ) const -> const float;

	auto IsEqual( const Vector2D& vector ) const -> bool;

	auto Add( const Vector2D& vector ) -> void;
	auto Subtract( const Vector2D& vector ) -> void;
	auto Multiply( const Vector2D& vector ) -> void;
	auto Divide( const Vector2D& vector ) -> void;

	auto Add( float value ) -> void;
	auto Subtract( float value ) -> void;
	auto Multiply( float value ) -> void;
	auto Divide( float value ) -> void;

	auto Negate() -> void;

	auto Dot( const Vector2D& vector ) const -> float;

	auto LengthSquared() const -> float;
	auto Length() const -> float;

	auto DistanceSquared( const Vector2D& vector ) const -> float;
	auto Distance( const Vector2D& vector ) const -> float;

	auto Normalize() -> float;
	auto NormalizeFast() -> void;

public:
	auto operator = ( const Vector2D& vector )->Vector2D&;

	auto operator [] ( unsigned int index ) -> float&;
	auto operator [] ( unsigned int index ) const -> const float;

	auto operator == ( const Vector2D& vector ) const -> bool;
	auto operator != ( const Vector2D& vector ) const -> bool;

public:
	auto operator += ( const Vector2D& vector )->Vector2D&;
	auto operator -= ( const Vector2D& vector )->Vector2D&;
	auto operator *= ( const Vector2D& vector )->Vector2D&;
	auto operator /= ( const Vector2D& vector )->Vector2D&;

	auto operator += ( float value )->Vector2D&;
	auto operator -= ( float value )->Vector2D&;
	auto operator *= ( float value )->Vector2D&;
	auto operator /= ( float value )->Vector2D&;

	auto operator + ( const Vector2D& vector ) const->Vector2D;
	auto operator - ( const Vector2D& vector ) const->Vector2D;
	auto operator * ( const Vector2D& vector ) const->Vector2D;
	auto operator / ( const Vector2D& vector ) const->Vector2D;

	auto operator + ( float value ) const->Vector2D;
	auto operator - ( float value ) const->Vector2D;
	auto operator * ( float value ) const->Vector2D;
	auto operator / ( float value ) const->Vector2D;

public:
	static Vector2D Zero;

//private:
	float m_x = 0.f;
	float m_y = 0.f;
};

inline auto operator * ( float value, const Vector2D& vector ) -> Vector2D
{
	return ( value * vector );
}