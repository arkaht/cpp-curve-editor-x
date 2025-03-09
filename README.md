# Curve-X Editor
**Curve-X is a small and easy-to-use C++ Bézier library to create 2D-splines with its own GUI editor.**

This is the repository for the GUI editor, made in C++ with Raylib.

Read the [Curve-X](https://github.com/arkaht/cpp-curve-x) repository's for more detailed information.

![image](https://github.com/arkaht/cpp-curve-x/assets/114919245/a32a058e-9ba1-4add-89f0-de4ad0f14434)
> Two examples: a geometrical shape curve ([`samples/heart.cvx`](https://github.com/arkaht/cpp-curve-x/blob/master/samples/heart.cvx)) and a timed-based curve ([`samples/controller-gamepad-sensitivity.cvx`](https://github.com/arkaht/cpp-curve-x/blob/master/samples/controller-gamepad-sensitivity.cvx))

## Dependencies 
+ C++17 compiler
+ CMake 3.11

## Libraries
+ [Curve-X](https://github.com/arkaht/cpp-curve-x)
+ [raylib](https://github.com/raysan5/raylib)

## Features
+ Written in **simple and straightforward C++17** using **snake_case** notation.
+ Project IDE-independent thanks to **CMake**.
+ Support for both geometrical shapes and timed-based curves.
+ Multiple evaluation methods: progress (from 0.0 to 1.0), time (using X-axis) and distance.
+ Add, remove and move curve points as well as changing their tangent mode.
+ Evaluate the values of the curves in-editor.
+ Saving and loading .cvx files inside the editor.
+ Grid scaling with zoom.
+ **Free and open-source**.

## Inputs
+ **Ctrl+S**: Save the selected spline to a file
+ **Ctrl+L**: Import a spline from a file
+ **Ctrl+;**: Toggle debug mode

Focusing editor:
+ **F1**, **F2**, **F3**: Switch curve interpolation mode to Bezier, Time or Distance respectively.
+ **F**: Fit viewport to all curves.
+ **TAB**: Toggle visibility of control points.
+ **Delete**: Delete selected control point.
+ **Left Click**: Select control or tangent points.
+ Double **Left Click**: Add a control point to mouse location.
+ Holding **Ctrl** while moving a point: Snap the selected point to the grid.
+ Holding **Shift**: Evaluate the selected curve with the current interpolation mode.
+ **ALT+Wheel**: Change curve drawing thickness.
+ **Wheel**: Zoom to mouse location.


Focusing layers tab:
+ **Left Click**: Select a curve layer
+ **Delete**: Delete selected curve layer

## Project Structure
This project is split into two Github repositories: the GUI editor (this one) and the [library](https://github.com/arkaht/cpp-curve-x). You are not forced to use the editor, it is entirely optional but it is here to facilitate your experience with Curve-X.

**Folder structure:**
+ **`libs/`** contains all libraries necessary for the editor to compile
+ **`src/`** contains source files of the editor