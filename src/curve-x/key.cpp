#include "key.h"

using namespace curve_x;

CurveKey::CurveKey( 
	const Point& control, 
	const Point& left_tangent, 
	const Point& right_tangent, 
	TangentMode tangent_mode 
)
	: control( control ),
	  left_tangent( left_tangent ),
	  right_tangent( right_tangent ),
	  tangent_mode( tangent_mode )
{}

void CurveKey::set_left_tangent( const Point& point )
{
	_set_tangent( point, &left_tangent, &right_tangent );
}

void CurveKey::set_right_tangent( const Point& point )
{
	_set_tangent( point, &right_tangent, &left_tangent );
}

void CurveKey::_set_tangent( 
	const Point& point, 
	Point* target_tangent, 
	Point* peer_tangent 
)
{
	//  Set target tangent
	*target_tangent = point;

	//  Apply constraint to the peer tangent
	switch ( tangent_mode )
	{
		case TangentMode::Mirrored:
		{
			*peer_tangent = -point;
			break;
		}
		case TangentMode::Aligned:
		{
			float length = ( *peer_tangent ).length();
			const Point aligned_point = 
				-point.normalized() * length;

			*peer_tangent = aligned_point;
			break;	
		}
		case TangentMode::Broken:
			break;
	}
}
