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
	_curve.add_point( { 2.0f, 0.0f } );

	/*curve.add_point( { 0.0f, 0.0f } );
	curve.add_point( { 100.0f, 50.0f } );
	curve.add_point( { 350.0f, 300.0f } );
	curve.add_point( { 500.0f, 0.0f } );*/

	fit_viewport_to_curve();
}

void Editor::update( float dt )
{
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
		const Point& point = _curve.get_point( i );
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
			!MUST_DOUBLE_CLICK_TO_DRAG_POINT || _hovered_point_id == _selected_point_id;
		_selected_point_id = _hovered_point_id;
	}
	else if ( IsMouseButtonReleased( MOUSE_BUTTON_LEFT ) )
	{
		_can_drag_selected_point = true;
	}
	//  LMB-down: move selected point
	else if ( _selected_point_id >= 0 
		&& _can_drag_selected_point
		&& IsMouseButtonDown( MOUSE_BUTTON_LEFT ) )
	{
		/*Point point = _curve.get_point( _selected_point_id );
		point.x += mouse_delta.x / _viewport.width * _zoom;
		point.y += mouse_delta.y / _viewport.height * _zoom;*/

		_curve.set_point(
			_selected_point_id, 
			_transform_screen_to_curve( mouse_pos )
		);
	}
}

void Editor::render()
{
	ClearBackground( BACKGROUND_COLOR );

	//  draw title
	const char* title_c_str = _title.c_str();
	DrawText( 
		title_c_str, 
		_frame.x, _frame.y, 
		TITLE_FONT_SIZE, 
		TEXT_COLOR 
	);

	//  draw frame
	DrawRectangleLinesEx( 
		_frame_outline, 
		2.0f, 
		CURVE_FRAME_COLOR 
	);

	int points_count = _curve.get_points_count();
	if ( points_count > 0 ) 
	{
		BeginScissorMode( 
			_frame_outline.x, _frame_outline.y, 
			_frame_outline.width, _frame_outline.height 
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
		Point point_zero = _curve.get_point( 0 );
		Vector2 point_a = _transform_curve_to_screen( point_zero );
		Vector2 point_b {};
		for ( int i = 1; i < points_count; i++ )
		{
			const Point& point = _curve.get_point( i );
			point_b = _transform_curve_to_screen( point );

			DrawLineBezier( 
				point_a,
				point_b,
				CURVE_THICKNESS,
				CURVE_COLOR
			);

			point_a = point_b;
		}

		//  draw points
		for ( int i = 0; i < points_count; i++ )
		{
			const Point& point = _curve.get_point( i );
			_render_point( i, _transform_curve_to_screen( point ) );
		}

		EndScissorMode();
	}
}

void Editor::fit_viewport_to_curve()
{
	_curve_extrems = _curve.get_extrems();
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

void Editor::_render_point( int id, const Vector2& pos )
{
	//  draw point
	bool is_hovered = id == _hovered_point_id || id == _selected_point_id;
	DrawCircleV( 
		pos, 
		POINT_SIZE * 0.5f, 
		is_hovered ? POINT_SELECTED_COLOR : POINT_COLOR
	);

	//  draw selected
	if ( id == _selected_point_id )
	{
		DrawCircleLinesV( 
			pos, 
			POINT_SIZE * 0.5f + POINT_SELECTED_OFFSET_SIZE, 
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
		_viewport.y, _viewport.y + _viewport.height * _zoom
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
		_viewport.y, _viewport.y + _viewport.height * _zoom,
		_curve_extrems.min_y, _curve_extrems.max_y
	);
}
