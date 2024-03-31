#pragma once

#include <vector>

#include <curve-editor-x/widget.h>
#include <curve-editor-x/usings.h>

namespace curve_editor_x
{
	class WidgetManager
	{
	public:
		void add_widget( ref<Widget> widget );
		bool remove_widget( ref<Widget> widget );

	protected:
		std::vector<ref<Widget>> _widgets {};
	};
}