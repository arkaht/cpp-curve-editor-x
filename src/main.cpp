
/*
 *  Curve X ─ a simple and open-source 2D spline editor
 *
 *  This application aims to serve as a 3rd party tool to create
 *  splines for your indie games, softwares or anything you want.
 *
 *  After creating a spline, you should be able to export it to
 *  disk and import it in your own code using the library.
 *
 *  Internally using Bézier splines, offering most control over
 *  Hermite (where tangent modes are forced to be mirrored) and
 *  other splines types.
 *
 *  References:
 *  - "The Continuity of Splines": https://www.youtube.com/watch?v=jvPPXbo87ds
 *  - "Cubic Hermite spline": https://en.wikipedia.org/wiki/Cubic_Hermite_spline
 *  - "Bézier spline": https://en.wikipedia.org/wiki/B%C3%A9zier_curve
 *  - "Curves and Splines": https://catlikecoding.com/unity/tutorials/curves-and-splines/
 *  - "A Primer on Bézier Curve": https://pomax.github.io/BezierInfo-2/
 *  - Unreal Engine's FRichCurve code for value evaluation by time
 *
 *  TODO LIST (by priority order):
 *  - [x] viewport user-movement (drag & scroll)
 *  - [x] points user-control (selection & drag)
 *  - [x] tangents user-control
 *  - [x] tangents modes (broken, aligned, mirrored)
 *  - [x] grid rendering
 *  - [x] grid-snapping
 *  - [x] quick curve evaluation (ctrl + hover)
 *  - [x] time curve evaluation (x-axis as time)
 *  - [x] grid scaling w/ zoom
 *  - [x] add & remove points
 *  - [x] export to file
 *  - [x] import from file
 *  - [*] multiple curves layering
 *  - [ ] dev-tool to build a quick release of the library
 *  - [ ] fix evaluation by time not accounting tangents X-axis
 *  - [ ] fix distance computation for keys
 */

 //  Includes

#include <iostream>

#include "raylib.h"
#include "raymath.h"

#include <external/curve-x/curve.h>
//#include <curve-x/curve.h>
#include <src/application.h>

using namespace curve_x;
using namespace curve_editor_x;

//  Application settings

const int	WINDOW_WIDTH = 1000;
const int	WINDOW_HEIGHT = 600;
const char*	WINDOW_TITLE = "Curve-X Application";
const int	WINDOW_TARGET_FPS = 60;

const float	WINDOW_PADDING = 16.0f;

//  Application code

int main( void )
{
	InitWindow( WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE );
	SetTargetFPS( WINDOW_TARGET_FPS );

	Application application( 
		Rectangle {
			WINDOW_PADDING,
			WINDOW_PADDING,
			WINDOW_WIDTH - WINDOW_PADDING * 2.0f,
			WINDOW_HEIGHT - WINDOW_PADDING * 2.0f,
		} 
	);
	application.init();

	while ( !WindowShouldClose() )
	{
		application.update( GetFrameTime() );

		BeginDrawing();
		application.render();
		EndDrawing();
	}

	CloseWindow();

	return 0;
}