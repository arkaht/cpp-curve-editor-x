#pragma once

#include <vector>

#include "point.h"
#include "key.h"

namespace curve_x
{
	struct CurveExtrems
	{
		float min_x, max_x;
		float min_y, max_y;
	};

	/*
	 * Bézier Cubic Curve
	 */
	class Curve
	{
	public:
		Curve();
		//  TODO: Add 'modes' vector
		//Curve( std::vector<Point> points );

		/*
		 * Evaluate a curve point at specified percent, in range 
		 * from 0.0f to 1.0f.
		 */
		Point evaluate_by_percent( float t ) const;
		/*
		 * Evaluate a curve point at specified distance.
		 * 
		 * Internally using 'evaluate_by_percent' by dividing the
		 * distance by the curve length.
		 */
		Point evaluate_by_distance( float d ) const;
		/*
		 * Evaluate the Y-axis value corresponding to the specified 
		 * time on the X-axis.
		 */
		float evaluate_by_time( float time ) const;

		void add_key( const CurveKey& key );
		void insert_key( int key_id, const CurveKey& key );
		void remove_key( int key_id );

		CurveKey& get_key( int key_id );
		const CurveKey& get_key( int key_id ) const;

		void set_point( int point_id, const Point& point );
		void set_tangent_point( 
			int point_id, 
			const Point& point,
			PointSpace point_space = PointSpace::Local
		);

		Point get_point( 
			int point_id, 
			PointSpace point_space = PointSpace::Local
		) const;

		int point_to_key_id( int point_id ) const;
		int key_to_point_id( int key_id ) const;
		
		/*
		 * Change tangent mode for the given key index.
		 * 
		 * By default, it applies the new mode constraint to both 
		 * tangents.
		 */
		void set_tangent_mode( 
			int key_id, 
			TangentMode mode, 
			bool should_apply_constraint = true
		);
		/*
		 * Returns the tangent mode of given tangent index.
		 */
		TangentMode get_tangent_mode( int key_id ) const;

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
		bool is_valid_key_id( int key_id ) const;
		bool is_valid_point_id( int point_id ) const;
		bool is_control_point_id( int point_id ) const;

		/*
		 * Fill given variables to the coordinates extrems of all
		 * points.
		 */
		void get_extrems( 
			float* min_x, float* max_x, 
			float* min_y, float* max_y 
		) const;
		/*
		 * Returns the coordinates extrems of all points.
		 */
		CurveExtrems get_extrems() const;

		/*
		 * Fill given variables with the first & last control 
		 * points indexes to use for evaluation by time.
		 */
		void find_evaluation_keys_id_by_time( 
			int* first_point_id,
			int* last_point_id,
			float time 
		) const;
		void find_evaluation_keys_id_by_percent(
			int* first_key_id,
			int* last_key_id,
			float& t
		) const;

		int get_keys_count() const;
		int get_curves_count() const;
		int get_points_count() const;

		//  TODO: Decide if this function should be non-const 
		float get_length();

	private:
		void _compute_length();

	private:
		bool _is_length_dirty = true;
		float _length = 0.0f;

		std::vector<CurveKey> _keys;

		/*
		 * Vector containing both control & tangent points.
		 * 
		 * Control points are set every 3 indexes (e.g. 0, 3, 6)
		 * and are in global-space.
		 * 
		 * Tangent points are in local-space, forming a scaled 
		 * direction from its control point. Their indexes are
		 * close to their control point (e.g. point 1 is a tangent
		 * of control point 0; points 2 and 4 are tangents of 
		 * control point 3)
		 * 
		 * A 'point index' is needed to index this vector.
		 */
		//std::vector<Point> _points;

		/*
		 * Vector of tangent modes.
		 * 
		 * Each element correspond to the mode for each control 
		 * point. That also mean that this should always a size of 
		 * '_points' divided by 3 and rounded up.
		 * 
		 * A 'key index' is needed to index this vector.
		 */
		//std::vector<TangentMode> _modes;
	};
}