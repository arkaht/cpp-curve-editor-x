#pragma once

#include <raylib.h>

#include <memory>

#include <curve-editor-x/usings.h>

namespace curve_editor_x
{
	class Widget : public std::enable_shared_from_this<Widget>
	{
	public:
		Widget() {}
		virtual ~Widget() {}

		virtual void update( float dt ) = 0;
		virtual void render() = 0;

		template <typename T>
		ref<T> cast()
		{
			return std::static_pointer_cast<T>( shared_from_this() );
		}

	public:
		Rectangle frame { 0.0f, 0.0f, 100.0f, 100.0f };
		Color color = WHITE;
	};
}