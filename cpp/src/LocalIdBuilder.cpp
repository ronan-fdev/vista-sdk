/**
 * @file LocalIdBuilder.cpp
 * @brief Implementation of the LocalIdBuilder class
 */

#include "dnv/vista/sdk/LocalIdBuilder.h"

#include "dnv/vista/sdk/constants/LocalIdConstants.h"
#include "internal/LocalIdParsingErrorBuilder.h"
#include "dnv/vista/sdk/internal/StringBuilderPool.h"

#include "dnv/vista/sdk/CodebookName.h"
#include "dnv/vista/sdk/Codebooks.h"
#include "dnv/vista/sdk/Gmod.h"
#include "dnv/vista/sdk/LocalId.h"
#include "dnv/vista/sdk/LocalIdBuilder.h"
#include "dnv/vista/sdk/ParsingErrors.h"
#include "dnv/vista/sdk/VIS.h"

namespace dnv::vista::sdk
{
	namespace
	{
		//=====================================================================
		// Static lookup tables
		//=====================================================================

		inline static const std::unordered_map<std::string_view, internal::LocalIdParsingState> prefixMap{
			{ constants::localId::META_QTY_SHORT, internal::LocalIdParsingState::MetaQuantity },
			{ constants::localId::META_QTY_LONG, internal::LocalIdParsingState::MetaQuantity },
			{ constants::localId::META_CNT_SHORT, internal::LocalIdParsingState::MetaContent },
			{ constants::localId::META_CNT_LONG, internal::LocalIdParsingState::MetaContent },
			{ constants::localId::META_CALC_SHORT, internal::LocalIdParsingState::MetaCalculation },
			{ constants::localId::META_CALC_LONG, internal::LocalIdParsingState::MetaCalculation },
			{ constants::localId::META_STATE_SHORT, internal::LocalIdParsingState::MetaState },
			{ constants::localId::META_STATE_LONG, internal::LocalIdParsingState::MetaState },
			{ constants::localId::META_CMD_SHORT, internal::LocalIdParsingState::MetaCommand },
			{ constants::localId::META_CMD_LONG, internal::LocalIdParsingState::MetaCommand },
			{ constants::localId::META_TYPE_SHORT, internal::LocalIdParsingState::MetaType },
			{ constants::localId::META_TYPE_LONG, internal::LocalIdParsingState::MetaType },
			{ constants::localId::META_POS_SHORT, internal::LocalIdParsingState::MetaPosition },
			{ constants::localId::META_POS_LONG, internal::LocalIdParsingState::MetaPosition },
			{ constants::localId::META_DETAIL_SHORT, internal::LocalIdParsingState::MetaDetail },
			{ constants::localId::META_DETAIL_LONG, internal::LocalIdParsingState::MetaDetail } };

		//=====================================================================
		// Static helper functions
		//=====================================================================

		std::pair<size_t, size_t> nextStateIndexes( std::string_view span, internal::LocalIdParsingState state )
		{
			size_t customIndex = span.find( constants::localId::PREFIX_TILDE );
			size_t endOfCustomIndex = ( customIndex != std::string_view::npos ) ? ( customIndex + constants::localId::PREFIX_TILDE.length() + 1 )
																				: std::string_view::npos;

			size_t metaIndex = span.find( constants::localId::PATH_META );
			size_t endOfMetaIndex = ( metaIndex != std::string_view::npos ) ? ( metaIndex + constants::localId::PATH_META.length() + 1 )
																			: std::string_view::npos;

			bool isVerbose = ( customIndex != std::string_view::npos ) &&
							 ( metaIndex != std::string_view::npos ) &&
							 ( customIndex < metaIndex );

			switch ( state )
			{
				case internal::LocalIdParsingState::PrimaryItem:
				{
					size_t secIndex = span.find( constants::localId::PATH_SEC );
					size_t endOfSecIndex =
						( secIndex != std::string_view::npos ) ? ( secIndex + constants::localId::PATH_SEC.length() + 1 )
															   : std::string_view::npos;

					if ( secIndex != std::string_view::npos )
					{
						return { secIndex, endOfSecIndex };
					}

					if ( isVerbose && customIndex != std::string_view::npos )
					{
						return { customIndex, endOfCustomIndex };
					}

					return { metaIndex, endOfMetaIndex };
				}
				case internal::LocalIdParsingState::SecondaryItem:
				{
					if ( isVerbose && customIndex != std::string_view::npos )
					{
						return { customIndex, endOfCustomIndex };
					}

					return { metaIndex, endOfMetaIndex };
				}
				case internal::LocalIdParsingState::NamingRule:
				case internal::LocalIdParsingState::VisVersion:
				case internal::LocalIdParsingState::ItemDescription:
				case internal::LocalIdParsingState::MetaQuantity:
				case internal::LocalIdParsingState::MetaContent:
				case internal::LocalIdParsingState::MetaCalculation:
				case internal::LocalIdParsingState::MetaState:
				case internal::LocalIdParsingState::MetaCommand:
				case internal::LocalIdParsingState::MetaType:
				case internal::LocalIdParsingState::MetaPosition:
				case internal::LocalIdParsingState::MetaDetail:
				case internal::LocalIdParsingState::EmptyState:
				case internal::LocalIdParsingState::Formatting:
				case internal::LocalIdParsingState::Completeness:
				case internal::LocalIdParsingState::NamingEntity:
				case internal::LocalIdParsingState::IMONumber:
				default:
				{
					return { metaIndex, endOfMetaIndex };
				}
			}
		}
	}

	//=====================================================================
	// LocalIdBuilder class
	//=====================================================================

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	LocalIdBuilder LocalIdBuilder::create( VisVersion version )
	{
		return LocalIdBuilder().withVisVersion( version );
	}

	//----------------------------------------------
	// Build methods (Immutable fluent interface)
	//----------------------------------------------

	//----------------------------
	// Build
	//----------------------------

	LocalId LocalIdBuilder::build() const
	{
		if ( isEmpty() )
		{
			throw std::invalid_argument( "Cannot build LocalId: builder is empty." );
		}
		if ( !isValid() )
		{
			throw std::invalid_argument( "Cannot build LocalId: builder state is invalid." );
		}

		return LocalId( *this );
	}

	//----------------------------------------------
	// Verbose mode
	//----------------------------------------------

	LocalIdBuilder LocalIdBuilder::withVerboseMode( bool verboseMode )
	{
		LocalIdBuilder result( *this );
		result.m_verboseMode = verboseMode;

		return result;
	}

	//----------------------------------------------
	// VIS version
	//----------------------------------------------

	LocalIdBuilder LocalIdBuilder::withVisVersion( const std::string& visVersionStr )
	{
		bool succeeded;
		auto localIdBuilder = tryWithVisVersion( visVersionStr, succeeded );

		if ( !succeeded )
		{
			throw std::invalid_argument( "Failed to parse VIS version" );
		}

		return localIdBuilder;
	}

	LocalIdBuilder LocalIdBuilder::withVisVersion( VisVersion version )
	{
		bool succeeded;
		auto localIdBuilder = tryWithVisVersion( version, succeeded );

		if ( !succeeded )
		{
			throw std::invalid_argument( "withVisVersion" );
		}

		return localIdBuilder;
	}

	LocalIdBuilder LocalIdBuilder::tryWithVisVersion( const std::optional<VisVersion>& version )
	{
		bool succeeded;

		return tryWithVisVersion( version, succeeded );
	}

	LocalIdBuilder LocalIdBuilder::tryWithVisVersion( const std::optional<VisVersion>& version, bool& succeeded )
	{
		succeeded = true;
		LocalIdBuilder result( *this );
		result.m_visVersion = version;

		return result;
	}

	LocalIdBuilder LocalIdBuilder::tryWithVisVersion( const std::optional<std::string>& visVersionStr, bool& succeeded )
	{
		if ( visVersionStr.has_value() )
		{
			VisVersion v;
			if ( VisVersionExtensions::tryParse( *visVersionStr, v ) )
			{
				auto localIdBuilder = tryWithVisVersion( v, succeeded );

				return localIdBuilder;
			}
		}

		succeeded = false;

		return LocalIdBuilder( *this );
	}

	LocalIdBuilder LocalIdBuilder::withoutVisVersion()
	{
		LocalIdBuilder result( *this );
		result.m_visVersion = std::nullopt;

		return result;
	}

	//----------------------------------------------
	// Primary item
	//----------------------------------------------

	LocalIdBuilder LocalIdBuilder::withPrimaryItem( GmodPath&& item )
	{
		bool succeeded;
		auto localIdBuilder = tryWithPrimaryItem( std::move( item ), succeeded );

		if ( !succeeded )
		{
			throw std::invalid_argument( "Failed to set primary item: invalid or empty GmodPath." );
		}

		return localIdBuilder;
	}

	LocalIdBuilder LocalIdBuilder::tryWithPrimaryItem( GmodPath&& item )
	{
		bool succeeded;

		return tryWithPrimaryItem( std::move( item ), succeeded );
	}

	LocalIdBuilder LocalIdBuilder::tryWithPrimaryItem( GmodPath&& item, bool& succeeded )
	{
		if ( item.length() == 0 )
		{
			succeeded = false;

			return LocalIdBuilder( *this );
		}

		succeeded = true;
		LocalIdBuilder result( *this );
		result.m_items = LocalIdItems( std::move( result.m_items ), std::move( item ) );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::tryWithPrimaryItem( std::optional<GmodPath>&& item )
	{
		bool succeeded;

		return tryWithPrimaryItem( std::move( item ), succeeded );
	}

	LocalIdBuilder LocalIdBuilder::tryWithPrimaryItem( std::optional<GmodPath>&& item, bool& succeeded )
	{
		if ( !item.has_value() )
		{
			succeeded = false;

			return LocalIdBuilder( *this );
		}

		succeeded = true;
		LocalIdBuilder result( *this );
		result.m_items = LocalIdItems( std::move( result.m_items ), std::move( *item ) );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutPrimaryItem()
	{
		LocalIdBuilder result( *this );
		result.m_items = LocalIdItems{};

		return result;
	}

	//----------------------------------------------
	// Secondary item
	//----------------------------------------------

	LocalIdBuilder LocalIdBuilder::withSecondaryItem( GmodPath&& item )
	{
		bool succeeded;
		auto localIdBuilder = tryWithSecondaryItem( std::move( item ), succeeded );

		if ( !succeeded )
		{
			throw std::invalid_argument( "Failed to set secondary item: invalid or empty GmodPath." );
		}

		return localIdBuilder;
	}

	LocalIdBuilder LocalIdBuilder::tryWithSecondaryItem( GmodPath&& item )
	{
		bool succeeded;

		return tryWithSecondaryItem( std::move( item ), succeeded );
	}

	LocalIdBuilder LocalIdBuilder::tryWithSecondaryItem( GmodPath&& item, bool& succeeded )
	{
		if ( item.length() == 0 )
		{
			succeeded = false;

			return LocalIdBuilder( *this );
		}

		succeeded = true;
		LocalIdBuilder result( *this );
		result.m_items = LocalIdItems( std::move( result.m_items ), std::make_optional( std::move( item ) ) );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::tryWithSecondaryItem( std::optional<GmodPath>&& item )
	{
		bool succeeded;

		return tryWithSecondaryItem( std::move( item ), succeeded );
	}

	LocalIdBuilder LocalIdBuilder::tryWithSecondaryItem( std::optional<GmodPath>&& item, bool& succeeded )
	{
		if ( !item.has_value() )
		{
			succeeded = false;

			return LocalIdBuilder( *this );
		}

		succeeded = true;
		LocalIdBuilder result( *this );
		result.m_items = LocalIdItems( std::move( result.m_items ), std::move( item ) );

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutSecondaryItem()
	{
		LocalIdBuilder result( *this );

		result.m_items = LocalIdItems( std::move( result.m_items ), std::nullopt );

		return result;
	}

	//----------------------------------------------
	// Metadata tags
	//----------------------------------------------

	LocalIdBuilder LocalIdBuilder::withMetadataTag( const MetadataTag& metadataTag )
	{
		bool succeeded;
		auto localIdBuilder = tryWithMetadataTag( metadataTag, succeeded );

		if ( !succeeded )
		{
			auto lease = internal::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "Invalid metadata codebook name: " );
			builder.append( CodebookNames::toPrefix( metadataTag.name() ) );
			throw std::invalid_argument( lease.toString() );
		}

		return localIdBuilder;
	}

	LocalIdBuilder LocalIdBuilder::tryWithMetadataTag( const std::optional<MetadataTag>& metadataTag )
	{
		bool succeeded;

		return tryWithMetadataTag( metadataTag, succeeded );
	}

	LocalIdBuilder LocalIdBuilder::tryWithMetadataTag( const std::optional<MetadataTag>& metadataTag, bool& succeeded )
	{
		if ( !metadataTag.has_value() )
		{
			succeeded = false;

			return LocalIdBuilder( *this );
		}

		switch ( metadataTag->name() )
		{
			case CodebookName::Quantity:
			{
				succeeded = true;
				return withQuantity( *metadataTag );
			}
			case CodebookName::Content:
			{
				succeeded = true;
				return withContent( *metadataTag );
			}
			case CodebookName::Calculation:
			{
				succeeded = true;
				return withCalculation( *metadataTag );
			}
			case CodebookName::State:
			{
				succeeded = true;
				return withState( *metadataTag );
			}
			case CodebookName::Command:
			{
				succeeded = true;
				return withCommand( *metadataTag );
			}
			case CodebookName::Type:
			{
				succeeded = true;
				return withType( *metadataTag );
			}
			case CodebookName::Position:
			{
				succeeded = true;
				return withPosition( *metadataTag );
			}
			case CodebookName::Detail:
			{
				succeeded = true;
				return withDetail( *metadataTag );
			}
			case CodebookName::FunctionalServices:
			case CodebookName::MaintenanceCategory:
			case CodebookName::ActivityType:
			default:
			{
				succeeded = false;
				return LocalIdBuilder( *this );
			}
		}
	}

	LocalIdBuilder LocalIdBuilder::withoutMetadataTag( CodebookName name )
	{
		switch ( name )
		{
			case CodebookName::Quantity:
			{
				return withoutQuantity();
			}
			case CodebookName::Content:
			{
				return withoutContent();
			}
			case CodebookName::Calculation:
			{
				return withoutCalculation();
			}
			case CodebookName::State:
			{
				return withoutState();
			}
			case CodebookName::Command:
			{
				return withoutCommand();
			}
			case CodebookName::Type:
			{
				return withoutType();
			}
			case CodebookName::Position:
			{
				return withoutPosition();
			}
			case CodebookName::Detail:
			{
				return withoutDetail();
			}
			case CodebookName::FunctionalServices:
			case CodebookName::MaintenanceCategory:
			case CodebookName::ActivityType:
			default:
			{
				return LocalIdBuilder( *this );
			}
		}
	}

	//----------------------------------------------
	// Specific metadata tag builder methods
	//----------------------------------------------

	//----------------------------
	// Quantity
	//----------------------------

	LocalIdBuilder LocalIdBuilder::withQuantity( const MetadataTag& quantity )
	{
		LocalIdBuilder result( *this );
		result.m_quantity = quantity;

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutQuantity()
	{
		LocalIdBuilder result( *this );
		result.m_quantity = std::nullopt;

		return result;
	}

	//----------------------------
	// Content
	//----------------------------

	LocalIdBuilder LocalIdBuilder::withContent( const MetadataTag& content )
	{
		LocalIdBuilder result( *this );
		result.m_content = content;

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutContent()
	{
		LocalIdBuilder result( *this );
		result.m_content = std::nullopt;

		return result;
	}

	//----------------------------
	// Calculation
	//----------------------------

	LocalIdBuilder LocalIdBuilder::withCalculation( const MetadataTag& calculation )
	{
		LocalIdBuilder result( *this );
		result.m_calculation = calculation;

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutCalculation()
	{
		LocalIdBuilder result( *this );
		result.m_calculation = std::nullopt;

		return result;
	}

	//----------------------------
	// State
	//----------------------------

	LocalIdBuilder LocalIdBuilder::withState( const MetadataTag& state )
	{
		LocalIdBuilder result( *this );
		result.m_state = state;

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutState()
	{
		LocalIdBuilder result( *this );
		result.m_state = std::nullopt;

		return result;
	}

	//----------------------------
	// Command
	//----------------------------

	LocalIdBuilder LocalIdBuilder::withCommand( const MetadataTag& command )
	{
		LocalIdBuilder result( *this );
		result.m_command = command;

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutCommand()
	{
		LocalIdBuilder result( *this );
		result.m_command = std::nullopt;

		return result;
	}

	//----------------------------
	// Type
	//----------------------------

	LocalIdBuilder LocalIdBuilder::withType( const MetadataTag& type )
	{
		LocalIdBuilder result( *this );
		result.m_type = type;

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutType()
	{
		LocalIdBuilder result( *this );
		result.m_type = std::nullopt;

		return result;
	}

	//----------------------------
	// Position
	//----------------------------

	LocalIdBuilder LocalIdBuilder::withPosition( const MetadataTag& position )
	{
		LocalIdBuilder result( *this );
		result.m_position = position;

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutPosition()
	{
		LocalIdBuilder result( *this );
		result.m_position = std::nullopt;

		return result;
	}

	//----------------------------
	// Detail
	//----------------------------

	LocalIdBuilder LocalIdBuilder::withDetail( const MetadataTag& detail )
	{
		LocalIdBuilder result( *this );
		result.m_detail = detail;

		return result;
	}

	LocalIdBuilder LocalIdBuilder::withoutDetail()
	{
		LocalIdBuilder result( *this );
		result.m_detail = std::nullopt;

		return result;
	}

	//----------------------------------------------
	// Static parsing methods
	//----------------------------------------------

	LocalIdBuilder LocalIdBuilder::parse( std::string_view localIdStr )
	{
		std::optional<LocalIdBuilder> localId;
		ParsingErrors errors;

		if ( !tryParse( localIdStr, errors, localId ) )
		{
			auto lease = internal::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "Couldn't parse local ID from: '" );
			builder.append( localIdStr );
			builder.append( "'. " );
			builder.append( errors.toString() );
			throw std::invalid_argument( lease.toString() );
		}

		return std::move( *localId );
	}

	bool LocalIdBuilder::tryParse( std::string_view localIdStr, std::optional<LocalIdBuilder>& localId )
	{
		ParsingErrors dummyErrors;

		return tryParse( localIdStr, dummyErrors, localId );
	}

	bool LocalIdBuilder::tryParse( std::string_view localIdStr, ParsingErrors& errors, std::optional<LocalIdBuilder>& localId )
	{
		localId = std::nullopt;
		auto errorBuilder = internal::LocalIdParsingErrorBuilder::create();
		bool success = tryParseInternal( localIdStr, errorBuilder, localId );

		errors = errorBuilder.build();

		return success;
	}

	//----------------------------------------------
	// Enum stringification utility
	//----------------------------------------------

	std::string LocalIdBuilder::codebookNametoString( CodebookName name )
	{
		/*
			TODO: This function returns the codebook names used for internal operations (plural, lowercase).

			In the C# implementation, error messages use the enum name directly via string interpolation:
			Example: $"Invalid {codebookName} metadata tag: '{value}'. Use prefix '~' for custom values"

			When C# interpolates CodebookName.Content, it automatically calls ToString() on the enum,
			which returns "Content" (singular, capitalized form) - matching the test expectations.

			For C++ error messages, we need a separate function that mimics this C# behavior
			to return "Content", "Quantity", "FunctionalServices", "MaintenanceCategory",etc. (singular, capitalized) instead of
			"contents", "quantities", "functional_services", "maintenance_category", etc. (plural, lowercase from constants).
		*/
		switch ( name )
		{
			case CodebookName::Position:
			{
				return "Position";
			}
			case CodebookName::Quantity:
			{
				return "Quantity";
			}
			case CodebookName::Calculation:
			{
				return "Calculation";
			}
			case CodebookName::State:
			{
				return "State";
			}
			case CodebookName::Content:
			{
				return "Content";
			}
			case CodebookName::Command:
			{
				return "Command";
			}
			case CodebookName::Type:
			{
				return "Type";
			}
			case CodebookName::FunctionalServices:
			{
				return "FunctionalServices";
			}
			case CodebookName::MaintenanceCategory:
			{
				return "MaintenanceCategory";
			}
			case CodebookName::ActivityType:
			{
				return "ActivityType";
			}
			case CodebookName::Detail:
			{
				return "Detail";
			}
			default:
			{
				throw std::invalid_argument( "Unknown codebook: " + std::to_string( static_cast<int>( name ) ) );
			}
		}
	}

	//----------------------------------------------
	// Private static helper parsing methods
	//----------------------------------------------

	bool LocalIdBuilder::tryParseInternal(
		std::string_view localIdStr, internal::LocalIdParsingErrorBuilder& errorBuilder, std::optional<LocalIdBuilder>& localIdBuilder )
	{
		localIdBuilder = std::nullopt;

		if ( localIdStr.empty() )
		{
			return false;
		}

		if ( localIdStr[0] != '/' )
		{
			errorBuilder.addError( internal::LocalIdParsingState::Formatting, "Invalid format: missing '/' as first character" );

			return false;
		}

		std::string_view span = localIdStr;

		std::optional<GmodPath> primaryItem = std::nullopt;
		std::optional<GmodPath> secondaryItem = std::nullopt;
		std::optional<MetadataTag> qty = std::nullopt;
		std::optional<MetadataTag> cnt = std::nullopt;
		std::optional<MetadataTag> calc = std::nullopt;
		std::optional<MetadataTag> stateTag = std::nullopt;
		std::optional<MetadataTag> cmd = std::nullopt;
		std::optional<MetadataTag> type = std::nullopt;
		std::optional<MetadataTag> pos = std::nullopt;
		std::optional<MetadataTag> detail = std::nullopt;
		bool verbose = false;
		std::string predefinedMessage;
		bool invalidSecondaryItem = false;

		size_t primaryItemStart = std::numeric_limits<size_t>::max();
		size_t secondaryItemStart = std::numeric_limits<size_t>::max();

		auto state = internal::LocalIdParsingState::NamingRule;
		size_t i = 1;

		VIS& vis = VIS::instance();
		auto visVersion = VisVersion::Unknown;
		const Gmod* gmod = nullptr;
		const Codebooks* codebooks = nullptr;

		while ( state <= internal::LocalIdParsingState::MetaDetail )
		{
			size_t nextStart = std::min( span.length(), i );
			size_t nextSlashPos = span.substr( nextStart ).find( '/' );
			std::string_view segment = ( nextSlashPos == std::string_view::npos ) ? span.substr( nextStart )
																				  : span.substr( nextStart, nextSlashPos );

			switch ( state )
			{
				case internal::LocalIdParsingState::NamingRule:
				{
					if ( segment.empty() )
					{
						errorBuilder.addError( internal::LocalIdParsingState::NamingRule, predefinedMessage );
						state = static_cast<internal::LocalIdParsingState>( static_cast<int>( state ) + 1 );
						break;
					}

					if ( segment != constants::iso19848::ANNEX_C_NAMING_RULE )
					{
						errorBuilder.addError( internal::LocalIdParsingState::NamingRule, predefinedMessage );

						return false;
					}

					advanceParser( i, segment, state );
					break;
				}
				case internal::LocalIdParsingState::VisVersion:
				{
					if ( segment.empty() )
					{
						errorBuilder.addError( internal::LocalIdParsingState::VisVersion, predefinedMessage );
						state = static_cast<internal::LocalIdParsingState>( static_cast<int>( state ) + 1 );
						break;
					}

					if ( !segment.starts_with( constants::localId::PREFIX_VIS ) )
					{
						errorBuilder.addError( internal::LocalIdParsingState::VisVersion, predefinedMessage );

						return false;
					}

					std::string_view versionStr = segment.substr( constants::localId::PREFIX_VIS.length() );
					if ( !VisVersionExtensions::tryParse( versionStr, visVersion ) )
					{
						errorBuilder.addError( internal::LocalIdParsingState::VisVersion, predefinedMessage );

						return false;
					}

					gmod = &vis.gmod( visVersion );
					codebooks = &vis.codebooks( visVersion );

					if ( !gmod || !codebooks )
					{
						return false;
					}

					advanceParser( i, segment, state );
					break;
				}
				case internal::LocalIdParsingState::PrimaryItem:
				{
					if ( segment.empty() )
					{
						if ( primaryItemStart != std::numeric_limits<size_t>::max() )
						{
							if ( !gmod )
							{
								return false;
							}

							std::string_view path = span.substr( primaryItemStart, i - 1 - primaryItemStart );
							std::optional<GmodPath> parsedPath;
							if ( !gmod->tryParsePath( path, parsedPath ) )
							{
								parsedPath = std::nullopt;
							}
							if ( !parsedPath )
							{
								auto lease = internal::StringBuilderPool::lease();
								auto builder = lease.builder();
								builder.append( "Invalid GmodPath in Primary item: " );
								builder.append( path );
								errorBuilder.addError( internal::LocalIdParsingState::PrimaryItem, lease.toString() );
							}
							else
							{
								primaryItem = std::move( *parsedPath );
							}
						}
						else
						{
							errorBuilder.addError( internal::LocalIdParsingState::PrimaryItem );
						}

						errorBuilder.addError(
							internal::LocalIdParsingState::PrimaryItem,
							"Invalid or missing '/meta' prefix after Primary item" );

						state = static_cast<internal::LocalIdParsingState>( static_cast<int>( state ) + 1 );
						break;
					}

					size_t dashIndex = segment.find( constants::localId::PREFIX_DASH );
					std::string_view code = ( dashIndex == std::string_view::npos )
												? segment
												: segment.substr( 0, dashIndex );

					if ( !gmod )
					{
						return false;
					}

					if ( primaryItemStart == std::numeric_limits<size_t>::max() )
					{
						const GmodNode* nodePtr = nullptr;
						if ( !gmod->tryGetNode( code, nodePtr ) )
						{
							auto lease = internal::StringBuilderPool::lease();
							auto builder = lease.builder();
							builder.append( "Invalid start GmodNode in Primary item: " );
							builder.append( code );
							errorBuilder.addError( internal::LocalIdParsingState::PrimaryItem, lease.toString() );
						}

						primaryItemStart = i;
						advanceParser( i, segment );
					}
					else
					{
						internal::LocalIdParsingState nextState = state;

						if ( segment.starts_with( constants::localId::PREFIX_SEC ) )
						{
							nextState = internal::LocalIdParsingState::SecondaryItem;
						}
						else if ( segment.starts_with( constants::localId::PREFIX_META ) )
						{
							nextState = internal::LocalIdParsingState::MetaQuantity;
						}
						else if ( !segment.empty() && segment[0] == constants::localId::CHAR_TILDE )
						{
							nextState = internal::LocalIdParsingState::ItemDescription;
						}

						if ( nextState != state )
						{
							std::string_view path = span.substr( primaryItemStart, i - 1 - primaryItemStart );
							std::optional<GmodPath> parsedPath;
							if ( !gmod->tryParsePath( path, parsedPath ) )
							{
								parsedPath = std::nullopt;
							}
							if ( !parsedPath )
							{
								auto lease = internal::StringBuilderPool::lease();
								auto builder = lease.builder();
								builder.append( "Invalid GmodPath in Primary item: " );
								builder.append( path );
								errorBuilder.addError( internal::LocalIdParsingState::PrimaryItem, lease.toString() );

								auto [_, endOfNextStateIndex] = nextStateIndexes( span, state );
								i = endOfNextStateIndex;
								advanceParser( state, nextState );
								break;
							}
							else
							{
								primaryItem = std::move( *parsedPath );
							}

							if ( !segment.empty() && segment[0] == constants::localId::CHAR_TILDE )
							{
								advanceParser( state, nextState );
							}
							else
							{
								advanceParser( i, segment, state, nextState );
							}
							break;
						}

						const GmodNode* nodePtr = nullptr;
						if ( !gmod->tryGetNode( code, nodePtr ) )
						{
							auto lease = internal::StringBuilderPool::lease();
							auto builder = lease.builder();
							builder.append( "Invalid GmodNode in Primary item: " );
							builder.append( code );
							errorBuilder.addError( internal::LocalIdParsingState::PrimaryItem, lease.toString() );

							auto [nextStateIndex, endOfNextStateIndex] = nextStateIndexes( span, state );

							if ( nextStateIndex == std::numeric_limits<size_t>::max() )
							{
								errorBuilder.addError(
									internal::LocalIdParsingState::PrimaryItem,
									"Invalid or missing '/meta' prefix after Primary item" );

								return false;
							}

							std::string_view nextSegment = span.substr( nextStateIndex + 1 );

							if ( nextSegment.starts_with( constants::localId::PREFIX_SEC ) )
							{
								nextState = internal::LocalIdParsingState::SecondaryItem;
							}
							else if ( nextSegment.starts_with( constants::localId::PREFIX_META ) )
							{
								nextState = internal::LocalIdParsingState::MetaQuantity;
							}
							else if ( !nextSegment.empty() && nextSegment[0] == constants::localId::CHAR_TILDE )
							{
								nextState = internal::LocalIdParsingState::ItemDescription;
							}

							std::string_view invalidPrimaryItemPath = span.substr( i, nextStateIndex - i );
							auto lease2 = internal::StringBuilderPool::lease();
							auto builder2 = lease2.builder();
							builder2.append( "Invalid GmodPath: Last part in Primary item: " );
							builder2.append( invalidPrimaryItemPath );
							errorBuilder.addError( internal::LocalIdParsingState::PrimaryItem, lease2.toString() );

							i = endOfNextStateIndex;
							advanceParser( state, nextState );
							break;
						}

						advanceParser( i, segment );
					}
					break;
				}
				case internal::LocalIdParsingState::SecondaryItem:
				{
					if ( segment.empty() )
					{
						state = static_cast<internal::LocalIdParsingState>( static_cast<int>( state ) + 1 );
						break;
					}

					size_t dashIndex = segment.find( constants::localId::PREFIX_DASH );
					std::string_view code = ( dashIndex == std::string_view::npos ) ? segment
																					: segment.substr( 0, dashIndex );

					if ( !gmod )
					{
						return false;
					}

					if ( secondaryItemStart == std::numeric_limits<size_t>::max() )
					{
						const GmodNode* nodePtr = nullptr;
						if ( !gmod->tryGetNode( code, nodePtr ) )
						{
							auto lease = internal::StringBuilderPool::lease();
							auto builder = lease.builder();
							builder.append( "Invalid start GmodNode in Secondary item: " );
							builder.append( code );
							errorBuilder.addError( internal::LocalIdParsingState::SecondaryItem, lease.toString() );
						}

						secondaryItemStart = i;
						advanceParser( i, segment );
					}
					else
					{
						internal::LocalIdParsingState nextState = state;

						if ( segment.starts_with( constants::localId::PREFIX_META ) )
						{
							nextState = internal::LocalIdParsingState::MetaQuantity;
						}
						else if ( !segment.empty() && segment[0] == constants::localId::CHAR_TILDE )
						{
							nextState = internal::LocalIdParsingState::ItemDescription;
						}

						if ( nextState != state )
						{
							std::string_view path = span.substr( secondaryItemStart, i - 1 - secondaryItemStart );
							std::optional<GmodPath> parsedPath;
							if ( !gmod->tryParsePath( path, parsedPath ) )
							{
								parsedPath = std::nullopt;
							}
							if ( !parsedPath )
							{
								invalidSecondaryItem = true;
								auto lease = internal::StringBuilderPool::lease();
								auto builder = lease.builder();
								builder.append( "Invalid GmodPath in Secondary item: " );
								builder.append( path );
								errorBuilder.addError( internal::LocalIdParsingState::SecondaryItem, lease.toString() );

								auto [_, endOfNextStateIndex] = nextStateIndexes( span, state );
								i = endOfNextStateIndex;
								advanceParser( state, nextState );
								break;
							}
							else
							{
								secondaryItem = std::move( *parsedPath );
							}

							if ( !segment.empty() && segment[0] == constants::localId::CHAR_TILDE )
							{
								advanceParser( state, nextState );
							}
							else
							{
								advanceParser( i, segment, state, nextState );
							}

							break;
						}

						const GmodNode* nodePtr = nullptr;
						if ( !gmod->tryGetNode( code, nodePtr ) )
						{
							invalidSecondaryItem = true;
							auto lease = internal::StringBuilderPool::lease();
							auto builder = lease.builder();
							builder.append( "Invalid GmodNode in Secondary item: " );
							builder.append( code );
							errorBuilder.addError( internal::LocalIdParsingState::SecondaryItem, lease.toString() );

							auto [nextStateIndex, endOfNextStateIndex] = nextStateIndexes( span, state );
							if ( nextStateIndex == std::numeric_limits<size_t>::max() )
							{
								errorBuilder.addError( internal::LocalIdParsingState::SecondaryItem, "Invalid or missing '/meta' prefix after Secondary item" );

								return false;
							}

							std::string_view nextSegment = span.substr( nextStateIndex + 1 );

							if ( nextSegment.starts_with( constants::localId::PREFIX_META ) )
							{
								nextState = internal::LocalIdParsingState::MetaQuantity;
							}
							else if ( !nextSegment.empty() && nextSegment[0] == constants::localId::CHAR_TILDE )
							{
								nextState = internal::LocalIdParsingState::ItemDescription;
							}

							std::string_view invalidSecondaryItemPath = span.substr( i, nextStateIndex - i );
							auto lease2 = internal::StringBuilderPool::lease();
							auto builder2 = lease2.builder();
							builder2.append( "Invalid GmodPath: Last part in Secondary item: " );
							builder2.append( invalidSecondaryItemPath );
							errorBuilder.addError( internal::LocalIdParsingState::SecondaryItem, lease2.toString() );

							i = endOfNextStateIndex;
							advanceParser( state, nextState );
							break;
						}

						advanceParser( i, segment );
					}
					break;
				}
				case internal::LocalIdParsingState::ItemDescription:
				{
					if ( segment.empty() )
					{
						state = static_cast<internal::LocalIdParsingState>( static_cast<int>( state ) + 1 );

						break;
					}

					verbose = true;

					size_t metaIndex = span.find( constants::localId::PATH_META );
					if ( metaIndex == std::string_view::npos )
					{
						errorBuilder.addError( internal::LocalIdParsingState::ItemDescription, predefinedMessage );

						return false;
					}

					segment = span.substr( i, ( metaIndex + constants::localId::PATH_META.length() ) - i );

					advanceParser( i, segment, state );
					break;
				}
				case internal::LocalIdParsingState::MetaQuantity:
				{
					if ( segment.empty() )
					{
						state = static_cast<internal::LocalIdParsingState>( static_cast<int>( state ) + 1 );

						break;
					}

					bool result = parseMetaTag( CodebookName::Quantity, state, i, segment, qty, codebooks, errorBuilder );
					if ( !result )
					{
						return false;
					}

					break;
				}
				case internal::LocalIdParsingState::MetaContent:
				{
					if ( segment.empty() )
					{
						state = static_cast<internal::LocalIdParsingState>( static_cast<int>( state ) + 1 );

						break;
					}

					bool result = parseMetaTag( CodebookName::Content, state, i, segment, cnt, codebooks, errorBuilder );
					if ( !result )
					{
						return false;
					}

					break;
				}
				case internal::LocalIdParsingState::MetaCalculation:
				{
					if ( segment.empty() )
					{
						state = static_cast<internal::LocalIdParsingState>( static_cast<int>( state ) + 1 );

						break;
					}

					bool result = parseMetaTag( CodebookName::Calculation, state, i, segment, calc, codebooks, errorBuilder );
					if ( !result )
					{
						return false;
					}

					break;
				}
				case internal::LocalIdParsingState::MetaState:
				{
					if ( segment.empty() )
					{
						state = static_cast<internal::LocalIdParsingState>( static_cast<int>( state ) + 1 );

						break;
					}

					bool result = parseMetaTag( CodebookName::State, state, i, segment, stateTag, codebooks, errorBuilder );
					if ( !result )
					{
						return false;
					}

					break;
				}
				case internal::LocalIdParsingState::MetaCommand:
				{
					if ( segment.empty() )
					{
						state = static_cast<internal::LocalIdParsingState>( static_cast<int>( state ) + 1 );
						break;
					}

					bool result = parseMetaTag( CodebookName::Command, state, i, segment, cmd, codebooks, errorBuilder );
					if ( !result )
					{
						return false;
					}

					break;
				}
				case internal::LocalIdParsingState::MetaType:
				{
					if ( segment.empty() )
					{
						state = static_cast<internal::LocalIdParsingState>( static_cast<int>( state ) + 1 );

						break;
					}

					bool result = parseMetaTag( CodebookName::Type, state, i, segment, type, codebooks, errorBuilder );
					if ( !result )
					{
						return false;
					}

					break;
				}
				case internal::LocalIdParsingState::MetaPosition:
				{
					if ( segment.empty() )
					{
						state = static_cast<internal::LocalIdParsingState>( static_cast<int>( state ) + 1 );
						break;
					}

					bool result = parseMetaTag( CodebookName::Position, state, i, segment, pos, codebooks, errorBuilder );
					if ( !result )
					{
						return false;
					}
					break;
				}
				case internal::LocalIdParsingState::MetaDetail:
				{
					if ( segment.empty() )
					{
						state = static_cast<internal::LocalIdParsingState>( static_cast<int>( state ) + 1 );
						break;
					}

					bool result = parseMetaTag( CodebookName::Detail, state, i, segment, detail, codebooks, errorBuilder );
					if ( !result )
					{
						return false;
					}
					break;
				}
				case internal::LocalIdParsingState::EmptyState:
				case internal::LocalIdParsingState::Formatting:
				case internal::LocalIdParsingState::Completeness:
				case internal::LocalIdParsingState::NamingEntity:
				case internal::LocalIdParsingState::IMONumber:
				default:
				{
					advanceParser( i, segment, state );
					break;
				}
			}
		}

		auto builder = LocalIdBuilder::create( visVersion );

		if ( primaryItem.has_value() )
		{
			builder = builder.tryWithPrimaryItem( primaryItem.value() );
		}
		if ( secondaryItem.has_value() )
		{
			builder = builder.tryWithSecondaryItem( secondaryItem.value() );
		}
		if ( verbose )
		{
			builder = builder.withVerboseMode( verbose );
		}
		if ( qty.has_value() )
		{
			builder = builder.tryWithMetadataTag( qty.value() );
		}
		if ( cnt.has_value() )
		{
			builder = builder.tryWithMetadataTag( cnt.value() );
		}
		if ( calc.has_value() )
		{
			builder = builder.tryWithMetadataTag( calc.value() );
		}
		if ( stateTag.has_value() )
		{
			builder = builder.tryWithMetadataTag( stateTag.value() );
		}
		if ( cmd.has_value() )
		{
			builder = builder.tryWithMetadataTag( cmd.value() );
		}
		if ( type.has_value() )
		{
			builder = builder.tryWithMetadataTag( type.value() );
		}
		if ( pos.has_value() )
		{
			builder = builder.tryWithMetadataTag( pos.value() );
		}
		if ( detail.has_value() )
		{
			builder = builder.tryWithMetadataTag( detail.value() );
		}
		if ( !qty.has_value() &&
			 !cnt.has_value() &&
			 !calc.has_value() &&
			 !stateTag.has_value() &&
			 !cmd.has_value() &&
			 !type.has_value() &&
			 !pos.has_value() &&
			 !detail.has_value() )
		{
			errorBuilder.addError( internal::LocalIdParsingState::Completeness, "No metadata tags specified. Local IDs require atleast 1 metadata tag." );
		}

		localIdBuilder = std::move( builder );

		return ( !errorBuilder.hasError() && !invalidSecondaryItem );
	}

	void LocalIdBuilder::advanceParser( size_t& i, std::string_view segment, internal::LocalIdParsingState& state )
	{
		state = static_cast<internal::LocalIdParsingState>( static_cast<int>( state ) + 1 );
		i += segment.length() + 1;
	}

	void LocalIdBuilder::advanceParser( size_t& i, std::string_view segment )
	{
		i += segment.length() + 1;
	}

	void LocalIdBuilder::advanceParser( internal::LocalIdParsingState& state, internal::LocalIdParsingState to )
	{
		state = to;
	}

	void LocalIdBuilder::advanceParser( size_t& i, std::string_view segment, internal::LocalIdParsingState& state, internal::LocalIdParsingState to )
	{
		i += segment.length() + 1;
		state = to;
	}

	std::optional<internal::LocalIdParsingState> LocalIdBuilder::metaPrefixToState( std::string_view prefix )
	{
		auto it = prefixMap.find( prefix );
		return ( it != prefixMap.end() )
				   ? std::make_optional( it->second )
				   : std::nullopt;
	}

	std::optional<internal::LocalIdParsingState> LocalIdBuilder::nextParsingState( internal::LocalIdParsingState prev )
	{
		switch ( prev )
		{
			case internal::LocalIdParsingState::MetaQuantity:
			{
				return internal::LocalIdParsingState::MetaContent;
			}
			case internal::LocalIdParsingState::MetaContent:
			{
				return internal::LocalIdParsingState::MetaCalculation;
			}
			case internal::LocalIdParsingState::MetaCalculation:
			{
				return internal::LocalIdParsingState::MetaState;
			}
			case internal::LocalIdParsingState::MetaState:
			{
				return internal::LocalIdParsingState::MetaCommand;
			}
			case internal::LocalIdParsingState::MetaCommand:
			{
				return internal::LocalIdParsingState::MetaType;
			}
			case internal::LocalIdParsingState::MetaType:
			{
				return internal::LocalIdParsingState::MetaPosition;
			}
			case internal::LocalIdParsingState::MetaPosition:
			{
				return internal::LocalIdParsingState::MetaDetail;
			}
			case internal::LocalIdParsingState::MetaDetail:
			{
				return std::nullopt;
			}
			case internal::LocalIdParsingState::NamingRule:
			case internal::LocalIdParsingState::VisVersion:
			case internal::LocalIdParsingState::PrimaryItem:
			case internal::LocalIdParsingState::SecondaryItem:
			case internal::LocalIdParsingState::ItemDescription:
			case internal::LocalIdParsingState::EmptyState:
			case internal::LocalIdParsingState::Formatting:
			case internal::LocalIdParsingState::Completeness:
			case internal::LocalIdParsingState::NamingEntity:
			case internal::LocalIdParsingState::IMONumber:
			default:
			{
				return std::nullopt;
			}
		}
	}

	bool LocalIdBuilder::parseMetaTag(
		CodebookName codebookName, internal::LocalIdParsingState& state, size_t& i, std::string_view segment,
		std::optional<MetadataTag>& tag, const Codebooks* codebooks, internal::LocalIdParsingErrorBuilder& errorBuilder )
	{
		if ( !codebooks )
		{
			return false;
		}

		auto dashIndex = segment.find( constants::localId::PREFIX_DASH );
		auto tildeIndex = segment.find( constants::localId::PREFIX_TILDE );
		auto prefixIndex = ( dashIndex == std::string_view::npos )
							   ? tildeIndex
							   : dashIndex;

		if ( prefixIndex == std::string_view::npos )
		{
			auto lease = internal::StringBuilderPool::lease();
			auto builder = lease.builder();

			builder.append( "Invalid metadata tag: missing prefix '" );
			builder.append( constants::localId::PREFIX_DASH );
			builder.append( "' or '" );
			builder.append( constants::localId::PREFIX_TILDE );
			builder.append( "' in " );
			builder.append( segment );

			errorBuilder.addError( state, lease.toString() );
			advanceParser( i, segment, state );

			return true;
		}

		auto actualPrefix = segment.substr( 0, prefixIndex );

		auto actualState = metaPrefixToState( actualPrefix );
		if ( !actualState.has_value() || actualState.value() < state )
		{
			auto lease = internal::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "Invalid metadata tag: unknown prefix " );
			builder.append( actualPrefix );
			errorBuilder.addError( state, lease.toString() );

			return false;
		}

		if ( actualState.value() > state )
		{
			advanceParser( state, actualState.value() );

			return true;
		}

		auto nextState = nextParsingState( actualState.value() );

		auto value = segment.substr( prefixIndex + 1 );
		if ( value.empty() )
		{
			auto codebookStr = codebookNametoString( codebookName );
			errorBuilder.addError( state, "Invalid " + codebookStr + " metadata tag: missing value" );

			return false;
		}

		tag = codebooks->tryCreateTag( codebookName, value );
		if ( !tag.has_value() )
		{
			auto codebookStr = codebookNametoString( codebookName );
			auto lease = internal::StringBuilderPool::lease();
			auto builder = lease.builder();

			if ( prefixIndex == tildeIndex )
			{
				builder.append( "Invalid custom " );
				builder.append( codebookStr );
				builder.append( " metadata tag: failed to create " );
				builder.append( value );
				errorBuilder.addError( state, lease.toString() );
			}
			else
			{
				builder.append( "Invalid " );
				builder.append( codebookStr );
				builder.append( " metadata tag: failed to create " );
				builder.append( value );
				errorBuilder.addError( state, lease.toString() );
			}

			advanceParser( i, segment, state );

			return true;
		}

		if ( prefixIndex == dashIndex && tag.value().prefix() == constants::localId::CHAR_TILDE )
		{
			auto codebookStr = codebookNametoString( codebookName );
			auto lease = internal::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "Invalid " );
			builder.append( codebookStr );
			builder.append( " metadata tag: '" );
			builder.append( value );
			builder.append( "'. Use prefix '" );
			builder.append( constants::localId::PREFIX_TILDE );
			builder.append( "' for custom values" );
			errorBuilder.addError( state, lease.toString() );
		}

		if ( !nextState.has_value() )
		{
			advanceParser( i, segment, state );
		}
		else
		{
			advanceParser( i, segment, state, nextState.value() );
		}

		return true;
	}

}
