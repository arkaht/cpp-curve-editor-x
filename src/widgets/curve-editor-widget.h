#pragma once
#include "widget.h"

#include <external/curve-x/curve-serializer.h>

#include <src/settings.h>
#include <src/application.fwd.h>
#include <src/curve-layer.h>
#include <src/curve-interpolate-mode.h>

namespace curve_editor_x
{
	using namespace curve_x;

	class CurveEditorWidget : public Widget
	{
	public:
		CurveEditorWidget( Application* application );

		bool consume_input( const UserInput& input ) override;
		void on_focus_changed( bool is_focused ) override;

		void update( float dt ) override;
		void render() override;

		void invalidate_layout() override;

		void fit_viewport();

	private:
		void _invalidate_grid();

		bool _is_double_clicking( bool should_consume );

		void _add_key_at_position( bool is_alt_down );

		float _transform_curve_to_screen_x( float x ) const;
		float _transform_curve_to_screen_y( float y ) const;
		Vector2 _transform_curve_to_screen( const Point& point ) const;
		Vector2 _transform_screen_to_curve( const Vector2& pos ) const;
		Vector2 _transform_rounded_grid_snap( const Vector2& pos ) const;
		Vector2 _transform_ceiled_grid_snap( const Vector2& pos ) const;

		void _render_title_text();

		void _render_curve_screen();
		void _render_invalid_curve_screen();

		void _render_curve_by_distance( const ref<CurveLayer>& layer );
		void _render_curve_by_time( const ref<CurveLayer>& layer );
		void _render_curve_by_bezier( const ref<CurveLayer>& layer );
		void _render_curve_points( const ref<CurveLayer>& layer );

		void _render_ui_interpolation_modes();

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

	private:
		Application* _application = nullptr;

		float _zoom = 1.0f;
		Rectangle _viewport {};
		Rectangle _viewport_frame {};

		CurveExtrems _curve_extrems {};
		CurveInterpolateMode _curve_interpolate_mode 
			= CurveInterpolateMode::TimeEvaluation;

		Vector2 _transformed_mouse_pos {};

		bool _is_moving_viewport = false;

		bool _is_grid_snapping = false;
		float _grid_gap = 1.0f;
		std::string _grid_label_format;

		bool _is_quick_evaluating = false;
		Vector2 _quick_evaluation_pos {};

		int _hovered_point_id = -1;
		int _selected_point_id = -1;
		bool _is_dragging_point = false;

		bool _is_showing_points = true;
		float _curve_thickness = settings::CURVE_THICKNESS;
		double _last_click_time = 0.0;
	};
}