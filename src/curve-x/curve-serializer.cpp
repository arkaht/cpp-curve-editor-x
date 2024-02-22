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

	return Curve( keys );
}

int CurveSerializer::_to_int( const std::string& str )
{
	return atoi( str.c_str() );
}

float CurveSerializer::_to_float( const std::string& str )
{
	return (float)atof( str.c_str() );
}

Point CurveSerializer::_to_point( 
	const std::string& str_x, 
	const std::string& str_y 
)
{
	return Point( _to_float( str_x ), _to_float( str_y ) );
}
