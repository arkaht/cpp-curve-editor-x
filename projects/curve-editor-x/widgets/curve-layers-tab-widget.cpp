#include "curve-layers-tab-widget.h"

#include <curve-editor-x/application.h>

using namespace curve_editor_x;

CurveLayersTabWidget::CurveLayersTabWidget( 
	Application* application 
) 
	: _application( application )
{
}

void CurveLayersTabWidget::update( float dt )
{
}

void CurveLayersTabWidget::render()
{
	DrawRectangleLinesEx( frame, 2.0f, GRAY );
}

void CurveLayersTabWidget::invalidate_layout()
{
	//  Invalidate curve layer rows
	const float PADDING = 2.0f;
	for ( int i = 0; i < _curve_layer_rows.size(); i++ )
	{
		auto& widget = _curve_layer_rows[i];
		widget->frame.width = frame.width - PADDING * 2.0f;
		widget->frame.height = 32.0f;
		widget->frame.x = frame.x + PADDING;
		widget->frame.y = frame.y + PADDING 
			+ i * widget->frame.height;  //  Vertical layout
		widget->child_index = i;
	}
}

ref<CurveLayerRowWidget> CurveLayersTabWidget::create_layer_row(
	ref<CurveLayer>& layer 
)
{
    auto widget = std::make_shared<CurveLayerRowWidget>( layer );
	widget->on_selected.listen( "editor", 
		std::bind( 
			&CurveLayersTabWidget::_on_curve_layer_row_selected, this,
			std::placeholders::_1 
		) 
	);
	_curve_layer_rows.push_back( widget );
    return widget;
}

void CurveLayersTabWidget::_on_curve_layer_row_selected( 
	ref<CurveLayerRowWidget> widget 
)
{
	_application->select_curve_layer( widget->child_index );
}
