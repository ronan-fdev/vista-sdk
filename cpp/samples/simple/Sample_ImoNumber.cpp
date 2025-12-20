/*
 * MIT License
 *
 * Copyright (c) 2025 DNV
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file Sample_ImoNumber.cpp
 * @brief Demonstrates usage of vista-sdk-cpp ImoNumber API
 * @details This sample shows how to create, validate, and parse IMO numbers
 *          with checksum verification per ISO 19848:2024
 */

#include <dnv/vista/sdk/ImoNumber.h>

#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main()
{
	using namespace dnv::vista::sdk;

	std::cout << "=== vista-sdk-cpp ImoNumber Sample ===\n\n";

	//=====================================================================
	// 1. ImoNumber: Creating from integer
	//=====================================================================
	{
		std::cout << "1. ImoNumber: Creating from integer\n";
		std::cout << "--------------------------------------\n";

		// Create valid IMO numbers
		auto imo1 = ImoNumber{ 9074729 };
		auto imo2 = ImoNumber{ 9785811 };
		auto imo3 = ImoNumber{ 1234567 };

		std::cout << "Created IMO numbers:\n";
		std::cout << "  " << imo1.toString() << "\n";
		std::cout << "  " << imo2.toString() << "\n";
		std::cout << "  " << imo3.toString() << "\n";

		// Try invalid number (will throw)
		std::cout << "\nAttempting to create invalid IMO number (1234507):\n";
		try
		{
			[[maybe_unused]] auto invalid = ImoNumber{ 1234507 };
			std::cout << "  ERROR: Invalid number accepted (unexpected)\n";
		}
		catch ( const std::invalid_argument& e )
		{
			std::cout << "  Correctly rejected: " << e.what() << "\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 2. ImoNumber: Creating from string
	//=====================================================================
	{
		std::cout << "2. ImoNumber: Creating from string\n";
		std::cout << "-------------------------------------\n";

		// With "IMO" prefix
		auto imo1 = ImoNumber{ "IMO9074729" };
		std::cout << "From \"IMO9074729\": " << imo1.toString() << "\n";

		// Without "IMO" prefix
		auto imo2 = ImoNumber{ "9785811" };
		std::cout << "From \"9785811\":    " << imo2.toString() << "\n";

		// Both create equivalent objects
		std::cout << std::boolalpha;
		std::cout << "Are they equal? " << ( imo1 == ImoNumber{ "IMO9074729" } ) << "\n";
		std::cout << "Are they equal? " << ( imo2 == ImoNumber{ "IMO9785811" } ) << "\n";

		// Try invalid string (will throw)
		std::cout << "\nAttempting to create from invalid string (\"IM9074729\"):\n";
		try
		{
			[[maybe_unused]] auto invalid = ImoNumber{ "IM9074729" };
			std::cout << "  ERROR: Invalid string accepted (unexpected)\n";
		}
		catch ( const std::invalid_argument& e )
		{
			std::cout << "  Correctly rejected: " << e.what() << "\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 3. ImoNumber::isValid(): Validating integers
	//=====================================================================
	{
		std::cout << "3. ImoNumber::isValid(): Validating integers\n";
		std::cout << "-----------------------------------------------\n";

		std::vector<int> testNumbers = {
			9074729,  // Valid
			9785811,  // Valid
			1234567,  // Valid
			1234507,  // Invalid checksum
			123456,	  // Too short
			12345678, // Too long
			0,		  // Invalid
			-1		  // Invalid
		};

		std::cout << std::boolalpha;
		std::cout << "Validation results:\n";
		for ( int num : testNumbers )
		{
			bool valid = ImoNumber::isValid( num );
			std::cout << "  " << std::setw( 10 ) << num << ": " << valid << "\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 4. ImoNumber::fromString(): Safe parsing without exceptions
	//=====================================================================
	{
		std::cout << "4. ImoNumber::fromString(): Safe parsing without exceptions\n";
		std::cout << "-------------------------------------------------------------\n";

		std::vector<std::string> testStrings = {
			"9074729",
			"IMO9785811",
			"1234567",
			"1234507",	 // Invalid checksum
			"IM9074729", // Typo in prefix
			"",			 // Empty
			"abc"		 // Not a number
		};

		std::cout << "Parsing results:\n";
		for ( const auto& str : testStrings )
		{
			auto result = ImoNumber::fromString( str );

			std::cout << "  \"" << std::setw( 15 ) << std::left << str << "\" -> ";

			if ( result.has_value() )
			{
				std::cout << result->toString() << "\n";
			}
			else
			{
				std::cout << "Invalid\n";
			}
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 5. ImoNumber::fromString()
	//=====================================================================
	{
		std::cout << "5. ImoNumber::fromString()\n";
		std::cout << "--------------------------------------------------\n";

		std::string input = "IMO9074729";

		// Pattern 1: Direct if with optional
		if ( auto imo = ImoNumber::fromString( input ) )
		{
			std::cout << "Parsed successfully: " << imo->toString() << "\n";
		}
		else
		{
			std::cout << "Parse failed\n";
		}

		// Pattern 2: has_value() check
		auto result = ImoNumber::fromString( "9785811" );
		if ( result.has_value() )
		{
			std::cout << "Parsed successfully: " << result.value().toString() << "\n";
		}

		// Pattern 3: value_or with default
		auto imo = ImoNumber::fromString( "invalid" ).value_or( ImoNumber{ 9074729 } );
		std::cout << "With fallback: " << imo.toString() << "\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 6. ImoNumber: Comparison operators
	//=====================================================================
	{
		std::cout << "6. ImoNumber: Comparison operators\n";
		std::cout << "-------------------------------------\n";

		auto imo1 = ImoNumber{ 9074729 };
		auto imo2 = ImoNumber{ "IMO9074729" };
		auto imo3 = ImoNumber{ 9785811 };

		std::cout << std::boolalpha;
		std::cout << "Equality:\n";
		std::cout << "  9074729 == IMO9074729? " << ( imo1 == imo2 ) << "\n";
		std::cout << "  9074729 == 9785811?    " << ( imo1 == imo3 ) << "\n";

		std::cout << "\nInequality:\n";
		std::cout << "  9074729 != 9785811?    " << ( imo1 != imo3 ) << "\n";
		std::cout << "  9074729 != IMO9074729? " << ( imo1 != imo2 ) << "\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 7. ImoNumber: toString() formatting
	//=====================================================================
	{
		std::cout << "7. ImoNumber: toString() formatting\n";
		std::cout << "--------------------------------------\n";

		auto imo = ImoNumber{ 9074729 };

		std::cout << "String representation:\n";
		std::cout << "  toString(): " << imo.toString() << "\n";
		std::cout << "  Format: Always includes \"IMO\" prefix\n";
		std::cout << "  Format: Always 7 digits after \"IMO\"\n";

		// Different input formats, same output
		std::cout << "\nConsistent output:\n";
		std::cout << "  From int 9074729:     " << ImoNumber{ 9074729 }.toString() << "\n";
		std::cout << "  From \"9074729\":        " << ImoNumber{ "9074729" }.toString() << "\n";
		std::cout << "  From \"IMO9074729\":     " << ImoNumber{ "IMO9074729" }.toString() << "\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 8. ImoNumber: Checksum validation explained
	//=====================================================================
	{
		std::cout << "8. ImoNumber: Checksum validation explained\n";
		std::cout << "----------------------------------------------\n";

		std::cout << "IMO number format:\n";
		std::cout << "  7 digits: 6-digit sequential number + 1 check digit\n";
		std::cout << "  Example: 9074729\n\n";

		std::cout << "Checksum calculation for 9074729:\n";
		std::cout << "  9×7 + 0×6 + 7×5 + 4×4 + 7×3 + 2×2 = 139\n";
		std::cout << "  Check digit = 139 % 10 = 9\n";
		std::cout << "  Last digit of IMO number = 9\n";
		std::cout << "  Valid? " << std::boolalpha << ( 9 == 9 ) << "\n\n";

		std::cout << "Invalid example: 1234507\n";
		std::cout << "  1×7 + 2×6 + 3×5 + 4×4 + 5×3 + 0×2 = 62\n";
		std::cout << "  Check digit = 62 % 10 = 2\n";
		std::cout << "  Last digit of IMO number = 7\n";
		std::cout << "  Valid? " << std::boolalpha << ( 7 == 2 ) << "\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 9. ImoNumber: Real-world examples
	//=====================================================================
	{
		std::cout << "9. ImoNumber: Real-world examples\n";
		std::cout << "------------------------------------\n";

		// Real IMO numbers
		struct Ship
		{
			std::string name;
			int imoNumber;
		};

		std::vector<Ship> ships = {
			{ "Queen Mary 2", 9241061 },
			{ "Oasis of the Seas", 9383936 },
			{ "Symphony of the Seas", 9744001 } };

		std::cout << "Famous ships:\n";
		for ( const auto& ship : ships )
		{
			if ( ImoNumber::isValid( ship.imoNumber ) )
			{
				auto imo = ImoNumber{ ship.imoNumber };
				std::cout << "  " << std::setw( 25 ) << std::left << ship.name;
				std::cout << ": " << imo.toString() << "\n";
			}
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 10. ImoNumber: Error handling patterns
	//=====================================================================
	{
		std::cout << "10. ImoNumber: Error handling patterns\n";
		std::cout << "-----------------------------------------\n";

		std::string userInput = "9074729"; // Simulated user input

		std::cout << "Pattern 1: Constructor with try-catch (throws on error):\n";
		try
		{
			auto imo = ImoNumber{ userInput };
			std::cout << "  Success: " << imo.toString() << "\n";
		}
		catch ( const std::invalid_argument& e )
		{
			std::cout << "  Error: " << e.what() << "\n";
		}

		std::cout << "\nPattern 2: fromString() with optional (no exceptions):\n";
		if ( auto imo = ImoNumber::fromString( userInput ) )
		{
			std::cout << "  Success: " << imo->toString() << "\n";
		}
		else
		{
			std::cout << "  Error: Invalid IMO number format\n";
		}

		std::cout << "\nPattern 3: Pre-validation with isValid():\n";
		if ( auto parsed = ImoNumber::fromString( userInput ) )
		{
			// Already validated by fromString
			std::cout << "  Valid IMO: " << parsed->toString() << "\n";
		}
		else
		{
			std::cout << "  Validation failed\n";
		}

		std::cout << "\n";
	}

	return 0;
}
