#pragma once

#include <vector>

#include "point.h"

namespace curve_x
{
	struct CurveExtrems
	{
		float min_x, max_x;
		float min_y, max_y;
	};

	/*
	 * Bézier Cubic Curve
	 */
	class Curve
	{
	public:
		Curve();
		Curve( std::vector<Point> points );

		Point evaluate( float t ) const;

		void add_point( const Point& point );
		void set_point( int id, const Point& point );

		/*
		 * Returns whenever the curve contains a valid amount 
		 * of points for further usage.
		 */
		bool is_valid() const;
		bool is_control_point( int id ) const;

		void get_extrems( 
			float& min_x, float& max_x, 
			float& min_y, float& max_y 
		) const;
		CurveExtrems get_extrems() const;

		//  Get control point index of given tangent point index
		int get_control_point_id( int id ) const;

		Point get_global_point( int id ) const;
		Point get_point( int id ) const;

		int get_points_count() const;

	private:
		/*
		 * Vector of control & tangent points.
		 * 
		 * Control points are set every 3 indexes (e.g. 0, 3, 6)
		 * and are in global space.
		 * 
		 * Tangent points are in local space, forming a scaled 
		 * direction from its control point. Their indexes are
		 * close to their control point (e.g. point 1 is a tangent
		 * of control point 0; points 2 and 4 are tangents of 
		 * control point 3)
		 */
		std::vector<Point> _points;
	};
}