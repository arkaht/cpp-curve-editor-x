#pragma once

#include "widget.h"

#include <vector>

#include <curve-editor-x/usings.h>

namespace curve_editor_x
{
	class WidgetManager
	{
	public:
		template <typename T, typename... Args>
		ref<T> new_widget( Args&& ...args )
		{
			auto widget = std::make_shared<T>( args... );

			add_widget( widget );
			return widget;
		}
		void add_widget( ref<Widget> widget );
		bool remove_widget( ref<Widget> widget );

	protected:
		std::vector<ref<Widget>> _widgets {};
	};
}