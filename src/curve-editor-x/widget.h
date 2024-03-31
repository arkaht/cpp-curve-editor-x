#pragma once

#include <raylib.h>

namespace curve_editor_x
{
	class Widget
	{
	public:
		Widget() {}
		virtual ~Widget() {}

		virtual void update( float dt ) = 0;
		virtual void render() = 0;

	public:
		Rectangle frame { 0.0f, 0.0f, 100.0f, 100.0f };
		Color color = WHITE;
	};
}