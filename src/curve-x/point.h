#pragma once

#include "utils.h"

namespace curve_x
{
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
	};
}