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
	 * 
	 * Key index:
	 * 
	 * Point index:
	 */
	class Curve
	{
	public:
		Curve();
		Curve( const std::vector<CurveKey>& keys );

		/*
		 * Evaluate a curve point at given percent, in range from 
		 * 0.0f to 1.0f.
		 */
		Point evaluate_by_percent( float t ) const;
		/*
		 * Evaluate a curve point at given distance.
		 * 
		 * Internally using 'evaluate_by_percent' by dividing the
		 * distance by the curve length.
		 */
		Point evaluate_by_distance( float d ) const;
		/*
		 * Evaluate the Y-axis value corresponding to the given 
		 * time on the X-axis.
		 */
		float evaluate_by_time( float time ) const;

		/*
		 * Add a key at the end of the vector.
		 */
		void add_key( const CurveKey& key );
		/*
		 * Insert a key at given index.
		 */
		void insert_key( int key_id, const CurveKey& key );
		/*
		 * Remove a key at given index.
		 */
		void remove_key( int key_id );

		/*
		 * Get a reference to the key at given index.
		 */
		CurveKey& get_key( int key_id );
		/*
		 * Get a const-reference to the key at given index.
		 */
		const CurveKey& get_key( int key_id ) const;

		/*
		 * Set the location at the given point index.
		 */
		void set_point( int point_id, const Point& point );
		/*
		 * Set the location at the given point index, assuming it's
		 * a tangent point, and apply the tangent mode to its peer.
		 * 
		 * Since tangents are stored relatively to their control 
		 * point, this function handles point-space conversion.
		 */
		void set_tangent_point( 
			int point_id, 
			const Point& point,
			PointSpace point_space = PointSpace::Local
		);

		/*
		 * Get the location from the specified point index.
		 */
		Point get_point( 
			int point_id, 
			PointSpace point_space = PointSpace::Local
		) const;
		/*
		 * Compute the nearest point on the curve from an arbitrary 
		 * global-space point.
		 */
		Point get_nearest_point_to(
			const Point& point,
			const float steps = 1.0f / 30.0f
		) const;
		/*
		 * Compute the nearest curve distance from an arbitrary 
		 * global-space point.
		 */
		float get_nearest_distance_to(
			const Point& point,
			const float steps = 1.0f / 30.0f
		) const;

		/*
		 * Convert any point index to its key index.
		 */
		int point_to_key_id( int point_id ) const;
		/*
		 * Convert a key index to its control point index.
		 */
		int key_to_point_id( int key_id ) const;
		
		/*
		 * Change the tangent mode for the given key index.
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
		 * Returns the tangent mode of given key index.
		 */
		TangentMode get_tangent_mode( int key_id ) const;

		/*
		 * Returns whenever the curve contains a valid amount of 
		 * points for further usage.
		 * 
		 * It's important to first check the curve validity before 
		 * using other methods since they do not check about it. 
		 * It prevent crashes to happen.
		 */
		bool is_valid() const;
		/*
		 * Returns whenever the given key index correspond to a
		 * valid key.
		 */
		bool is_valid_key_id( int key_id ) const;
		/*
		 * Returns whenever the given point index correspond to a
		 * valid point.
		 */
		bool is_valid_point_id( int point_id ) const;
		/*
		 * Returns whenever the given point index may correspond to
		 * a control point.
		 */
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
		 * Fill given variables with the first & last key indexes 
		 * to use for evaluation from given time.
		 */
		void find_evaluation_keys_id_by_time( 
			int* first_key_id,
			int* last_key_id,
			float time 
		) const;
		/*
		 * Fill given variables with the first & last key indexes 
		 * to use for evaluation from given percent. 
		 * 
		 * Variable 't' may be modified after the function call.
		 */
		void find_evaluation_keys_id_by_percent(
			int* first_key_id,
			int* last_key_id,
			float& t
		) const;
		/*
		 * Fill given variables with the first & last key indexes 
		 * to use for evaluation from given distance.
		 */
		void find_evaluation_keys_id_by_distance(
			int* first_key_id,
			int* last_key_id,
			float d
		) const;

		/*
		 * Returns the size of the keys vector.
		 */
		int get_keys_count() const;
		/*
		 * Returns the number of curves formed by the keys.
		 * Basically, the number of keys minus one.
		 */
		int get_curves_count() const;
		/*
		 * Returns the number of apparent points.
		 */
		int get_points_count() const;

		/*
		 * Compute the curve's length, representing the maximum 
		 * evaluable distance.
		 */
		void compute_length( const float steps = 1.0f / 100.0f );

		/*
		 * Get the curve's length. 
		 * It'll update the length if marked as dirty.
		 */
		float get_length();
		/*
		 * Get the previously computed curve's length. 
		 * 
		 * Due to constness, it will NOT update the length if 
		 * marked as dirty. You may want to manually compute the 
		 * length AFTER modifying the curve and BEFORE passing it 
		 * as a const-reference.
		 */
		float get_length() const;

	public:
		/*
		 * Boolean stating whenever the length need to be updated.
		 */
		bool is_length_dirty = true;

	private:
		float _length = 0.0f;

		/*
		 * Vector containing the keys.
		 * 
		 * The required index is refered as a 'key index'.
		 */
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