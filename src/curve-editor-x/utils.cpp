#include "utils.h"

#include <Windows.h>
#include <corecrt_math.h>

using namespace curve_editor_x;

float Utils::lerp( float a, float b, float t )
{
    return a + t * ( b - a );
}

float Utils::round( float value, float idp )
{
    float mul = powf( 10.0f, idp );
    return floorf( value * mul + 0.5f ) / mul;
}

float Utils::approach( float value, float target, float delta )
{
	delta = fabsf( delta );

	if ( value < target )
	{
		return fminf( value + delta, target );
	}
	else if ( value > target )
	{
		return fmaxf( value - delta, target );
	}

	return target;
}

std::string Utils::get_filename_from_path( const std::string& path )
{
	return path.substr( path.find_last_of( "/\\" ) + 1 );
}

std::wstring str_to_wstr( const std::string& str )
{
	return std::wstring( str.begin(), str.end() );
}

std::string concat_extensions( 
	const std::vector<std::string>& extensions 
)
{
	std::string text;

	int size = extensions.size();
	for ( int i = 0; i < size; i++ )
	{
		text += "*." + extensions[i];
		if ( i + 1 < size ) 
		{
			text += ";";
		}
	}

	return text;
}

std::string Utils::get_user_open_file_path(
	std::string title,
	std::string filter,
	std::vector<std::string> extensions
)
{
	//  Using string literals so '\0' are taken in account somehow
	using namespace std::literals::string_literals;

	//  Format texts
	std::string full_title = "Select a " + title + " file";
	std::string concated_extensions = concat_extensions( extensions );
	std::wstring filter_wstr = str_to_wstr( 
		  filter + "\0"s
		+ concated_extensions + "\0\0"s
	);
	std::wstring full_title_wstr = str_to_wstr( full_title );

	//  Setup Windows's open file name structure
    LPWSTR filebuff = new wchar_t[256];
	OPENFILENAME open = { 0 };
    open.lStructSize = sizeof( OPENFILENAME );
    open.hwndOwner = GetActiveWindow();
    open.lpstrFilter = filter_wstr.c_str();
    open.lpstrCustomFilter = NULL;
    open.lpstrFile = filebuff;
    open.lpstrFile[0] = '\0';
    open.nMaxFile = 256;
    open.nFilterIndex = 1;
    open.lpstrInitialDir = NULL;
    open.lpstrTitle = full_title_wstr.c_str();
    open.nMaxFileTitle = strlen( full_title.c_str() );
    open.Flags = OFN_PATHMUSTEXIST 
			   | OFN_FILEMUSTEXIST 
			   | OFN_EXPLORER;
	
	//  Open file dialog
	if ( GetOpenFileName( &open ) )
	{
		std::wstring path( open.lpstrFile );
		return std::string( path.begin(), path.end() );
	}

	return std::string();
}

std::string Utils::get_user_save_file_path( 
	std::string title, 
	std::string filter, 
	std::vector<std::string> extensions
)
{
	//  Using string literals so '\0' are taken in account somehow
	using namespace std::literals::string_literals;

	//  Format texts
	auto full_title = "Select a " + title + " file";
	auto concated_extensions = concat_extensions( extensions );
	auto filter_wstr = str_to_wstr( 
		  filter + "\0"s
		+ concated_extensions + "\0\0"s
	);
	auto full_title_wstr = str_to_wstr( full_title );
	auto default_extension_wstr = str_to_wstr( extensions.at( 0 ) );

	//  Setup Windows's open file name structure
    LPWSTR filebuff = new wchar_t[256];
	OPENFILENAME open = { 0 };
    open.lStructSize = sizeof( OPENFILENAME );
    open.hwndOwner = GetActiveWindow();
    open.lpstrFilter = filter_wstr.c_str();
    open.lpstrCustomFilter = NULL;
    open.lpstrFile = filebuff;
    open.lpstrFile[0] = '\0';
    open.nMaxFile = 256;
    open.nFilterIndex = 1;
    open.lpstrInitialDir = NULL;
    open.lpstrTitle = full_title_wstr.c_str();
    open.nMaxFileTitle = strlen( full_title.c_str() );
	open.lpstrDefExt = default_extension_wstr.c_str();
    open.Flags = OFN_OVERWRITEPROMPT;
	
	//  Open file dialog
	if ( GetSaveFileName( &open ) )
	{
		std::wstring path( open.lpstrFile );
		return std::string( path.begin(), path.end() );
	}

	return std::string();
}