#include "curve.h"

using namespace curve_x;

float Curve::evaluate( float time ) const
{
	return 0.0f;
}

void Curve::add_point( const Point& point )
{
	_points.push_back( point );
}

void Curve::set_point( int id, const Point& point )
{
	_points[id] = point;
}

void Curve::get_extrems( 
	float& min_x, float& max_x, 
	float& min_y, float& max_y 
) const
{
	min_x = min_y = INFINITY;
	max_x = max_y = -INFINITY;

	for ( int i = 0; i < _points.size(); i++ )
	{
		const Point& point = _points[i];

		if ( point.x > max_x )
		{
			max_x = point.x;
		}
		if ( point.x < min_x )
		{
			min_x = point.x;
		}

		if ( point.y > max_y )
		{
			max_y = point.y;
		}
		if ( point.y < min_y )
		{
			min_y = point.y;
		}
	}
}

CurveExtrems Curve::get_extrems() const
{
	CurveExtrems extrems {};

	get_extrems( 
		extrems.min_x, 
		extrems.max_x, 
		extrems.min_y, 
		extrems.max_y 
	);

	return extrems;
}

Point Curve::get_point( int id ) const { return _points[id]; }

int Curve::get_points_count() const { return (int)_points.size(); }
