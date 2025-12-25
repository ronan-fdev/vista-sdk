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
 * @file UniversalIdBuilder.cpp
 * @brief Implementation of VIS UniversalIdBuilder
 * @details Provides implementations for UniversalIdBuilder class for building UniversalId instances.
 */

#include "dnv/vista/sdk/UniversalIdBuilder.h"

#include "dnv/vista/sdk/ParsingErrors.h"
#include "dnv/vista/sdk/UniversalId.h"
#include "parsing/LocalIdParsingErrorBuilder.h"

#include <stdexcept>

namespace dnv::vista::sdk
{
	UniversalIdBuilder UniversalIdBuilder::create( VisVersion visVersion ) noexcept
	{
		UniversalIdBuilder builder;
		builder.m_localIdBuilder = LocalIdBuilder::create( visVersion );
		return builder;
	}

	UniversalIdBuilder UniversalIdBuilder::withImoNumber( const ImoNumber& imoNumber ) const
	{
		auto copy = *this;
		copy.m_imoNumber = imoNumber;
		return copy;
	}

	UniversalIdBuilder UniversalIdBuilder::withImoNumber( const std::optional<ImoNumber>& imoNumber ) const
	{
		if ( !imoNumber.has_value() )
		{
			throw std::invalid_argument( "ImoNumber cannot be nullopt" );
		}
		return withImoNumber( *imoNumber );
	}

	UniversalIdBuilder UniversalIdBuilder::withoutImoNumber() const noexcept
	{
		auto copy = *this;
		copy.m_imoNumber = std::nullopt;
		return copy;
	}

	UniversalIdBuilder UniversalIdBuilder::withLocalId( const LocalIdBuilder& localIdBuilder ) const
	{
		auto copy = *this;
		copy.m_localIdBuilder = localIdBuilder;
		return copy;
	}

	UniversalIdBuilder UniversalIdBuilder::withLocalId( const std::optional<LocalIdBuilder>& localIdBuilder ) const
	{
		if ( !localIdBuilder.has_value() )
		{
			throw std::invalid_argument( "LocalIdBuilder cannot be nullopt" );
		}
		return withLocalId( *localIdBuilder );
	}

	UniversalIdBuilder UniversalIdBuilder::withoutLocalId() const noexcept
	{
		auto copy = *this;
		copy.m_localIdBuilder = std::nullopt;
		return copy;
	}

	UniversalId UniversalIdBuilder::build() const
	{
		return UniversalId{ *this };
	}

	std::string UniversalIdBuilder::toString() const
	{
		if ( !m_imoNumber.has_value() )
		{
			throw std::invalid_argument( "Invalid Universal Id state: Missing IMO Number" );
		}
		if ( !m_localIdBuilder.has_value() )
		{
			throw std::invalid_argument( "Invalid Universal Id state: Missing LocalId" );
		}

		StringBuilder sb;

		sb.append( NamingEntity );
		sb.append( "/" );
		sb.append( m_imoNumber->toString() );

		m_localIdBuilder->toString( sb );

		return sb.toString();
	}

	std::optional<UniversalId> UniversalIdBuilder::fromString( std::string_view universalIdStr ) noexcept
	{
		ParsingErrors errors;
		return fromString( universalIdStr, errors );
	}

	std::optional<UniversalId> UniversalIdBuilder::fromString( std::string_view universalIdStr,
		ParsingErrors& errors ) noexcept
	{
		using namespace internal;

		// Format: data.dnv.com/9999999/dnv-v2/vis-3-4a/...
		LocalIdParsingErrorBuilder errorBuilder;

		if ( universalIdStr.empty() )
		{
			errorBuilder.addError( LocalIdParsingState::NamingRule, "Failed to find localId start segment" );
			errors = errorBuilder.build();
			return std::nullopt;
		}

		// Find the start of LocalId (search for "/dnv-v")
		const auto localIdStartPos = universalIdStr.find( "/dnv-v" );
		if ( localIdStartPos == std::string_view::npos )
		{
			errorBuilder.addError( LocalIdParsingState::NamingRule, "Failed to find localId start segment" );
			errors = errorBuilder.build();
			return std::nullopt;
		}

		// Split into universal ID segment and LocalId segment
		const auto universalIdSegment = universalIdStr.substr( 0, localIdStartPos );
		const auto localIdSegment = universalIdStr.substr( localIdStartPos );

		// Parse LocalId
		ParsingErrors localIdErrors;
		auto localId = LocalIdBuilder::fromString( localIdSegment, localIdErrors );
		if ( !localId.has_value() )
		{
			// Pass through errors from LocalIdBuilder
			errors = localIdErrors;
			return std::nullopt;
		}

		// Get the LocalIdBuilder from the parsed LocalId
		const auto& localIdBuilder = localId->builder();

		// Parse universal ID segment: "data.dnv.com/9999999"
		// Expected format: {NamingEntity}/{ImoNumber}
		std::optional<ImoNumber> imoNumber;

		size_t pos = 0;

		// Parse naming entity
		const auto firstSlash = universalIdSegment.find( '/', pos );
		if ( firstSlash == std::string_view::npos )
		{
			errorBuilder.addError( LocalIdParsingState::NamingEntity, "Invalid Universal ID format: missing IMO number" );
			errors = errorBuilder.build();
			return std::nullopt;
		}

		const auto namingEntitySegment = universalIdSegment.substr( pos, firstSlash );
		if ( namingEntitySegment != NamingEntity )
		{
			errorBuilder.addError( LocalIdParsingState::NamingEntity,
				std::string( "Naming entity segment didn't match. Found: " ) + std::string( namingEntitySegment ) );
			errors = errorBuilder.build();
			return std::nullopt;
		}

		pos = firstSlash + 1;

		// Parse IMO number
		const auto nextSlash = universalIdSegment.find( '/', pos );
		const auto imoSegment = nextSlash == std::string_view::npos ? universalIdSegment.substr( pos )
																	: universalIdSegment.substr( pos, nextSlash - pos );

		if ( !imoSegment.empty() )
		{
			auto imo = ImoNumber::fromString( imoSegment );
			if ( !imo.has_value() )
			{
				errorBuilder.addError( LocalIdParsingState::IMONumber, "Invalid IMO number segment" );
				errors = errorBuilder.build();
				return std::nullopt;
			}
			imoNumber = imo;
		}

		// Get VisVersion from localIdBuilder
		if ( !localIdBuilder.version().has_value() )
		{
			errorBuilder.addError( LocalIdParsingState::VisVersion, "Invalid VisVersion in LocalId" );
			errors = errorBuilder.build();
			return std::nullopt;
		}

		// Build UniversalIdBuilder
		auto builder = UniversalIdBuilder::create( *localIdBuilder.version() ).withLocalId( localIdBuilder );

		if ( imoNumber.has_value() )
		{
			builder = builder.withImoNumber( *imoNumber );
		}

		// Validate and build UniversalId
		if ( !builder.isValid() )
		{
			errorBuilder.addError( LocalIdParsingState::Completeness, "UniversalIdBuilder is invalid after parsing" );
			errors = errorBuilder.build();
			return std::nullopt;
		}

		errors = errorBuilder.build();
		return builder.build();
	}
} // namespace dnv::vista::sdk
