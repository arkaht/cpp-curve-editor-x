#pragma once

#include "point.h"

namespace curve_x
{
	enum class TangentMode
	{
		/*
		 * Mirroring both tangents directions and lengths.
		 */
		Mirrored	= 0,

		/*
		 * Mirroring both tangents directions with custom lengths.
		 */
		Aligned		= 1,

		/*
		 * Both tangents have its own direction and length.
		 */
		Broken		= 2,

		MAX,
	};

	class CurveKey
	{
	public:
		CurveKey( 
			const Point& control, 
			const Point& left_tangent = { -1.0f, 0.0f },
			const Point& right_tangent = { 1.0f, 0.0f },
			TangentMode tangent_mode = TangentMode::Mirrored
		);

		void set_left_tangent( const Point& point );
		void set_right_tangent( const Point& point );

	public:
		Point control;
		Point left_tangent;
		Point right_tangent;

		TangentMode tangent_mode = TangentMode::Mirrored;

		float distance = -1.0f;

	private:
		void _set_tangent( 
			const Point& point, 
			Point* target_tangent,
			Point* peer_tangent
		);
	};
}