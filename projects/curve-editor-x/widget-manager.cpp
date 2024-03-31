#include "widget-manager.h"

using namespace curve_editor_x;

void WidgetManager::add_widget( ref<Widget> widget )
{
	//  NOTE: Adding widget during Widgets update is not 
	//	supported yet.

	_widgets.push_back( widget );
}

bool WidgetManager::remove_widget( ref<Widget> widget )
{
	//  NOTE: Removing widget during Widgets update is not 
	//	supported yet.

	auto itr = std::find( _widgets.begin(), _widgets.end(), widget );
	if ( itr == _widgets.end() ) return false;

	_widgets.erase( itr );
	return true;
}
