#include "widget-manager.h"

using namespace curve_editor_x;

void WidgetManager::add_widget( ref<Widget> widget )
{
	//  Defer the addition for the next frame
	if ( _is_locking_widgets_vector )
	{
		_to_add_widgets.push_back( widget );
		return;
	}

	//  
	_widgets.push_back( widget );
}

void WidgetManager::remove_widget( ref<Widget> widget )
{
	//  Defer the deletion for the next frame
	if ( _is_locking_widgets_vector )
	{
		_to_remove_widgets.push_back( widget );
		return;
	}

	//  Find the widget inside the vector
	auto itr = std::find( _widgets.begin(), _widgets.end(), widget );
	if ( itr == _widgets.end() ) return;

	//  Erase it
	_widgets.erase( itr );
	return;
}

void WidgetManager::add_pending_widgets()
{
	if ( _is_locking_widgets_vector ) return;

	for ( auto& widget : _to_add_widgets )
	{
		add_widget( widget );
	}
	_to_add_widgets.clear();
}

void WidgetManager::remove_pending_widgets()
{
	if ( _is_locking_widgets_vector ) return;

	for ( auto& widget : _to_remove_widgets )
	{
		remove_widget( widget );
	}
	_to_remove_widgets.clear();
}

void WidgetManager::lock_widgets_vector( bool is_locked )
{
	_is_locking_widgets_vector = is_locked;
}
