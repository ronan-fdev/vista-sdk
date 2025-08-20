/**
 * @file Locations.cpp
 * @brief Implements the Locations, Location, RelativeLocation, and related helper classes.
 */

#include "pch.h"

#include "dnv/vista/sdk/Locations.h"

#include "dnv/vista/sdk/constants/LocationsConstants.h"
#include "dnv/vista/sdk/utils/StringBuilderPool.h"

#include "dnv/vista/sdk/LocationParsingErrorBuilder.h"
#include "dnv/vista/sdk/ParsingErrors.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk
{
	namespace
	{
		static constexpr std::string_view groupNameToString( LocationGroup group )
		{
			switch ( group )
			{
				case LocationGroup::Number:
				{
					return constants::locations::GROUP_NAME_NUMBER;
				}
				case LocationGroup::Side:
				{
					return constants::locations::GROUP_NAME_SIDE;
				}
				case LocationGroup::Vertical:
				{
					return constants::locations::GROUP_NAME_VERTICAL;
				}
				case LocationGroup::Transverse:
				{
					return constants::locations::GROUP_NAME_TRANSVERSE;
				}
				case LocationGroup::Longitudinal:
				{
					return constants::locations::GROUP_NAME_LONGITUDINAL;
				}
				default:
				{
					return constants::locations::GROUP_NAME_UNKNOWN;
				}
			}
		}
	}

	//=====================================================================
	// Location Class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	Location::Location( std::string_view value )
		: m_value{ value } {}

	//=====================================================================
	// RelativeLocation Class
	//=====================================================================

	RelativeLocation::RelativeLocation(
		char code, std::string_view name, const Location& location, const std::optional<std::string> definition )
		: m_code{ code },
		  m_name{ name },
		  m_location{ location },
		  m_definition{ definition }
	{
	}

	//=====================================================================
	// LocationCharDict Class
	//=====================================================================

	//----------------------------------------------
	// Lookup operators
	//----------------------------------------------

	std::optional<char>& LocationCharDict::operator[]( LocationGroup key )
	{
		if ( static_cast<int>( key ) <= 0 )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Unsupported code: " );
			builder.append( std::to_string( static_cast<int>( key ) ) );

			throw std::runtime_error( lease.toString() );
		}

		auto index{ static_cast<size_t>( key ) - 1 };
		if ( index >= m_table.size() )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Unsupported code: " );
			builder.append( std::to_string( static_cast<int>( key ) ) );

			throw std::runtime_error( lease.toString() );
		}

		return m_table[index];
	}

	//----------------------------------------------
	// Public methods
	//----------------------------------------------

	bool LocationCharDict::tryAdd( LocationGroup key, char value, std::optional<char>& existingValue )
	{
		auto& v = ( *this )[key];
		if ( v.has_value() )
		{
			existingValue = v;

			return false;
		}

		existingValue = std::nullopt;
		v = value;

		return true;
	}

	//=====================================================================
	// Locations Class
	//=====================================================================

	Locations::Locations( VisVersion version, const LocationsDto& dto )
		: m_visVersion{ version }
	{
		m_locationCodes.reserve( dto.items().size() );
		for ( const auto& item : dto.items() )
		{
			m_locationCodes.emplace( item.code() );
		}

		m_relativeLocations.reserve( dto.items().size() );
		for ( const auto& relLocDto : dto.items() )
		{
			auto code = relLocDto.code();
			Location loc{ std::string{ 1, code } };

			m_relativeLocations.emplace_back( code, relLocDto.name(), loc, relLocDto.definition() );

			if ( code == constants::locations::CHAR_HORIZONTAL ||
				 code == constants::locations::CHAR_VERTICAL )
			{
				continue;
			}

			LocationGroup key;
			if ( code == constants::locations::CHAR_NUMBER )
			{
				key = LocationGroup::Number;
			}
			else if ( code == constants::locations::CHAR_PORT ||
					  code == constants::locations::CHAR_CENTER ||
					  code == constants::locations::CHAR_STARBOARD )
			{
				key = LocationGroup::Side;
			}
			else if ( code == constants::locations::CHAR_UPPER ||
					  code == constants::locations::CHAR_MIDDLE ||
					  code == constants::locations::CHAR_LOWER )
			{
				key = LocationGroup::Vertical;
			}
			else if ( code == constants::locations::CHAR_INBOARD ||
					  code == constants::locations::CHAR_OUTBOARD )
			{
				key = LocationGroup::Transverse;
			}
			else if ( code == constants::locations::CHAR_FORWARD ||
					  code == constants::locations::CHAR_AFT )
			{
				key = LocationGroup::Longitudinal;
			}
			else
			{
				auto lease = utils::StringBuilderPool::instance();
				auto builder = lease.builder();
				builder.append( "Unsupported code: " );
				builder.append( std::string_view{ &code, 1 } );
				throw std::invalid_argument( lease.toString() );
			}

			if ( m_groups.find( key ) == m_groups.end() )
			{
				m_groups[key] = std::vector<RelativeLocation>();
			}

			if ( key == LocationGroup::Number )
			{
				continue;
			}

			m_reversedGroups[code] = key;
			m_groups[key].emplace_back( code, relLocDto.name(), loc, relLocDto.definition() );
		}
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	VisVersion Locations::visVersion() const noexcept
	{
		return m_visVersion;
	}

	const std::vector<RelativeLocation>& Locations::relativeLocations() const noexcept
	{
		return m_relativeLocations;
	}

	const std::unordered_map<LocationGroup, std::vector<RelativeLocation>>& Locations::groups() const noexcept
	{
		return m_groups;
	}

	const std::map<char, LocationGroup>& Locations::reversedGroups() const noexcept
	{
		return m_reversedGroups;
	}

	//----------------------------------------------
	// Public methods - Parsing
	//----------------------------------------------

	Location Locations::parse( std::string_view locationStr ) const
	{
		Location location;
		if ( !tryParse( locationStr, location ) )
		{
			auto lease = utils::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Invalid location: " );
			builder.append( locationStr );
			throw std::invalid_argument( lease.toString() );
		}

		return location;
	}

	bool Locations::tryParse( const std::string& value, Location& location ) const
	{
		return tryParse( std::string_view( value ), location );
	}

	bool Locations::tryParse( const std::optional<std::string>& value, Location& location ) const
	{
		if ( !value.has_value() )
		{
			return false;
		}

		internal::LocationParsingErrorBuilder errorBuilder;

		return tryParseInternal( value.value(), value, location, errorBuilder );
	}

	bool Locations::tryParse( const std::optional<std::string>& value, Location& location, ParsingErrors& errors ) const
	{
		if ( !value.has_value() )
		{
			internal::LocationParsingErrorBuilder errorBuilder = internal::LocationParsingErrorBuilder::create();

			errorBuilder.addError( internal::LocationValidationResult::NullOrWhiteSpace, "Location is null" );

			errors = errorBuilder.build();

			location = Location{};

			return false;
		}

		internal::LocationParsingErrorBuilder errorBuilder = internal::LocationParsingErrorBuilder::create();

		bool result = tryParseInternal( value.value(), value, location, errorBuilder );
		errors = errorBuilder.build();

		return result;
	}

	bool Locations::tryParse( std::string_view value, Location& location ) const
	{
		internal::LocationParsingErrorBuilder errorBuilder;

		return tryParseInternal( value, std::nullopt, location, errorBuilder );
	}

	bool Locations::tryParse( std::string_view value, Location& location, ParsingErrors& errors ) const
	{
		internal::LocationParsingErrorBuilder errorBuilder;

		bool result = tryParseInternal( value, std::nullopt, location, errorBuilder );
		if ( !result )
		{
			errors = errorBuilder.build();
		}

		return result;
	}

	//----------------------------------------------
	// Private Methods
	//----------------------------------------------

	bool Locations::tryParseInternal(
		std::string_view span, const std::optional<std::string>& originalStr, Location& location, internal::LocationParsingErrorBuilder& errorBuilder ) const
	{
		auto displayString =
			[&span, &originalStr]() -> std::string { return originalStr.has_value() ? *originalStr : std::string{ span }; };

		if ( span.empty() )
		{
			errorBuilder.addError(
				internal::LocationValidationResult::NullOrWhiteSpace,
				"Invalid location: contains only whitespace" );

			return false;
		}

		bool isOnlyWhitespace = std::all_of( span.begin(), span.end(), []( unsigned char c_uc ) { return std::isspace( c_uc ); } );

		if ( isOnlyWhitespace )
		{
			errorBuilder.addError(
				internal::LocationValidationResult::NullOrWhiteSpace,
				"Invalid location: contains only whitespace" );

			return false;
		}

		std::string result;
		LocationCharDict charDict{};

		int digitStartIndex = -1;
		int prevDigitIndex = -1;
		int charsStartIndex = -1;

		for ( size_t i = 0; i < span.length(); ++i )
		{
			char ch = span[i];

			if ( std::isdigit( ch ) )
			{
				if ( prevDigitIndex != -1 && prevDigitIndex != static_cast<int>( i ) - 1 )
				{
					auto lease = utils::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "Invalid location: cannot have multiple separated digits in location: '" );
					builder.append( displayString() );
					builder.append( "'" );

					errorBuilder.addError( internal::LocationValidationResult::Invalid, lease.toString() );

					return false;
				}

				if ( charsStartIndex != -1 )
				{
					auto lease = utils::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "Invalid location: numeric location should start before location code(s) in location: '" );
					builder.append( displayString() );
					builder.append( "'" );

					errorBuilder.addError( internal::LocationValidationResult::InvalidOrder, lease.toString() );

					return false;
				}

				if ( digitStartIndex == -1 )
				{
					digitStartIndex = static_cast<int>( i );
				}

				prevDigitIndex = static_cast<int>( i );

				result.push_back( ch );

				continue;
			}

			if ( charsStartIndex == -1 )
			{
				charsStartIndex = static_cast<int>( i );
			}

			bool valid = m_locationCodes.find( ch ) != m_locationCodes.end();

			if ( !valid )
			{
				auto lease = utils::StringBuilderPool::instance();
				auto builder = lease.builder();
				const std::string& source = displayString();
				bool first = true;

				for ( char c : source )
				{
					if ( !std::isdigit( c ) &&
						 ( c == constants::locations::CHAR_NUMBER || m_locationCodes.find( c ) == m_locationCodes.end() ) )
					{
						if ( !first )
						{
							builder.append( "," );
						}
						first = false;
						builder.append( "'" );
						builder.append( std::string_view{ &c, 1 } );
						builder.append( "'" );
					}
				}

				auto errorLease = utils::StringBuilderPool::instance();
				auto errorMsgBuilder = errorLease.builder();
				errorMsgBuilder.append( "Invalid location code: '" );
				errorMsgBuilder.append( displayString() );
				errorMsgBuilder.append( "' with invalid location code(s): " );
				errorMsgBuilder.append( lease.toString() );

				errorBuilder.addError( internal::LocationValidationResult::InvalidCode, errorLease.toString() );

				return false;
			}

			if ( m_reversedGroups.find( ch ) != m_reversedGroups.end() )
			{
				LocationGroup group = m_reversedGroups.at( ch );
				std::optional<char> existingValue;

				if ( !charDict.tryAdd( group, ch, existingValue ) )
				{
					const std::string_view groupName = groupNameToString( group );
					auto lease = utils::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "Invalid location: Multiple '" );
					builder.append( groupName );
					builder.append( "' values. Got both '" );
					builder.append( std::string_view{ &existingValue.value(), 1 } );
					builder.append( "' and '" );
					builder.append( std::string_view{ &ch, 1 } );
					builder.append( "' in '" );
					builder.append( displayString() );
					builder.append( "'" );

					errorBuilder.addError( internal::LocationValidationResult::Invalid, lease.toString() );

					return false;
				}
			}

			if ( i > 0 && charsStartIndex != static_cast<int>( i ) )
			{
				char prevCh = span[i - 1];
				if ( !std::isdigit( prevCh ) && ch < prevCh )
				{
					auto lease = utils::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "Invalid location: '" );
					builder.append( displayString() );
					builder.append( "' not alphabetically sorted" );

					errorBuilder.addError( internal::LocationValidationResult::InvalidOrder, lease.toString() );

					return false;
				}
			}

			result.push_back( ch );
		}

		location = Location( originalStr.has_value() ? *originalStr : result );

		return true;
	}
}
