/**
 * @file Exceptions.h
 * @brief Vista SDK exception classes
 */

#pragma once

namespace dnv::vista::sdk
{
	//=====================================================================
	// Base Vista exception
	//=====================================================================

	/**
	 * @brief Base exception class for Vista SDK
	 */
	class VistaException : public std::runtime_error
	{
	public:
		explicit VistaException( const std::string& message )
			: std::runtime_error{ "VistaException - " + message }
		{
		}
	};

	//=====================================================================
	// Validation exception
	//=====================================================================

	/**
	 * @brief Exception thrown for validation failures
	 */
	class ValidationException : public VistaException
	{
	public:
		explicit ValidationException( const std::string& message )
			: VistaException{ "Validation failed - Message='" + message + "'" }
		{
		}
	};
}
