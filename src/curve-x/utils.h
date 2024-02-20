#pragma once

#include <cmath>

namespace curve_x
{
	template<typename T>
	inline T bezier_interp( T p0, T p1, T p2, T p3, float t )
	{
		//  Following the formula described here:
		//  https://en.wikipedia.org/wiki/B%C3%A9zier_curve

		const float it = 1.0f - t;
		const float it2 = it * it;
		const float it3 = it2 * it;

		const float t2 = t * t;
		const float t3 = t2 * t;

		return p0 * it3
			 + p1 * ( 3.0f * it2 * t )
			 + p2 * ( 3.0f * it * t2 )
			 + p3 * t3;
	}

	inline float remap(
		float value,
		float min_a, float max_a,
		float min_b, float max_b
	)
	{
		return min_b
			+ ( max_b - min_b )
			* ( ( value - min_a ) / ( max_a - min_a ) );
	}
}