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
 * @file LocalIdParsingErrorBuilder.cpp
 * @brief Implementation of LocalIdParsingErrorBuilder
 */

#include "LocalIdParsingErrorBuilder.h"

#include "dnv/vista/sdk/ParsingErrors.h"

namespace dnv::vista::sdk::internal
{
	const LocalIdParsingErrorBuilder& LocalIdParsingErrorBuilder::empty() noexcept
	{
		static const LocalIdParsingErrorBuilder instance{};
		return instance;
	}

	bool LocalIdParsingErrorBuilder::hasError() const noexcept
	{
		return !m_errors.empty();
	}

	ParsingErrors LocalIdParsingErrorBuilder::build() const
	{
		if ( m_errors.empty() )
		{
			return ParsingErrors::empty();
		}

		std::vector<ParsingErrors::ErrorEntry> errors;
		errors.reserve( m_errors.size() );

		for ( const auto& [state, message] : m_errors )
		{
			std::string stateStr;
			switch ( state )
			{
				case LocalIdParsingState::NamingRule:
					stateStr = "NamingRule";
					break;
				case LocalIdParsingState::VisVersion:
					stateStr = "VisVersion";
					break;
				case LocalIdParsingState::PrimaryItem:
					stateStr = "PrimaryItem";
					break;
				case LocalIdParsingState::SecondaryItem:
					stateStr = "SecondaryItem";
					break;
				case LocalIdParsingState::ItemDescription:
					stateStr = "ItemDescription";
					break;
				case LocalIdParsingState::MetaQuantity:
					stateStr = "MetaQuantity";
					break;
				case LocalIdParsingState::MetaContent:
					stateStr = "MetaContent";
					break;
				case LocalIdParsingState::MetaCalculation:
					stateStr = "MetaCalculation";
					break;
				case LocalIdParsingState::MetaState:
					stateStr = "MetaState";
					break;
				case LocalIdParsingState::MetaCommand:
					stateStr = "MetaCommand";
					break;
				case LocalIdParsingState::MetaType:
					stateStr = "MetaType";
					break;
				case LocalIdParsingState::MetaPosition:
					stateStr = "MetaPosition";
					break;
				case LocalIdParsingState::MetaDetail:
					stateStr = "MetaDetail";
					break;
				case LocalIdParsingState::EmptyState:
					stateStr = "EmptyState";
					break;
				case LocalIdParsingState::Formatting:
					stateStr = "Formatting";
					break;
				case LocalIdParsingState::Completeness:
					stateStr = "Completeness";
					break;
				case LocalIdParsingState::NamingEntity:
					stateStr = "NamingEntity";
					break;
				case LocalIdParsingState::IMONumber:
					stateStr = "IMONumber";
					break;
			}

			errors.emplace_back( std::move( stateStr ), message );
		}

		return ParsingErrors{ std::move( errors ) };
	}

	LocalIdParsingErrorBuilder& LocalIdParsingErrorBuilder::addError( LocalIdParsingState state,
		std::string_view message )
	{
		m_errors.emplace_back( state, std::string{ message } );
		return *this;
	}
} // namespace dnv::vista::sdk::internal
