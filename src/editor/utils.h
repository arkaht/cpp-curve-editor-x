#pragma once

#include <corecrt_math.h>

namespace curve_editor_x
{
	inline float lerp( float a, float b, float t )
	{
		return a + t * ( b - a );
	}

	inline float round( float value, float idp )
	{
		float mul = powf( 10.0f, idp );
		return floorf( value * mul + 0.5f ) / mul;
	}

	inline float approach( float value, float target, float delta )
	{
		delta = fabsf( delta );

		if ( value < target )
		{
			return fminf( value + delta, target );
		}
		else if ( value > target )
		{
			return fmaxf( value - delta, target );
		}

		return target;
	}
}