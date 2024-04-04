#pragma once

#include <raylib.h>
#include <curve-x/curve.h>

namespace curve_editor_x
{
	using namespace curve_x;

	struct CurveLayer
	{
		CurveLayer() {}
		CurveLayer( const Curve& curve )
			: curve( curve ) {}

		Curve curve;
		Color color = RED;
		bool is_selected = false;

		bool has_unsaved_changes = true;
		bool is_file_exists = false;
		std::string path = "default.cvx";
		std::string name = "default";
	};
}