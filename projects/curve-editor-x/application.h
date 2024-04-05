#pragma once
#include "application.fwd.h"

#include <raylib.h>
#include <raymath.h>

#include <string>

#include <curve-editor-x/utils.h>
#include <curve-editor-x/curve-layer.h>

#include <curve-editor-x/widgets/widget-manager.h>
#include <curve-editor-x/widgets/curve-editor-widget.h>
#include <curve-editor-x/widgets/curve-layers-tab-widget.h>

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

		Color _get_curve_color_at( int index );

	//  Application settings
	private:
		const Color BACKGROUND_COLOR = LIGHTGRAY;
		const Color TEXT_COLOR = DARKGRAY;
		const Color TEXT_ERROR_COLOR = RED;
		const Color CURVE_FRAME_COLOR = DARKGRAY;

		const float CURVE_FRAME_PADDING = 32.0f;

		const int   TITLE_FONT_SIZE = 20;
		const float TITLE_DOCK_MARGIN_BOTTOM = 4.0f;

		const std::string DEFAULT_CURVE_PATH = "tests/test.cvx";

	private:
		std::string _title {};

		Font _font {};

		ref<CurveEditorWidget> _curve_editor = nullptr;
		ref<CurveLayersTabWidget> _curve_layers_tab = nullptr;

		std::vector<ref<CurveLayer>> _curve_layers {};
		int _selected_curve_id = 0;

		Rectangle _frame {};
	};
}