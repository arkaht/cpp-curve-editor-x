#include "curve-editor-widget.h"

#include <curve-editor-x/application.h>

using namespace curve_editor_x;

CurveEditorWidget::CurveEditorWidget( 
	Application* application 
)
	: _application( application )
{
}

void CurveEditorWidget::update( float dt )
{
	if ( !_application->is_valid_curve_id( _application->get_selected_curve_id() ) ) return;

	auto curve_ref = _application->get_selected_curve_layer();
	Curve& curve = curve_ref->curve;

	Vector2 mouse_pos = GetMousePosition();
	Vector2 mouse_delta = GetMouseDelta();

	bool is_alt_down = IsKeyDown( KEY_LEFT_ALT );
	bool is_valid_selected_point = 
		curve.is_valid_point_id( _selected_point_id );
	bool is_hovered = CheckCollisionPointRec( 
		mouse_pos, frame );

	//  LCTRL-down: Grid snapping
	_is_grid_snapping = IsKeyDown( KEY_LEFT_CONTROL );
	if ( _is_grid_snapping )
	{
		_transformed_mouse_pos = _transform_curve_to_screen(
			_transform_rounded_grid_snap(
				_transform_screen_to_curve( mouse_pos ) 
			)
		);
	}
	else
	{
		_transformed_mouse_pos = mouse_pos;
	}

	//  LSHIFT-down: Quick curve evaluation
	_is_quick_evaluating = IsKeyDown( KEY_LEFT_SHIFT );

	//  RMB: Move viewport around
	if ( IsMouseButtonPressed( MOUSE_BUTTON_RIGHT ) )
	{
		_is_moving_viewport = is_hovered;
	}
	else if ( IsMouseButtonReleased( MOUSE_BUTTON_RIGHT ) )
	{
		_is_moving_viewport = false;
	}
	if ( _is_moving_viewport )
	{
		_viewport.x += mouse_delta.x;
		_viewport.y += mouse_delta.y;
	}

	//  WHEEL
	if ( float mouse_wheel_delta = GetMouseWheelMove() )
	{
		if ( is_hovered )
		{
			//  WHEEL + ALT-down: control curve thickness
			if ( is_alt_down )
			{
				_curve_thickness = fmaxf( 
					CURVE_THICKNESS, 
					_curve_thickness + mouse_wheel_delta * CURVE_THICKNESS_SENSITIVITY 
				);
			}
			//  WHEEL: Zoom to mouse
			else
			{
				float old_zoom = _zoom;

				//  Change zoom scale
				float scale = 1.0f + mouse_wheel_delta * ZOOM_SENSITIVITY;
				if ( IS_ZOOM_CLAMPED )
				{
					_zoom = fminf( 
						ZOOM_MAX, 
						fmaxf( 
							ZOOM_MIN, 
							_zoom * scale
						) 
					);
				}
				else
				{
					_zoom *= scale;
				}
				_invalidate_grid();

				//  Offset viewport to simulate zooming to mouse position
				if ( _zoom != old_zoom )
				{
					Vector2 offset {
						mouse_pos.x - _viewport.x,
						mouse_pos.y - _viewport.y
					};

					float zoom_ratio = mouse_wheel_delta > 0.0f
						? 1.0f / ( old_zoom / _zoom )
						: _zoom / old_zoom;

					_viewport.x += offset.x - offset.x * zoom_ratio;
					_viewport.y += offset.y - offset.y * zoom_ratio;
				}
			}
		}
	}

	//  F1-press: interpolate curve in Bezier
	if ( IsKeyPressed( KEY_F1 ) )
	{
		_curve_interpolate_mode = CurveInterpolateMode::Bezier;
	}
	//  F2-press: interpolate curve in Time
	else if ( IsKeyPressed( KEY_F2 ) )
	{
		_curve_interpolate_mode = CurveInterpolateMode::TimeEvaluation;
	}
	//  F3-press: interpolate curve in Distance
	else if ( IsKeyPressed( KEY_F3 ) )
	{
		_curve_interpolate_mode = CurveInterpolateMode::DistanceEvaluation;
	}
	//  F: Fit viewport to curve
	else if ( IsKeyPressed( KEY_F ) )
	{
		fit_viewport();
	}
	//  TAB: Toggle editor points visibility
	else if ( IsKeyPressed( KEY_TAB ) )
	{
		_is_showing_points = !_is_showing_points;
	}
	//  Suppr: Delete current selected key
	else if ( IsKeyPressed( KEY_DELETE ) 
		&& is_valid_selected_point 
		&& curve.get_keys_count() > 2 )
	{
		int key_id = curve.point_to_key_id( _selected_point_id );
		curve.remove_key( key_id );
		curve_ref->has_unsaved_changes = true;
	}

	//  Find the mouse hovered point
	_hovered_point_id = -1;
	for ( int i = 0; i < curve.get_points_count(); i++ )
	{
		const Point& point = curve.get_point( i, PointSpace::Global );
		const Vector2 screen_point = _transform_curve_to_screen( 
			point );

		if ( CheckCollisionPointCircle( 
			mouse_pos,
			screen_point, 
			SELECTION_RADIUS
		) )
		{
			_hovered_point_id = i;
			break;
		}
	}

	if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) )
	{
		//  Double LMB-pressed: add a key at position
		if ( _is_double_clicking( true ) )
		{
			CurveKey key( 
				_transform_screen_to_curve( _transformed_mouse_pos )
			);

			//  ALT-down: insert key
			if ( is_alt_down )
			{
				//  TODO: fix this feature and Curve::compute_length
				//  Find distance on curve from point
				float dist = curve.get_nearest_distance_to( 
					key.control );

				//  Find the key index to insert from
				int first_key_id, last_key_id;
				curve.find_evaluation_keys_id_by_distance( 
					&first_key_id, &last_key_id, dist );

				printf( "=> %d:%d\n", first_key_id, last_key_id );

				//  Insert and select the point
				curve.insert_key( last_key_id, key );
				_selected_point_id = curve.key_to_point_id( 
					last_key_id );
			}
			//  NO ALT-down: add key
			else
			{
				curve.add_key( key );
				_selected_point_id = curve.get_points_count() - 1;
			}

			curve_ref->has_unsaved_changes = true;
		}
		//  LMB-pressed: Select hovered point
		else
		{
			_can_drag_selected_point = 
				!MUST_DOUBLE_CLICK_TO_DRAG_POINT 
			 || _hovered_point_id == _selected_point_id;

			_selected_point_id = _hovered_point_id;
		}
	}
	else if ( IsMouseButtonReleased( MOUSE_BUTTON_LEFT ) )
	{
		_can_drag_selected_point = true;
	}
	//  LMB-down: Move selected point
	else if ( IsMouseButtonDown( MOUSE_BUTTON_LEFT )
		&& is_valid_selected_point 
		&& _can_drag_selected_point )
	{
		//  Translate mouse screen-position to curve-position
		Point new_point = _transform_screen_to_curve( _transformed_mouse_pos );

		//  Tangents use a different function to apply the tangent 
		//  mode constraint.
		if ( !curve.is_control_point_id( _selected_point_id ) )
		{
			curve.set_tangent_point( 
				_selected_point_id,
				new_point,
				PointSpace::Global
			);
		}
		else
		{
			curve.set_point(
				_selected_point_id, 
				new_point
			);
		}

		curve_ref->has_unsaved_changes = true;
	}
	//  MMB-press: Switch tangent mode
	else if ( IsMouseButtonPressed( MOUSE_BUTTON_MIDDLE ) 
		&& is_valid_selected_point )
	{
		int key_id = 
			curve.point_to_key_id( _selected_point_id );
		TangentMode tangent_mode = 
			curve.get_tangent_mode( key_id );

		//  Cycle through tangent modes
		TangentMode next_tangent_mode = 
			(int)tangent_mode + 1 == (int)TangentMode::MAX
			? (TangentMode)0
			: (TangentMode)( (int)tangent_mode + 1 );

		//  Apply the new tangent constraint
		curve.set_tangent_mode( key_id, next_tangent_mode );
		curve_ref->has_unsaved_changes = true;
	}
	else if ( curve.is_length_dirty ) 
	{
		curve.compute_length();
	}

	//  Quick curve evaluation
	if ( _is_quick_evaluating )
	{
		Point new_point = _transform_screen_to_curve( 
			_transformed_mouse_pos );
		
		//  Evaluate at mouse position
		switch ( _curve_interpolate_mode )
		{
			case CurveInterpolateMode::Bezier:
			case CurveInterpolateMode::DistanceEvaluation:
				_quick_evaluation_pos = curve.get_nearest_point_to(
					new_point
				);
				break;

			case CurveInterpolateMode::TimeEvaluation:
				_quick_evaluation_pos.x = new_point.x;
				_quick_evaluation_pos.y = curve.evaluate_by_time( 
					new_point.x );
				break;
		}

		float dist = curve.get_nearest_distance_to( 
			_transform_screen_to_curve( 
				_transformed_mouse_pos 
			) 
		);

		int first_key_id, last_key_id;
		curve.find_evaluation_keys_id_by_distance( 
			&first_key_id, &last_key_id, dist );

		printf( "%d:%d\n", first_key_id, last_key_id );
	}
}

void CurveEditorWidget::render()
{
	_render_title_text();

	//  Draw viewport frame
	DrawRectangleLinesEx( _viewport_frame, 2.0f, GRAY );

	//  Enable clipping
	if ( ENABLE_CLIPPING )
	{
		BeginScissorMode( 
			(int)_viewport_frame.x, (int)_viewport_frame.y, 
			(int)_viewport_frame.width, (int)_viewport_frame.height 
		);
	}

	//  Draw in-frame
	const auto& layer = _application->get_selected_curve_layer();
	if ( layer->curve.is_valid() ) 
	{
		_render_curve_screen();
	}
	else
	{
		_render_invalid_curve_screen();
	}

	//  Disable clipping
	if ( ENABLE_CLIPPING )
	{
		EndScissorMode();
	}

	//  DEBUG: Draw frame
	//DrawRectangleLinesEx( frame, 2.0f, PINK );
	//  DEBUG: Draw viewport
	//DrawRectangleLinesEx( _viewport, 2.0f, PINK );
}

void CurveEditorWidget::invalidate_layout()
{
	//  Update viewport frame
	const float offset_y = 
		TITLE_FONT_SIZE + TITLE_DOCK_MARGIN_BOTTOM;
	_viewport_frame.x = frame.x;
	_viewport_frame.y = frame.y + offset_y;
	_viewport_frame.width = frame.width;
	_viewport_frame.height = frame.height - offset_y;

	//  Update viewport
	const float ui_height = 32.0f;
	_viewport.x = _viewport_frame.x + CURVE_FRAME_PADDING;
	_viewport.y = _viewport_frame.y + CURVE_FRAME_PADDING + ui_height;
	_viewport.width = _viewport_frame.width - CURVE_FRAME_PADDING * 2.0f;
	_viewport.height = _viewport_frame.height - CURVE_FRAME_PADDING * 2.0f - ui_height;

	_invalidate_grid();
}

void CurveEditorWidget::fit_viewport()
{
	_curve_extrems = CurveExtrems {};
	auto layers = _application->get_curve_layers();
	for ( const auto& layer : layers )
	{
		if ( layer->curve.is_valid() )
		{
			auto extrems = layer->curve.get_extrems();
			_curve_extrems.min_x = 
				std::min( _curve_extrems.min_x, extrems.min_x );
			_curve_extrems.max_x = 
				std::max( _curve_extrems.max_x, extrems.max_x );
			_curve_extrems.min_y = 
				std::min( _curve_extrems.min_y, extrems.min_y );
			_curve_extrems.max_y = 
				std::max( _curve_extrems.max_y, extrems.max_y );
		}
	}
	_zoom = 1.0f;

	invalidate_layout();
}

void CurveEditorWidget::_invalidate_grid()
{
	//  Determine visible range in curve units
	const Vector2& left_pos = _transform_screen_to_curve( 
		{ frame.x, 0.0f } );
	const Vector2& right_pos = _transform_screen_to_curve(
		{ frame.x + frame.width, 0.0f } );
	const float visible_range = right_pos.x - left_pos.x;

	/*printf( "inputs: visible_range=%f; zoom=%f\n", 
		visible_range, _zoom );*/

	//  Subdivide visible range
	_grid_gap = visible_range / ( GRID_LARGE_COUNT * GRID_SMALL_GAP );

	//  Get level power depending on zoom range
	float level_power = 0.0f;
	if ( _grid_gap >= 1.0f )
	{
		std::string str_grid_gap = std::to_string( (int)floorf( _grid_gap ) );
		//printf( "%s\n", str_grid_gap.c_str() );

		level_power = (float)( str_grid_gap.length() - 1 );

		//  Update label format
		_grid_label_format = "%.0f";
	}
	else
	{
		std::string str_grid_gap = std::to_string( _grid_gap );
		//printf( "%s\n", str_grid_gap.c_str() );

		//  Count number of zeros until first digit
		int zero_count = 0;
		for ( char letter : str_grid_gap )
		{
			if ( letter == '.' ) continue;
			if ( letter != '0' ) break;

			zero_count++;
		}

		level_power = (float)( -zero_count );
		
		//  Update label format
		_grid_label_format = "%.0" + std::to_string( zero_count ) + "f";
	}

	//  Snap grid gap to closest level
	float closest_dist = visible_range;
	float closest_level = 0; 
	for ( float level : GRID_LEVELS )
	{
		float exponent = powf( 10.0f, level_power );
		level *= exponent;

		float dist = fabsf( _grid_gap - level );
		if ( dist < closest_dist )
		{
			closest_level = level;
			closest_dist = dist;
		}
	}
	_grid_gap = closest_level;

	//printf( "output: grid_gap=%f\n", _grid_gap );
}

bool CurveEditorWidget::_is_double_clicking( bool should_consume )
{
	double time = GetTime();
	bool is_double_clicking = time - _last_click_time <= DOUBLE_CLICK_TIME;

	if ( should_consume )
	{
		_last_click_time = is_double_clicking
			? 0.0
			: time;
	}

	return is_double_clicking;
}

float CurveEditorWidget::_transform_curve_to_screen_x( float x ) const
{
	return curve_x::Utils::remap( 
		x, 
		_curve_extrems.min_x, _curve_extrems.max_x, 
		_viewport.x, _viewport.x + _viewport.width * _zoom
	);
}

float CurveEditorWidget::_transform_curve_to_screen_y( float y ) const
{
	return curve_x::Utils::remap( 
		y, 
		_curve_extrems.min_y, _curve_extrems.max_y, 
		_viewport.y + _viewport.height * _zoom, _viewport.y
	);
}

Vector2 CurveEditorWidget::_transform_curve_to_screen( 
	const Point& point 
) const
{	
	return Vector2 {
		_transform_curve_to_screen_x( point.x ),
		_transform_curve_to_screen_y( point.y ),
	};
}

Vector2 CurveEditorWidget::_transform_screen_to_curve( 
	const Vector2& pos 
) const
{
	Point to_curve = pos;
	return to_curve.remap(
		//  x-mapping
		_viewport.x, _viewport.x + _viewport.width * _zoom,
		_curve_extrems.min_x, _curve_extrems.max_x, 
		//  y-mapping
		_viewport.y + _viewport.height * _zoom, _viewport.y,
		_curve_extrems.min_y, _curve_extrems.max_y
	);
}

Vector2 CurveEditorWidget::_transform_rounded_grid_snap( 
	const Vector2& pos 
) const
{
	return Vector2 {
		roundf( pos.x / _grid_gap ) * _grid_gap,
		roundf( pos.y / _grid_gap ) * _grid_gap,
	};
}

Vector2 CurveEditorWidget::_transform_ceiled_grid_snap( 
	const Vector2& pos 
) const
{
	return Vector2 {
		ceilf( pos.x / _grid_gap ) * _grid_gap,
		ceilf( pos.y / _grid_gap ) * _grid_gap,
	};
}

void CurveEditorWidget::_render_title_text()
{
	std::string title = "No selected curve";
	std::string keys = "";

	int selected_curve_id = _application->get_selected_curve_id();
	if ( _application->is_valid_curve_id( selected_curve_id ) )
	{
		auto layer = _application->get_curve_layer( selected_curve_id );
		
		//  Format title
		title = layer->name;
		if ( layer->has_unsaved_changes )
		{
			title += "*";
		}

		//  Format keys count text
		keys = TextFormat( "%d keys", layer->curve.get_keys_count() );
	}

	//  Draw title text
	DrawText(
		title.c_str(),
		(int)frame.x, (int)frame.y,
		TITLE_FONT_SIZE,
		TEXT_COLOR
	);

	const char* keys_text = keys.c_str();
	int points_width = MeasureText( keys_text, TITLE_FONT_SIZE );

	//  Draw keys count
	DrawText(
		keys_text,
		(int)frame.x + (int)frame.width - points_width,
		(int)frame.y,
		TITLE_FONT_SIZE,
		TEXT_COLOR
	);
}

void CurveEditorWidget::_render_curve_screen()
{
	const auto& selected_layer = _application->get_selected_curve_layer();
	int points_count = selected_layer->curve.get_points_count();

	//  Draw grid
	_render_grid();

	//  Draw mouse position
	if ( DRAW_MOUSE_POSITION )
	{
		DrawCircleV( 
			_transformed_mouse_pos, 
			SELECTION_RADIUS, 
			TEXT_COLOR 
		);
	}

	//  Draw curve layers
	auto layers = _application->get_curve_layers();
	for ( const auto& layer : layers )
	{
		switch ( _curve_interpolate_mode )
		{
			case CurveInterpolateMode::Bezier:
				_render_curve_by_bezier( layer );
				break;
			case CurveInterpolateMode::TimeEvaluation:
				_render_curve_by_time( layer );
				break;
			case CurveInterpolateMode::DistanceEvaluation:
				_render_curve_by_distance( layer );
				break;
		}
	}

	//  Draw points
	if ( _is_showing_points )
	{
		_render_curve_points( selected_layer );
	}

	//  Draw quick evaluation
	if ( _is_quick_evaluating )
	{
		const float font_size = 20.0f;
		Vector2 pos = _transform_curve_to_screen( 
			_quick_evaluation_pos );
		
		switch ( _curve_interpolate_mode )
		{
			case CurveInterpolateMode::Bezier:
			case CurveInterpolateMode::DistanceEvaluation:
				DrawLineEx( 
					_transformed_mouse_pos,
					pos,
					QUICK_EVALUATION_THICKNESS,
					QUICK_EVALUATION_COLOR
				);
				break;

			case CurveInterpolateMode::TimeEvaluation:
				DrawLineEx(
					Vector2 {
						pos.x,
						frame.y,
					},
					Vector2 {
						pos.x,
						frame.y + frame.height,
					},
					QUICK_EVALUATION_THICKNESS,
					QUICK_EVALUATION_COLOR
				);
				break;
		}
		
		DrawCircleV( 
			pos, 
			QUICK_EVALUATION_THICKNESS * 2.0f, 
			QUICK_EVALUATION_COLOR 
		);

		DrawTextEx(
			GetFontDefault(),
			TextFormat( "x=%.3f\ny=%.3f",
				_quick_evaluation_pos.x, _quick_evaluation_pos.y ),
			Vector2 {
				pos.x + 10.0f,
				pos.y,
			},
			font_size,
			2.0f,
			QUICK_EVALUATION_COLOR
		);
	}

	//  Draw interpolation modes
	_render_ui_interpolation_modes();
}

void CurveEditorWidget::_render_invalid_curve_screen()
{
	const auto& selected_layer = _application->get_selected_curve_layer();
	int keys_count = selected_layer->curve.get_keys_count();

	//  Setup text strings
	const int TEXT_SIZE = 2;
	const char* texts[TEXT_SIZE] {
		"INVALID KEYS COUNT!",
		TextFormat( 
			"%d keys instead of 2 at minimum", 
			keys_count
		),
	};

	//  Setup text rendering
	float font_size = 20.0f;
	float spacing = 2.0f;
	Vector2 pos {
		_viewport_frame.x + _viewport_frame.width * 0.5f,
		_viewport_frame.y + _viewport_frame.height * 0.5f
	};

	//  Draw all lines
	for ( int i = 0; i < TEXT_SIZE; i++ )
	{
		const char* text = texts[i];

		//  Measure line size
		Vector2 text_size = MeasureTextEx( 
			GetFontDefault(), 
			text, 
			font_size, 
			spacing 
		);

		//  Draw centered line
		DrawTextPro( 
			GetFontDefault(),
			text, 
			Vector2 {
				pos.x - text_size.x * 0.5f,
				pos.y - text_size.y * 0.5f,
			},
			Vector2 {},
			0.0f,
			font_size,
			spacing,
			TEXT_ERROR_COLOR
		);

		//  Offset next line
		pos.y += text_size.y + 2.0f;
	}
}

void CurveEditorWidget::_render_curve_by_distance( 
	const ref<CurveLayer>& layer 
)
{
	//  Draw curve's length
	const float length = layer->curve.get_length();
	/*DrawText(
		TextFormat( "length: %.3f", length ),
		(int) ( _viewport_frame.x + 25 + CURVE_FRAME_PADDING * 0.5f ),
		(int) ( _viewport_frame.y + 25 + CURVE_FRAME_PADDING * 0.5f ),
		20,
		TEXT_COLOR
	);*/

	const Color color {
		layer->color.r,
		layer->color.g,
		layer->color.b,
		layer->is_selected 
			? CURVE_SELECTED_OPACITY 
			: CURVE_UNSELECTED_OPACITY
	};

	Vector2 previous_pos = _transform_curve_to_screen(
		layer->curve.evaluate_by_distance( 0.0f ) );

	//  Draw curve using distance-evaluation
	const float step = length * CURVE_RENDER_SUBDIVISIONS;
	for ( float dist = step; dist < length; dist += step )
	{
		const Vector2 pos = _transform_curve_to_screen(
			layer->curve.evaluate_by_distance( dist ) );

		//  Draw line
		DrawLineEx(
			previous_pos,
			pos,
			_curve_thickness,
			color
		);

		previous_pos = pos;
	}
}

void CurveEditorWidget::_render_curve_by_time( 
	const ref<CurveLayer>& layer 
)
{
	const int points_count = layer->curve.get_points_count();

	const Color color {
		layer->color.r,
		layer->color.g,
		layer->color.b,
		layer->is_selected 
			? CURVE_SELECTED_OPACITY 
			: CURVE_UNSELECTED_OPACITY
	};

	//  Determine bounds and steps
	const float min_x = layer->curve.get_point( 0 ).x;
	const float max_x = layer->curve.get_point( points_count - 1 ).x;
	const float step = ( max_x - min_x ) * CURVE_RENDER_SUBDIVISIONS;

	Vector2 previous_pos = _transform_curve_to_screen(
		layer->curve.evaluate_by_percent( 0.0f ) );

	//  Draw curve using time-evaluation
	for ( float x = min_x; x < max_x + step; x += step )
	{
		const Vector2 pos = _transform_curve_to_screen(
			Point {
				x,
				layer->curve.evaluate_by_time( x ),
			}
		);

		//  Draw line
		DrawLineEx(
			previous_pos,
			pos,
			_curve_thickness,
			color
		);

		previous_pos = pos;
	}
}

void CurveEditorWidget::_render_curve_by_bezier( 
	const ref<CurveLayer>& layer 
)
{
	int points_count = layer->curve.get_points_count();

	const Color color {
		layer->color.r,
		layer->color.g,
		layer->color.b,
		layer->is_selected 
			? CURVE_SELECTED_OPACITY 
			: CURVE_UNSELECTED_OPACITY
	};

	for ( int i = 0; i < points_count - 1; i += 3 )
	{
		//  Get points
		Point p0 = layer->curve.get_point( i );
		Point t0 = layer->curve.get_point( i + 1 );
		Point t1 = layer->curve.get_point( i + 2 );
		Point p1 = layer->curve.get_point( i + 3 );

		//  Translate them from curve-space to screen-space
		Vector2 pos0 = _transform_curve_to_screen( p0 );
		Vector2 pos1 = _transform_curve_to_screen( p0 + t0 );
		Vector2 pos2 = _transform_curve_to_screen( p1 + t1 );
		Vector2 pos3 = _transform_curve_to_screen( p1 );

		//  Draw curve
		DrawSplineSegmentBezierCubic(
			pos0,
			pos1,
			pos2,
			pos3,
			_curve_thickness,
			color
		);
	}
}

void CurveEditorWidget::_render_curve_points( 
	const ref<CurveLayer>& layer 
)
{
	int keys_count = layer->curve.get_keys_count();

	for ( int key_id = 0; key_id < keys_count; key_id++ )
	{
		int control_point_id = layer->curve.key_to_point_id( key_id );

		//  Get control point
		const CurveKey& key = layer->curve.get_key( key_id );
		const Vector2& control_pos =
			_transform_curve_to_screen( key.control );

		//  Draw left tangent
		if ( key_id > 0 )
		{
			const Vector2& tangent_pos = _transform_curve_to_screen(
				key.control + key.left_tangent );

			DrawLineEx(
				control_pos,
				tangent_pos,
				TANGENT_THICKNESS,
				TANGENT_COLOR
			);

			_render_point( control_point_id - 1, tangent_pos );
		}

		//  Draw right tangent
		if ( key_id < keys_count - 1 )
		{
			const Vector2& tangent_pos = _transform_curve_to_screen(
				key.control + key.right_tangent
			);

			DrawLineEx(
				control_pos,
				tangent_pos,
				TANGENT_THICKNESS,
				TANGENT_COLOR
			);

			_render_point( control_point_id + 1, tangent_pos );
		}

		_render_point( control_point_id, control_pos );
	}
}

void CurveEditorWidget::_render_ui_interpolation_modes()
{
	const float margin = 16.0f;
	const float background_padding = 4.0f;

	Vector2 pos {
		_viewport_frame.x + CURVE_FRAME_PADDING * 1.0f,
		_viewport_frame.y + CURVE_FRAME_PADDING * 0.75f
	};

	for ( int i = 0; i < (int)CurveInterpolateMode::MAX; i++ )
	{
		bool is_selected = (CurveInterpolateMode)i == _curve_interpolate_mode;

		std::string text = "F" + std::to_string( i + 1 );
		Color text_color { 210, 210, 210, 255 };
		Color background_color { 170, 170, 170, 255 };
		Color background_outline_color = GRAY;

		if ( is_selected )
		{
			switch ( _curve_interpolate_mode )
			{
				case CurveInterpolateMode::Bezier:
					text = "AUTO";
					text_color = RED;
					break;
				case CurveInterpolateMode::TimeEvaluation:
					text = "TIME";
					text_color = Color { 0, 200, 0, 255 };
					break;
				case CurveInterpolateMode::DistanceEvaluation:
					text = "DISTANCE";
					text_color = BLUE;
					break;
			}

			background_color = Color { 190, 190, 190, 255 };
			background_outline_color = { 160, 160, 160, 255 };
		}

		//  Measure text size
		const char* text_cstr = text.c_str();
		const float font_size = is_selected ? 20.0f : 16.0f;
		const float spacing = 1.0f;
		Vector2 text_size = 
			MeasureTextEx( GetFontDefault(), text_cstr, font_size, spacing );

		//  Draw background
		const Rectangle background_rect {
			pos.x - background_padding * 2.0f,
			pos.y - background_padding,
			text_size.x + background_padding * 4.0f,
			text_size.y + background_padding * 2.0f,
		};
		DrawRectangleRec( 
			background_rect, 
			background_color 
		);
		DrawRectangleLinesEx( background_rect, 2.0f, background_outline_color );

		//  Draw text
		DrawTextEx(
			GetFontDefault(),
			text_cstr,
			pos,
			font_size,
			spacing,
			text_color
		);

		//  Offset X-pos
		pos.x += text_size.x + margin + background_padding * 2.0f;
	}
}

void CurveEditorWidget::_render_grid()
{
	//  Find in-frame curve coordinates extrems, these positions
	//  will be used to draw our grid in a performant way where
	//  only visible grid lines will be rendered
	Vector2 curve_top_left = _transform_screen_to_curve( { 
		_viewport_frame.x, 
		_viewport_frame.y 
	} );
	Vector2 curve_bottom_right = _transform_screen_to_curve( { 
		_viewport_frame.x + _viewport_frame.width, 
		_viewport_frame.y + _viewport_frame.height 
	} );

	//  Grid-snap positions 
	//  (ceiling instead of rounding fixes sudden lines appearing)
	curve_top_left = _transform_ceiled_grid_snap( curve_top_left );
	curve_bottom_right = _transform_ceiled_grid_snap( curve_bottom_right );

	//  Draw vertical lines
	for ( 
		float x = curve_top_left.x; 
		      x < curve_bottom_right.x; 
		      x += _grid_gap 
	)
	{
		_render_grid_line( x, false );
	}

	//  Draw horizontal lines
	for ( 
		float y = curve_bottom_right.y; 
		      y < curve_top_left.y; 
		      y += _grid_gap 
	)
	{
		_render_grid_line( y, true );
	}
}

void CurveEditorWidget::_render_grid_line( float value, bool is_horizontal )
{
	float screen_value = is_horizontal 
		? _transform_curve_to_screen_y( value )
		: _transform_curve_to_screen_x( value );

	//  Fix some weird cases where zero was negative
	if ( Utils::near_zero( value ) )
	{
		value = 0.0f;
	}

	//  Determine style depending on line type
	bool is_large_line = Utils::near_zero( fmodf( 
		value, GRID_LARGE_COUNT * _grid_gap ) );
	float font_size = is_large_line 
		? GRID_LARGE_GRID_FONT_SIZE 
		: GRID_SMALL_GRID_FONT_SIZE;
	float line_thickness = is_large_line 
		? GRID_LARGE_LINE_THICKNESS 
		: GRID_SMALL_LINE_THICKNESS;

	//  Compute text & its size
	const char* text = TextFormat( _grid_label_format.c_str(), value );
	Vector2 text_size = MeasureTextEx( 
		GetFontDefault(), text, font_size, GRID_FONT_SPACING );

	//  Determine positions
	Vector2 text_pos {};
	Vector2 line_start_pos {};
	Vector2 line_end_pos {};
	if ( is_horizontal )
	{
		text_pos.x = _viewport_frame.x + GRID_TEXT_PADDING * 2.0f;
		text_pos.y = screen_value - text_size.y * 0.5f;

		line_start_pos.x = _viewport_frame.x 
			+ GRID_TEXT_PADDING * 4.0f + text_size.x;
		line_start_pos.y = screen_value;

		line_end_pos.x = _viewport_frame.x + _viewport_frame.width;
		line_end_pos.y = screen_value;
	}
	else
	{
		text_pos.x = screen_value - text_size.x * 0.5f;
		text_pos.y = _viewport_frame.y + GRID_TEXT_PADDING;

		line_start_pos.x = screen_value;
		line_start_pos.y = _viewport_frame.y 
			+ GRID_TEXT_PADDING + text_size.y;

		line_end_pos.x = screen_value;
		line_end_pos.y = _viewport_frame.y + _viewport_frame.height;
	}

	//  Draw line
	DrawLineEx( 
		line_start_pos,
		line_end_pos,
		line_thickness,
		GRID_LINE_COLOR
	);

	//  Draw label
	DrawTextEx( 
		GetFontDefault(),
		text, 
		text_pos,
		font_size,
		GRID_FONT_SPACING,
		GRID_LINE_COLOR
	);
}

void CurveEditorWidget::_render_point( int point_id, const Vector2& pos )
{
	const auto& selected_layer = _application->get_selected_curve_layer();
	bool is_tangent = !selected_layer->curve.is_control_point_id( point_id );
	bool is_hovered = point_id == _hovered_point_id 
				   || point_id == _selected_point_id;
	bool is_selected = point_id == _selected_point_id;

	//  Choose color
	Color color;
	if ( is_hovered )
	{
		color = POINT_SELECTED_COLOR;
	}
	else
	{
		color = is_tangent
			? TANGENT_COLOR
			: POINT_COLOR;
	}

	//  Draw point
	if ( is_tangent )
	{
		int key_id = selected_layer->curve.point_to_key_id( point_id );
		TangentMode mode = selected_layer->curve.get_tangent_mode( key_id );

		//  Draw point depending on mode
		const char* mode_name = "N/A";
		switch ( mode )
		{
			case TangentMode::Mirrored:
				_render_circle_point( pos, color, is_selected );
				mode_name = "mirrored";
				break;
				
			case TangentMode::Aligned:
				_render_circle_point( pos, color, is_selected );
				mode_name = "aligned";
				break;

			case TangentMode::Broken:
				_render_square_point( pos, color, is_selected );
				mode_name = "broken";
				break;
		}

		//  Draw mode name
		if ( is_selected )
		{
			float font_size = 20.0f;
			float spacing = 2.0f;

			//  measure text size
			Vector2 size = MeasureTextEx( 
				GetFontDefault(), 
				mode_name,
				font_size,
				spacing
			);

			//  draw text
			DrawTextEx( 
				GetFontDefault(),
				mode_name,
				Vector2 {
					pos.x - size.x * 0.5f,
					pos.y - size.y * 1.5f,
				},
				font_size,
				spacing,
				TANGENT_COLOR
			);
		}
	}
	else
	{
		_render_circle_point( pos, color, is_selected );
	}

	if ( is_selected && !_is_quick_evaluating )
	{
		const Point& point = selected_layer->curve.get_point( point_id );

		//  Draw coordinates
		DrawTextEx( 
			GetFontDefault(),
			TextFormat( "x=%.3f\ny=%.3f", point.x, point.y ),
			Vector2 {
				pos.x + POINT_SIZE * 2.0f,
				pos.y - POINT_SIZE,
			},
			20.0f,
			1.0f,
			TANGENT_COLOR
		);
	}
}

void CurveEditorWidget::_render_circle_point( 
	const Vector2& pos, 
	const Color& color,
	bool is_selected
)
{
	//  Draw point
	DrawCircleV( 
		pos, 
		POINT_SIZE * 0.5f, 
		color
	);

	//  Draw selected
	if ( is_selected )
	{
		DrawCircleLinesV( 
			pos, 
			POINT_SIZE * 0.5f + POINT_SELECTED_OFFSET_SIZE, 
			POINT_SELECTED_COLOR 
		);
	}
}

void CurveEditorWidget::_render_square_point( 
	const Vector2& pos, 
	const Color& color,
	bool is_selected
)
{
	int size = (int)POINT_SIZE;

	//  Draw point
	DrawRectangle( 
		(int)pos.x - size / 2,
		(int)pos.y - size / 2,
		size,
		size,
		color
	);

	//  Draw selected
	if ( is_selected )
	{
		size += (int)( POINT_SELECTED_OFFSET_SIZE + 1.0f );

		DrawRectangleLines(
			(int)pos.x - size / 2,
			(int)pos.y - size / 2,
			size,
			size,
			POINT_SELECTED_COLOR
		);
	}
}
