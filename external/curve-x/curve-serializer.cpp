#include "curve-serializer.h"

#include <regex>

using namespace curve_x;

CurveSerializer::CurveSerializer()
{
}

std::string CurveSerializer::serialize( const Curve& curve )
{
	std::stringstream stream;

	//  Append library version
	stream << "version:" << FORMAT_VERSION << '\n';

	//  Append keys
	int keys_count = curve.get_keys_count();
	for ( int key_id = 0; key_id < keys_count; key_id++ )
	{
		const CurveKey& key = curve.get_key( key_id );
		stream << key_id << ':';
		stream << key.control.str() << ',';
		stream << key.left_tangent.str() << ',';
		stream << key.right_tangent.str() << ',';
		stream << (int)key.tangent_mode << '\n';
	}

	return stream.str();
}

Curve CurveSerializer::unserialize( const std::string& data )
{
	int version = -1;
	std::vector<CurveKey> keys;

	//  Regexes
	std::regex REGEX_VERSION( "version:(\\d+)" );
	std::regex REGEX_KEY_ID( "(\\d+):" );
	std::regex REGEX_POINT( "x=(\\-?\\d+\\.\\d+);y=(\\-?\\d+\\.\\d+)" );
	std::regex REGEX_NUMBER( "\\-?(\\d+)" );

	//  Read data line per line
	std::istringstream iss( data );
	for ( std::string line; std::getline( iss, line ); )
	{
		//printf( "%s\n", line.c_str() );

		std::smatch match;

		//  Find format version
		if ( version == -1 )
		{
			if ( !std::regex_match( line, match, REGEX_VERSION ) )
			{
				throw std::runtime_error( 
					"Expected format version at the first line!" 
				);
			}

			//  Convert string to integer
			version = _to_int( match[1].str() );

			continue;
		}

		//  Parse a curve key
		if ( std::regex_search( line, match, REGEX_KEY_ID ) )
		{
			//  NOT USED:
			// int key_id = atoi( match[1].str().c_str() );
			line = match.suffix().str();

			//  Match control point
			std::regex_search( line, match, REGEX_POINT );
			Point control = _to_point( match[1], match[2] );
			line = match.suffix().str();

			//  Match left tangent point
			std::regex_search( line, match, REGEX_POINT );
			Point left_tangent = _to_point( match[1], match[2] );
			line = match.suffix().str();

			//  Match right tangent point
			std::regex_search( line, match, REGEX_POINT );
			Point right_tangent = _to_point( match[1], match[2] );
			line = match.suffix().str();

			//  Match tangent mode
			std::regex_search( line, match, REGEX_NUMBER );
			int mode_id = _to_int( match[1] );
			TangentMode tangent_mode = (TangentMode)mode_id;
			line = match.suffix().str();

			//  Create key
			keys.emplace_back(
				control, 
				left_tangent, 
				right_tangent, 
				tangent_mode 
			);
			//printf( "New key!\n" );
		}
	}

	return Curve( keys );
}

int CurveSerializer::_to_int( const std::string& str )
{
	return std::stoi( str.c_str() );
}

float CurveSerializer::_to_float( const std::string& str )
{
	return std::stof( str.c_str() );
}

Point CurveSerializer::_to_point( 
	const std::string& str_x, 
	const std::string& str_y 
)
{
	return Point( _to_float( str_x ), _to_float( str_y ) );
}
