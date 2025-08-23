/**
 * @file DataChannel.cpp
 * @brief Data channel infrastructure implementation
 */

#include "dnv/vista/sdk/transport/DataChannel/DataChannel.h"

#include "dnv/vista/sdk/utils/StringUtils.h"
#include "dnv/vista/sdk/internal/StringBuilderPool.h"

namespace dnv::vista::sdk::transport::datachannel
{
	//=====================================================================
	// Restriction class
	//=====================================================================

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	void Restriction::setTotalDigits( std::optional<uint32_t> totalDigits )
	{
		if ( totalDigits && *totalDigits == 0 )
		{
			throw std::invalid_argument( "Total digits must be greater than zero" );
		}

		m_totalDigits = totalDigits;
	}

	//=====================================================================
	// Range class
	//=====================================================================

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	void Range::setLow( double low )
	{
		if ( low > m_high )
		{
			throw std::invalid_argument( "Range low bound cannot be greater than high bound" );
		}

		if ( low < std::numeric_limits<double>::lowest() )
		{
			throw std::invalid_argument( "Range low value is below minimum limit" );
		}

		m_low = low;
	}

	void Range::setHigh( double high )
	{
		if ( high < m_low )
		{
			throw std::invalid_argument( "Range high bound cannot be less than low bound" );
		}

		if ( high > std::numeric_limits<double>::max() )
		{
			throw std::invalid_argument( "Range high value is above maximum limit" );
		}

		m_high = high;
	}

	//----------------------------------------------
	// Validation
	//----------------------------------------------

	ValidateResult Restriction::validateValue( const std::string& value, const Format& format ) const
	{
		if ( m_enumeration && !m_enumeration->empty() )
		{
			auto it = std::find( m_enumeration->begin(), m_enumeration->end(), value );
			if ( it == m_enumeration->end() )
			{
				return ValidateResult::Invalid{ { "Value not in enumeration list" } };
			}
		}

		if ( m_pattern )
		{
			try
			{
				std::regex pattern_regex( *m_pattern );
				if ( !std::regex_match( value, pattern_regex ) )
				{
					return ValidateResult::Invalid{ { "Value does not match pattern: " + *m_pattern } };
				}
			}
			catch ( const std::exception& e )
			{
				return ValidateResult::Invalid{ { "Invalid regex pattern: " + std::string( e.what() ) } };
			}
		}

		if ( m_length && value.length() != *m_length )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Value length must be exactly " );
			builder.append( std::to_string( *m_length ) );
			builder.append( " characters" );

			return ValidateResult::Invalid{ { lease.toString() } };
		}

		if ( m_minLength && value.length() < *m_minLength )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Value length must be at least " );
			builder.append( std::to_string( *m_minLength ) );
			builder.append( " characters" );

			return ValidateResult::Invalid{ { lease.toString() } };
		}

		if ( m_maxLength && value.length() > *m_maxLength )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Value length must be at most " );
			builder.append( std::to_string( *m_maxLength ) );
			builder.append( " characters" );

			return ValidateResult::Invalid{ { lease.toString() } };
		}

		if ( format.isDecimal() )
		{
			datatypes::Decimal128 decimalValueResult;
			if ( !datatypes::Decimal128::tryParse( value, decimalValueResult ) )
			{
				return ValidateResult::Invalid{ { "Invalid numeric value" } };
			}

			auto numericResult = validateNumber( decimalValueResult.toDouble() );
			if ( !numericResult.isOk() )
			{
				return numericResult;
			}

			if ( m_fractionDigits )
			{
				auto decimalPlaces = countDecimalPlaces( decimalValueResult );
				if ( decimalPlaces > *m_fractionDigits )
				{
					auto lease = internal::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "Value has too many decimal places (max: " );
					builder.append( std::to_string( *m_fractionDigits ) );
					builder.append( ")" );

					return ValidateResult::Invalid{ { lease.toString() } };
				}
			}

			if ( m_totalDigits )
			{
				std::string numStr = decimalValueResult.toString();

				/* Remove decimal point and minus sign for counting */
				numStr.erase( std::remove_if( numStr.begin(), numStr.end(), []( char c ) { return c == '.' || c == '-'; } ), numStr.end() );
				if ( numStr.length() > *m_totalDigits )
				{
					auto lease = internal::StringBuilderPool::instance();
					auto builder = lease.builder();
					builder.append( "Value has too many total digits (max: " );
					builder.append( std::to_string( *m_totalDigits ) );
					builder.append( ")" );

					return ValidateResult::Invalid{ { lease.toString() } };
				}
			}
		}

		return ValidateResult::Ok{};
	}

	//----------------------------------------------
	// Private validation helpers
	//----------------------------------------------

	ValidateResult Restriction::validateNumber( double number ) const
	{
		if ( m_minExclusive && number <= *m_minExclusive )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Value must be greater than " );
			builder.append( std::to_string( *m_minExclusive ) );

			return ValidateResult::Invalid{ { lease.toString() } };
		}

		if ( m_maxExclusive && number >= *m_maxExclusive )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Value must be less than " );
			builder.append( std::to_string( *m_maxExclusive ) );

			return ValidateResult::Invalid{ { lease.toString() } };
		}

		if ( m_minInclusive && number < *m_minInclusive )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Value must be greater than or equal to " );
			builder.append( std::to_string( *m_minInclusive ) );

			return ValidateResult::Invalid{ { lease.toString() } };
		}

		if ( m_maxInclusive && number > *m_maxInclusive )
		{
			auto lease = internal::StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "Value must be less than or equal to " );
			builder.append( std::to_string( *m_maxInclusive ) );

			return ValidateResult::Invalid{ { lease.toString() } };
		}

		return ValidateResult::Ok{};
	}

	uint32_t Restriction::countDecimalPlaces( datatypes::Decimal128 decimal )
	{
		std::string_view str = decimal.toString();
		size_t decimalPos = str.find( '.' );
		if ( decimalPos == std::string_view::npos )
		{
			return 0;
		}

		/* Find last non-zero digit to remove trailing zeros */
		size_t lastNonZero = str.find_last_not_of( '0' );
		if ( lastNonZero == std::string_view::npos || lastNonZero <= decimalPos )
		{
			/* No decimal digits or only trailing zeros */
			return 0;
		}

		return static_cast<uint32_t>( lastNonZero - decimalPos );
	}

	//=====================================================================
	// Format class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	Format::Format( std::string type )
	{
		setType( std::move( type ) );
	}

	//----------------------------------------------
	// Validation
	//----------------------------------------------

	ValidateResult Format::validateValue( const std::string& value, dnv::vista::sdk::transport::Value& parsedValue ) const
	{
		if ( !m_dataType )
		{
			return ValidateResult::Invalid{ { "Format data type not initialized" } };
		}

		/* Use FormatDataType to validate and parse the value */
		auto result = m_dataType->validate( value, parsedValue );
		if ( !result.isOk() )
		{
			return result;
		}

		/* Apply additional restrictions if present */
		if ( m_restriction )
		{
			return m_restriction->validateValue( value, *this );
		}

		return ValidateResult::Ok{};
	}

	//=====================================================================
	// DataChannelType class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	DataChannelType::DataChannelType( std::string type )
	{
		setType( std::move( type ) );
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	const std::string& DataChannelType::type() const
	{
		if ( !m_type )
		{
			throw std::runtime_error( "DataChannelType type not set" );
		}
		return *m_type;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	void DataChannelType::setType( std::string type )
	{
		/* Validate against ISO19848 data channel types */
		auto channelTypes = ISO19848::instance().dataChannelTypeNames( ISO19848::LatestVersion );
		auto result = channelTypes.parse( type );

		if ( !result.isOk() )
		{
			throw std::invalid_argument( "Invalid data channel type: " + type );
		}

		m_type = std::move( type );
	}

	void DataChannelType::setUpdateCycle( std::optional<double> updateCycle )
	{
		if ( updateCycle && *updateCycle < 0.0 )
		{
			throw std::invalid_argument( "Update cycle must be non-negative" );
		}

		m_updateCycle = updateCycle;
	}

	void DataChannelType::setCalculationPeriod( std::optional<double> calculationPeriod )
	{
		if ( calculationPeriod && *calculationPeriod < 0.0 )
		{
			throw std::invalid_argument( "Calculation period must be non-negative" );
		}

		m_calculationPeriod = calculationPeriod;
	}

	//=====================================================================
	// Property class
	//=====================================================================

	//----------------------------------------------
	// Validation
	//----------------------------------------------

	ValidateResult Property::validate() const
	{
		/* Business rule: If format is decimal, range should be specified */
		if ( m_format.isDecimal() && !m_range )
		{
			return ValidateResult::Invalid{ { "Decimal format requires range specification" } };
		}

		/* Business rule: If format is decimal, unit should be specified */
		if ( m_format.isDecimal() && !m_unit )
		{
			return ValidateResult::Invalid{ { "Decimal format requires unit specification" } };
		}

		/* Business rule: If data channel type is alert, alert priority should be specified */
		if ( m_dataChannelType.isAlert() && !m_alertPriority )
		{
			return ValidateResult::Invalid{ { "Alert data channel type requires alert priority specification" } };
		}

		return ValidateResult::Ok{};
	}

	//=====================================================================
	// DataChannel class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	DataChannel::DataChannel( DataChannelId dataChannelId, Property property )
		: m_dataChannelId{ std::move( dataChannelId ) }
	{
		setProperty( std::move( property ) );
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	const Property& DataChannel::property() const
	{
		if ( !m_property )
		{
			throw std::runtime_error( "DataChannel property not set" );
		}

		return *m_property;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	void DataChannel::setProperty( Property property )
	{
		auto validationResult = property.validate();
		if ( !validationResult.isOk() )
		{
			/* Extract error message from Invalid result */
			std::string errorMessage;
			if ( !validationResult.invalid().errors().empty() )
			{
				errorMessage = validationResult.invalid().errors()[0];
			}
			throw std::invalid_argument( "Property validation failed: " + errorMessage );
		}

		m_property = std::move( property );
	}

	//=====================================================================
	// DataChannelList class
	//=====================================================================

	//----------------------------------------------
	// Collection interface
	//----------------------------------------------

	void DataChannelList::clear()
	{
		m_dataChannels.clear();
		m_shortIdMap.clear();
		m_localIdMap.clear();
	}

	//----------------------------------------------
	// Safe lookup methods
	//----------------------------------------------

	const DataChannel* DataChannelList::tryGetByShortId( const std::string& shortId ) const
	{
		auto it = m_shortIdMap.find( shortId );
		if ( it != m_shortIdMap.end() )
		{
			return &( it->second.get() );
		}

		return nullptr;
	}

	const DataChannel* DataChannelList::tryGetByLocalId( const LocalId& localId ) const
	{
		auto it = m_localIdMap.find( localId );
		if ( it != m_localIdMap.end() )
		{
			return &( it->second.get() );
		}

		return nullptr;
	}

	//----------------------------------------------
	// Modification methods
	//----------------------------------------------

	void DataChannelList::add( DataChannel dataChannel )
	{
		/* Check for LocalId conflicts */
		if ( m_localIdMap.find( dataChannel.dataChannelId().localId() ) != m_localIdMap.end() )
		{
			throw std::invalid_argument( "LocalId already exists in collection" );
		}

		/* Check for ShortId conflicts (if ShortId is present) */
		if ( dataChannel.dataChannelId().shortId() )
		{
			const auto& shortId = *dataChannel.dataChannelId().shortId();
			if ( m_shortIdMap.find( shortId ) != m_shortIdMap.end() )
			{
				throw std::invalid_argument( "ShortId already exists in collection: " + shortId );
			}
		}

		/* Add to main collection*/
		m_dataChannels.push_back( std::move( dataChannel ) );
		const auto& addedChannel = m_dataChannels.back();

		/* Update indexes */
		m_localIdMap.emplace( addedChannel.dataChannelId().localId(), std::cref( addedChannel ) );

		if ( addedChannel.dataChannelId().shortId() )
		{
			m_shortIdMap.emplace( *addedChannel.dataChannelId().shortId(), std::cref( addedChannel ) );
		}
	}

	void DataChannelList::add( const std::vector<DataChannel>& dataChannels )
	{
		for ( const auto& dataChannel : dataChannels )
		{
			add( dataChannel );
		}
	}

	bool DataChannelList::remove( const DataChannel& dataChannel )
	{
		/* Find in main collection */
		auto it = std::find_if( m_dataChannels.begin(), m_dataChannels.end(),
			[&]( const DataChannel& dc ) {
				return dc.dataChannelId().localId() == dataChannel.dataChannelId().localId();
			} );

		if ( it == m_dataChannels.end() )
		{
			return false;
		}

		/* Remove from indexes */
		m_localIdMap.erase( it->dataChannelId().localId() );
		if ( it->dataChannelId().shortId() )
		{
			m_shortIdMap.erase( *it->dataChannelId().shortId() );
		}

		/* Remove from main collection */
		m_dataChannels.erase( it );
		return true;
	}
}
