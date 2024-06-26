#pragma once

#include <string>
#include <vector>

namespace curve_editor_x
{
	class Utils
	{
	public:
		static float near_zero( float value, float epsilon = 0.001f );

		static float lerp( float a, float b, float t );

		static float round( float value, float idp );

		static float approach( float value, float target, float delta );

		static std::string get_filename_from_path( const std::string& path );

		/*
		 * Open a dialog asking the user to open a file.
		 * Supported OS: Windows only
		 */
		static std::string get_user_open_file(
			std::string title,
			std::string filter,
			std::vector<std::string> extensions
		);

		/*
		 * Open a dialog asking the user to open multiple files.
		 * Supported OS: Windows only
		 */
		static std::vector<std::string> get_user_open_files(
			std::string title,
			std::string filter,
			std::vector<std::string> extensions
		);

		/*
		 * Open a dialog asking the user to save a file.
		 * Supported OS: Windows only
		 */
		static std::string get_user_save_file(
			std::string title,
			std::string filter,
			std::vector<std::string> extensions
		);
	};
}