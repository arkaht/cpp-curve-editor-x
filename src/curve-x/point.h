#pragma once

#include <sstream>

#include "utils.h"

namespace curve_x
{
	enum class PointSpace
	{
		/*
		 * Relative to another point, used for tangent points
		 */
		Local,

		/*
		 * Global point, used for control points
		 */
		Global,
	};

	struct Point
	{
	public:
		float x, y;

	public:
		Point()
			: x( 0.0f ), y( 0.0f ) {}
		Point( float x, float y )
			: x( x ), y( y ) {}

#ifdef RAYLIB_H
		Point( const Vector2& vec )
			: x( vec.x ), y( vec.y ) {}
		operator Vector2() const { return Vector2 { x, y }; }
#endif

		Point operator+( const Point& point ) const
		{
			return {
				x + point.x,
				y + point.y
			};
		}
		Point operator-( const Point& point ) const
		{
			return {
				x - point.x,
				y - point.y
			};
		}
		Point operator*( float value ) const
		{
			return {
				x * value,
				y * value
			};
		}
		Point operator/( float value ) const
		{
			return {
				x / value,
				y / value
			};
		}

		Point operator-() const
		{
			return {
				-x,
				-y
			};
		}

		std::string str() const
		{
			return "x=" + std::to_string( x ) + ";" 
				 + "y=" + std::to_string( y );
		}

		Point remap( 
			float in_min_x, float in_max_x, 
			float out_min_x, float out_max_x,
			float in_min_y, float in_max_y,
			float out_min_y, float out_max_y
		) const
		{
			return Point {
				curve_x::remap( x, 
					in_min_x, in_max_x, 
					out_min_x, out_max_x ),
				curve_x::remap( y, 
					in_min_y, in_max_y, 
					out_min_y, out_max_y ),
			};
		}

		float length_sqr() const 
		{
			return x * x + y * y;
		}

		float length() const
		{
			return sqrtf( length_sqr() );
		}

		Point normalized() const
		{
			float m = length();
			return Point {
				x / m,
				y / m,
			};
		}
	};
}