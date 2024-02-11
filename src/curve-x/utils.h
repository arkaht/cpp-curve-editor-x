#pragma once

namespace curve_x
{
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

	inline float round( float value );
}