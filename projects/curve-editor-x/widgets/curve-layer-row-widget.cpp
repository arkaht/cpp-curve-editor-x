#include "curve-layer-row-widget.h"

using namespace curve_editor_x;

CurveLayerRowWidget::CurveLayerRowWidget(
	ref<CurveLayer> layer 
)
	: layer( layer )
{}

bool CurveLayerRowWidget::consume_input( const UserInput& input )
{
	if ( input.is( InputKey::LeftClick, InputState::Pressed ) )
	{
		const Vector2 mouse_pos = GetMousePosition();

		//  Select current layer if hovered
		if ( !layer->is_selected 
		  && CheckCollisionPointRec( mouse_pos, frame ) )
		{
			auto caller = cast<CurveLayerRowWidget>();
			on_selected.invoke( caller );
			return true;
		}
	}
	else if ( input.is( InputKey::Delete, InputState::Pressed ) )
	{
		//  Delete current layer
		if ( layer->is_selected )
		{
			auto caller = cast<CurveLayerRowWidget>();
			on_deleted.invoke( caller );
			return true;
		}
	}

	return false;
}

void CurveLayerRowWidget::update( float dt )
{
}

void CurveLayerRowWidget::render()
{
	//  Draw background
	const Color background_color = is_selected() 
		? BACKGROUND_SELECTED_COLOR 
		: BACKGROUND_COLOR;
	DrawRectangleRec( frame, background_color );

	//  Draw radial button
	const float padding = 2.0f;
	const float radius = frame.height * 0.35f - padding;
	DrawCircle( 
		frame.x + frame.height * 0.5f, 
		frame.y + frame.height * 0.5f,
		radius,
		GRAY
	);
	if ( !layer->is_selected && CURVE_UNSELECTED_OPACITY < 255 )
	{
		DrawCircle( 
			frame.x + frame.height * 0.5f, 
			frame.y + frame.height * 0.5f,
			radius * 0.8f,
			LIGHTGRAY
		);
	}
	DrawCircle( 
		frame.x + frame.height * 0.5f, 
		frame.y + frame.height * 0.5f,
		radius * 0.8f,
		Color {
			layer->color.r,
			layer->color.g,
			layer->color.b,
			layer->is_selected 
				? CURVE_SELECTED_OPACITY 
				: CURVE_UNSELECTED_OPACITY
		}
	);

	//  Draw layer name text
	const Font font = GetFontDefault();
	const float font_size = 16.0f;
	const float text_spacing = 1.0f;
	const char* text_cstr = layer->name.c_str();
	const Vector2 text_size = MeasureTextEx( font, text_cstr, font_size, text_spacing );
	DrawTextEx( 
		font,
		text_cstr,
		Vector2 {
			frame.x + frame.height,
			frame.y + frame.height * 0.5f - text_size.y * 0.5f,
		},
		font_size, 
		text_spacing, 
		BLACK 
	);
}

bool CurveLayerRowWidget::is_selected() const
{
	if ( layer == nullptr ) return false;
	return layer->is_selected;
}
