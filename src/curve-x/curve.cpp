#include "curve.h"

#include <cassert>

using namespace curve_x;

Curve::Curve()
{}

//Curve::Curve( std::vector<Point> points )
//	: _points( points ) {}

Point Curve::evaluate_by_percent( float t ) const
{
	int first_key_id, last_key_id;
	find_evaluation_keys_id_by_percent( 
		&first_key_id, &last_key_id, t );
	t = fmaxf( fminf( t, 1.0f ), 0.0f );

	const CurveKey& k0 = get_key( first_key_id );
	const CurveKey& k1 = get_key( last_key_id );

	const Point p0 = k0.control;
	const Point p1 = p0 + k0.right_tangent;
	const Point p3 = k1.control;
	const Point p2 = p3 + k1.left_tangent;

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
	//  Bound evaluation to first & last points
	const Point first_point = get_key( 0 ).control;
	const Point last_point = get_key( get_keys_count() - 1 ).control;
	if ( time <= first_point.x ) return first_point.y;
	if ( time >= last_point.x ) return last_point.y;

	//  Find evaluation points by time
	int first_key_id, last_key_id;
	find_evaluation_keys_id_by_time( 
		&first_key_id, 
		&last_key_id, 
		time 
	);

	//  Get keys in range
	const CurveKey& k0 = get_key( first_key_id );
	const CurveKey& k1 = get_key( last_key_id );

	//  Get control points
	const Point p0 = k0.control;
	const Point p3 = k1.control;

	//  Get tangent points
	const Point t1 = k0.right_tangent;
	const Point t2 = k1.left_tangent;
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

void Curve::add_key( const CurveKey& key )
{
	_keys.push_back( key );

	_is_length_dirty = true;
}

void Curve::insert_key( int key_id, const CurveKey& key )
{
	auto itr = _keys.begin() + key_id;
	_keys.insert( itr, key );

	_is_length_dirty = true;

	//  First key doesn't have a left tangent
	/*if ( key_id > 0 )
	{
		_keys.insert( itr, left_tangent_point );
	}

	_points.insert( itr + 1, control_point );*/

	//  Last key doesn't have a right tangent
	/*if ( key_id <= get_key_id( get_points_count() - 1 ) )
	{
		_points.insert( itr + 2, right_tangent_point );
	}*/
}

void Curve::remove_key( int key_id )
{
	auto itr = _keys.begin() + key_id;
	_keys.erase( itr );

	_is_length_dirty = true;
}

CurveKey& Curve::get_key( int key_id )
{
	return _keys[key_id];
}

const CurveKey& Curve::get_key( int key_id ) const
{
	return _keys[key_id];
}

void Curve::set_point( int point_id, const Point& point )
{
	int key_id = get_point_key_id( point_id );
	CurveKey& key = get_key( key_id );

	switch ( point_id % 3 )
	{
		case 0:
			key.control = point;
			break;
		case 1:
			key.right_tangent = point;
			break;
		case 2:
			key.left_tangent = point;
			break;
	}
}

void Curve::set_tangent_point( 
	int point_id, 
	const Point& point,
	PointSpace point_space 
)
{
	int key_id = get_point_key_id( point_id );
	CurveKey& key = get_key( key_id );

	//  In global space, convert the given point into local space
	//  by substracting the control point
	Point tangent = point;
	if ( point_space == PointSpace::Global )
	{
		tangent = point - key.control;
	}

	//  Apply the tangent point
	switch ( point_id % 3 )
	{
		case 0:
			key.control = point;
			break;
		case 1:
			key.set_right_tangent( tangent );
			break;
		case 2:
			key.set_left_tangent( tangent );
			break;
	}
}

Point Curve::get_point( int point_id, PointSpace point_space ) const
{
	int key_id = get_point_key_id( point_id );
	const CurveKey& key = get_key( key_id );

	switch ( point_id % 3 )
	{
		case 0:
			return key.control;
		case 1:
			return point_space == PointSpace::Global
				?  key.control + key.right_tangent
				:  key.right_tangent;
		case 2:
			return point_space == PointSpace::Global
				?  key.control + key.left_tangent
				:  key.left_tangent;
	}

	//  Unreachable code
	assert( false );
}

int Curve::get_point_key_id( int point_id ) const
{
	return (int)roundf( point_id / 3.0f );
}

void Curve::find_evaluation_keys_id_by_percent(
	int* first_key_id,
	int* last_key_id,
	float& t
) const
{
	int key_id = -1;

	if ( t >= 1.0f )
	{
		t = 1.0f;
		
		key_id = get_keys_count() - 1;
	}
	else
	{
		t = fmaxf( t, 0.0f ) * get_curves_count();
		key_id = (int)floorf( t );
		t -= (float)key_id;
	}

	*first_key_id = key_id;
	*last_key_id = key_id + 1;
}

void Curve::set_tangent_mode( 
	int key_id, 
	TangentMode mode,
	bool should_apply_constraint 
)
{
	CurveKey& key = get_key( key_id );
	key.tangent_mode = mode;

	if ( should_apply_constraint )
	{
		key.set_left_tangent( key.left_tangent );
	}
}

TangentMode Curve::get_tangent_mode( int key_id ) const
{
	return get_key( key_id ).tangent_mode;
}

bool Curve::is_valid() const
{
	int count = get_keys_count();
	return count > 1;
}

bool Curve::is_valid_key_id( int key_id ) const
{
	return key_id >= 0 && key_id < get_keys_count();
}

bool Curve::is_valid_point_id( int point_id ) const
{
	return point_id >= 0 && point_id < get_points_count();
}

bool Curve::is_control_point_id( int point_id ) const
{
	return point_id % 3 == 0;
}

void Curve::get_extrems( 
	float* min_x, float* max_x, 
	float* min_y, float* max_y 
) const
{
	*min_x = *min_y = INFINITY;
	*max_x = *max_y = -INFINITY;

	for ( int i = 0; i < get_points_count(); i++ )
	{
		const Point& point = get_point( i, PointSpace::Global );

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

void Curve::find_evaluation_keys_id_by_time( 
	int* first_key_id,
	int* last_key_id,
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
	int last_id = get_keys_count() - 1;

	int count = last_id - first_id;
	while ( count > 0 )
	{
		int step = count / 2;
		int middle_id = first_id + step;

		if ( time >= get_key( middle_id ).control.x )
		{
			first_id = middle_id + 1;
			count -= step + 1;
		}
		else
		{
			count = step;
		}
	}

	*first_key_id = first_id - 1;
	*last_key_id = first_id;
}

int Curve::get_keys_count() const
{
	return (int)_keys.size();
}

int Curve::get_curves_count() const
{
	return get_keys_count() - 1;
}

int Curve::get_points_count() const
{
	return ( get_keys_count() - 2 ) * 3 + 4;
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

	Point last_point = get_key( 0 ).control;
	for ( float t = steps; t < 1.0f; t += steps )
	{
		const Point point = evaluate_by_percent( t );
		_length += ( point - last_point ).length();
		last_point = point;
	}
}
