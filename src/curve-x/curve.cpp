#include "curve.h"

using namespace curve_x;

Curve::Curve()
{}

Curve::Curve( std::vector<Point> points )
	: _points( points ) {}

Point Curve::evaluate( float t ) const
{
	//  TODO: find points with 1D mapping
	const Point p0 = _points[0];
	const Point v0 = _points[1];
	const Point p1 = _points[2];
	const Point v1 = _points[3];

	const float t2 = t * t;
	const float t3 = t * t * t;

	//  https://en.wikipedia.org/wiki/Cubic_Hermite_spline
	return 
		p0 * ( 2.0f * t3 - 3.0f * t2 + 1.0f )
	  + v0 * ( t3 - 2.0f * t2 + t )
	  + p1 * ( -2.0f * t3 + 3.0f * t2 )
	  + v1 * ( t3 - t2 );
}

void Curve::add_point( const Point& point )
{
	_points.push_back( point );
}

void Curve::set_point( int id, const Point& point )
{
	_points[id] = point;
}

bool Curve::is_control_point( int id ) const
{
	return id % 2 == 0;
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
	/*int curve_id = id % 3;
	if ( curve_id == 1 ) return id - 1;
	if ( curve_id == 2 ) return id + 1;*/

	return is_control_point( id ) ? id : id - 1;
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
