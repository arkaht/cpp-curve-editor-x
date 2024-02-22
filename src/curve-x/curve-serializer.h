#pragma once

#include <string>

#include "curve.h"

namespace curve_x
{
	const int         FORMAT_VERSION = 1;
	const std::string FORMAT_EXTENSION = "cvx";


	class CurveSerializer
	{
	public:
		CurveSerializer();

		std::string serialize( const Curve& curve );

		Curve unserialize( const std::string& data );

	private:
		int _to_int( const std::string& str );
		float _to_float( const std::string& str );
		Point _to_point( 
			const std::string& str_x, 
			const std::string& str_y 
		);
	};
}