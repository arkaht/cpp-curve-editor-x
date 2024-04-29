#pragma once

#include <raylib.h>
#include <curve-x/curve.h>

namespace curve_editor_x
{
	using namespace curve_x;

	struct CurveLayer
	{
	public:
		CurveLayer() {}
		CurveLayer( const Curve& curve )
			: curve( curve ) {}

	public:
		std::string name = "default";
		std::string path = "default.cvx";

		Curve curve;
		Color color = RED;
		bool is_selected = false;

		bool has_unsaved_changes = true;
		bool is_file_exists = false;
	};
}