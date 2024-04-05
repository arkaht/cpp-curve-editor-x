#include "curve-layers-tab-widget.h"

#include <curve-editor-x/application.h>

using namespace curve_editor_x;

CurveLayersTabWidget::CurveLayersTabWidget( 
	Application* application 
) 
	: _application( application )
{
}

bool CurveLayersTabWidget::handle_key_input( UserInput key )
{
	//  Gives input to children
	//  TODO?: This is repeating code, move this in WidgetManager
	//		   and make this class one
	for ( auto& row : _curve_layer_rows )
	{
		if ( row->handle_key_input( key ) )
		{
			return true;
		}
	}

	return false;
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
		auto& row = _curve_layer_rows[i];
		row->frame.width = frame.width - PADDING * 2.0f;
		row->frame.height = 32.0f;
		row->frame.x = frame.x + PADDING;
		row->frame.y = frame.y + PADDING 
			+ i * row->frame.height;  //  Vertical layout
		row->child_index = i;
	}
}

ref<CurveLayerRowWidget> CurveLayersTabWidget::create_layer_row(
	ref<CurveLayer> layer 
)
{
    auto row = std::make_shared<CurveLayerRowWidget>( layer );
	row->on_selected.listen( "editor", 
		std::bind( 
			&CurveLayersTabWidget::_on_curve_layer_row_selected, this,
			std::placeholders::_1 
		) 
	);
	row->on_deleted.listen( "editor",
		std::bind(
			&CurveLayersTabWidget::_on_curve_layer_row_deleted, this,
			std::placeholders::_1
		)
	);
	_curve_layer_rows.push_back( row );

	//  Update rows
	invalidate_layout();
    return row;
}

void CurveLayersTabWidget::remove_layer_row( ref<CurveLayer> layer )
{
	//  Find associated layer row widget
	auto itr = std::find_if( 
		_curve_layer_rows.begin(), 
		_curve_layer_rows.end(), 
		[&]( const auto row ) { return row->layer == layer; } 
	);
	if ( itr == _curve_layer_rows.end() ) return;

	//  Destroy row widget
	auto& row = *itr;
	_application->remove_widget( row );

	//  Remove row from vector
	_curve_layer_rows.erase( itr );

	//  Update rows
	invalidate_layout();
}

void CurveLayersTabWidget::_on_curve_layer_row_selected( 
	ref<CurveLayerRowWidget> widget 
)
{
	_application->select_curve_layer( widget->child_index );
}

void CurveLayersTabWidget::_on_curve_layer_row_deleted( 
	ref<CurveLayerRowWidget> widget 
)
{
	_application->remove_curve_layer( widget->layer );
}
