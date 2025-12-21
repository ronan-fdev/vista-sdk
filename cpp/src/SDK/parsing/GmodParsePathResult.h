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
 * @file GmodParsePathResult.h
 * @brief Internal result type for GMOD path parsing operations
 * @details Result monad (Ok/Error) for propagating detailed parsing errors from internal parsing functions.
 *          Not exposed in public API - only used internally between parsing implementation layers.
 */

#pragma once

#include "dnv/vista/sdk/GmodPath.h"

#include <string>
#include <variant>

namespace dnv::vista::sdk::internal
{
	struct GmodParsePathResult
	{
		struct Ok
		{
			GmodPath path;

			inline explicit Ok( GmodPath p )
				: path{ std::move( p ) }
			{
			}
		};

		struct Error
		{
			std::string error;

			inline explicit Error( std::string e )
				: error{ std::move( e ) }
			{
			}
		};

		std::variant<Ok, Error> result;

		inline GmodParsePathResult( Ok ok )
			: result{ std::move( ok ) }
		{
		}

		inline GmodParsePathResult( Error err )
			: result{ std::move( err ) }
		{
		}

		inline bool isOk() const noexcept
		{
			return std::holds_alternative<Ok>( result );
		}

		inline bool isError() const noexcept
		{
			return std::holds_alternative<Error>( result );
		}

		inline Ok& ok()
		{
			return std::get<Ok>( result );
		}

		inline const Ok& ok() const
		{
			return std::get<Ok>( result );
		}

		inline Error& error()
		{
			return std::get<Error>( result );
		}

		inline const Error& error() const
		{
			return std::get<Error>( result );
		}
	};
} // namespace dnv::vista::sdk::internal
