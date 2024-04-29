#pragma once
#include "application.fwd.h"

#include <raylib.h>

#include <string>

#include <src/curve-layer.h>
#include <src/user-input.h>

#include <src/widgets/widget-manager.h>
#include <src/widgets/curve-editor-widget.h>
#include <src/widgets/curve-layers-tab-widget.h>

namespace curve_editor_x
{
	using namespace curve_x;

	class Application : public WidgetManager
	{
	public:
		Application( 
			const Rectangle& frame 
		);

		void init();
		void update( float dt );
		void render();

		void invalidate_layout();

		void focus_widget( ref<Widget> widget );
		void unfocus_widget();

		void set_title( const std::string& title );
		bool export_to_file( 
			ref<CurveLayer> layer, 
			const std::string& path 
		);
		bool import_from_file( const std::string& path );

		void add_curve_layer( ref<CurveLayer> layer );
		void remove_curve_layer( ref<CurveLayer> layer );

		void unselect_curve_layer();
		void select_curve_layer( int layer_id );

		int get_selected_curve_id() const;
		ref<CurveLayer> get_curve_layer( int layer_id );
		ref<CurveLayer> get_selected_curve_layer();
		bool is_valid_curve_id( int layer_id ) const;
		bool is_valid_selected_curve() const;
		/*
		 * Returns a copy vector of the curve layers.
		 */
		std::vector<ref<CurveLayer>> get_curve_layers() const;

	private:
		void _invalidate_widgets();

		void _detect_mouse_input( 
			const MouseButton button, 
			const InputKey input_type 
		);
		void _detect_key_input(
			const KeyboardKey key,
			const InputKey input_type
		);

		Color _get_curve_color_at( int index );

	private:
		std::string _title {};

		Font _font {};

		ref<CurveEditorWidget> _curve_editor = nullptr;
		ref<CurveLayersTabWidget> _curve_layers_tab = nullptr;

		std::vector<UserInput> _key_inputs {};
		ref<Widget> _focused_widget = nullptr;

		std::vector<ref<CurveLayer>> _curve_layers {};
		int _selected_curve_id = 0;

		//  Has mouse clicks been received this frame?
		bool _has_new_mouse_clicks = false;

		bool _is_debug_enabled = false;

		Rectangle _frame {};
	};
}