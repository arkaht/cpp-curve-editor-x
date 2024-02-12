#include "curve.h"

#include <cassert>

using namespace curve_x;

Curve::Curve()
{}

Curve::Curve( std::vector<Point> points )
	: _points( points ) {}

Point Curve::evaluate( float t ) const
{
	int curve_id = get_curve_id_by_time( t );
	t = fmaxf( fminf( t, 1.0f ), 0.0f );

	const Point p0 = get_point( curve_id );
	const Point p1 = p0 + get_point( curve_id + 1 );
	const Point p3 = get_point( curve_id + 3 );
	const Point p2 = p3 + get_point( curve_id + 2 );

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

	//  Creates a default tangent mode if none
	int max_tangent_id = get_point_tangent_id( get_points_count() );
	if ( max_tangent_id > (int)_modes.size() - 1 )
	{
		_modes.push_back( TangentMode::Mirrored );
	}

	_is_length_dirty = true;
}

void Curve::set_point( int id, const Point& point )
{
	_points[id] = point;

	_is_length_dirty = true;
}

void Curve::set_tangent_point( int point_id, const Point& point )
{
	//  TODO: Decide to keep or not the assert
	assert( !is_control_point( point_id ) );

	int peer_point_id = get_tangent_peer_point_id( point_id );
	if ( is_valid_point_id( peer_point_id ) )
	{
		int tangent_id = get_point_tangent_id( point_id );
		TangentMode tangent_mode = get_tangent_mode( tangent_id );

		switch ( tangent_mode )
		{
			case TangentMode::Mirrored:
			{
				set_point( peer_point_id, -point );
				break;
			}
			case TangentMode::Aligned:
			{
				const Point peer_point = get_point( peer_point_id );
				float length = peer_point.length();
				const Point aligned_point = 
					-point.normalized() * length;

				set_point( peer_point_id, aligned_point );
				break;	
			}
			case TangentMode::Broken:
				break;
		}
	}

	set_point( point_id, point );
}

int Curve::get_tangent_peer_point_id( int point_id ) const
{
	int curve_id = point_id % 3;
	if ( curve_id == 1 ) return point_id - 2;
	if ( curve_id == 2 ) return point_id + 2;

	return -1;
}

int Curve::get_point_tangent_id( int point_id ) const
{
	return (int)roundf( point_id / 3.0f );
}

int Curve::get_control_point_id( int point_id ) const
{
	int curve_id = point_id % 3;
	if ( curve_id == 1 ) return point_id - 1;
	if ( curve_id == 2 ) return point_id + 1;

	return point_id;
}

int Curve::get_curve_id_by_time( float& t ) const
{
	if ( t >= 1.0f )
	{
		t = 1.0f;
		return get_points_count() - 4;
	}

	t = fmaxf( t, 0.0f ) * get_curves_count();
	int curve_id = (int)floorf( t );
	t -= (float)curve_id;
	
	return curve_id * 3;
}

void Curve::set_tangent_mode( 
	int tangent_id, 
	TangentMode mode,
	bool should_apply_constraint 
)
{
	_modes[tangent_id] = mode;

	if ( should_apply_constraint )
	{
		//  Ignore first & last modes
		if ( tangent_id == 0 || tangent_id == (int)_modes.size() - 1 ) 
			return;

		//  Apply new tangent mode constraint
		int point_id = tangent_id * 3 + 1;
		set_tangent_point( point_id, get_point( point_id ) );
	}
}

TangentMode Curve::get_tangent_mode( int tangent_id ) const
{
	return _modes[tangent_id];
}

bool Curve::is_valid() const
{
	int count = get_points_count();
	return count > 0 && ( count - 1 ) % 3 == 0;
}

bool Curve::is_valid_point_id( int point_id ) const
{
	return point_id >= 0 && point_id < get_points_count();
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

Point Curve::get_global_point( int point_id ) const
{
	Point point = get_point( point_id );

	//  Transform the local tangent point to global space by adding
	//  its control point
	if ( !is_control_point( point_id ) ) 
	{
		point = get_point( get_control_point_id( point_id ) ) + point;
	}

	return point;
}

Point Curve::get_point( int point_id ) const
{ 
	return _points[point_id]; 
}

int Curve::get_points_count() const 
{ 
	return (int)_points.size(); 
}

int Curve::get_curves_count() const
{
	return ( get_points_count() - 1 ) / 3.0f;
}

float Curve::get_length()
{
	if ( _is_length_dirty )
	{
		_compute_length();
		_is_length_dirty = false;
	}

	return _length;
}

void Curve::_compute_length()
{
	_length = 0.0f;

	const float steps = 1.0f / 100.0f;

	Point last_point = get_point( 0 );
	for ( float t = steps; t < 1.0f; t += steps )
	{
		const Point point = evaluate( t );
		_length += ( point - last_point ).length();
		last_point = point;
	}
}
