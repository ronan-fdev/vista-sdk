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
 * @file CodebookName.inl
 * @brief CodebookName inline implementations
 * @details Inline method implementations for CodebookName conversions
 */

#include <stdexcept>

namespace dnv::vista::sdk::CodebookNames
{
	inline CodebookName fromPrefix( std::string_view prefix )
	{
		if ( prefix == "qty" )
		{
			return CodebookName::Quantity;
		}
		if ( prefix == "cnt" )
		{
			return CodebookName::Content;
		}
		if ( prefix == "calc" )
		{
			return CodebookName::Calculation;
		}
		if ( prefix == "state" )
		{
			return CodebookName::State;
		}
		if ( prefix == "cmd" )
		{
			return CodebookName::Command;
		}
		if ( prefix == "type" )
		{
			return CodebookName::Type;
		}
		if ( prefix == "funct.svc" )
		{
			return CodebookName::FunctionalServices;
		}
		if ( prefix == "maint.cat" )
		{
			return CodebookName::MaintenanceCategory;
		}
		if ( prefix == "act.type" )
		{
			return CodebookName::ActivityType;
		}
		if ( prefix == "pos" )
		{
			return CodebookName::Position;
		}
		if ( prefix == "detail" )
		{
			return CodebookName::Detail;
		}

		throw std::invalid_argument{ "Unknown prefix: " + std::string{ prefix } };
	}

	inline std::string toPrefix( CodebookName name )
	{
		switch ( name )
		{
			case CodebookName::Quantity:
			{
				return "qty";
			}
			case CodebookName::Content:
			{
				return "cnt";
			}
			case CodebookName::Calculation:
			{
				return "calc";
			}
			case CodebookName::State:
			{
				return "state";
			}
			case CodebookName::Command:
			{
				return "cmd";
			}
			case CodebookName::Type:
			{
				return "type";
			}
			case CodebookName::FunctionalServices:
			{
				return "funct.svc";
			}
			case CodebookName::MaintenanceCategory:
			{
				return "maint.cat";
			}
			case CodebookName::ActivityType:
			{
				return "act.type";
			}
			case CodebookName::Position:
			{
				return "pos";
			}
			case CodebookName::Detail:
			{
				return "detail";
			}
			default:
			{
				throw std::invalid_argument{ "Unknown codebook: " + std::to_string( static_cast<std::uint8_t>( name ) ) };
				break;
			}
		}

		return "";
	}
} // namespace dnv::vista::sdk::CodebookNames
