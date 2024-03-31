#include "curve-layer-row-widget.h"

using namespace curve_editor_x;

CurveLayerRowWidget::CurveLayerRowWidget( ref<CurveLayer> layer )
	: layer( layer )
{}

void CurveLayerRowWidget::update( float dt )
{
	const Vector2 mouse_pos = GetMousePosition();
	bool is_hovered = CheckCollisionPointRec( mouse_pos, frame );

	if ( is_hovered && IsMouseButtonPressed( MOUSE_BUTTON_LEFT ) )
	{
		is_selected = !is_selected;
	}
}

void CurveLayerRowWidget::render()
{
	DrawRectangleRec( frame, color );

	const float padding = 2.0f;
	const float radius = frame.height * 0.5f - padding;
	DrawCircle( 
		frame.x + frame.height * 0.5f, 
		frame.y + frame.height * 0.5f,
		radius * 0.9f,
		GRAY
	);
	DrawCircle( 
		frame.x + frame.height * 0.5f, 
		frame.y + frame.height * 0.5f,
		radius * 0.8f,
		is_selected ? WHITE : LIGHTGRAY
	);

	const Font font = GetFontDefault();
	const float font_size = 16.0f;
	const float text_spacing = 1.0f;
	const char* text_cstr = layer->name.c_str();
	const Vector2 text_size = MeasureTextEx( font, text_cstr, font_size, text_spacing );
	DrawTextEx( 
		font,
		text_cstr,
		Vector2 {
			frame.x + radius * 2.0f + padding * 2.0f,
			frame.y + frame.height * 0.5f - text_size.y * 0.5f,
		},
		font_size, 
		text_spacing, 
		BLACK 
	);
}
