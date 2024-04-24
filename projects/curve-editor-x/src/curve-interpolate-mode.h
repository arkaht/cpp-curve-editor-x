#pragma once

namespace curve_editor_x
{
	enum class CurveInterpolateMode
	{
		/*
		 * Use of raylib functions
		 */
		Bezier,

		/*
		 * Use of time-evaluation
		 */
		TimeEvaluation,

		/*
		 * Use of distance-evaluation
		 */
		DistanceEvaluation,

		MAX,
	};
}