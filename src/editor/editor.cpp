#include "editor.h"

using namespace curve_editor_x;

Editor::Editor( const Rectangle& frame )
	: _frame( frame )
{}

void Editor::init()
{
	_title = "curve-editor-test.cvx";

	_curve.add_point( { 0.0f, 1.0f } );
	_curve.add_point( { 0.75f, 5.0f } );

	_curve.add_point( { -2.0f, -2.0f } );
	_curve.add_point( { 4.0f, 2.0f } );
	_curve.add_point( { 1.0f, -1.0f } );

	_curve.add_point( { -1.0f, -0.5f } );
	_curve.add_point( { 6.0f, 2.0f } );
	_curve.add_point( { 10.0f, 0.0f } );

	_curve.add_point( { -1.0f, -0.5f } );
	_curve.add_point( { 10.0f, -3.0f } );
	_curve.add_point( { 1.0f, 0.5f } );

	_curve.add_point( { -5.0f, 0.0f } );
	_curve.add_point( { 15.0f, -5.0f } );

	/*curve.add_point( { 0.0f, 0.0f } );
	curve.add_point( { 100.0f, 50.0f } );
	curve.add_point( { 350.0f, 300.0f } );
	curve.add_point( { 500.0f, 0.0f } );*/

	fit_viewport_to_curve();
}

void Editor::update( float dt )
{
	if ( !_curve.is_valid() ) return;

	//printf( "dt:%f\n", dt );

	Vector2 mouse_pos = GetMousePosition();
	Vector2 mouse_delta = GetMouseDelta();

	/*Vector2 viewport_mouse_pos = _transform_screen_to_viewport( mouse_pos );
	printf( "x:%f | y:%f\n", 
		viewport_mouse_pos.x, viewport_mouse_pos.y );*/

	//  RMB: move viewport around
	if ( IsMouseButtonDown( MOUSE_BUTTON_RIGHT ) )
	{
		_viewport.x += mouse_delta.x;
		_viewport.y += mouse_delta.y;
	}
	//  WHEEL: zoom to mouse
	if ( float mouse_wheel_delta = GetMouseWheelMove() )
	{
		float old_zoom = _zoom;

		//  change zoom scale
		_zoom = fminf( 
			ZOOM_MAX, 
			fmaxf( 
				ZOOM_MIN, 
				_zoom + mouse_wheel_delta * ZOOM_SENSITIVITY 
			) 
		);

		//  offset viewport simulating zoom to mouse position
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
	//  F: fit viewport to curve
	if ( IsKeyPressed( KEY_F ) )
	{
		fit_viewport_to_curve();
	}

	//  find the mouse hovered point
	_hovered_point_id = -1;
	for ( int i = 0; i < _curve.get_points_count(); i++ )
	{
		const Point& point = _curve.get_global_point( i );
		const Vector2 screen_point = _transform_curve_to_screen( point );

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

	//  LMB-pressed: select hovered point
	if ( IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) )
	{
		_can_drag_selected_point = 
			!MUST_DOUBLE_CLICK_TO_DRAG_POINT 
		 || _hovered_point_id == _selected_point_id;

		_selected_point_id = _hovered_point_id;
	}
	else if ( IsMouseButtonReleased( MOUSE_BUTTON_LEFT ) )
	{
		_can_drag_selected_point = true;
	}
	//  LMB-down: move selected point
	else if ( IsMouseButtonDown( MOUSE_BUTTON_LEFT )
		&& _curve.is_valid_point_id( _selected_point_id ) 
		&& _can_drag_selected_point )
	{
		/*Point point = _curve.get_point( _selected_point_id );
		point.x += mouse_delta.x / _viewport.width * _zoom;
		point.y += mouse_delta.y / _viewport.height * _zoom;*/

		Point new_point = _transform_screen_to_curve( mouse_pos );

		//  Tangents are in local space so we need to convert them
		//  from global space. They also use a different function
		//  to apply the tangent mode.
		if ( !_curve.is_control_point( _selected_point_id ) )
		{
			int control_point_id = 
				_curve.get_control_point_id( _selected_point_id );
			new_point = 
				new_point - _curve.get_point( control_point_id );

			_curve.set_tangent_point( 
				_selected_point_id,
				new_point 
			);
		}
		else
		{
			_curve.set_point(
				_selected_point_id, 
				new_point
			);
		}
	}
	else if ( IsMouseButtonPressed( MOUSE_BUTTON_MIDDLE ) 
		&& _curve.is_valid_point_id( _selected_point_id ) )
	{
		int tangent_id = 
			_curve.get_point_tangent_id( _selected_point_id );
		TangentMode tangent_mode = 
			_curve.get_tangent_mode( tangent_id );

		TangentMode next_tangent_mode = 
			(int)tangent_mode + 1 == (int)TangentMode::MAX
			? (TangentMode)0
			: (TangentMode)( (int)tangent_mode + 1 );

		_curve.set_tangent_mode( tangent_id, next_tangent_mode );
	}
}

void Editor::render()
{
	ClearBackground( BACKGROUND_COLOR );

	int points_count = _curve.get_points_count();

	//  draw title
	{
		const char* title_c_str = _title.c_str();
		DrawText( 
			title_c_str, 
			(int)_frame.x, (int)_frame.y, 
			TITLE_FONT_SIZE, 
			TEXT_COLOR 
		);

		const char* points_c_str = 
			TextFormat( "%d points", points_count );
		int points_width = 
			MeasureText( points_c_str, TITLE_FONT_SIZE );
		DrawText(
			points_c_str,
			(int)_frame.x + (int)_frame.width - points_width,
			(int)_frame.y,
			TITLE_FONT_SIZE,
			TEXT_COLOR
		);
	}
	//  draw frame
	DrawRectangleLinesEx( 
		_frame_outline, 
		2.0f, 
		CURVE_FRAME_COLOR 
	);

	if ( _curve.is_valid() ) 
	{
		BeginScissorMode( 
			(int)_frame_outline.x, (int)_frame_outline.y, 
			(int)_frame_outline.width, (int)_frame_outline.height 
		);

		//  draw mouse position
		if ( DRAW_MOUSE_POSITION )
		{
			Vector2 mouse_pos = GetMousePosition();
			DrawCircleV( mouse_pos, SELECTION_RADIUS, TEXT_COLOR );
		}

		/*printf( "x(%f -> %f) | y(%f -> %f)\n", 
			extrems.min_x, extrems.max_x, 
			extrems.min_y, extrems.max_y );*/

		//  draw curves
		for ( int i = 0; i < points_count - 1; i += 3 )
		{
			//  get points
			Point p0 = _curve.get_point( i );
			Point t0 = _curve.get_point( i + 1 );
			Point t1 = _curve.get_point( i + 2 );
			Point p1 = _curve.get_point( i + 3 );

			Vector2 pos0 = _transform_curve_to_screen( p0 );
			Vector2 pos1 = _transform_curve_to_screen( p0 + t0 );
			Vector2 pos2 = _transform_curve_to_screen( p1 + t1 );
			Vector2 pos3 = _transform_curve_to_screen( p1 );

			//  draw spline
			DrawSplineSegmentBezierCubic(
				pos0,
				pos1,
				pos2,
				pos3,
				CURVE_THICKNESS,
				CURVE_COLOR
			);

			//  draw tangents
			DrawLineV( 
				pos0, 
				pos1, 
				TANGENT_COLOR 
			);
			DrawLineV(
				pos3, 
				pos2, 
				TANGENT_COLOR 
			);
		}

		/*int steps = 100;
		for ( int i = 0; i < steps; i++ )
		{
			DrawCircleV( 
				_transform_curve_to_screen( 
					_curve.evaluate( (float)i / steps ) 
				), 
				CURVE_THICKNESS, 
				PURPLE 
			);
		}*/

		//  draw points
		for ( int i = 0; i < points_count; i++ )
		{
			const Point& point = _curve.get_global_point( i );
			_render_point( i, _transform_curve_to_screen( point ) );
		}

		EndScissorMode();
	}
	else
	{
		//  setup text strings
		const int TEXT_SIZE = 2;
		const char* texts[TEXT_SIZE] {
			"INVALID POINTS COUNT!",
			TextFormat( 
				"%d points instead of %d or %d", 
				points_count,
				(int)floorf( ( points_count + 1 ) / 3.0f ) * 3 + 1,
				(int)ceilf( ( points_count + 1 ) / 3.0f ) * 3 + 1
			),
		};

		//  setup text rendering
		Font font = GetFontDefault();
		float font_size = 20.0f;
		float spacing = 2.0f;
		Vector2 pos {
			_frame.x + _frame.width * 0.5f,
			_frame.y + _frame.height * 0.5f
		};

		//  draw all lines
		for ( int i = 0; i < TEXT_SIZE; i++ )
		{
			const char* text = texts[i];

			//  measure line size
			Vector2 text_size = MeasureTextEx( 
				font, 
				text, 
				font_size, 
				spacing 
			);

			//  draw centered line
			DrawTextPro( 
				font,
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

			//  offset next line
			pos.y += text_size.y + 2.0f;
		}
	}
}

void Editor::fit_viewport_to_curve()
{
	if ( _curve.is_valid() )
	{
		_curve_extrems = _curve.get_extrems();
	}
	_zoom = 1.0f;

	_invalidate_layout();
}

void Editor::_invalidate_layout()
{
	const float offset_y = 
		TITLE_FONT_SIZE + TITLE_DOCK_MARGIN_BOTTOM;

	//  update viewport
	_viewport.x = _frame.x + CURVE_FRAME_PADDING;
	_viewport.y = _frame.y + CURVE_FRAME_PADDING + offset_y;
	_viewport.width = _frame.width - CURVE_FRAME_PADDING * 2.0f;
	_viewport.height = _frame.height - CURVE_FRAME_PADDING * 2.0f - offset_y;

	//  update frame outline
	_frame_outline.x = _frame.x;
	_frame_outline.y = _frame.y + offset_y;
	_frame_outline.width = _frame.width;
	_frame_outline.height = _frame.height - offset_y;
}

void Editor::_render_point( int point_id, const Vector2& pos )
{
	bool is_tangent = !_curve.is_control_point( point_id );
	bool is_hovered = point_id == _hovered_point_id || point_id == _selected_point_id;
	bool is_selected = point_id == _selected_point_id;

	//  choose color
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

	if ( is_tangent )
	{
		int tangent_id = _curve.get_point_tangent_id( point_id );
		TangentMode mode = _curve.get_tangent_mode( tangent_id );

		//  render point depending on mode
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

		//  draw mode name
		if ( is_selected )
		{
			float font_size = 16.0f;
			float spacing = 2.0f;
			Font font = GetFontDefault();

			//  measure text size
			Vector2 size = MeasureTextEx( 
				font, 
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
}

void Editor::_render_circle_point( 
	const Vector2& pos, 
	const Color& color,
	bool is_selected
)
{
	//  draw point
	DrawCircleV( 
		pos, 
		POINT_SIZE * 0.5f, 
		color
	);

	//  draw selected
	if ( is_selected )
	{
		DrawCircleLinesV( 
			pos, 
			POINT_SIZE * 0.5f + POINT_SELECTED_OFFSET_SIZE, 
			POINT_SELECTED_COLOR 
		);
	}
}

void Editor::_render_square_point( 
	const Vector2& pos, 
	const Color& color,
	bool is_selected
)
{
	int size = (int)POINT_SIZE;

	//  draw point
	DrawRectangle( 
		(int)pos.x - size / 2,
		(int)pos.y - size / 2,
		size,
		size,
		color
	);

	//  draw selected
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

Vector2 Editor::_transform_curve_to_screen( const Point& point ) const
{	
	return point.remap(
		//  x-mapping
		_curve_extrems.min_x, _curve_extrems.max_x, 
		_viewport.x, _viewport.x + _viewport.width * _zoom,
		//  y-mapping
		_curve_extrems.min_y, _curve_extrems.max_y, 
		_viewport.y + _viewport.height * _zoom, _viewport.y
	);
}

Vector2 Editor::_transform_screen_to_curve( const Vector2& pos ) const
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
