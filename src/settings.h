#pragma once

#include <raylib.h>

namespace curve_editor_x
{
	namespace settings
	{
		constexpr Color BACKGROUND_COLOR = LIGHTGRAY;
		constexpr Color ROW_BACKGROUND_COLOR = { 180, 180, 180, 255 };
		constexpr Color ROW_BACKGROUND_SELECTED_COLOR = { 161, 210, 241, 255 };
		constexpr Color TEXT_COLOR = DARKGRAY;
		constexpr Color TEXT_ERROR_COLOR = RED;
		constexpr Color CURVE_FRAME_COLOR = GRAY;
		constexpr Color TANGENT_COLOR { 90, 90, 90, 255 };
		constexpr Color POINT_COLOR { 255, 0, 0, 255 };
		constexpr Color POINT_SELECTED_COLOR { 255, 255, 255, 255 };
		constexpr Color GRID_LINE_COLOR { 120, 120, 120, 255 };
		constexpr Color QUICK_EVALUATION_COLOR { 90, 90, 90, 255 };
		constexpr unsigned char CURVE_UNSELECTED_OPACITY = 80;
		constexpr unsigned char CURVE_SELECTED_OPACITY = 255;

		constexpr const char* DEFAULT_CURVE_PATH = "tests/test.cvx";
		
		constexpr float QUICK_EVALUATION_THICKNESS = 2.0f;
		constexpr float CURVE_THICKNESS = 2.0f;
		//  How much to offset the thickness per wheel scroll?
		constexpr float CURVE_THICKNESS_SENSITIVITY = 0.5f;
		//  Subdivisions for rendering a curve
		constexpr float CURVE_RENDER_SUBDIVISIONS = 0.01f;
		constexpr float CURVE_FRAME_PADDING = 32.0f;
		constexpr float TANGENT_THICKNESS = 2.0f;
		constexpr float POINT_SIZE = CURVE_THICKNESS * 3.0f;
		constexpr float POINT_SELECTED_OFFSET_SIZE = 3.0f;
		constexpr double DOUBLE_CLICK_TIME = 0.2;
		constexpr float SELECTION_RADIUS = 8.0f;

		//  In curve units, the gap for each grid line
		constexpr float GRID_SMALL_GAP = 1.0f;
		//  Number of small grid lines to form a larger line
		constexpr float GRID_LARGE_COUNT = 10.0f;
		//  Levels to snap on for zooming inside the grid
		constexpr float GRID_LEVELS[3] { 1.0f, 2.0f, 5.0f };
		constexpr float GRID_SMALL_LINE_THICKNESS = 1.0f;
		constexpr float GRID_LARGE_LINE_THICKNESS = 2.0f;
		constexpr float GRID_SMALL_GRID_FONT_SIZE = 16.0f;
		constexpr float GRID_LARGE_GRID_FONT_SIZE = 20.0f;
		constexpr float GRID_FONT_SPACING = 1.0f;
		constexpr float GRID_TEXT_PADDING = 2.0f;

		constexpr float FRAME_PADDING = 32.0f;

		constexpr int   TITLE_FONT_SIZE = 20;
		constexpr float TITLE_DOCK_MARGIN_BOTTOM = 4.0f;

		constexpr float ZOOM_SENSITIVITY = 0.1f;
		constexpr float ZOOM_MIN = 0.1f;
		constexpr float ZOOM_MAX = 5.0f;

		//  Does the frame rendering clips its content?
		constexpr bool  ENABLE_CLIPPING = true;
		constexpr bool  DRAW_MOUSE_POSITION = true;
		//  Does the zoom is clamped between ZOOM_MIN and ZOOM_MAX?
		constexpr bool  IS_ZOOM_CLAMPED = false;
	}
}