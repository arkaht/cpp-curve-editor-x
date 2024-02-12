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

	enum class TangentMode
	{
		/*
		 * Mirroring both tangents directions and lengths.
		 */
		Mirrored	= 0,

		/*
		 * Mirroring both tangents directions with custom lengths.
		 */
		Aligned		= 1,

		/*
		 * Both tangents have its own direction and length.
		 */
		Broken		= 2,

		MAX,
	};

	/*
	 * Bézier Cubic Curve
	 */
	class Curve
	{
	public:
		Curve();
		//  TODO: Add 'modes' vector
		Curve( std::vector<Point> points );

		/*
		 * Evaluate the curve at specified time.
		 */
		Point evaluate( float t ) const;

		/*
		 * Add a new point to the curve. 
		 * 
		 * You must know in advance whether the point will be 
		 * considered as a control or a tangent point before using 
		 * this function. See comments on '_points' member variable.
		 */
		void add_point( const Point& point );

		/*
		 * Change point for given point index, without any
		 * transformations.
		 */
		void set_point( int point_id, const Point& point );
		/*
		 * Change point for given tangent point index, and applies 
		 * the tangent mode constraint to its peer.
		 */
		void set_tangent_point( int point_id, const Point& point );
		
		/*
		 * Change tangent mode for the given tangent index.
		 * 
		 * By default, it applies the new mode constraint to both 
		 * tangents.
		 */
		void set_tangent_mode( 
			int tangent_id, 
			TangentMode mode, 
			bool should_apply_constraint = true
		);
		/*
		 * Returns the tangent mode of given tangent index.
		 */
		TangentMode get_tangent_mode( int tangent_id ) const;

		/*
		 * Returns the peer tangent point index of given point index.
		 * 
		 * The given point index must correspond to a tangent point. 
		 * Therefore, giving a control point index returns -1.
		 * 
		 * The returned index may not correspond to a valid point,
		 * specially for the first and last tangents as they do not
		 * have a peer. Consider using 'is_valid_point_id'.
		 */
		int get_tangent_peer_point_id( int point_id ) const;
		/*
		 * Returns the corresponding tangent index for the given
		 * point index.
		 */
		int get_point_tangent_id( int point_id ) const;
		/*
		 * Returns the control point index of given tangent point 
		 * index.
		 */
		int get_control_point_id( int point_id ) const;

		/*
		 * Returns whenever the curve contains a valid amount 
		 * of points for further usage.
		 * 
		 * It's important to first check the curve validity before 
		 * using other methods since they do not check about it. It 
		 * prevent crashes to happen.
		 */
		bool is_valid() const;
		/*
		 * Returns whenever the given point index correspond to a
		 * valid point. Essentially checking if the index is in the
		 * '_points' vector range.
		 */
		bool is_valid_point_id( int point_id ) const;
		/*
		 * Returns whenever the given point index correspond to a 
		 * control point. Otherwise, it represents a tangent point.
		 */
		bool is_control_point( int point_id ) const;

		/*
		 * Fill given variables to the coordinates extrems of all
		 * points.
		 */
		void get_extrems( 
			float& min_x, float& max_x, 
			float& min_y, float& max_y 
		) const;
		/*
		 * Returns the coordinates extrems of all points.
		 */
		CurveExtrems get_extrems() const;

		/*
		 * Returns point at given index in global space, whether
		 * it's a control point or a tangent point. 
		 */
		Point get_global_point( int point_id ) const;
		/*
		 * Returns point at given index as it's stored in the 
		 * curve, without any transformation.
		 */
		Point get_point( int point_id ) const;

		/*
		 * Returns count of points stored in the curve.
		 */
		int get_points_count() const;

	private:
		/*
		 * Vector containing both control & tangent points.
		 * 
		 * Control points are set every 3 indexes (e.g. 0, 3, 6)
		 * and are in global space.
		 * 
		 * Tangent points are in local space, forming a scaled 
		 * direction from its control point. Their indexes are
		 * close to their control point (e.g. point 1 is a tangent
		 * of control point 0; points 2 and 4 are tangents of 
		 * control point 3)
		 * 
		 * A 'point index' correspond to an index in this vector.
		 */
		std::vector<Point> _points;

		//  TODO: Find a better name for 'tangent index' as it 
		//		  will be confused with a 'tangent point index'. 
		//		  It can't be 'control index' either.
		/*
		 * Vector of tangent modes.
		 * 
		 * Each element correspond to the mode for each control 
		 * point. That also mean that this should always a size of 
		 * '_points' divided by 3 and rounded up.
		 * 
		 * A 'tangent index' correspond to an index in this vector.
		 */
		std::vector<TangentMode> _modes;
	};
}