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
		Curve() {}
		Curve( std::vector<Point> points )
			: _points( points ) {}

		float evaluate( float time ) const;

		void add_point( const Point& point );
		void set_point( int id, const Point& point );

		void get_extrems( 
			float& min_x, float& max_x, 
			float& min_y, float& max_y 
		) const;
		CurveExtrems get_extrems() const;

		Point get_point( int id ) const;
		int get_points_count() const;

	private:
		std::vector<Point> _points;
	};
}