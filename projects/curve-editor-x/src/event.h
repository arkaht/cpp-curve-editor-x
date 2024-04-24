#pragma once

#include <string>
#include <functional>
#include <map>

/*
 * Directly copied from my 'suprengine' project:
 * https://github.com/arkaht/cpp-suprengine/blob/main/src/suprengine/event.hpp
 */

namespace curve_editor_x
{
	template <typename ...TVarargs> 
	class Event
	{
	public:
		using func_signature = std::function<void( TVarargs... )>;

	private:
		std::map<std::string, func_signature> observers;

	public:
		void listen( const std::string& key, func_signature observer )
		{
			observers[key] = observer;
		}

		void unlisten( const std::string& key )
		{
			auto itr = observers.find( key );
			if ( itr == observers.end() ) return;

			observers.erase( itr );  //  TODO: check if erasing during 'invoke()' makes issues  //  yes it does..
		}

		void invoke( TVarargs ...args )
		{
			for ( auto pair : observers )
				pair.second( args... );
		}
	};
}