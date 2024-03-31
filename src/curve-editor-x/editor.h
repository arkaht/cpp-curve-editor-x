#pragma once

#include <raylib.h>
#include <raymath.h>

#include <string>

#include <curve-editor-x/utils.h>
#include <curve-editor-x/curve-layer.h>

#include <curve-editor-x/widget-manager.h>
#include <curve-editor-x/curve-layer-row-widget.h>

namespace curve_editor_x
{
	using namespace curve_x;

	enum class CurveInterpolateMode
	{
		/*
		 * Use of raylib functions
		 */
		Bezier,

		/*
		 * Use of time-evaluation
		 */
		TimeEvaluation,

		/*
		 * Use of distance-evaluation
		 */
		DistanceEvaluation,

		MAX,
	};

	class Editor : public WidgetManager
	{
	public:
		Editor( 
			const Rectangle& frame 
		);

		void init();
		void update( float dt );
		void render();

		void fit_viewport_to_curves();

		void set_path( const std::string& path );
		bool export_to_file( 
			const ref<CurveLayer>& layer, 
			const std::string& path 
		);
		bool import_from_file( const std::string& path );

		void unselect_curve_layer();
		void select_curve_layer( int layer_id );

	private:
		void _add_curve_layer( ref<CurveLayer>& layer );
		void _on_curve_layer_row_selected( ref<CurveLayerRowWidget> widget );

		void _invalidate_layout();
		void _invalidate_widgets();
		void _invalidate_grid();

		void _render_title_text();
		void _render_frame();
		//  TODO: Delete
		void _render_layers_tab();

		void _render_curve_screen();
		void _render_curve_by_distance( const ref<CurveLayer>& layer );
		void _render_curve_by_time( const ref<CurveLayer>& layer );
		void _render_curve_by_bezier( const ref<CurveLayer>& layer );
		void _render_curve_points( const ref<CurveLayer>& layer );

		void _render_ui_interpolation_modes();

		void _render_invalid_curve_screen();

		void _render_grid();
		void _render_grid_line( 
			float value,
			bool is_horizontal
		);

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

		ref<CurveLayer> _get_selected_curve_layer();
		Color _get_curve_color_at( int index );
		bool _is_selected_curve_valid() const;
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
		const unsigned char CURVE_UNSELECTED_OPACITY = 80;

		const float QUICK_EVALUATION_THICKNESS = 2.0f;
		const float CURVE_THICKNESS = 2.0f;
		//  How much to offset the thickness per wheel scroll?
		const float CURVE_THICKNESS_SENSITIVITY = 0.5f;
		//  Subdivisions for rendering a curve
		const float CURVE_RENDER_SUBDIVISIONS = 0.01f;
		const float TANGENT_THICKNESS = 2.0f;
		const float POINT_SIZE = CURVE_THICKNESS * 3.0f;
		const float POINT_SELECTED_OFFSET_SIZE = 3.0f;
		const float CURVE_FRAME_PADDING = 32.0f;
		const double DOUBLE_CLICK_TIME = 0.2;

		//  In curve units, the gap for each grid line
		const float GRID_SMALL_GAP = 1.0f;
		//  Number of small grid lines to form a larger line
		const float GRID_LARGE_COUNT = 10.0f;
		//  Levels to snap on for zooming inside the grid
		const float GRID_LEVELS[3] { 1.0f, 2.0f, 5.0f };
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
		//  Does the zoom is clamped between ZOOM_MIN and ZOOM_MAX?
		const bool  IS_ZOOM_CLAMPED = false;

		const std::string DEFAULT_CURVE_PATH = "tests/test.cvx";

	private:
		std::string _title {};
		bool _is_current_file_exists = false;
		bool _is_moving_viewport = false;

		Font _font {};

		std::vector<ref<CurveLayerRowWidget>> _curve_layer_rows {};

		std::vector<ref<CurveLayer>> _curve_layers {};
		int _selected_curve_id = 0;

		CurveExtrems _curve_extrems {};
		CurveInterpolateMode _curve_interpolate_mode 
			= CurveInterpolateMode::TimeEvaluation;

		Rectangle _frame {};

		Vector2 _transformed_mouse_pos {};

		float _zoom = 1.0f;
		Rectangle _viewport {};
		Rectangle _frame_outline {};
		Rectangle _layers_tab {};

		bool _is_grid_snapping = false;
		float _grid_gap = 1.0f;
		std::string _grid_label_format;

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