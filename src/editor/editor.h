#pragma once

#include <raylib.h>
#include <raymath.h>

#include <string>

#include <curve-x/curve.h>
#include <editor/utils.h>

namespace curve_editor_x
{
	using namespace curve_x;

	enum class CurveInterpolateMode
	{
		//  Use of raylib functions
		Bezier,
		//  Use of time-evaluation
		TimeEvaluation,
		//  Use of distance-evaluation
		DistanceEvaluation,

		MAX,
	};

	class Editor
	{
	public:
		Editor( 
			const Rectangle& frame 
		);

		void init();
		void update( float dt );
		void render();

		void fit_viewport_to_curve();

		void set_path( const std::string& path );
		bool export_to_file( const std::string& path );
		bool import_from_file( const std::string& path );

	private:
		void _invalidate_layout();
		void _invalidate_grid();

		void _render_title_text();
		void _render_frame();

		void _render_curve_screen();
		void _render_curve_by_distance();
		void _render_curve_by_time();
		void _render_curve_by_bezier();
		void _render_curve_points();

		void _render_invalid_curve_screen();

		void _render_grid();

		void _render_point( int point_id, const Vector2& pos );
		void _render_circle_point( 
			const Vector2& pos, 
			const Color& color,
			bool is_selected
		);
		void _render_square_point( 
			const Vector2& pos, 
			const Color& color,
			bool is_selected
		);

		bool _is_double_clicking( bool should_consume );

		float _transform_curve_to_screen_x( float x ) const;
		float _transform_curve_to_screen_y( float y ) const;
		Vector2 _transform_curve_to_screen( const Point& point ) const;
		Vector2 _transform_screen_to_curve( const Vector2& pos ) const;
		Vector2 _transform_rounded_grid_snap( const Vector2& pos ) const;
		Vector2 _transform_ceiled_grid_snap( const Vector2& pos ) const;

	//  Application settings
	private:
		const Color BACKGROUND_COLOR = LIGHTGRAY;
		const Color TEXT_COLOR = DARKGRAY;
		const Color TEXT_ERROR_COLOR = RED;
		const Color CURVE_FRAME_COLOR = DARKGRAY;
		const Color CURVE_COLOR { 255, 90, 90, 255 };
		const Color TANGENT_COLOR { 90, 90, 90, 255 };
		const Color POINT_COLOR { 255, 0, 0, 255 };
		const Color POINT_SELECTED_COLOR { 255, 255, 255, 255 };
		const Color GRID_LINE_COLOR { 120, 120, 120, 255 };
		const Color QUICK_EVALUATION_COLOR { 90, 90, 90, 255 };

		const float QUICK_EVALUATION_THICKNESS = 2.0f;
		const float CURVE_THICKNESS = 2.0f;
		//  How much to offset the thickness per wheel scroll?
		const float CURVE_THICKNESS_SENSITIVITY = 0.5f;
		const float CURVE_RENDER_STEPS = 0.1f;
		const float TANGENT_THICKNESS = 2.0f;
		const float POINT_SIZE = CURVE_THICKNESS * 3.0f;
		const float POINT_SELECTED_OFFSET_SIZE = 3.0f;
		const float CURVE_FRAME_PADDING = 32.0f;
		const double DOUBLE_CLICK_TIME = 0.2;

		//  In curve units, the gap for each grid line
		const float GRID_SMALL_GAP = 1.0f;
		//  Number of small grid lines to form a larger line
		const float GRID_LARGE_COUNT = 10.0f;
		const float GRID_SMALL_LINE_THICKNESS = 1.0f;
		const float GRID_LARGE_LINE_THICKNESS = 2.0f;
		const float GRID_SMALL_GRID_FONT_SIZE = 16.0f;
		const float GRID_LARGE_GRID_FONT_SIZE = 20.0f;
		const float GRID_FONT_SPACING = 1.0f;
		const float GRID_TEXT_PADDING = 2.0f;

		const float SELECTION_RADIUS = 8.0f;

		const int   TITLE_FONT_SIZE = 20;
		const float TITLE_DOCK_MARGIN_BOTTOM = 4.0f;

		const float ZOOM_SENSITIVITY = 0.1f;
		const float ZOOM_MIN = 0.1f;
		const float ZOOM_MAX = 5.0f;

		//  Does the frame rendering clips its content?
		const bool  ENABLE_CLIPPING = true;
		const bool  DRAW_MOUSE_POSITION = true;
		const bool  MUST_DOUBLE_CLICK_TO_DRAG_POINT = false;

	private:
		std::string _title {};
		std::string _path = "tests/test.cvx";
		bool _is_current_file_exists = false;

		Curve _curve {};
		CurveExtrems _curve_extrems {};
		CurveInterpolateMode _curve_interpolate_mode 
			= CurveInterpolateMode::Bezier;

		Rectangle _frame {};

		Vector2 _transformed_mouse_pos {};

		float _zoom = 1.0f;
		Rectangle _viewport {};
		Rectangle _frame_outline {};

		bool _is_grid_snapping = false;
		float _grid_gap = 1.0f;

		bool _is_quick_evaluating = false;
		Vector2 _quick_evaluation_pos {};

		int _hovered_point_id = -1;
		int _selected_point_id = -1;
		bool _can_drag_selected_point = false;

		bool _is_showing_points = true;
		float _curve_thickness = CURVE_THICKNESS;
		double _last_click_time = 0.0;

		bool _has_unsaved_changes = true;
	};
}