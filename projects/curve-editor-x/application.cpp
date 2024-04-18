#include "application.h"

#include <curve-x/curve-serializer.h>

#include <fstream>

using namespace curve_editor_x;

Application::Application( const Rectangle& frame )
	: _frame( frame )
{
}

void Application::init()
{
	//  Initialize widgets
	_curve_editor = new_widget<CurveEditorWidget>( this );
	_curve_layers_tab = new_widget<CurveLayersTabWidget>( this );
	invalidate_layout();

	//  Set the default font after raylib has been initialized
	_font = GetFontDefault();

	//  Import a curve file or create a simple one by default
	if ( !import_from_file( DEFAULT_CURVE_PATH ) )
	{
		//  Create default curve
		Curve curve {};
		curve.add_key( CurveKey(
			{ 0.0f, 1.0f }
		) );
		curve.add_key( CurveKey(
			{ 1.0f, 0.0f }
		) );

		//  Create associated layer and select it
		auto layer = std::make_shared<CurveLayer>( curve );
		layer->is_selected = true;
		layer->color = _get_curve_color_at( 0 );
		add_curve_layer( layer );

		//  Fit viewport
		_curve_editor->fit_viewport();
	}
}

void Application::update( float dt )
{
	bool is_shift_down = IsKeyDown( KEY_LEFT_SHIFT );
	bool is_ctrl_down = IsKeyDown( KEY_LEFT_CONTROL );

	if ( is_ctrl_down )
	{
		//  Ctrl+S: Save to current file
		if ( is_valid_selected_curve() && IsKeyPressed( KEY_S ) )
		{
			const ref<CurveLayer>& layer = get_selected_curve_layer();
			std::string path = layer->path;

			//  Shift-down: Choosing save file location
			if ( is_shift_down || !layer->is_file_exists )
			{
				path = Utils::get_user_save_file(
					"Curve-X",
					"Curve-X Files(.cvx)",
					std::vector<std::string> { FORMAT_EXTENSION }
				);
			}

			//  Save file
			if ( path.length() > 0 )
			{
				//  Force path to hold the format extension
				path = TextFormat( 
					"%s.%s",
					GetFileNameWithoutExt( path.c_str() ),
					FORMAT_EXTENSION.c_str()
				);
				export_to_file( layer, path );
			}
		}
		//  Ctrl+L: Load a file
		else if ( IsKeyPressed( KEY_L ) )
		{
			auto paths = Utils::get_user_open_files(
				"Curve-X",
				"Curve-X Files(.cvx)",
				std::vector<std::string> { FORMAT_EXTENSION }
			);

			for ( const auto& path : paths )
			{
				import_from_file( path );
			}
		}
		//  Ctrl+;: Toggle debug mode
		else if ( IsKeyPressed( KEY_COMMA ) )
		{
			_is_debug_enabled = !_is_debug_enabled;
		}
	}

	//  Reset inputs
	_key_inputs.clear();
	_has_new_mouse_clicks = false;

	//  Retrieve mouse inputs
	_detect_mouse_input( MOUSE_BUTTON_LEFT, InputKey::LeftClick );
	_detect_mouse_input( MOUSE_BUTTON_MIDDLE, InputKey::MiddleClick );
	_detect_mouse_input( MOUSE_BUTTON_RIGHT, InputKey::RightClick );

	if ( _has_new_mouse_clicks )
	{
		const Vector2 mouse_pos = GetMousePosition();

		//  Focus the widget under the cursor
		ref<Widget> new_focus = nullptr;
		for ( auto& widget : _widgets )
		{
			if ( CheckCollisionPointRec( mouse_pos, widget->frame ) )
			{
				new_focus = widget;

				//  Pass any mouse inputs to that widget
				for ( const auto& input : _key_inputs )
				{
					new_focus->consume_input( input );
				}
				_key_inputs.clear();

				break;
			}
		}
		focus_widget( new_focus );
	}

	//  Retrieve key inputs
	_detect_key_input( KEY_F1,		InputKey::F1 );
	_detect_key_input( KEY_F2,		InputKey::F2 );
	_detect_key_input( KEY_F3,		InputKey::F3 );
	_detect_key_input( KEY_TAB,		InputKey::Mode );
	_detect_key_input( KEY_F,		InputKey::Focus );
	_detect_key_input( KEY_DELETE,	InputKey::Delete );

	//  Add pending widgets
	add_pending_widgets();
	lock_widgets_vector( true );

	//  Propagate inputs to widgets
	for ( const auto& input : _key_inputs )
	{
		//  Prioritizing sending inputs to the focused widget
		if ( _focused_widget != nullptr 
		  && _focused_widget->consume_input( input ) )
		{
			printf( "Focused widget consumed the input!\n" );
			continue;
		}
		/*else if ( _focused_widget != nullptr )
		{
			printf( "Focused widget didn't consumed the input!" );
		}*/

		for ( auto& widget : _widgets )
		{
			if ( widget->consume_input( input ) )
			{
				break;
			}
		}
	}

	//  Update widgets
	for ( auto& widget : _widgets )
	{
		widget->update( dt );
	}

	//  Remove pending widgets
	lock_widgets_vector( false );
	remove_pending_widgets();
}

void Application::render()
{
	ClearBackground( BACKGROUND_COLOR );

	//  Render widgets
	lock_widgets_vector( true );
	for ( auto& widget : _widgets )
	{
		widget->render();
	}
	lock_widgets_vector( false );

	//  Debug render
	if ( _is_debug_enabled )
	{
		//  Draw frame
		DrawRectangleLinesEx( _frame, 1.0f, PINK );

		//  Outline focused widget
		if ( _focused_widget != nullptr )
		{
			DrawRectangleLinesEx( _focused_widget->frame, 2.0f, RED );
		}
	}
}

void Application::invalidate_layout()
{
	//  Update layers tab
	_curve_layers_tab->frame.height = _frame.height;
	_curve_layers_tab->frame.width = _frame.width * 0.3f;
	_curve_layers_tab->frame.x = _frame.x + _frame.width 
			- _curve_layers_tab->frame.width;
	_curve_layers_tab->frame.y = _frame.y;

	//  Update frame outline
	_curve_editor->frame.x = _frame.x;
	_curve_editor->frame.y = _frame.y;
	_curve_editor->frame.width = _frame.width 
		- _curve_layers_tab->frame.width - CURVE_FRAME_PADDING;
	_curve_editor->frame.height = _frame.height;

	_invalidate_widgets();
}

void Application::focus_widget( ref<Widget> widget )
{
	//  Prevent focusing once again the same widget
	if ( widget == _focused_widget ) return;

	unfocus_widget();

	if ( widget == nullptr ) return;

	_focused_widget = widget;
	_focused_widget->on_focus_changed( true );

	printf( "Focus changed to another widget!\n" );
}

void Application::unfocus_widget()
{
	if ( _focused_widget == nullptr ) return;

	_focused_widget->on_focus_changed( false );
	_focused_widget = nullptr;
}

void Application::set_title( const std::string& title )
{
	_title = title;
}

bool Application::export_to_file( 
	ref<CurveLayer> layer, 
	const std::string& path 
)
{
	const char* c_path = path.c_str();
	std::ofstream file;
	file.open( c_path );

	//  Check file exists
	if ( !file.is_open() )
	{
		printf( 
			"File '%s' isn't writtable, aborting export from file!\n", 
			c_path
		);
		return false;
	}

	//  Serialize curve
	CurveSerializer serializer;
	std::string data = serializer.serialize( layer->curve );

	//  Write data to file
	file << data;
	file.close();

	//  Apply file
	layer->has_unsaved_changes = false;
	layer->is_file_exists = true;

	printf( "Exported curve '%s' to file '%s'\n", 
		layer->name.c_str(), c_path );
	return true;
}

bool Application::import_from_file( const std::string& path )
{
	const char* c_path = path.c_str();
	std::ifstream file;
	file.open( c_path );

	//  Check file exists
	if ( !file.is_open() )
	{
		printf( 
			"File '%s' doesn't exists, aborting import from file!\n", 
			c_path
		);
		return false;
	}

	//  Read file's content
	std::string data;
	for ( std::string line; std::getline( file, line ); )
	{
		data += line + '\n';
	}
	file.close();

	//  Unserialize data into curve
	CurveSerializer serializer;
	auto layer = std::make_shared<CurveLayer>();
	layer->curve = serializer.unserialize( data );
	layer->path = path;
	layer->name = GetFileNameWithoutExt( path.c_str() );
	layer->color = _get_curve_color_at( (int)_curve_layers.size() );
	layer->is_selected = true;
	layer->is_file_exists = true;
	layer->has_unsaved_changes = false;
	add_curve_layer( layer );

	_curve_editor->fit_viewport();

	printf( "Imported curve from file '%s'\n", c_path );
	return true;
}

void Application::add_curve_layer( ref<CurveLayer> layer )
{
	//  Add to layers
	_curve_layers.push_back( layer );

	//  If asked for, select the new layer
	if ( layer->is_selected )
	{
		select_curve_layer( (int)_curve_layers.size() - 1 );
	}

	//  Create a layer row widget
	auto widget = _curve_layers_tab->create_layer_row( layer );
	add_widget( widget );
}

void Application::remove_curve_layer( ref<CurveLayer> layer )
{
	auto itr = std::find( _curve_layers.begin(), _curve_layers.end(), layer );
	if ( itr == _curve_layers.end() ) return;

	//  Remove from layers
	_curve_layers.erase( itr );

	//  Un-select layer
	if ( layer->is_selected )
	{
		select_curve_layer( 0 );
	}

	//  Remove the layer row widget
	_curve_layers_tab->remove_layer_row( layer );
}

void Application::unselect_curve_layer()
{
	if ( !is_valid_curve_id( _selected_curve_id ) ) return;

	auto selected_layer = get_selected_curve_layer();
	selected_layer->is_selected = false;
	_selected_curve_id = -1;
}

void Application::select_curve_layer( int layer_id )
{
	//  Un-select previous layer
	unselect_curve_layer();

	if ( !is_valid_curve_id( layer_id ) ) return;

	//  Select specified layer
	auto& layer = _curve_layers.at( layer_id );
	layer->is_selected = true;
	_selected_curve_id = layer_id;

	//  Set title to layer's filename
	set_title( Utils::get_filename_from_path( layer->path ) );
}

int Application::get_selected_curve_id() const
{
	return _selected_curve_id;
}

ref<CurveLayer> Application::get_curve_layer( int layer_id )
{
	return _curve_layers[layer_id];
}

ref<CurveLayer> Application::get_selected_curve_layer()
{
	return _curve_layers[_selected_curve_id];
}

bool Application::is_valid_curve_id( int layer_id ) const
{
	int count = (int)_curve_layers.size();
	return layer_id >= 0 && layer_id < count;
}

bool Application::is_valid_selected_curve() const
{
	return is_valid_curve_id( _selected_curve_id )
		&& _curve_layers[_selected_curve_id]->curve.is_valid();
}

std::vector<ref<CurveLayer>> Application::get_curve_layers() const
{
	return _curve_layers;
}

void Application::_invalidate_widgets()
{
	//  Invalidate widgets
	lock_widgets_vector( true );
	for ( auto& widget : _widgets )
	{
		widget->invalidate_layout();
	}
	lock_widgets_vector( false );
}

void Application::_detect_mouse_input(
	const MouseButton button, 
	const InputKey input_type 
)
{
	if ( IsMouseButtonPressed( button ) )
	{
		_key_inputs.push_back( 
			UserInput(
				InputState::Pressed, 
				input_type 
			) 
		);
		_has_new_mouse_clicks = true;
	}
	else if ( IsMouseButtonReleased( button ) )
	{
		_key_inputs.push_back( 
			UserInput(
				InputState::Released,
				input_type
			) 
		);
		_has_new_mouse_clicks = true;
	}
}

void Application::_detect_key_input( 
	const KeyboardKey key, 
	const InputKey input_type 
)
{
	if ( IsKeyPressed( key ) )
	{
		_key_inputs.push_back( 
			UserInput(
				InputState::Pressed, 
				input_type 
			) 
		);
	}
	else if ( IsKeyReleased( key ) )
	{
		_key_inputs.push_back(
			UserInput(
				InputState::Released,
				input_type
			)
		);
	}
}

Color Application::_get_curve_color_at( int index )
{
	switch ( index )
	{
		case 0:
			return RED;
		case 1:
			return GREEN;
		case 2:
			return BLUE;
		case 3:
			return ORANGE;
		case 4:
			return PURPLE;
		case 5:
			return YELLOW;
		case 6:
			return BROWN;
	}

	return PINK;
}