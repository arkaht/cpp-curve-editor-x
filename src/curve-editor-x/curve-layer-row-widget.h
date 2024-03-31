#pragma once
#include <curve-editor-x/widget.h>

#include <curve-editor-x/usings.h>
#include <curve-editor-x/curve-layer.h>

namespace curve_editor_x
{
	class CurveLayerRowWidget : public Widget
	{
	public:
		CurveLayerRowWidget( ref<CurveLayer> layer );

		void update( float dt ) override;
		void render() override;

	public:
		bool is_selected = false;
		ref<CurveLayer> layer = nullptr;
	};
}