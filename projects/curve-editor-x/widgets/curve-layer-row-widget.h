#pragma once
#include "widget.h"

#include <curve-editor-x/usings.h>
#include <curve-editor-x/event.h>
#include <curve-editor-x/curve-layer.h>

namespace curve_editor_x
{
	class CurveLayerRowWidget : public Widget
	{
	public:
		CurveLayerRowWidget( ref<CurveLayer> layer );

		void update( float dt ) override;
		void render() override;

		bool is_selected() const;

	public:
		/*
		 * Called when the widget has been selected by input.
		 * 
		 * Params:
		 * - ref<CurveLayerRowWidget> this
		 */
		Event<ref<CurveLayerRowWidget>> on_selected;

	public:
		int child_index = -1;
		ref<CurveLayer> layer = nullptr;

	private:
		const Color BACKGROUND_COLOR = { 180, 180, 180, 255 };
		const Color BACKGROUND_SELECTED_COLOR = { 161, 210, 241, 255 };
	};
}