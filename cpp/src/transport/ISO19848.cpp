/**
 * @file ISO19848.cpp
 * @brief Implementation of ISO 19848 standard data access
 */

#include "dnv/vista/sdk/transport/ISO19848.h"

#include "dnv/vista/sdk/constants/ISO19848Constants.h"
#include "dnv/vista/sdk/internal/HashMap.h"
#include "dnv/vista/sdk/internal/StringBuilderPool.h"
#include "dnv/vista/sdk/utils/StringUtils.h"

#include "dnv/vista/sdk/EmbeddedResource.h"

namespace dnv::vista::sdk::transport
{
	//=====================================================================
	// ISO19848 class
	//=====================================================================

	//----------------------------------------------
	// Singleton access
	//----------------------------------------------

	ISO19848& ISO19848::instance() noexcept
	{
		static ISO19848 s_instance;
		return s_instance;
	}

	//----------------------------------------------
	// Singleton construction
	//----------------------------------------------

	ISO19848::ISO19848() noexcept
		: m_dataChannelTypeNamesDtoCache{ internal::MemoryCacheOptions{ 10, std::chrono::hours( 1 ), std::chrono::hours( 1 ) } },
		  m_dataChannelTypeNamesCache{ internal::MemoryCacheOptions{ 10, std::chrono::hours( 1 ), std::chrono::hours( 1 ) } },
		  m_formatDataTypesDtoCache{ internal::MemoryCacheOptions{ 10, std::chrono::hours( 1 ), std::chrono::hours( 1 ) } },
		  m_formatDataTypesCache{ internal::MemoryCacheOptions{ 10, std::chrono::hours( 1 ), std::chrono::hours( 1 ) } }
	{
	}

	//----------------------------------------------
	// Public interface
	//----------------------------------------------

	DataChannelTypeNames ISO19848::dataChannelTypeNames( ISO19848Version version )
	{
		return m_dataChannelTypeNamesCache.getOrCreate(
			version,
			[this, version]() -> DataChannelTypeNames {
				const auto dto = dataChannelTypeNamesDto( version );

				std::vector<DataChannelTypeName> values;
				values.reserve( dto.values().size() );

				for ( const auto& x : dto.values() )
				{
					values.emplace_back( x.type(), x.description() );
				}

				return DataChannelTypeNames{ std::move( values ) };
			},
			[]( internal::CacheEntry& entry ) {
				entry.size = 1;
				entry.slidingExpiration = std::chrono::hours( 1 );
			} );
	}

	FormatDataTypes ISO19848::formatDataTypes( ISO19848Version version )
	{
		return m_formatDataTypesCache.getOrCreate(
			version,
			[this, version]() -> FormatDataTypes {
				const auto dto = formatDataTypesDto( version );

				std::vector<FormatDataType> values;
				values.reserve( dto.values().size() );

				for ( const auto& x : dto.values() )
				{
					values.emplace_back( x.type(), x.description() );
				}

				return FormatDataTypes{ std::move( values ) };
			},
			[]( internal::CacheEntry& entry ) {
				entry.size = 1;
				entry.slidingExpiration = std::chrono::hours( 1 );
			} );
	}

	//----------------------------------------------
	// DTO access
	//----------------------------------------------

	DataChannelTypeNamesDto ISO19848::dataChannelTypeNamesDto( ISO19848Version version )
	{
		return m_dataChannelTypeNamesDtoCache.getOrCreate(
			version,
			[version]() -> DataChannelTypeNamesDto {
				const auto dto = loadDataChannelTypeNamesDto( version );
				if ( !dto.has_value() )
				{
					throw std::runtime_error( "Invalid state: Failed to load DataChannelTypeNamesDto" );
				}

				return dto.value();
			},
			[]( internal::CacheEntry& entry ) {
				entry.size = 1;
				entry.slidingExpiration = std::chrono::hours( 1 );
			} );
	}

	FormatDataTypesDto ISO19848::formatDataTypesDto( ISO19848Version version )
	{
		return m_formatDataTypesDtoCache.getOrCreate(
			version,
			[version]() -> FormatDataTypesDto {
				const auto dto = loadFormatDataTypesDto( version );
				if ( !dto.has_value() )
				{
					throw std::runtime_error( "Invalid state: Failed to load FormatDataTypesDto" );
				}

				return dto.value();
			},
			[]( internal::CacheEntry& entry ) {
				entry.size = 1;
				entry.slidingExpiration = std::chrono::hours( 1 );
			} );
	}

	//----------------------------------------------
	// Loading
	//----------------------------------------------=

	std::optional<DataChannelTypeNamesDto> ISO19848::loadDataChannelTypeNamesDto( ISO19848Version version )
	{
		switch ( version )
		{
			case ISO19848Version::v2018:
			{
				return EmbeddedResource::dataChannelTypeNames( constants::iso19848::VERSION_2018 );
			}
			case ISO19848Version::v2024:
			{
				return EmbeddedResource::dataChannelTypeNames( constants::iso19848::VERSION_2024 );
			}
			default:
			{
				throw std::invalid_argument( "Invalid ISO19848Version" );
			}
		}
	}

	std::optional<FormatDataTypesDto> ISO19848::loadFormatDataTypesDto( ISO19848Version version )
	{
		switch ( version )
		{
			case ISO19848Version::v2018:
			{
				return EmbeddedResource::formatDataTypes( constants::iso19848::VERSION_2018 );
			}
			case ISO19848Version::v2024:
			{
				return EmbeddedResource::formatDataTypes( constants::iso19848::VERSION_2024 );
			}
			default:
			{
				throw std::invalid_argument( "Invalid ISO19848Version" );
			}
		}
	}

	//=====================================================================
	// DataChannelTypeNames class
	//=====================================================================

	//----------------------------------------------
	// DataChannelTypeNames::ParseResult  class
	//----------------------------------------------

	//-----------------------------
	// Parsing
	//-----------------------------

	DataChannelTypeNames::ParseResult DataChannelTypeNames::parse( std::string_view type ) const
	{
		static thread_local internal::HashMap<std::string, DataChannelTypeName> s_lookupCache;
		static thread_local bool s_cacheInitialized = false;

		if ( !s_cacheInitialized )
		{
			for ( const auto& value : m_values )
			{
				s_lookupCache.insertOrAssign( std::string{ value.type() }, value );
			}
			s_cacheInitialized = true;
		}

		const auto* result = s_lookupCache.tryGetValue<std::string_view>( type );
		if ( result != nullptr )
		{
			return ParseResult{ ParseResult::Ok{ *result } };
		}

		return ParseResult{ ParseResult::Invalid{} };
	}

	//=====================================================================
	// FormatDataType class
	//=====================================================================

	//----------------------------------------------
	// Validation
	//----------------------------------------------

	ValidateResult FormatDataType::validate( std::string_view value, Value& outValue ) const
	{
		outValue = Value{ Value::String{ value } };

		if ( utils::equals( m_type, constants::iso19848::FORMAT_TYPE_DECIMAL ) )
		{
			datatypes::Decimal128 d;
			if ( !datatypes::Decimal128::tryParse( value, d ) )
			{
				auto lease = internal::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "Invalid decimal value - Value='" );
				builder.append( value );
				builder.append( "'" );

				return ValidateResult{ ValidateResult::Invalid{ { lease.toString() } } };
			}

			outValue = Value{ Value::Decimal{ d } };
			return ValidateResult{ ValidateResult::Ok{} };
		}
		else if ( utils::equals( m_type, constants::iso19848::FORMAT_TYPE_DOUBLE ) )
		{
			double d;
			if ( !utils::tryParseDouble( value, d ) )
			{
				auto lease = internal::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "Invalid double value - Value='" );
				builder.append( value );
				builder.append( "'" );

				return ValidateResult{ ValidateResult::Invalid{ { lease.toString() } } };
			}

			outValue = Value{ Value::Double{ d } };
			return ValidateResult{ ValidateResult::Ok{} };
		}
		else if ( utils::equals( m_type, constants::iso19848::FORMAT_TYPE_INTEGER ) )
		{
			int i;
			if ( !utils::tryParseInt( value, i ) )
			{
				auto lease = internal::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "Invalid integer value - Value='" );
				builder.append( value );
				builder.append( "'" );

				return ValidateResult{ ValidateResult::Invalid{ { lease.toString() } } };
			}

			outValue = Value{ Value::Integer{ i } };
			return ValidateResult{ ValidateResult::Ok{} };
		}
		else if ( utils::equals( m_type, constants::iso19848::FORMAT_TYPE_BOOLEAN ) )
		{
			bool b;
			if ( !utils::tryParseBool( value, b ) )
			{
				auto lease = internal::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "Invalid boolean value - Value='" );
				builder.append( value );
				builder.append( "'" );

				return ValidateResult{ ValidateResult::Invalid{ { lease.toString() } } };
			}

			outValue = Value{ Value::Boolean{ b } };
			return ValidateResult{ ValidateResult::Ok{} };
		}
		else if ( utils::equals( m_type, constants::iso19848::FORMAT_TYPE_CHAR ) )
		{
			if ( !utils::hasExactLength( value, 1 ) )
			{
				auto lease = internal::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "Invalid char value - Value='" );
				builder.append( value );
				builder.append( "'" );

				return ValidateResult{ ValidateResult::Invalid{ { lease.toString() } } };
			}

			outValue = Value{ Value::Char{ value[0] } };
			return ValidateResult{ ValidateResult::Ok{} };
		}
		else if ( utils::equals( m_type, constants::iso19848::FORMAT_TYPE_UNSIGNED_INTEGER ) )
		{
			std::uint32_t ui;
			if ( !utils::tryParseUInt( value, ui ) )
			{
				auto lease = internal::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "Invalid unsigned integer value - Value='" );
				builder.append( value );
				builder.append( "'" );

				return ValidateResult{ ValidateResult::Invalid{ { lease.toString() } } };
			}

			outValue = Value{ Value::UnsignedInteger{ ui } };
			return ValidateResult{ ValidateResult::Ok{} };
		}
		else if ( utils::equals( m_type, constants::iso19848::FORMAT_TYPE_LONG ) )
		{
			std::int64_t l;
			if ( !utils::tryParseLong( value, l ) )
			{
				auto lease = internal::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "Invalid long value - Value='" );
				builder.append( value );
				builder.append( "'" );

				return ValidateResult{ ValidateResult::Invalid{ { lease.toString() } } };
			}

			outValue = Value{ Value::Long{ l } };
			return ValidateResult{ ValidateResult::Ok{} };
		}
		else if ( utils::equals( m_type, constants::iso19848::FORMAT_TYPE_STRING ) )
		{
			return ValidateResult{ ValidateResult::Ok{} };
		}
		else if ( utils::equals( m_type, constants::iso19848::FORMAT_TYPE_DATETIME ) )
		{
			datatypes::DateTimeOffset dt;
			if ( !datatypes::DateTimeOffset::tryParse( value, dt ) )
			{
				auto lease = internal::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "Invalid datetime value - Value='" );
				builder.append( value );
				builder.append( "'" );

				return ValidateResult{ ValidateResult::Invalid{ { lease.toString() } } };
			}

			outValue = Value{ Value::DateTime{ std::move( dt ) } };
			return ValidateResult{ ValidateResult::Ok{} };
		}
		else
		{
			auto lease = internal::StringBuilderPool::lease();
			auto builder = lease.builder();

			builder.append( "Invalid format type " );
			builder.append( m_type );

			throw std::runtime_error( lease.toString() );
		}
	}

	//=====================================================================
	// FormatDataTypes class
	//=====================================================================

	//----------------------------------------------
	// FormatDataTypes::ParseResult class
	//----------------------------------------------

	//-----------------------------
	// Parsing
	//-----------------------------

	FormatDataTypes::ParseResult FormatDataTypes::parse( std::string_view type ) const
	{
		static thread_local internal::HashMap<std::string, FormatDataType> s_lookupCache;
		static thread_local bool s_cacheInitialized = false;

		if ( !s_cacheInitialized )
		{
			for ( const auto& value : m_values )
			{
				s_lookupCache.insertOrAssign( std::string{ value.type() }, value );
			}
			s_cacheInitialized = true;
		}

		const auto* result = s_lookupCache.tryGetValue<std::string_view>( type );
		if ( result != nullptr )
		{
			return ParseResult{ ParseResult::Ok{ *result } };
		}

		return ParseResult{ ParseResult::Invalid{} };
	}
}
