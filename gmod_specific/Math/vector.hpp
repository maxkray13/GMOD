#pragma once


struct matrix3x4_t;

class QAngle;
class Vector4D;

class Vector
{
public:
	Vector() = default;
	Vector( float x, float y, float z );
	Vector( const Vector& vector );
	Vector( const float* vector );

	auto Init( float x = 0.f, float y = 0.f, float z = 0.f ) -> void;

	auto ToArray() -> float*;
	auto ToArray() const -> const float*;

	auto At( unsigned int index ) -> float&;
	auto At( unsigned int index ) const -> const float;

	auto IsEqual( const Vector& vector ) const -> bool;
	
	auto Add( const Vector& vector ) -> void;
	auto Subtract( const Vector& vector ) -> void;
	auto Multiply( const Vector& vector ) -> void;
	auto Divide( const Vector& vector ) -> void;

	auto Add( float value ) -> void;
	auto Subtract( float value ) -> void;
	auto Multiply( float value ) -> void;
	auto Divide( float value ) -> void;

	auto Dot( const Vector& vector ) const -> float;

	auto LengthSquared() const -> float;
	auto Length() const -> float;

	auto Length2DSquared() const -> float;
	auto Length2D() const -> float;

	auto DistanceSquared( const Vector& vector ) const -> float;
	auto Distance( const Vector& vector ) const -> float;

	auto Normalize() -> float;
	auto NormalizeFast() -> void;

	auto Transform( const matrix3x4_t& transform ) const -> Vector;

	auto IsZero() const -> bool;

	auto Cross( const Vector& vector ) -> Vector;

	auto ToEulerAngles( Vector* pseudoup = nullptr ) -> QAngle;
	auto ToVector4D( float w = 0.f ) const -> Vector4D;

public:
	auto operator = ( const Vector& vector ) -> Vector&;

	auto operator [] ( unsigned int index ) -> float&;
	auto operator [] ( unsigned int index ) const -> const float;

	auto operator == ( const Vector& vector ) const -> bool;
	auto operator != ( const Vector& vector ) const -> bool;

public:
	auto operator += ( const Vector& vector ) -> Vector&;
	auto operator -= ( const Vector& vector ) -> Vector&;
	auto operator *= ( const Vector& vector ) -> Vector&;
	auto operator /= ( const Vector& vector ) -> Vector&;

	auto operator += ( float value ) -> Vector&;
	auto operator -= ( float value ) -> Vector&;
	auto operator *= ( float value ) -> Vector&;
	auto operator /= ( float value ) -> Vector&;

	auto operator + ( const Vector& vector ) const -> Vector;
	auto operator - ( const Vector& vector ) const -> Vector;
	auto operator * ( const Vector& vector ) const -> Vector;
	auto operator / ( const Vector& vector ) const -> Vector;

	auto operator + ( float value ) const -> Vector;
	auto operator - ( float value ) const -> Vector;
	auto operator * ( float value ) const -> Vector;
	auto operator / ( float value ) const -> Vector;

	Vector	operator-(void) const;

public:
	static Vector Zero;

//private:
	float m_x = 0.f;
	float m_y = 0.f;
	float m_z = 0.f;
};

inline auto operator * ( float value, const Vector& vector ) -> Vector
{
	return ( value * vector );
}