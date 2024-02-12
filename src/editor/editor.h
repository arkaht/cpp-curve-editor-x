#pragma once

#include <raylib.h>
#include <raymath.h>

#include <string>

#include <curve-x/curve.h>

namespace curve_editor_x
{
	using namespace curve_x;

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

	private:
		void _invalidate_layout();

		void _render_title_text();
		void _render_frame();

		void _render_curve_screen();
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

		float _transform_curve_to_screen_x( float x ) const;
		float _transform_curve_to_screen_y( float y ) const;
		Vector2 _transform_curve_to_screen( const Point& point ) const;
		Vector2 _transform_screen_to_curve( const Vector2& pos ) const;

	//  Application settings
	private:
		const Color BACKGROUND_COLOR = LIGHTGRAY;
		const Color TEXT_COLOR = DARKGRAY;
		const Color TEXT_ERROR_COLOR = RED;
		const Color CURVE_FRAME_COLOR = DARKGRAY;
		const Color CURVE_COLOR { 255, 90, 90, 255 };
		const Color TANGENT_COLOR { 120, 120, 120, 255 };
		const Color POINT_COLOR { 255, 0, 0, 255 };
		const Color POINT_SELECTED_COLOR { 255, 255, 255, 255 };
		const Color GRID_LINE_COLOR { 120, 120, 120, 255 };

		const float CURVE_THICKNESS = 2.0f;
		const float POINT_SIZE = CURVE_THICKNESS * 3.0f;
		const float POINT_SELECTED_OFFSET_SIZE = 3.0f;
		const float CURVE_FRAME_PADDING = 32.0f;

		//  In curve units, the gap for each grid line
		const float GRID_SMALL_GAP = 1.0f;
		//  Number of small grid lines to form a larger line
		const float GRID_LARGE_COUNT = 10.0f;
		const float GRID_SMALL_LINE_THICKNESS = 1.0f;
		const float GRID_LARGE_LINE_THICKNESS = 2.0f;

		const float SELECTION_RADIUS = 8.0f;

		const int   TITLE_FONT_SIZE = 20;
		const float TITLE_DOCK_MARGIN_BOTTOM = 4.0f;

		const float ZOOM_SENSITIVITY = 0.1f;
		const float ZOOM_MIN = 0.3f;
		const float ZOOM_MAX = 1.5f;

		const bool  DRAW_MOUSE_POSITION = true;
		const bool  MUST_DOUBLE_CLICK_TO_DRAG_POINT = false;

	private:
		std::string _title {};

		Curve _curve {};
		CurveExtrems _curve_extrems {};

		Rectangle _frame {};

		float _zoom = 1.0f;
		Rectangle _viewport {};
		Rectangle _frame_outline {};

		int _hovered_point_id = -1;
		int _selected_point_id = -1;
		bool _can_drag_selected_point = false;
	};
}