/**
 * @file TimeSeriesData.inl
 * @brief Inline implementations for TimeSeriesData classes
 */

namespace dnv::vista::sdk::transport::timeseries
{
	//=====================================================================
	// TimeRange class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE TimeRange::TimeRange( nfx::time::DateTimeOffset start, nfx::time::DateTimeOffset end )
		: m_start{ std::move( start ) },
		  m_end{ std::move( end ) }
	{
		if ( m_start > m_end )
		{
			throw std::invalid_argument{ "Start time must be before or equal to end time" };
		}
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE const nfx::time::DateTimeOffset& TimeRange::start() const noexcept
	{
		return m_start;
	}

	VISTA_SDK_CPP_FORCE_INLINE const nfx::time::DateTimeOffset& TimeRange::end() const noexcept
	{
		return m_end;
	}

	//=====================================================================
	// ConfigurationReference class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE ConfigurationReference::ConfigurationReference( std::string_view id, nfx::time::DateTimeOffset timeStamp )
		: m_id{ id },
		  m_timeStamp{ std::move( timeStamp ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE const std::string& ConfigurationReference::id() const noexcept
	{
		return m_id;
	}

	VISTA_SDK_CPP_FORCE_INLINE const nfx::time::DateTimeOffset& ConfigurationReference::timeStamp() const noexcept
	{
		return m_timeStamp;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE void ConfigurationReference::setId( std::string_view id )
	{
		m_id = id;
	}

	VISTA_SDK_CPP_FORCE_INLINE void ConfigurationReference::setTimeStamp( nfx::time::DateTimeOffset timeStamp )
	{
		m_timeStamp = std::move( timeStamp );
	}

	//=====================================================================
	// TabularDataSet class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE TabularDataSet::TabularDataSet(
		nfx::time::DateTimeOffset timeStamp,
		size_t capacity )
		: m_timeStamp{ std::move( timeStamp ) },
		  m_value{},
		  m_quality{ std::nullopt }
	{
		m_value.reserve( capacity );
	}

	VISTA_SDK_CPP_FORCE_INLINE TabularDataSet::TabularDataSet(
		nfx::time::DateTimeOffset timeStamp,
		std::vector<std::string> value,
		std::optional<std::vector<std::string>> quality )
		: m_timeStamp{ std::move( timeStamp ) },
		  m_value{ std::move( value ) },
		  m_quality{ std::move( quality ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE const nfx::time::DateTimeOffset& TabularDataSet::timeStamp() const noexcept
	{
		return m_timeStamp;
	}

	VISTA_SDK_CPP_FORCE_INLINE const std::vector<std::string>& TabularDataSet::value() const noexcept
	{
		return m_value;
	}

	VISTA_SDK_CPP_FORCE_INLINE const std::optional<std::vector<std::string>>& TabularDataSet::quality() const noexcept
	{
		return m_quality;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE void TabularDataSet::setTimeStamp( nfx::time::DateTimeOffset timeStamp )
	{
		m_timeStamp = std::move( timeStamp );
	}

	VISTA_SDK_CPP_FORCE_INLINE void TabularDataSet::setValue( std::vector<std::string> value )
	{
		m_value = std::move( value );
	}

	VISTA_SDK_CPP_FORCE_INLINE void TabularDataSet::setQuality( std::optional<std::vector<std::string>> quality )
	{
		m_quality = std::move( quality );
	}

	//=====================================================================
	// EventDataSet class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE EventDataSet::EventDataSet(
		nfx::time::DateTimeOffset timeStamp,
		DataChannelId dataChannelId,
		std::string value,
		std::optional<std::string> quality )
		: m_timeStamp{ std::move( timeStamp ) },
		  m_dataChannelId{ std::move( dataChannelId ) },
		  m_value{ std::move( value ) },
		  m_quality{ std::move( quality ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE const nfx::time::DateTimeOffset& EventDataSet::timeStamp() const noexcept
	{
		return m_timeStamp;
	}

	VISTA_SDK_CPP_FORCE_INLINE const DataChannelId& EventDataSet::dataChannelId() const noexcept
	{
		return m_dataChannelId;
	}

	VISTA_SDK_CPP_FORCE_INLINE const std::string& EventDataSet::value() const noexcept
	{
		return m_value;
	}

	VISTA_SDK_CPP_FORCE_INLINE const std::optional<std::string>& EventDataSet::quality() const noexcept
	{
		return m_quality;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE void EventDataSet::setTimeStamp( nfx::time::DateTimeOffset timeStamp )
	{
		m_timeStamp = std::move( timeStamp );
	}

	VISTA_SDK_CPP_FORCE_INLINE void EventDataSet::setDataChannelId( DataChannelId dataChannelId )
	{
		m_dataChannelId = std::move( dataChannelId );
	}

	VISTA_SDK_CPP_FORCE_INLINE void EventDataSet::setValue( std::string_view value )
	{
		m_value = value;
	}

	VISTA_SDK_CPP_FORCE_INLINE void EventDataSet::setQuality( std::optional<std::string_view> quality )
	{
		if ( quality )
		{
			m_quality = *quality;
		}
		else
		{
			m_quality = std::nullopt;
		}
	}

	//=====================================================================
	// TabularData class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE TabularData::TabularData(
		std::vector<DataChannelId> dataChannelIds,
		std::vector<TabularDataSet> dataSets )
		: m_dataChannelIds{ std::move( dataChannelIds ) },
		  m_dataSets{ std::move( dataSets ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE const std::vector<DataChannelId>& TabularData::dataChannelIds() const noexcept
	{
		return m_dataChannelIds;
	}

	VISTA_SDK_CPP_FORCE_INLINE const std::vector<TabularDataSet>& TabularData::dataSets() const noexcept
	{
		return m_dataSets;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE void TabularData::setDataChannelIds( std::vector<DataChannelId> dataChannelIds )
	{
		m_dataChannelIds = std::move( dataChannelIds );
	}

	VISTA_SDK_CPP_FORCE_INLINE void TabularData::setDataSets( std::vector<TabularDataSet> dataSets )
	{
		m_dataSets = std::move( dataSets );
	}

	//=====================================================================
	// EventData class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE EventData::EventData( size_t dataSet )
	{
		m_dataSet.reserve( dataSet );
	}

	VISTA_SDK_CPP_FORCE_INLINE EventData::EventData( std::vector<EventDataSet> dataSet )
		: m_dataSet{ std::move( dataSet ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE const std::vector<EventDataSet>& EventData::dataSet() const noexcept
	{
		return m_dataSet;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE void EventData::setDataSet( std::vector<EventDataSet> dataSet )
	{
		m_dataSet = std::move( dataSet );
	}

	//----------------------------------------------
	// Collection interface
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE void EventData::add( EventDataSet dataSet )
	{
		m_dataSet.push_back( std::move( dataSet ) );
	}

	VISTA_SDK_CPP_FORCE_INLINE void EventData::clear()
	{
		m_dataSet.clear();
	}

	//=====================================================================
	// Header class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE Header::Header(
		ShipId shipId,
		TimeRange timeRange,
		std::optional<std::string> author )
		: m_shipId{ std::move( shipId ) },
		  m_timeRange{ std::move( timeRange ) },
		  m_dateCreated{ nfx::time::DateTimeOffset::utcNow() },
		  m_dateModified{ nfx::time::DateTimeOffset::utcNow() },
		  m_author{ std::move( author ) },
		  m_systemConfiguration{ std::nullopt },
		  m_customHeaders{ std::nullopt }
	{
	}

	VISTA_SDK_CPP_FORCE_INLINE Header::Header(
		ShipId shipId,
		std::optional<TimeRange> timeRange,
		std::optional<nfx::time::DateTimeOffset> dateCreated,
		std::optional<nfx::time::DateTimeOffset> dateModified,
		std::optional<std::string> author,
		std::optional<std::vector<ConfigurationReference>> systemConfiguration,
		std::optional<nfx::containers::StringMap<Value>> customHeaders )
		: m_shipId{ std::move( shipId ) },
		  m_timeRange{ std::move( timeRange ) },
		  m_dateCreated{ std::move( dateCreated ) },
		  m_dateModified{ std::move( dateModified ) },
		  m_author{ std::move( author ) },
		  m_systemConfiguration{ std::move( systemConfiguration ) },
		  m_customHeaders{ std::move( customHeaders ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE const ShipId& Header::shipId() const noexcept
	{
		return m_shipId;
	}

	VISTA_SDK_CPP_FORCE_INLINE const std::optional<TimeRange>& Header::timeRange() const noexcept
	{
		return m_timeRange;
	}

	VISTA_SDK_CPP_FORCE_INLINE const std::optional<nfx::time::DateTimeOffset>& Header::dateCreated() const noexcept
	{
		return m_dateCreated;
	}

	VISTA_SDK_CPP_FORCE_INLINE const std::optional<nfx::time::DateTimeOffset>& Header::dateModified() const noexcept
	{
		return m_dateModified;
	}

	VISTA_SDK_CPP_FORCE_INLINE const std::optional<std::string>& Header::author() const noexcept
	{
		return m_author;
	}

	VISTA_SDK_CPP_FORCE_INLINE const std::optional<std::vector<ConfigurationReference>>& Header::systemConfiguration() const noexcept
	{
		return m_systemConfiguration;
	}

	VISTA_SDK_CPP_FORCE_INLINE const std::optional<nfx::containers::StringMap<Value>>& Header::customHeaders() const noexcept
	{
		return m_customHeaders;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE void Header::setShipId( ShipId shipId )
	{
		m_shipId = std::move( shipId );
	}

	VISTA_SDK_CPP_FORCE_INLINE void Header::setTimeRange( std::optional<TimeRange> timeRange )
	{
		m_timeRange = std::move( timeRange );
	}

	VISTA_SDK_CPP_FORCE_INLINE void Header::setDateCreated( std::optional<nfx::time::DateTimeOffset> dateCreated )
	{
		m_dateCreated = std::move( dateCreated );
	}

	VISTA_SDK_CPP_FORCE_INLINE void Header::setDateModified( std::optional<nfx::time::DateTimeOffset> dateModified )
	{
		m_dateModified = std::move( dateModified );
	}

	VISTA_SDK_CPP_FORCE_INLINE void Header::setAuthor( std::optional<std::string_view> author )
	{
		if ( author )
		{
			m_author = *author;
		}
		else
		{
			m_author = std::nullopt;
		}
	}

	VISTA_SDK_CPP_FORCE_INLINE void Header::setSystemConfiguration( std::optional<std::vector<ConfigurationReference>> systemConfiguration )
	{
		m_systemConfiguration = std::move( systemConfiguration );
	}

	VISTA_SDK_CPP_FORCE_INLINE void Header::setCustomHeaders( std::optional<nfx::containers::StringMap<Value>> customHeaders )
	{
		m_customHeaders = std::move( customHeaders );
	}

	//=====================================================================
	// TimeSeriesData class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE TimeSeriesData::TimeSeriesData(
		std::optional<ConfigurationReference> dataConfiguration,
		std::optional<std::vector<TabularData>> tabularData,
		std::optional<EventData> eventData,
		std::optional<nfx::containers::StringMap<Value>> customDataKinds )
		: m_dataConfiguration{ std::move( dataConfiguration ) },
		  m_tabularData{ std::move( tabularData ) },
		  m_eventData{ std::move( eventData ) },
		  m_customDataKinds{ std::move( customDataKinds ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE const std::optional<ConfigurationReference>& TimeSeriesData::dataConfiguration() const noexcept
	{
		return m_dataConfiguration;
	}

	VISTA_SDK_CPP_FORCE_INLINE const std::optional<std::vector<TabularData>>& TimeSeriesData::tabularData() const noexcept
	{
		return m_tabularData;
	}

	VISTA_SDK_CPP_FORCE_INLINE const std::optional<EventData>& TimeSeriesData::eventData() const noexcept
	{
		return m_eventData;
	}

	VISTA_SDK_CPP_FORCE_INLINE const std::optional<nfx::containers::StringMap<Value>>& TimeSeriesData::customDataKinds() const noexcept
	{
		return m_customDataKinds;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE void TimeSeriesData::setDataConfiguration( std::optional<ConfigurationReference> dataConfiguration )
	{
		m_dataConfiguration = std::move( dataConfiguration );
	}

	VISTA_SDK_CPP_FORCE_INLINE void TimeSeriesData::setTabularData( std::optional<std::vector<TabularData>> tabularData )
	{
		m_tabularData = std::move( tabularData );
	}

	VISTA_SDK_CPP_FORCE_INLINE void TimeSeriesData::setEventData( std::optional<EventData> eventData )
	{
		m_eventData = std::move( eventData );
	}

	VISTA_SDK_CPP_FORCE_INLINE void TimeSeriesData::setCustomDataKinds( std::optional<nfx::containers::StringMap<Value>> customDataKinds )
	{
		m_customDataKinds = std::move( customDataKinds );
	}

	//=====================================================================
	// Package class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE Package::Package(
		std::optional<Header> header,
		std::vector<TimeSeriesData> timeSeriesData )
		: m_header{ std::move( header ) },
		  m_timeSeriesData{ std::move( timeSeriesData ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE const std::optional<Header>& Package::header() const noexcept
	{
		return m_header;
	}

	VISTA_SDK_CPP_FORCE_INLINE const std::vector<TimeSeriesData>& Package::timeSeriesData() const noexcept
	{
		return m_timeSeriesData;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE void Package::setHeader( std::optional<Header> header )
	{
		m_header = std::move( header );
	}

	VISTA_SDK_CPP_FORCE_INLINE void Package::setTimeSeriesData( std::vector<TimeSeriesData> timeSeriesData )
	{
		m_timeSeriesData = std::move( timeSeriesData );
	}

	//=====================================================================
	// TimeSeriesDataPackage class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE TimeSeriesDataPackage::TimeSeriesDataPackage( Package package )
		: m_package{ std::move( package ) }
	{
	}

	//----------------------------------------------
	// Property access
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE const Package& TimeSeriesDataPackage::package() const noexcept
	{
		return m_package;
	}

	VISTA_SDK_CPP_FORCE_INLINE Package& TimeSeriesDataPackage::package() noexcept
	{
		return m_package;
	}

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	VISTA_SDK_CPP_FORCE_INLINE void TimeSeriesDataPackage::setPackage( Package package )
	{
		m_package = std::move( package );
	}
}
