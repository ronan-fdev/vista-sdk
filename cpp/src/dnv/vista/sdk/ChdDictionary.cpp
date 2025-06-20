/**
 * @file ChdDictionary.cpp
 * @brief Implementation of CHD Dictionary components
 */

#include "pch.h"

#include "dnv/vista/sdk/ChdDictionary.h"

namespace dnv::vista::sdk
{
	//======================================================================
	// Exception class implementations
	//======================================================================

	key_not_found_exception::key_not_found_exception( std::string_view key )
		: std::runtime_error{ fmt::format( "No value associated to key: {}", key ) }
	{
	}

	invalid_operation_exception::invalid_operation_exception()
		: std::runtime_error{ "Operation is not valid due to the current state of the object." }
	{
	}

	invalid_operation_exception::invalid_operation_exception( std::string_view message )
		: std::runtime_error{ std::string{ message } }
	{
	}

	namespace
	{
		//=====================================================================
		// Internal helper components
		//=====================================================================

		//----------------------------------------------
		// ThrowHelper class
		//----------------------------------------------

		//----------------------------
		// Public static methods
		//----------------------------

		void ThrowHelper::throwKeyNotFoundException( std::string_view key )
		{
			throw key_not_found_exception( key );
		}

		void ThrowHelper::throwInvalidOperationException()
		{
			throw invalid_operation_exception();
		}
	}
}
