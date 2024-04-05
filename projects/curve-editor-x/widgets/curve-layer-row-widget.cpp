#include "curve-layer-row-widget.h"

using namespace curve_editor_x;

CurveLayerRowWidget::CurveLayerRowWidget(
	ref<CurveLayer> layer 
)
	: layer( layer )
{}

bool CurveLayerRowWidget::handle_key_input( UserInput key )
{
	switch ( key )
	{
		case UserInput::LeftClick:
		{
			const Vector2 mouse_pos = GetMousePosition();
			bool is_hovered = CheckCollisionPointRec( 
				mouse_pos, 
				frame 
			);

			//  Select current layer
			if ( is_hovered && !layer->is_selected )
			{
				auto caller = cast<CurveLayerRowWidget>();
				on_selected.invoke( caller );
				return true;
			}

			break;
		}
		case UserInput::Delete:
		{
			//  Delete current layer
			if ( layer->is_selected )
			{
				auto caller = cast<CurveLayerRowWidget>();
				on_deleted.invoke( caller );
				return true;
			}

			break;
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
	DrawCircle( 
		frame.x + frame.height * 0.5f, 
		frame.y + frame.height * 0.5f,
		radius * 0.8f,
		is_selected() 
			? layer->color 
			: LIGHTGRAY
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
