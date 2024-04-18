#pragma once
#include "widget.h"

#include <curve-editor-x/application.fwd.h>
#include <curve-editor-x/event.h>
#include <curve-editor-x/curve-layer.h>

namespace curve_editor_x
{
	class CurveLayerRowWidget : public Widget
	{
	public:
		CurveLayerRowWidget( ref<CurveLayer> layer );

		bool consume_input( const UserInput& input ) override;

		void update( float dt ) override;
		void render() override;

		bool is_selected() const;

	public:
		/*
		 * Called when the layer has been selected by input.
		 * 
		 * Params:
		 * - ref<CurveLayerRowWidget> this
		 */
		Event<ref<CurveLayerRowWidget>> on_selected;
		/*
		 * Called when the layer has been deleted by input.
		 * 
		 * Params:
		 * - ref<CurveLayerRowWidget> this
		 */
		Event<ref<CurveLayerRowWidget>> on_deleted;

	public:
		int child_index = -1;
		ref<CurveLayer> layer = nullptr;

	private:
		const Color BACKGROUND_COLOR = { 180, 180, 180, 255 };
		const Color BACKGROUND_SELECTED_COLOR = { 161, 210, 241, 255 };
	
		//  TODO: Export these common variables inside an 
		//		  ApplicationStyle class.
		const unsigned char CURVE_UNSELECTED_OPACITY = 80;
		const unsigned char CURVE_SELECTED_OPACITY = 255;
	};
}