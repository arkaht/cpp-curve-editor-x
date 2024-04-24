#pragma once

#include "widget.h"

#include <vector>
#include <algorithm>

#include <src/usings.h>

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
		void remove_widget( ref<Widget> widget );

		void add_pending_widgets();
		void remove_pending_widgets();

		void lock_widgets_vector( bool is_locked );

	protected:
		std::vector<ref<Widget>> _to_add_widgets {};
		std::vector<ref<Widget>> _widgets {};
		std::vector<ref<Widget>> _to_remove_widgets {};

		bool _is_locking_widgets_vector = false;
	};
}