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
		Curve( std::vector<Point> points );

		Point evaluate( float t ) const;

		void add_point( const Point& point );
		/*
		 * Change point for given point index.
		 */
		void set_point( int point_id, const Point& point );

		/*
		 * Change point for given tangent point index. 
		 * It applies the tangent mode constraint to its peer.
		 */
		void set_tangent_point( int point_id, const Point& point );
		/*
		 * Change tangent mode for the given tangent index.
		 * It applies the new mode constraint to both tangents.
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
		 * 
		 */
		int get_tangent_peer_point_id( int point_id ) const;
		int get_point_tangent_id( int point_id ) const;

		/*
		 * Returns whenever the curve contains a valid amount 
		 * of points for further usage.
		 */
		bool is_valid() const;
		bool is_valid_point_id( int point_id ) const;
		bool is_control_point( int point_id ) const;

		/*
		 * Fill given variables to the coordinates extrems of all
		 * points.
		 */
		void get_extrems( 
			float& min_x, float& max_x, 
			float& min_y, float& max_y 
		) const;
		CurveExtrems get_extrems() const;

		/*
		 * Get control point index of given tangent point index.
		 */
		int get_control_point_id( int point_id ) const;

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
		 * Vector of control & tangent points.
		 * 
		 * Control points are set every 3 indexes (e.g. 0, 3, 6)
		 * and are in global space.
		 * 
		 * Tangent points are in local space, forming a scaled 
		 * direction from its control point. Their indexes are
		 * close to their control point (e.g. point 1 is a tangent
		 * of control point 0; points 2 and 4 are tangents of 
		 * control point 3)
		 */
		std::vector<Point> _points;

		std::vector<TangentMode> _modes;
	};
}