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

	class Curve
	{
	public:
		Curve();
		Curve( std::vector<Point> points );

		Point evaluate( float t ) const;

		void add_point( const Point& point );
		void set_point( int id, const Point& point );

		bool is_control_point( int id ) const;

		void get_extrems( 
			float& min_x, float& max_x, 
			float& min_y, float& max_y 
		) const;
		CurveExtrems get_extrems() const;

		//  Get control point index of the given velocity point index
		int get_control_point_id( int id ) const;

		Point get_global_point( int id ) const;
		Point get_point( int id ) const;

		int get_points_count() const;

	private:
		/*
		 * TODO: correct this description after full tangent control
		 * Vector of control & velocity points.
		 * 
		 * Control points are set every 3 indexes (e.g. 0, 3, 6)
		 * and are in global space.
		 * 
		 * Velocity points are in local space, forming a scaled 
		 * direction from its control point. Their indexes are
		 * close to their control point (e.g. point 1 affects 
		 * velocity of control point 0; points 2 and 4 affect 
		 * velocity of control point 3)
		 */
		std::vector<Point> _points;
	};
}