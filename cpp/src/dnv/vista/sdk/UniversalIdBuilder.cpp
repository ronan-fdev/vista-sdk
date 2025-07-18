#include "pch.h"

#include "dnv/vista/sdk/UniversalIdBuilder.h"

#include "dnv/vista/sdk/ImoNumber.h"
#include "dnv/vista/sdk/LocalIdBuilder.h"
#include "dnv/vista/sdk/LocalIdParsingErrorBuilder.h"
#include "dnv/vista/sdk/ParsingErrors.h"
#include "dnv/vista/sdk/UniversalId.h"
#include "dnv/vista/sdk/VISVersion.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Constants
	//=====================================================================

	const std::string UniversalIdBuilder::namingEntity = "data.dnv.com";

	//=====================================================================
	// UniversalIdBuilder class
	//=====================================================================

	//----------------------------------------------
	// Assignment operators
	//----------------------------------------------

	UniversalIdBuilder& UniversalIdBuilder::operator=( UniversalIdBuilder&& other ) noexcept
	{
		if ( this != &other )
		{
			m_imoNumber = std::move( other.m_imoNumber );
			m_localIdBuilder = std::move( other.m_localIdBuilder );
		}

		return *this;
	}

	//----------------------------------------------
	// String conversion
	//----------------------------------------------

	std::string UniversalIdBuilder::toString() const
	{
		if ( !m_imoNumber.has_value() )
			throw std::invalid_argument( "Invalid Universal Id state: Missing IMO Number" );

		if ( !m_localIdBuilder.has_value() )
			throw std::invalid_argument( "Invalid Universal Id state: Missing LocalId" );

		std::string result;
		/*  entity + "/" + IMO + localId estimate */
		result.reserve( namingEntity.size() + 1 + 10 + 200 );

		result.append( namingEntity );
		result.append( "/" );
		result.append( m_imoNumber->toString() );
		result.append( m_localIdBuilder->toString() );

		return result;
	}

	//----------------------------------------------
	// Static factory methods
	//----------------------------------------------

	UniversalIdBuilder UniversalIdBuilder::create( VisVersion version )
	{
		return UniversalIdBuilder().withLocalId( LocalIdBuilder::create( version ) );
	}

	//----------------------------------------------
	// Build methods (Immutable fluent interface)
	//----------------------------------------------

	//----------------------------
	// Build
	//----------------------------

	UniversalId UniversalIdBuilder::build() const
	{
		return UniversalId( *this );
	}

	//----------------------------------------------
	// Local id
	//----------------------------------------------

	UniversalIdBuilder UniversalIdBuilder::withLocalId( const LocalIdBuilder& localId ) const
	{
		bool succeeded;
		auto result = tryWithLocalId( localId, succeeded );
		if ( !succeeded )
		{
			throw std::invalid_argument( "withLocalId" );
		}
		return result;
	}

	UniversalIdBuilder UniversalIdBuilder::tryWithLocalId( const std::optional<LocalIdBuilder>& localId ) const
	{
		bool dummy;
		return tryWithLocalId( localId, dummy );
	}

	UniversalIdBuilder UniversalIdBuilder::tryWithLocalId( const std::optional<LocalIdBuilder>& localId, bool& succeeded ) const
	{
		if ( !localId.has_value() )
		{
			succeeded = false;
			return *this;
		}

		succeeded = true;

		UniversalIdBuilder result( *this );

		result.m_localIdBuilder.emplace( localId.value() );

		return result;
	}

	UniversalIdBuilder UniversalIdBuilder::withoutLocalId() const
	{
		UniversalIdBuilder result( *this );
		result.m_localIdBuilder = std::nullopt;
		return result;
	}

	//----------------------------------------------
	// IMO number
	//----------------------------------------------

	UniversalIdBuilder UniversalIdBuilder::withImoNumber( const ImoNumber& imoNumber ) const
	{
		bool succeeded;
		auto result = tryWithImoNumber( imoNumber, succeeded );
		if ( !succeeded )
		{
			throw std::invalid_argument( "imoNumber" );
		}
		return result;
	}

	UniversalIdBuilder UniversalIdBuilder::tryWithImoNumber( const std::optional<ImoNumber>& imoNumber ) const
	{
		bool dummy;
		return tryWithImoNumber( imoNumber, dummy );
	}

	UniversalIdBuilder UniversalIdBuilder::tryWithImoNumber( const std::optional<ImoNumber>& imoNumber, bool& succeeded ) const
	{
		if ( !imoNumber.has_value() )
		{
			succeeded = false;
			return *this;
		}

		succeeded = true;

		UniversalIdBuilder result( *this );
		result.m_imoNumber = imoNumber;

		return result;
	}

	UniversalIdBuilder UniversalIdBuilder::withoutImoNumber() const
	{
		UniversalIdBuilder result( *this );
		result.m_imoNumber = std::nullopt;
		return result;
	}

	//----------------------------------------------
	// Static parsing methods
	//----------------------------------------------

	UniversalIdBuilder UniversalIdBuilder::parse( std::string_view universalIdStr )
	{
		ParsingErrors errors;
		std::optional<UniversalIdBuilder> builder;
		if ( !tryParse( universalIdStr, errors, builder ) )
		{
			throw std::invalid_argument( fmt::format( "Couldn't parse universal ID from: '{}'. {}", universalIdStr, errors.toString() ) );
		}
		return builder.value();
	}

	bool UniversalIdBuilder::tryParse( std::string_view universalId, std::optional<UniversalIdBuilder>& universalIdBuilder )
	{
		ParsingErrors errors;
		return tryParse( universalId, errors, universalIdBuilder );
	}

	bool UniversalIdBuilder::tryParse( std::string_view universalId, ParsingErrors& errors, std::optional<UniversalIdBuilder>& universalIdBuilder )
	{
		universalIdBuilder = std::nullopt;
		auto errorBuilder = LocalIdParsingErrorBuilder::create();

		if ( universalId.empty() )
		{
			errorBuilder.addError( LocalIdParsingState::NamingRule, std::string( "Failed to find localId start segment" ) );
			errors = errorBuilder.build();
			return false;
		}

		auto localIdStartIndex = universalId.find( "/dnv-v" );
		if ( localIdStartIndex == std::string::npos )
		{
			errorBuilder.addError( LocalIdParsingState::NamingRule, std::string( "Failed to find localId start segment" ) );
			errors = errorBuilder.build();
			return false;
		}

		std::string universalIdSegment = std::string( universalId.substr( 0, localIdStartIndex ) );
		std::string localIdSegment = std::string( universalId.substr( localIdStartIndex ) );

		std::optional<ImoNumber> imoNumber = std::nullopt;

		std::optional<LocalIdBuilder> localIdBuilder = std::nullopt;
		ParsingErrors localIdErrors;
		if ( !LocalIdBuilder::tryParse( localIdSegment, localIdErrors, localIdBuilder ) )
		{
			errors = errorBuilder.build();
			return false;
		}

		std::string_view span = universalIdSegment;
		LocalIdParsingState state = LocalIdParsingState::NamingEntity;
		size_t i = 0;

		while ( state <= LocalIdParsingState::IMONumber )
		{
			if ( i >= span.length() )
				break;

			auto nextSlash = span.substr( i ).find( '/' );
			std::string_view segment = ( nextSlash == std::string_view::npos ) ? span.substr( i ) : span.substr( i, nextSlash );

			switch ( state )
			{
				case LocalIdParsingState::NamingRule:
				case LocalIdParsingState::VisVersion:
				case LocalIdParsingState::PrimaryItem:
				case LocalIdParsingState::SecondaryItem:
				case LocalIdParsingState::ItemDescription:
				case LocalIdParsingState::MetaQuantity:
				case LocalIdParsingState::MetaContent:
				case LocalIdParsingState::MetaCalculation:
				case LocalIdParsingState::MetaState:
				case LocalIdParsingState::MetaCommand:
				case LocalIdParsingState::MetaType:
				case LocalIdParsingState::MetaPosition:
				case LocalIdParsingState::MetaDetail:
				case LocalIdParsingState::EmptyState:
				case LocalIdParsingState::Formatting:
				case LocalIdParsingState::Completeness:
					break;
				case LocalIdParsingState::NamingEntity:
					if ( segment != namingEntity )
					{
						errorBuilder.addError( state, std::string( "Naming entity segment didnt match. Found: " ) + std::string( segment ) );
						break;
					}
					break;
				case LocalIdParsingState::IMONumber:
				{
					auto imoResult = ImoNumber::tryParse( segment );
					if ( !imoResult.has_value() )
					{
						errorBuilder.addError( state, std::string( "Invalid IMO number segment" ) );
						break;
					}
					else
					{
						imoNumber = imoResult;
					}
				}
				break;
			}
			state = static_cast<LocalIdParsingState>( static_cast<int>( state ) + 1 );
			i += segment.length() + 1;
		}

		auto visVersion = localIdBuilder->visVersion();
		if ( !visVersion.has_value() )
		{
			errorBuilder.addError( LocalIdParsingState::VisVersion );
			errors = errorBuilder.build();
			return false;
		}

		universalIdBuilder.emplace( create( visVersion.value() )
				.tryWithLocalId( localIdBuilder )
				.tryWithImoNumber( imoNumber ) );

		errors = errorBuilder.build();

		return true;
	}
}
