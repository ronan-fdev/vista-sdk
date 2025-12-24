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
 * @file LocalIdBuilder.cpp
 * @brief Implementation of VIS LocalId builder
 * @details Provides implementations for LocalIdBuilder class for building LocalId instances.
 */

#include "dnv/vista/sdk/LocalIdBuilder.h"

#include "dnv/vista/sdk/LocalId.h"
#include "dnv/vista/sdk/ParsingErrors.h"
#include "dnv/vista/sdk/VIS.h"
#include "parsing/LocalIdParsingErrorBuilder.h"

#include <nfx/string/Utils.h>

#include <algorithm>
#include <cctype>

namespace dnv::vista::sdk
{
	namespace
	{
		using namespace dnv::vista::sdk;
		using namespace dnv::vista::sdk::internal;

		inline size_t findNext( std::string_view str, size_t pos, char ch )
		{
			auto idx = str.find( ch, pos );
			return idx == std::string_view::npos ? str.size() : idx;
		}

		inline std::string_view extractSegment( std::string_view str, size_t& pos )
		{
			if ( pos >= str.size() )
				return {};

			size_t start = pos;
			size_t end = findNext( str, pos, '/' );
			pos = end;

			return str.substr( start, end - start );
		}

		inline void skipSlash( size_t& pos )
		{
			++pos;
		}

		/**
		 * @brief Normalize a common name for URI-safe verbose mode output
		 * @details Converts common names to lowercase, replaces spaces with dots,
		 *          removes special characters, and collapses consecutive dots.
		 *          This follows VIS LocalId conventions for verbose mode.
		 * @param name The common name to normalize
		 * @param builder StringBuilder to append the normalized name to
		 */
		inline void appendNormalizedCommonName( std::string_view name, StringBuilder& builder )
		{
			using namespace nfx::string;

			// Remove unwanted characters: /, (, ), , &
			auto cleaned = removeIf( name, []( char c ) {
				return c == '/' || c == '(' || c == ')' || c == ',' || c == '&';
			} );

			// Collapse consecutive whitespace to single spaces
			auto collapsed = collapseWhitespace( cleaned );

			// Replace spaces with dots, convert to lowercase, avoid consecutive dots
			char prevChar = '\0';
			for ( char ch : collapsed )
			{
				char current = ( ch == ' ' ) ? '.' : toLower( ch );

				// Skip consecutive dots
				if ( current == '.' && prevChar == '.' )
				{
					continue;
				}

				builder.append( std::string_view( &current, 1 ) );
				prevChar = current;
			}
		}

		bool parseNamingRule( std::string_view str, size_t& pos, LocalIdParsingErrorBuilder& errorBuilder )
		{
			using namespace nfx::string;

			auto segment = extractSegment( str, pos );

			if ( isEmpty( segment ) )
			{
				errorBuilder.addError( LocalIdParsingState::NamingRule, "Missing naming rule" );
				return false;
			}

			if ( !equals( segment, LocalIdBuilder::NamingRule ) )
			{
				errorBuilder.addError( LocalIdParsingState::NamingRule,
					std::string{ "Invalid naming rule, expected 'dnv-v2', got '" } + std::string{ segment } + "'" );
				return false;
			}

			skipSlash( pos );
			return true;
		}

		std::optional<VisVersion> parseVisVersion( std::string_view str, size_t& pos, LocalIdParsingErrorBuilder& errorBuilder )
		{
			using namespace nfx::string;

			auto segment = extractSegment( str, pos );

			if ( isEmpty( segment ) )
			{
				errorBuilder.addError( LocalIdParsingState::VisVersion, "Missing VIS version" );
				return std::nullopt;
			}

			if ( !startsWith( segment, "vis-" ) )
			{
				errorBuilder.addError( LocalIdParsingState::VisVersion,
					std::string{ "Invalid VIS version format, expected 'vis-X-Ya', got '" } + std::string{ segment } + "'" );
				return std::nullopt;
			}

			auto versionStr = substringAfter( segment, "vis-" );
			VisVersion version;
			if ( !VisVersions::fromString( versionStr, version ) )
			{
				errorBuilder.addError( LocalIdParsingState::VisVersion,
					std::string{ "Unknown VIS version: '" } + std::string{ versionStr } + "'" );
				return std::nullopt;
			}

			skipSlash( pos );
			return version;
		}

		std::optional<GmodPath> parsePrimaryItem(
			std::string_view str,
			size_t& pos,
			const Gmod& gmod,
			const Locations& locations,
			LocalIdParsingErrorBuilder& errorBuilder )
		{
			// Find end of primary item (either /sec, /meta, or /~ for verbose mode)
			size_t start = pos;
			size_t pathEnd = pos;

			while ( pathEnd < str.size() )
			{
				size_t nextSlash = findNext( str, pathEnd, '/' );
				if ( nextSlash == str.size() )
				{
					pathEnd = nextSlash;
					break;
				}

				// Peek ahead to see if we hit /sec, /meta, or /~ (verbose mode)
				size_t peekStart = nextSlash + 1;
				if ( peekStart < str.size() )
				{
					using namespace nfx::string;
					auto remaining = str.substr( peekStart );

					if ( startsWith( remaining, "sec/" ) || startsWith( remaining, "sec" ) )
					{
						pathEnd = nextSlash;
						break;
					}
					if ( startsWith( remaining, "meta" ) )
					{
						pathEnd = nextSlash;
						break;
					}
					if ( str[peekStart] == '~' )
					{
						pathEnd = nextSlash;
						break;
					}
				}

				pathEnd = nextSlash + 1;
			}

			if ( pathEnd == start )
			{
				errorBuilder.addError( LocalIdParsingState::PrimaryItem, "Missing primary item" );
				return std::nullopt;
			}

			auto pathStr = str.substr( start, pathEnd - start );
			ParsingErrors parseErrors;
			auto path = GmodPath::fromString( pathStr, gmod, locations, parseErrors );

			if ( !path.has_value() )
			{
				errorBuilder.addError( LocalIdParsingState::PrimaryItem,
					std::string{ "Invalid GMOD path in primary item: '" } + std::string{ pathStr } + "'" );
				// Include parse errors if available
				if ( parseErrors.hasErrors() )
				{
					for ( const auto& [type, message] : parseErrors )
					{
						errorBuilder.addError( LocalIdParsingState::PrimaryItem, message );
					}
				}
			}

			pos = pathEnd;
			if ( pos < str.size() && str[pos] == '/' )
			{
				skipSlash( pos );
			}

			return path;
		}

		std::optional<GmodPath> parseSecondaryItem(
			std::string_view str,
			size_t& pos,
			const Gmod& gmod,
			const Locations& locations,
			LocalIdParsingErrorBuilder& errorBuilder )
		{
			// Expect "sec" keyword
			using namespace nfx::string;

			auto keyword = extractSegment( str, pos );
			if ( !equals( keyword, "sec" ) )
			{
				errorBuilder.addError( LocalIdParsingState::SecondaryItem,
					std::string{ "Expected 'sec' keyword, got '" } + std::string{ keyword } + "'" );
				return std::nullopt;
			}

			skipSlash( pos );

			// Find end of secondary item (/meta or /~ for verbose mode)
			size_t start = pos;
			size_t pathEnd = pos;

			while ( pathEnd < str.size() )
			{
				size_t nextSlash = findNext( str, pathEnd, '/' );
				if ( nextSlash == str.size() )
				{
					pathEnd = nextSlash;
					break;
				}

				// Check if we hit /meta or /~ (verbose mode)
				size_t peekStart = nextSlash + 1;
				if ( peekStart < str.size() )
				{
					auto remaining = str.substr( peekStart );

					if ( startsWith( remaining, "meta" ) )
					{
						pathEnd = nextSlash;
						break;
					}
					if ( str[peekStart] == '~' )
					{
						pathEnd = nextSlash;
						break;
					}
				}

				pathEnd = nextSlash + 1;
			}
			if ( pathEnd == start )
			{
				errorBuilder.addError( LocalIdParsingState::SecondaryItem, "Missing secondary item after /sec/" );
				return std::nullopt;
			}

			auto pathStr = str.substr( start, pathEnd - start );
			ParsingErrors parseErrors;
			auto path = GmodPath::fromString( pathStr, gmod, locations, parseErrors );

			if ( !path.has_value() )
			{
				errorBuilder.addError( LocalIdParsingState::SecondaryItem,
					std::string{ "Invalid GMOD path in secondary item: '" } + std::string{ pathStr } + "'" );
			}

			pos = pathEnd;
			if ( pos < str.size() && str[pos] == '/' )
			{
				skipSlash( pos );
			}

			return path;
		}

		std::optional<MetadataTag> parseMetadataTag(
			std::string_view str,
			size_t& pos,
			const Codebooks& codebooks,
			LocalIdParsingState expectedState,
			LocalIdParsingErrorBuilder& errorBuilder )
		{
			using namespace nfx::string;

			auto segment = extractSegment( str, pos );

			if ( isEmpty( segment ) )
			{
				return std::nullopt;
			}

			// Find separator (- or ~)
			size_t sepPos = segment.find( '-' );
			bool isCustom = false;

			if ( sepPos == std::string_view::npos )
			{
				sepPos = segment.find( '~' );
				isCustom = true;
			}

			if ( sepPos == std::string_view::npos )
			{
				errorBuilder.addError( expectedState,
					std::string{ "Invalid metadata tag format: '" } + std::string{ segment } + "'" );
				skipSlash( pos );
				return std::nullopt;
			}

			auto prefix = segment.substr( 0, sepPos );
			auto value = segment.substr( sepPos + 1 );

			// Get codebook name from prefix
			CodebookName name;
			try
			{
				name = CodebookNames::fromPrefix( prefix );
			}
			catch ( ... )
			{
				errorBuilder.addError( expectedState,
					std::string{ "Unknown metadata tag prefix: '" } + std::string{ prefix } + "'" );
				skipSlash( pos );
				return std::nullopt;
			}

			// Create tag using createTag() which handles isCustom automatically
			auto tag = codebooks[name].createTag( value );

			if ( !tag.has_value() )
			{
				errorBuilder.addError( expectedState,
					std::string{ "Invalid metadata tag value for " } + std::string{ prefix } + ": '" + std::string{ value } + "'" );
			}

			skipSlash( pos );
			return tag;
		}

		std::optional<LocalId> parseInternal(
			std::string_view localIdStr,
			LocalIdParsingErrorBuilder& errorBuilder )
		{
			using namespace nfx::string;

			if ( isEmpty( localIdStr ) )
			{
				errorBuilder.addError( LocalIdParsingState::EmptyState, "LocalId string is empty" );
				return std::nullopt;
			}

			if ( localIdStr[0] != '/' )
			{
				errorBuilder.addError( LocalIdParsingState::Formatting, "LocalId must start with '/'" );
				return std::nullopt;
			}

			size_t pos = 1; // Skip initial '/'

			// Parse naming rule
			if ( !parseNamingRule( localIdStr, pos, errorBuilder ) )
			{
				return std::nullopt;
			}

			// Parse VIS version
			auto version = parseVisVersion( localIdStr, pos, errorBuilder );
			if ( !version.has_value() )
			{
				return std::nullopt;
			}

			// Get Gmod and Codebooks for this version
			const auto& vis = VIS::instance();
			const auto& gmod = vis.gmod( *version );
			const auto& locations = vis.locations( *version );
			const auto& codebooks = vis.codebooks( *version );

			// Parse primary item
			auto primaryItem = parsePrimaryItem( localIdStr, pos, gmod, locations, errorBuilder );
			if ( !primaryItem.has_value() )
			{
				return std::nullopt;
			}

			// Parse optional secondary item
			std::optional<GmodPath> secondaryItem;
			if ( pos < localIdStr.size() )
			{
				auto peekSegment = extractSegment( localIdStr, pos );

				if ( peekSegment == "sec" )
				{
					pos -= peekSegment.size(); // Rewind for parseSecondaryItem
					secondaryItem = parseSecondaryItem( localIdStr, pos, gmod, locations, errorBuilder );
				}
				else
				{
					pos -= peekSegment.size(); // Rewind
				}
			}

			// Skip verbose mode common names (segments starting with ~)
			bool hasVerboseMode = false;
			while ( pos < localIdStr.size() )
			{
				using namespace nfx::string;

				auto segment = extractSegment( localIdStr, pos );
				if ( isEmpty( segment ) || !segment.starts_with( '~' ) )
				{
					pos -= segment.size(); // Rewind
					break;
				}
				hasVerboseMode = true;
				skipSlash( pos );
			}

			// Parse /meta keyword
			if ( pos >= localIdStr.size() )
			{
				errorBuilder.addError( LocalIdParsingState::Completeness, "Missing '/meta' section" );
				return std::nullopt;
			}

			auto metaKeyword = extractSegment( localIdStr, pos );
			if ( !equals( metaKeyword, "meta" ) )
			{
				errorBuilder.addError( LocalIdParsingState::Completeness,
					std::string{ "Expected '/meta', got '/" } + std::string{ metaKeyword } + "'" );
				return std::nullopt;
			}

			skipSlash( pos );

			// Parse all metadata tags
			std::vector<MetadataTag> tags;
			while ( pos < localIdStr.size() )
			{
				using namespace nfx::string;

				size_t peekPos = pos;
				auto segment = extractSegment( localIdStr, peekPos );

				if ( isEmpty( segment ) )
				{
					break;
				}

				// Determine tag type from prefix
				size_t sepPos = segment.find( '-' );
				if ( sepPos == std::string_view::npos )
				{
					sepPos = segment.find( '~' );
				}

				if ( sepPos == std::string_view::npos )
				{
					errorBuilder.addError( LocalIdParsingState::Formatting,
						std::string{ "Invalid metadata tag: '" } + std::string{ segment } + "'" );
					pos = peekPos;

					if ( pos < localIdStr.size() && localIdStr[pos] == '/' )
					{
						skipSlash( pos );
					}

					continue;
				}

				auto prefix = segment.substr( 0, sepPos );
				LocalIdParsingState state;

				if ( equals( prefix, "qty" ) )
				{
					state = LocalIdParsingState::MetaQuantity;
				}
				else if ( equals( prefix, "cnt" ) )
				{
					state = LocalIdParsingState::MetaContent;
				}
				else if ( equals( prefix, "calc" ) )
				{
					state = LocalIdParsingState::MetaCalculation;
				}
				else if ( equals( prefix, "state" ) )
				{
					state = LocalIdParsingState::MetaState;
				}
				else if ( equals( prefix, "cmd" ) )
				{
					state = LocalIdParsingState::MetaCommand;
				}
				else if ( equals( prefix, "type" ) )
				{
					state = LocalIdParsingState::MetaType;
				}
				else if ( equals( prefix, "pos" ) )
				{
					state = LocalIdParsingState::MetaPosition;
				}
				else if ( equals( prefix, "detail" ) )
				{
					state = LocalIdParsingState::MetaDetail;
				}
				else
				{
					errorBuilder.addError( LocalIdParsingState::Formatting,
						std::string{ "Unknown metadata tag prefix: '" } + std::string{ prefix } + "'" );
					pos = peekPos;
					if ( pos < localIdStr.size() && localIdStr[pos] == '/' )
					{
						skipSlash( pos );
					}
					continue;
				}

				auto tag = parseMetadataTag( localIdStr, pos, codebooks, state, errorBuilder );
				if ( tag.has_value() )
				{
					tags.emplace_back( std::move( *tag ) );
				}
			}

			if ( tags.empty() )
			{
				errorBuilder.addError( LocalIdParsingState::Completeness, "At least one metadata tag is required" );
				return std::nullopt;
			}

			// Build LocalId with all collected data
			try
			{
				// Start with version and primary item
				LocalIdBuilder builder = LocalIdBuilder::create( *version ).withPrimaryItem( *primaryItem );

				// Add optional secondary item
				if ( secondaryItem.has_value() )
				{
					builder = builder.withSecondaryItem( *secondaryItem );
				}

				// Set verbose mode if we encountered ~ segments
				if ( hasVerboseMode )
				{
					builder = builder.withVerboseMode( true );
				}

				// Add all metadata tags
				for ( const auto& tag : tags )
				{
					builder = builder.withMetadataTag( tag );
				}
				return builder.build();
			}
			catch ( const std::exception& e )
			{
				errorBuilder.addError( LocalIdParsingState::Completeness, e.what() );
				return std::nullopt;
			}
		}
	} // namespace

	LocalIdBuilder LocalIdBuilder::create( VisVersion visVersion ) noexcept
	{
		LocalIdBuilder builder;
		builder.m_visVersion = visVersion;
		return builder;
	}

	LocalIdBuilder LocalIdBuilder::withVisVersion( VisVersion version ) const noexcept
	{
		LocalIdBuilder builder{ *this };
		builder.m_visVersion = version;
		return builder;
	}

	LocalIdBuilder LocalIdBuilder::withVisVersion( std::string_view visVersionStr ) const
	{
		VisVersion version;
		if ( !VisVersions::fromString( visVersionStr, version ) )

			throw std::invalid_argument{ "Invalid VisVersion string" };

		return withVisVersion( version );
	}

	LocalIdBuilder LocalIdBuilder::withoutVisVersion() const noexcept
	{
		LocalIdBuilder builder{ *this };
		builder.m_visVersion.reset();
		return builder;
	}

	LocalIdBuilder LocalIdBuilder::withPrimaryItem( const GmodPath& path ) const
	{
		LocalIdBuilder builder{ *this };
		builder.m_primaryItem = path;
		return builder;
	}

	LocalIdBuilder LocalIdBuilder::withPrimaryItem( const std::optional<GmodPath>& path ) const
	{
		if ( !path.has_value() )
		{
			throw std::invalid_argument{ "Primary item cannot be null" };
		}
		return withPrimaryItem( *path );
	}

	LocalIdBuilder LocalIdBuilder::withoutPrimaryItem() const noexcept
	{
		LocalIdBuilder builder{ *this };
		builder.m_primaryItem.reset();
		return builder;
	}

	LocalIdBuilder LocalIdBuilder::withSecondaryItem( const GmodPath& path ) const
	{
		LocalIdBuilder builder{ *this };
		builder.m_secondaryItem = path;
		return builder;
	}

	LocalIdBuilder LocalIdBuilder::withSecondaryItem( const std::optional<GmodPath>& path ) const
	{
		if ( !path.has_value() )
		{
			return withoutSecondaryItem();
		}
		return withSecondaryItem( *path );
	}

	LocalIdBuilder LocalIdBuilder::withoutSecondaryItem() const noexcept
	{
		LocalIdBuilder builder{ *this };
		builder.m_secondaryItem.reset();
		return builder;
	}

	LocalIdBuilder LocalIdBuilder::withMetadataTag( const MetadataTag& tag ) const
	{
		LocalIdBuilder builder{ *this };

		switch ( tag.name() )
		{
			case CodebookName::Quantity:
				builder.m_quantity.emplace( tag );
				break;
			case CodebookName::Content:
				builder.m_content.emplace( tag );
				break;
			case CodebookName::Calculation:
				builder.m_calculation.emplace( tag );
				break;
			case CodebookName::State:
				builder.m_state.emplace( tag );
				break;
			case CodebookName::Command:
				builder.m_command.emplace( tag );
				break;
			case CodebookName::Type:
				builder.m_type.emplace( tag );
				break;
			case CodebookName::Position:
				builder.m_position.emplace( tag );
				break;
			case CodebookName::Detail:
				builder.m_detail.emplace( tag );
				break;
			default:
				throw std::invalid_argument{ "Invalid metadata tag codebook name for LocalId" };
		}

		return builder;
	}

	LocalIdBuilder LocalIdBuilder::withMetadataTag( const std::optional<MetadataTag>& tag ) const
	{
		if ( !tag.has_value() )
		{
			return *this;
		}
		return withMetadataTag( *tag );
	}

	LocalIdBuilder LocalIdBuilder::withoutMetadataTag( CodebookName name ) const noexcept
	{
		LocalIdBuilder builder{ *this };

		switch ( name )
		{
			case CodebookName::Quantity:
				builder.m_quantity.reset();
				break;
			case CodebookName::Content:
				builder.m_content.reset();
				break;
			case CodebookName::Calculation:
				builder.m_calculation.reset();
				break;
			case CodebookName::State:
				builder.m_state.reset();
				break;
			case CodebookName::Command:
				builder.m_command.reset();
				break;
			case CodebookName::Type:
				builder.m_type.reset();
				break;
			case CodebookName::Position:
				builder.m_position.reset();
				break;
			case CodebookName::Detail:
				builder.m_detail.reset();
				break;
			default:
				break;
		}

		return builder;
	}

	LocalIdBuilder LocalIdBuilder::withVerboseMode( bool verbose ) const noexcept
	{
		LocalIdBuilder builder{ *this };
		builder.m_verboseMode = verbose;
		return builder;
	}

	LocalId LocalIdBuilder::build() const
	{
		if ( !isValid() )
		{
			throw std::invalid_argument{
				"Cannot build LocalId: VisVersion, primary item, and at least one metadata tag are required" };
		}

		return LocalId{ *this };
	}

	std::string LocalIdBuilder::toString() const
	{
		StringBuilder sb;
		toString( sb );

		return sb.toString();
	}

	void LocalIdBuilder::toString( StringBuilder& builder ) const
	{
		// Format: /dnv-v2/vis-{version}/{primary}[/sec/{secondary}]/meta[/qty-{val}]...

		// Naming rule
		builder.append( "/" );
		builder.append( NamingRule );

		// VIS version
		if ( m_visVersion.has_value() )
		{
			builder.append( "/vis-" );
			builder.append( VisVersions::toString( *m_visVersion ) );
		}

		// Primary item
		if ( m_primaryItem.has_value() )
		{
			builder.append( "/" );
			builder.append( m_primaryItem->toString() );
			builder.append( "/" );
		}

		// Secondary item
		if ( m_secondaryItem.has_value() )
		{
			builder.append( "sec/" );
			builder.append( m_secondaryItem->toString() );
			builder.append( "/" );
		}

		// Verbose mode: Add common names
		if ( m_verboseMode )
		{
			if ( m_primaryItem.has_value() )
			{
				auto commonNamesVec = m_primaryItem->commonNames();
				for ( const auto& [depth, name] : commonNamesVec )
				{
					builder.append( "~" );
					appendNormalizedCommonName( name, builder );

					// Append location if present
					const auto& node = ( *m_primaryItem )[depth];
					if ( node.location().has_value() )
					{
						builder.append( "." );
						builder.append( node.location()->value() );
					}
					builder.append( "/" );
				}
			}

			if ( m_secondaryItem.has_value() )
			{
				auto commonNamesVec = m_secondaryItem->commonNames();
				const char* prefix = "~for.";
				for ( const auto& [depth, name] : commonNamesVec )
				{
					builder.append( prefix );
					if ( std::string_view( prefix ) != "~" )
					{
						prefix = "~";
					}

					appendNormalizedCommonName( name, builder );

					// Append location if present
					const auto& node = ( *m_secondaryItem )[depth];
					if ( node.location().has_value() )
					{
						builder.append( "." );
						builder.append( node.location()->value() );
					}
					builder.append( "/" );
				}
			}
		}

		// Metadata section
		builder.append( "meta/" );
		if ( m_quantity.has_value() )
		{
			m_quantity->toString( builder );
		}
		if ( m_content.has_value() )
		{
			m_content->toString( builder );
		}
		if ( m_calculation.has_value() )
		{
			m_calculation->toString( builder );
		}
		if ( m_state.has_value() )
		{
			m_state->toString( builder );
		}
		if ( m_command.has_value() )
		{
			m_command->toString( builder );
		}
		if ( m_type.has_value() )
		{
			m_type->toString( builder );
		}
		if ( m_position.has_value() )
		{
			m_position->toString( builder );
		}
		if ( m_detail.has_value() )
		{
			m_detail->toString( builder );
		}

		auto str = builder.toString();
		if ( !str.empty() && str.back() == '/' )
		{
			builder.clear();
			builder.append( std::string_view( str.data(), str.size() - 1 ) );
		}
	}

	std::optional<LocalId> LocalIdBuilder::fromString( std::string_view localIdStr ) noexcept
	{
		internal::LocalIdParsingErrorBuilder errorBuilder;
		return parseInternal( localIdStr, errorBuilder );
	}

	std::optional<LocalId> LocalIdBuilder::fromString( std::string_view localIdStr, ParsingErrors& errors ) noexcept
	{
		internal::LocalIdParsingErrorBuilder errorBuilder;
		auto result = parseInternal( localIdStr, errorBuilder );
		errors = errorBuilder.build();
		return result;
	}
} // namespace dnv::vista::sdk
