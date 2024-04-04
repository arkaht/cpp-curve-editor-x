#pragma once
#include "widget.h"
#include "curve-layer-row-widget.h"

#include <curve-editor-x/application.fwd.h>

namespace curve_editor_x
{
	class CurveLayersTabWidget : public Widget
	{
	public:
		CurveLayersTabWidget( Application* application );

		void update( float dt ) override;
		void render() override;

		void invalidate_layout() override;

		ref<CurveLayerRowWidget> create_layer_row( ref<CurveLayer>& layer );

	private:
		void _on_curve_layer_row_selected( ref<CurveLayerRowWidget> widget );

	private:
		Application* _application = nullptr;

		std::vector<ref<CurveLayerRowWidget>> _curve_layer_rows {};
	};
}