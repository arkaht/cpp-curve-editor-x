#include "curve.h"

using namespace curve_x;

Curve::Curve()
{}

Curve::Curve( std::vector<Point> points )
	: _points( points ) {}

Point Curve::evaluate( float t ) const
{
	//  TODO: find points with 1D mapping
	const Point p0 = get_point( 0 );
	const Point p1 = get_global_point( 1 );
	const Point p2 = get_global_point( 2 );
	const Point p3 = get_point( 3 );

	const float it = 1.0f - t;
	const float it2 = it * it;
	const float it3 = it * it;

	const float t2 = t * t;
	const float t3 = t2 * t;

	//  https://en.wikipedia.org/wiki/B%C3%A9zier_curve
	return 
		p0 * it3
	  + p1 * ( 3.0f * it2 * t )
	  + p2 * ( 3.0f * it * t2 )
	  + p3 * t3;
}

void Curve::add_point( const Point& point )
{
	_points.push_back( point );
}

void Curve::set_point( int id, const Point& point )
{
	_points[id] = point;
}

bool Curve::is_valid() const
{
	int count = get_points_count();
	return count > 0 && ( count - 1 ) % 3 == 0;
}

bool Curve::is_control_point( int id ) const
{
	return id % 3 == 0;
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
		const Point& point = get_global_point( i );

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

int Curve::get_control_point_id( int id ) const
{
	int curve_id = id % 3;
	if ( curve_id == 1 ) return id - 1;
	if ( curve_id == 2 ) return id + 1;

	return id;
}

Point Curve::get_global_point( int id ) const
{
	Point point = get_point( id );

	//  add control point for velocity point
	if ( !is_control_point( id ) ) 
	{
		point = get_point( get_control_point_id( id ) ) + point;
	}

	return point;
}

Point Curve::get_point( int id ) const
{ 
	return _points[id]; 
}

int Curve::get_points_count() const 
{ 
	return (int)_points.size(); 
}
