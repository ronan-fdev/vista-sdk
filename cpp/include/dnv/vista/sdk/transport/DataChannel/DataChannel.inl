/**
 * @file DataChannel.inl
 * @brief Data channel infrastructure inline implementations
 * @details Inline method implementations for data channel classes
 */

#include "dnv/vista/sdk/constants/ISO19848Constants.h"

namespace dnv::vista::sdk::transport::datachannel
{
	//=====================================================================
	// VersionInformation class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE VersionInformation::VersionInformation()
		: m_namingRule{ constants::iso19848::ANNEX_C_NAMING_RULE },
		  m_namingSchemeVersion{ constants::iso19848::ANNEX_C_NAMING_SCHEME_VERSION },
		  m_referenceUrl{ constants::iso19848::ANNEX_C_REFERENCE_URL }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE const std::string& VersionInformation::namingRule() const noexcept
	{
		return m_namingRule;
	}

	VISTA_SDK_CPP_INLINE const std::string& VersionInformation::namingSchemeVersion() const noexcept
	{
		return m_namingSchemeVersion;
	}

	VISTA_SDK_CPP_INLINE const std::optional<std::string>& VersionInformation::referenceUrl() const noexcept
	{
		return m_referenceUrl;
	}

	//----------------------------------------------
	// Setters
	//---------------------------------------------

	VISTA_SDK_CPP_INLINE void VersionInformation::setNamingRule( std::string_view namingRule )
	{
		m_namingRule = namingRule;
	}

	VISTA_SDK_CPP_INLINE void VersionInformation::setNamingSchemeVersion( std::string_view namingSchemeVersion )
	{
		m_namingSchemeVersion = namingSchemeVersion;
	}

	VISTA_SDK_CPP_INLINE void VersionInformation::setReferenceUrl( std::optional<std::string_view> referenceUrl )
	{
		if ( referenceUrl )
		{
			m_referenceUrl = *referenceUrl;
		}
		else
		{
			m_referenceUrl = std::nullopt;
		}
	}

	//=====================================================================
	// ConfigurationReference class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE ConfigurationReference::ConfigurationReference( std::string_view id, nfx::time::DateTimeOffset timeStamp, std::optional<std::string_view> version )
		: m_id{ id },
		  m_timeStamp{ timeStamp },
		  m_version{ version
						 ? std::optional<std::string>{ *version }
						 : std::nullopt }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE const std::string& ConfigurationReference::id() const noexcept
	{
		return m_id;
	}

	VISTA_SDK_CPP_INLINE const std::optional<std::string>& ConfigurationReference::version() const noexcept
	{
		return m_version;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE void ConfigurationReference::setId( std::string_view id )
	{
		m_id = id;
	}

	VISTA_SDK_CPP_INLINE void ConfigurationReference::setVersion( std::optional<std::string_view> version )
	{
		if ( version )
		{
			m_version = *version;
		}
		else
		{
			m_version = std::nullopt;
		}
	}

	VISTA_SDK_CPP_INLINE void ConfigurationReference::setTimeStamp( nfx::time::DateTimeOffset timeStamp )
	{
		m_timeStamp = timeStamp;
	}

	//=====================================================================
	// Restriction class
	//=====================================================================

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE const std::optional<std::vector<std::string>>& Restriction::enumeration() const noexcept
	{
		return m_enumeration;
	}

	VISTA_SDK_CPP_INLINE std::optional<uint32_t> Restriction::fractionDigits() const noexcept
	{
		return m_fractionDigits;
	}

	VISTA_SDK_CPP_INLINE std::optional<uint32_t> Restriction::length() const noexcept
	{
		return m_length;
	}

	VISTA_SDK_CPP_INLINE std::optional<double> Restriction::maxExclusive() const noexcept
	{
		return m_maxExclusive;
	}

	VISTA_SDK_CPP_INLINE std::optional<double> Restriction::maxInclusive() const noexcept
	{
		return m_maxInclusive;
	}

	VISTA_SDK_CPP_INLINE std::optional<uint32_t> Restriction::maxLength() const noexcept
	{
		return m_maxLength;
	}

	VISTA_SDK_CPP_INLINE std::optional<double> Restriction::minExclusive() const noexcept
	{
		return m_minExclusive;
	}

	VISTA_SDK_CPP_INLINE std::optional<double> Restriction::minInclusive() const noexcept
	{
		return m_minInclusive;
	}

	VISTA_SDK_CPP_INLINE std::optional<uint32_t> Restriction::minLength() const noexcept
	{
		return m_minLength;
	}

	VISTA_SDK_CPP_INLINE const std::optional<std::string>& Restriction::pattern() const noexcept
	{
		return m_pattern;
	}

	VISTA_SDK_CPP_INLINE std::optional<uint32_t> Restriction::totalDigits() const noexcept
	{
		return m_totalDigits;
	}

	VISTA_SDK_CPP_INLINE std::optional<WhiteSpace> Restriction::whiteSpace() const noexcept
	{
		return m_whiteSpace;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE void Restriction::setEnumeration( std::optional<std::vector<std::string>> enumeration )
	{
		m_enumeration = std::move( enumeration );
	}

	VISTA_SDK_CPP_INLINE void Restriction::setFractionDigits( std::optional<uint32_t> fractionDigits )
	{
		m_fractionDigits = fractionDigits;
	}

	VISTA_SDK_CPP_INLINE void Restriction::setLength( std::optional<uint32_t> length )
	{
		m_length = length;
	}

	VISTA_SDK_CPP_INLINE void Restriction::setMaxExclusive( std::optional<double> maxExclusive )
	{
		m_maxExclusive = maxExclusive;
	}

	VISTA_SDK_CPP_INLINE void Restriction::setMaxInclusive( std::optional<double> maxInclusive )
	{
		m_maxInclusive = maxInclusive;
	}

	VISTA_SDK_CPP_INLINE void Restriction::setMaxLength( std::optional<uint32_t> maxLength )
	{
		m_maxLength = maxLength;
	}

	VISTA_SDK_CPP_INLINE void Restriction::setMinExclusive( std::optional<double> minExclusive )
	{
		m_minExclusive = minExclusive;
	}

	VISTA_SDK_CPP_INLINE void Restriction::setMinInclusive( std::optional<double> minInclusive )
	{
		m_minInclusive = minInclusive;
	}

	VISTA_SDK_CPP_INLINE void Restriction::setMinLength( std::optional<uint32_t> minLength )
	{
		m_minLength = minLength;
	}

	VISTA_SDK_CPP_INLINE void Restriction::setPattern( std::optional<std::string> pattern )
	{
		m_pattern = std::move( pattern );
	}

	VISTA_SDK_CPP_INLINE void Restriction::setWhiteSpace( std::optional<WhiteSpace> whiteSpace )
	{
		m_whiteSpace = whiteSpace;
	}

	//=====================================================================
	// Range class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE Range::Range()
		: m_low{ std::numeric_limits<double>::lowest() },
		  m_high{ std::numeric_limits<double>::max() }
	{
	}

	VISTA_SDK_CPP_INLINE Range::Range( double low, double high )
		: m_low{ low },
		  m_high{ high }
	{
		if ( low > high )
		{
			throw std::invalid_argument( "Range low bound cannot be greater than high bound" );
		}

		if ( low < std::numeric_limits<double>::lowest() )
		{
			throw std::invalid_argument( "Range low value is below minimum limit" );
		}

		if ( high > std::numeric_limits<double>::max() )
		{
			throw std::invalid_argument( "Range high value is above maximum limit" );
		}
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE double Range::low() const noexcept
	{
		return m_low;
	}

	VISTA_SDK_CPP_INLINE double Range::high() const noexcept
	{
		return m_high;
	}

	//=====================================================================
	// Unit class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE Unit::Unit( std::string_view unitSymbol )
		: m_unitSymbol{ unitSymbol }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE const std::string& Unit::unitSymbol() const noexcept
	{
		return m_unitSymbol;
	}

	VISTA_SDK_CPP_INLINE const std::optional<std::string>& Unit::quantityName() const noexcept
	{
		return m_quantityName;
	}

	VISTA_SDK_CPP_INLINE const std::optional<nfx::containers::StringMap<transport::Value>>& Unit::customElements() const noexcept
	{
		return m_customElements;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE void Unit::setUnitSymbol( std::string_view unitSymbol )
	{
		m_unitSymbol = unitSymbol;
	}

	VISTA_SDK_CPP_INLINE void Unit::setQuantityName( std::optional<std::string> quantityName )
	{
		m_quantityName = std::move( quantityName );
	}

	VISTA_SDK_CPP_INLINE void Unit::setCustomElements( std::optional<nfx::containers::StringMap<transport::Value>> customElements )
	{
		m_customElements = std::move( customElements );
	}

	//=====================================================================
	// Header class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE Header::Header(
		ShipId shipId,
		ConfigurationReference dataChannelListId,
		std::optional<std::string> author )
		: m_shipId{ std::move( shipId ) },
		  m_dataChannelListId{ std::move( dataChannelListId ) },
		  m_versionInformation{ VersionInformation{} },
		  m_author{ std::move( author ) },
		  m_dateCreated{ nfx::time::DateTimeOffset::utcNow() },
		  m_customHeaders{ std::nullopt }
	{
	}

	VISTA_SDK_CPP_INLINE Header::Header(
		ShipId shipId,
		ConfigurationReference dataChannelListId,
		std::optional<VersionInformation> versionInformation,
		std::optional<std::string> author,
		std::optional<nfx::time::DateTimeOffset> dateCreated,
		std::optional<nfx::containers::StringMap<Value>> customHeaders )
		: m_shipId{ std::move( shipId ) },
		  m_dataChannelListId{ std::move( dataChannelListId ) },
		  m_versionInformation{ std::move( versionInformation ) },
		  m_author{ std::move( author ) },
		  m_dateCreated{ std::move( dateCreated ) },
		  m_customHeaders{ std::move( customHeaders ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE const ShipId& Header::shipId() const noexcept
	{
		return m_shipId;
	}

	VISTA_SDK_CPP_INLINE const ConfigurationReference& Header::dataChannelListId() const noexcept
	{
		return m_dataChannelListId;
	}

	VISTA_SDK_CPP_INLINE const std::optional<VersionInformation>& Header::versionInformation() const noexcept
	{
		return m_versionInformation;
	}

	VISTA_SDK_CPP_INLINE const std::optional<std::string>& Header::author() const noexcept
	{
		return m_author;
	}

	VISTA_SDK_CPP_INLINE const std::optional<nfx::time::DateTimeOffset>& Header::dateCreated() const noexcept
	{
		return m_dateCreated;
	}

	VISTA_SDK_CPP_INLINE const std::optional<nfx::containers::StringMap<transport::Value>>& Header::customHeaders() const noexcept
	{
		return m_customHeaders;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE void Header::setShipId( ShipId shipId )
	{
		m_shipId = std::move( shipId );
	}

	VISTA_SDK_CPP_INLINE void Header::setDataChannelListId( ConfigurationReference dataChannelListId )
	{
		m_dataChannelListId = std::move( dataChannelListId );
	}

	VISTA_SDK_CPP_INLINE void Header::setVersionInformation( std::optional<VersionInformation> versionInformation )
	{
		m_versionInformation = std::move( versionInformation );
	}

	VISTA_SDK_CPP_INLINE void Header::setAuthor( std::optional<std::string> author )
	{
		m_author = std::move( author );
	}

	VISTA_SDK_CPP_INLINE void Header::setDateCreated( std::optional<nfx::time::DateTimeOffset> dateCreated )
	{
		m_dateCreated = std::move( dateCreated );
	}

	VISTA_SDK_CPP_INLINE void Header::setCustomHeaders( std::optional<nfx::containers::StringMap<transport::Value>> customHeaders )
	{
		m_customHeaders = std::move( customHeaders );
	}

	//=====================================================================
	// NameObject class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE NameObject::NameObject()
		: m_namingRule{ constants::iso19848::ANNEX_C_NAMING_RULE }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE const std::string& NameObject::namingRule() const noexcept
	{
		return m_namingRule;
	}

	VISTA_SDK_CPP_INLINE const std::optional<nfx::containers::StringMap<transport::Value>>& NameObject::customNameObjects() const noexcept
	{
		return m_customNameObjects;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE void NameObject::setNamingRule( std::string namingRule )
	{
		m_namingRule = std::move( namingRule );
	}

	VISTA_SDK_CPP_INLINE void NameObject::setCustomNameObjects( std::optional<nfx::containers::StringMap<transport::Value>> customNameObjects )
	{
		m_customNameObjects = std::move( customNameObjects );
	}

	//=====================================================================
	// Format class
	//=====================================================================

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE const std::string& Format::type() const
	{
		if ( !m_type )
		{
			throw std::runtime_error( "Format type not set" );
		}
		return *m_type;
	}

	VISTA_SDK_CPP_INLINE const std::optional<Restriction>& Format::restriction() const noexcept
	{
		return m_restriction;
	}

	VISTA_SDK_CPP_INLINE bool Format::isDecimal() const noexcept
	{
		return m_type && *m_type == constants::iso19848::FORMAT_TYPE_DECIMAL;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE void Format::setType( std::string_view type )
	{
		/* Validate against ISO19848 format types */
		auto formatTypes = ISO19848::instance().formatDataTypes( ISO19848::LatestVersion );
		auto result = formatTypes.parse( std::string( type ) );

		if ( !result.isOk() )
		{
			throw std::invalid_argument( "Invalid format type: " + std::string( type ) );
		}

		m_type = type;
		m_dataType = result.ok().typeName();
	}

	VISTA_SDK_CPP_INLINE void Format::setRestriction( std::optional<Restriction> restriction )
	{
		m_restriction = std::move( restriction );
	}

	//=====================================================================
	// DataChannelType class
	//=====================================================================

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE std::optional<double> DataChannelType::updateCycle() const noexcept
	{
		return m_updateCycle;
	}

	VISTA_SDK_CPP_INLINE std::optional<double> DataChannelType::calculationPeriod() const noexcept
	{
		return m_calculationPeriod;
	}

	VISTA_SDK_CPP_INLINE bool DataChannelType::isAlert() const noexcept
	{
		return m_type && *m_type == "Alert";
	}

	//=====================================================================
	// Property class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE Property::Property(
		DataChannelType dataChannelType,
		Format format, std::optional<Range> range,
		std::optional<Unit> unit,
		std::optional<std::string> alertPriority )
		: m_dataChannelType{ std::move( dataChannelType ) },
		  m_format{ std::move( format ) },
		  m_range{ std::move( range ) },
		  m_unit{ std::move( unit ) },
		  m_alertPriority{ std::move( alertPriority ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE const DataChannelType& Property::dataChannelType() const noexcept
	{
		return m_dataChannelType;
	}

	VISTA_SDK_CPP_INLINE const Format& Property::format() const noexcept
	{
		return m_format;
	}

	VISTA_SDK_CPP_INLINE const std::optional<Range>& Property::range() const noexcept
	{
		return m_range;
	}

	VISTA_SDK_CPP_INLINE const std::optional<Unit>& Property::unit() const noexcept
	{
		return m_unit;
	}

	VISTA_SDK_CPP_INLINE const std::optional<std::string>& Property::qualityCoding() const noexcept
	{
		/* TODO: Validate Quality coding 'In the case of “IEC 61162-STATUS”, “A” (Data valid) and “V” (Data invalid) are used for the data quality.' */
		return m_qualityCoding;
	}

	VISTA_SDK_CPP_INLINE const std::optional<std::string>& Property::alertPriority() const noexcept
	{
		/* TODO: Validate - Priority level and criteria for classification shall be in accordance with IEC 62923-1:2018, 6.2.2.1 */
		return m_alertPriority;
	}

	VISTA_SDK_CPP_INLINE const std::optional<std::string>& Property::name() const noexcept
	{
		return m_name;
	}

	VISTA_SDK_CPP_INLINE const std::optional<std::string>& Property::remarks() const noexcept
	{
		return m_remarks;
	}

	VISTA_SDK_CPP_INLINE const std::optional<nfx::containers::StringMap<transport::Value>>& Property::customProperties() const noexcept
	{
		return m_customProperties;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE void Property::setDataChannelType( DataChannelType dataChannelType )
	{
		m_dataChannelType = std::move( dataChannelType );
	}

	VISTA_SDK_CPP_INLINE void Property::setFormat( Format format )
	{
		m_format = std::move( format );
	}

	VISTA_SDK_CPP_INLINE void Property::setRange( std::optional<Range> range )
	{
		m_range = std::move( range );
	}

	VISTA_SDK_CPP_INLINE void Property::setUnit( std::optional<Unit> unit )
	{
		m_unit = std::move( unit );
	}

	VISTA_SDK_CPP_INLINE void Property::setQualityCoding( std::optional<std::string> qualityCoding )
	{
		m_qualityCoding = std::move( qualityCoding );
	}

	VISTA_SDK_CPP_INLINE void Property::setAlertPriority( std::optional<std::string> alertPriority )
	{
		m_alertPriority = std::move( alertPriority );
	}

	VISTA_SDK_CPP_INLINE void Property::setName( std::optional<std::string> name )
	{
		m_name = std::move( name );
	}

	VISTA_SDK_CPP_INLINE void Property::setRemarks( std::optional<std::string> remarks )
	{
		m_remarks = std::move( remarks );
	}

	VISTA_SDK_CPP_INLINE void Property::setCustomProperties( std::optional<nfx::containers::StringMap<transport::Value>> customProperties )
	{
		m_customProperties = std::move( customProperties );
	}

	//=====================================================================
	// DataChannelId class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE DataChannelId::DataChannelId( LocalId localId, std::optional<std::string> shortId )
		: m_localId{ std::move( localId ) },
		  m_shortId{ std::move( shortId ) },
		  m_nameObject{ NameObject{} }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE const LocalId& DataChannelId::localId() const noexcept
	{
		return m_localId;
	}

	VISTA_SDK_CPP_INLINE const std::optional<std::string>& DataChannelId::shortId() const noexcept
	{
		return m_shortId;
	}

	VISTA_SDK_CPP_INLINE const std::optional<NameObject>& DataChannelId::nameObject() const noexcept
	{
		return m_nameObject;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE void DataChannelId::setLocalId( LocalId localId )
	{
		m_localId = std::move( localId );
	}

	VISTA_SDK_CPP_INLINE void DataChannelId::setShortId( std::optional<std::string> shortId )
	{
		m_shortId = std::move( shortId );
	}

	VISTA_SDK_CPP_INLINE void DataChannelId::setNameObject( std::optional<NameObject> nameObject )
	{
		m_nameObject = std::move( nameObject );
	}

	//=====================================================================
	// DataChannel class
	//=====================================================================

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE const DataChannelId& DataChannel::dataChannelId() const noexcept
	{
		return m_dataChannelId;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE void DataChannel::setDataChannelId( DataChannelId dataChannelId )
	{
		m_dataChannelId = std::move( dataChannelId );
	}

	//=====================================================================
	// DataChannelList class
	//=====================================================================

	//----------------------------------------------
	// Collection interface
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE DataChannelList::size_type DataChannelList::size() const noexcept
	{
		return m_dataChannels.size();
	}

	VISTA_SDK_CPP_INLINE bool DataChannelList::isEmpty() const noexcept
	{
		return m_dataChannels.empty();
	}

	//----------------------------------------------
	// Element access
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE const DataChannel& DataChannelList::operator[]( size_type index ) const
	{
		return m_dataChannels.at( index );
	}

	VISTA_SDK_CPP_INLINE const DataChannel& DataChannelList::operator[]( const std::string& shortId ) const
	{
		auto it = m_shortIdMap.find( shortId );
		if ( it == m_shortIdMap.end() )
		{
			throw std::out_of_range( "Short ID not found: " + shortId );
		}

		return it->second.get();
	}

	VISTA_SDK_CPP_INLINE const DataChannel& DataChannelList::operator[]( const LocalId& localId ) const
	{
		auto it = m_localIdMap.find( localId );
		if ( it == m_localIdMap.end() )
		{
			throw std::out_of_range( "Local ID not found" );
		}

		return it->second.get();
	}

	//----------------------------------------------
	// Iterators
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE DataChannelList::iterator DataChannelList::begin() noexcept
	{
		return m_dataChannels.begin();
	}

	VISTA_SDK_CPP_INLINE DataChannelList::const_iterator DataChannelList::begin() const noexcept
	{
		return m_dataChannels.begin();
	}

	VISTA_SDK_CPP_INLINE DataChannelList::iterator DataChannelList::end() noexcept
	{
		return m_dataChannels.end();
	}

	VISTA_SDK_CPP_INLINE DataChannelList::const_iterator DataChannelList::end() const noexcept
	{
		return m_dataChannels.end();
	}

	//----------------------------------------------
	// Data access
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE const std::vector<DataChannel>& DataChannelList::dataChannels() const noexcept
	{
		return m_dataChannels;
	}

	//=====================================================================
	// Package class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE Package::Package( Header header, DataChannelList dataChannelList )
		: m_header{ std::move( header ) },
		  m_dataChannelList{ std::move( dataChannelList ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE const Header& Package::header() const noexcept
	{
		return m_header;
	}

	VISTA_SDK_CPP_INLINE Header& Package::header() noexcept
	{
		return m_header;
	}

	VISTA_SDK_CPP_INLINE const DataChannelList& Package::dataChannelList() const noexcept
	{
		return m_dataChannelList;
	}

	VISTA_SDK_CPP_INLINE DataChannelList& Package::dataChannelList() noexcept
	{
		return m_dataChannelList;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE void Package::setHeader( Header header )
	{
		m_header = std::move( header );
	}

	VISTA_SDK_CPP_INLINE void Package::setDataChannelList( DataChannelList dataChannelList )
	{
		m_dataChannelList = std::move( dataChannelList );
	}

	//=====================================================================
	// DataChannelListPackage class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE DataChannelListPackage::DataChannelListPackage( Package package )
		: m_package{ std::move( package ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE const Package& DataChannelListPackage::package() const noexcept
	{
		return m_package;
	}

	VISTA_SDK_CPP_INLINE Package& DataChannelListPackage::package() noexcept
	{
		return m_package;
	}

	VISTA_SDK_CPP_INLINE const DataChannelList& DataChannelListPackage::dataChannelList() const noexcept
	{
		return m_package.dataChannelList();
	}

	VISTA_SDK_CPP_INLINE DataChannelList& DataChannelListPackage::dataChannelList() noexcept
	{
		return m_package.dataChannelList();
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_INLINE void DataChannelListPackage::setPackage( Package package )
	{
		m_package = std::move( package );
	}
}
