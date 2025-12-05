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
 * @file ResourceGenerator.cpp
 * @brief Fast binary-to-C++ resource embedder
 * @details Converts binary files (typically .json.gz) into C++ byte arrays.
 */

#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

int main( int argc, char* argv[] )
{
	// Validate arguments
	if ( argc != 5 )
	{
		std::cerr << "Usage: ResourceGenerator <input_file> <output_file> <resource_id> <resource_name>\n";
		std::cerr << "\n";
		std::cerr << "Arguments:\n";
		std::cerr << "  input_file   : Path to binary resource file (e.g., vis-3-4a.json.gz)\n";
		std::cerr << "  output_file  : Path to generated C++ file (e.g., embedded_vis_3_4a_json_gz.cpp)\n";
		std::cerr << "  resource_id  : C++ identifier for the resource (e.g., vis_3_4a_json_gz)\n";
		std::cerr << "  resource_name: Original resource filename (e.g., vis-3-4a.json.gz)\n";
		return 1;
	}

	const std::string inputFile = argv[1];
	const std::string outputFile = argv[2];
	const std::string resourceId = argv[3];
	const std::string resourceName = argv[4];

	// Read binary input file
	std::ifstream input( inputFile, std::ios::binary );
	if ( !input )
	{
		std::cerr << "Error: Cannot open input file: " << inputFile << "\n";
		return 1;
	}

	// Read entire file into memory using istreambuf_iterator
	std::vector<uint8_t> data(
		( std::istreambuf_iterator<char>( input ) ),
		std::istreambuf_iterator<char>() );
	input.close();

	if ( data.empty() )
	{
		std::cerr << "Error: Input file is empty: " << inputFile << "\n";
		return 1;
	}

	// Open output file for writing
	std::ofstream output( outputFile );
	if ( !output )
	{
		std::cerr << "Error: Cannot create output file: " << outputFile << "\n";
		return 1;
	}

	// Write C++ file header
	output << "// Auto-generated resource: " << resourceName << "\n";
	output << "// Source: " << inputFile << "\n";
	output << "// Generator: ResourceGenerator (vista-sdk-cpp)\n";
	output << "\n";
	output << "#include <cstdint>\n";
	output << "#include <cstddef>\n";
	output << "\n";
	output << "namespace dnv::vista::sdk::registry\n";
	output << "{\n";

	// Write byte array declaration
	// Write data array with external linkage
	output << "    extern const uint8_t " << resourceId << "_data[] = {\n";

	// Write bytes in hex format (16 bytes per line)
	for ( size_t i = 0; i < data.size(); ++i )
	{
		// Indentation at start of line
		if ( i % 16 == 0 )
		{
			output << "        ";
		}

		// Write hex byte: 0xAB
		output << "0x" << std::hex << std::setw( 2 ) << std::setfill( '0' ) << static_cast<int>( data[i] );

		// Add comma except for last byte
		if ( i < data.size() - 1 )
		{
			output << ",";
		}

		// Add newline every 16 bytes or at end
		if ( i % 16 == 15 || i == data.size() - 1 )
		{
			output << "\n";
		}
	}

	// Write array size and name constants with external linkage
	output << "    };\n";
	output << "\n";
	output << "    extern const size_t " << resourceId << "_size = " << std::dec << data.size() << ";\n";
	output << "\n";
	output << "    extern const char " << resourceId << "_name[] = \"" << resourceName << "\";\n";
	output << "} // namespace dnv::vista::sdk::registry\n";

	output.close();

	std::cout << "Generated: " << resourceName << " (" << data.size() << " bytes)\n";

	return 0;
}
