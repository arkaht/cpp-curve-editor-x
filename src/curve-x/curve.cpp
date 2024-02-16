#include "curve.h"

#include <cassert>

using namespace curve_x;

Curve::Curve()
{}

Curve::Curve( std::vector<Point> points )
	: _points( points ) {}

Point Curve::evaluate_by_percent( float t ) const
{
	int curve_id = get_curve_id_by_percent( t );
	t = fmaxf( fminf( t, 1.0f ), 0.0f );

	const Point p0 = get_point( curve_id );
	const Point p1 = p0 + get_point( curve_id + 1 );
	const Point p3 = get_point( curve_id + 3 );
	const Point p2 = p3 + get_point( curve_id + 2 );

	return bezier_interp( p0, p1, p2, p3, t );
}

Point Curve::evaluate_by_distance( float d ) const
{
	//  TODO: After deciding if 'get_length' should be const or not,
	//		  potentially change the '_length' by 'get_length'.
	return evaluate_by_percent( d / _length );
}

float Curve::evaluate_by_time( float time ) const
{
	int points_count = get_points_count();
	
	//  Bound evaluation to first & last points
	const Point first_point = get_point( 0 );
	const Point last_point = get_point( points_count - 1 );
	if ( time <= first_point.x ) return first_point.y;
	if ( time >= last_point.x ) return last_point.y;

	//  Find evaluation points by time
	int first_point_id, last_point_id;
	find_evaluation_point_id_by_time( 
		&first_point_id, 
		&last_point_id, 
		time 
	);

	//  Get control points
	const Point p0 = get_point( first_point_id );
	const Point p3 = get_point( last_point_id );

	//  Get tangent points
	const Point t1 = get_point( first_point_id + 1 );
	const Point t2 = get_point( last_point_id - 1 );
	/*const float m1 = t1.length();
	const float m2 = t2.length();*/

	//  Compute time difference
	const float time_diff = p3.x - p0.x;
	//if ( time_diff <= 0.0f ) return p0.y;

	//  Compute time ratio from p0 & p3 (from 0.0f to 1.0f)
	const float t = ( time - p0.x ) / time_diff;

	//  Compute tangents Y-positions
	/*const float y1 = p0.y + atan2f( t1.y / m1, t1.x / m1 ) * time_diff * m1 / 3.0f;
	const float y2 = p3.y + atan2f( t2.y / m2, t2.x / m2 ) * time_diff * m2 / 3.0f;*/
	const float y1 = p0.y + t1.y;
	const float y2 = p3.y + t2.y;

	return bezier_interp( p0.y, y1, y2, p3.y, t );
}

void Curve::add_point( const Point& point )
{
	_points.push_back( point );

	//  Creates a default tangent mode if none
	int max_key_id = get_key_id( get_points_count() );
	if ( max_key_id > (int)_modes.size() - 1 )
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
	int peer_point_id = get_tangent_peer_point_id( point_id );
	if ( is_valid_point_id( peer_point_id ) )
	{
		int key_id = get_key_id( point_id );
		TangentMode tangent_mode = get_tangent_mode( key_id );

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

int Curve::get_key_id( int point_id ) const
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

int Curve::get_curve_id_by_percent( float& t ) const
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
	int key_id, 
	TangentMode mode,
	bool should_apply_constraint 
)
{
	_modes[key_id] = mode;

	if ( should_apply_constraint )
	{
		//  Ignore first & last modes
		if ( key_id == 0 || key_id == (int)_modes.size() - 1 ) 
			return;

		//  Apply new tangent mode constraint
		int point_id = key_id * 3 + 1;
		set_tangent_point( point_id, get_point( point_id ) );
	}
}

TangentMode Curve::get_tangent_mode( int key_id ) const
{
	return _modes[key_id];
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
	float* min_x, float* max_x, 
	float* min_y, float* max_y 
) const
{
	*min_x = *min_y = INFINITY;
	*max_x = *max_y = -INFINITY;

	for ( int i = 0; i < _points.size(); i++ )
	{
		const Point& point = get_global_point( i );

		if ( point.x > *max_x )
		{
			*max_x = point.x;
		}
		if ( point.x < *min_x )
		{
			*min_x = point.x;
		}

		if ( point.y > *max_y )
		{
			*max_y = point.y;
		}
		if ( point.y < *min_y )
		{
			*min_y = point.y;
		}
	}
}

CurveExtrems Curve::get_extrems() const
{
	CurveExtrems extrems {};

	get_extrems( 
		&extrems.min_x, 
		&extrems.max_x, 
		&extrems.min_y, 
		&extrems.max_y 
	);

	return extrems;
}

void Curve::find_evaluation_point_id_by_time( 
	int* first_point_id,
	int* last_point_id,
	float time 
) const
{
	/*
	 * Perform a lower bound to find out the two control points
	 * to evaluate from.
	 * 
	 * Code highly inspired on Unreal Engine's code
	 */

	int first_id = 1;
	int last_id = ( get_points_count() - 1 ) / 3;

	int count = last_id - first_id;
	while ( count > 0 )
	{
		int step = count / 2;
		int middle_id = first_id + step;

		if ( time >= get_point( middle_id * 3 ).x )
		{
			first_id = middle_id + 1;
			count -= step + 1;
		}
		else
		{
			count = step;
		}
	}

	*first_point_id = ( first_id - 1 ) * 3;
	*last_point_id = first_id * 3;
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
	return ( get_points_count() - 1 ) / 3;
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
		const Point point = evaluate_by_percent( t );
		_length += ( point - last_point ).length();
		last_point = point;
	}
}
