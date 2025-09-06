/**
 * @file TimeSeriesData.h
 * @brief Time series data infrastructure with comprehensive validation system
 * @details Complete implementation of time series data package,
 *          tabular and event data following ISO-19848 standard
 */

#pragma once

#include <nfx/containers/StringMap.h>
#include <nfx/time/DateTime.h>

#include "dnv/vista/sdk/config/config.h"
#include "dnv/vista/sdk/transport/DataChannel/DataChannel.h"
#include "dnv/vista/sdk/transport/TimeSeriesData/DataChannelId.h"
#include "dnv/vista/sdk/transport/ShipId.h"
#include "dnv/vista/sdk/LocalId.h"

namespace dnv::vista::sdk
{
	//=====================================================================
	// Forward declarations
	//=====================================================================

	class ValidateResult;

	namespace transport
	{
		class Value;
	}

	namespace transport::timeseries
	{
		//=====================================================================
		// TimeRange class
		//=====================================================================

		/**
		 * @brief Time range with start and end timestamps
		 * @details Represents a time window or interval defined by start and end points.
		 *          Used to specify the temporal bounds for time series data collection.
		 *          Validates that start time is always before or equal to end time.
		 */
		class TimeRange final
		{
		public:
			//----------------------------------------------
			// Construction
			//----------------------------------------------

			/** @brief Default constructor */
			TimeRange() = delete;

			/**
			 * @brief Constructs TimeRange with start and end times
			 * @param start Start timestamp
			 * @param end End timestamp
			 * @throws std::invalid_argument If start > end
			 */
			VISTA_SDK_CPP_INLINE TimeRange( nfx::time::DateTimeOffset start, nfx::time::DateTimeOffset end );

			/** @brief Copy constructor */
			TimeRange( const TimeRange& ) = default;

			/** @brief Move constructor */
			TimeRange( TimeRange&& ) noexcept = default;

			/** @brief Destructor */
			~TimeRange() = default;

			//----------------------------------------------
			// Assignment operators
			//----------------------------------------------

			/** @brief Copy assignment operator */
			TimeRange& operator=( const TimeRange& ) = default;

			/** @brief Move assignment operator */
			TimeRange& operator=( TimeRange&& ) noexcept = default;

			//----------------------------------------------
			// Property access
			//----------------------------------------------

			/**
			 * @brief Get start timestamp
			 * @return Start time
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] VISTA_SDK_CPP_INLINE const nfx::time::DateTimeOffset& start() const noexcept;

			/**
			 * @brief Get end timestamp
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const nfx::time::DateTimeOffset& end() const noexcept;

			//----------------------------------------------
			// Setters
			//----------------------------------------------

			/**
			 * @brief Set start timestamp with validation
			 * @param start New start time
			 * @throws std::invalid_argument If start > current end
			 */
			void setStart( nfx::time::DateTimeOffset start );

			/**
			 * @brief Set end timestamp with validation
			 * @param end New end time
			 * @throws std::invalid_argument If end < current start
			 */
			void setEnd( nfx::time::DateTimeOffset end );

		private:
			//----------------------------------------------
			// Private members
			//----------------------------------------------

			nfx::time::DateTimeOffset m_start;
			nfx::time::DateTimeOffset m_end;
		};

		//=====================================================================
		// ConfigurationReference class
		//=====================================================================

		/**
		 * @brief Configuration reference for time series data
		 */
		class ConfigurationReference final
		{
		public:
			//----------------------------------------------
			// Construction
			//----------------------------------------------

			/** @brief Default constructor */
			ConfigurationReference() = delete;

			/**
			 * @brief Constructs ConfigurationReference with required parameters
			 * @param id Configuration identifier
			 * @param timeStamp Configuration timestamp
			 */
			VISTA_SDK_CPP_INLINE ConfigurationReference( std::string_view id, nfx::time::DateTimeOffset timeStamp );

			/** @brief Copy constructor */
			ConfigurationReference( const ConfigurationReference& ) = default;

			/** @brief Move constructor */
			ConfigurationReference( ConfigurationReference&& ) noexcept = default;

			/** @brief Destructor */
			~ConfigurationReference() = default;

			//----------------------------------------------
			// Assignment operators
			//----------------------------------------------

			/** @brief Copy assignment operator */
			ConfigurationReference& operator=( const ConfigurationReference& ) = default;

			/** @brief Move assignment operator */
			ConfigurationReference& operator=( ConfigurationReference&& ) noexcept = default;

			//----------------------------------------------
			// Property access
			//----------------------------------------------

			/**
			 * @brief Get configuration ID
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::string& id() const noexcept;

			/**
			 * @brief Get timestamp
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const nfx::time::DateTimeOffset& timeStamp() const noexcept;

			//----------------------------------------------
			// Setters
			//----------------------------------------------

			/**
			 * @brief Set configuration ID
			 * @param id New configuration identifier
			 */
			VISTA_SDK_CPP_INLINE void setId( std::string_view id );

			/**
			 * @brief Set timestamp
			 * @param timeStamp New configuration timestamp
			 */
			VISTA_SDK_CPP_INLINE void setTimeStamp( nfx::time::DateTimeOffset timeStamp );

		private:
			//----------------------------------------------
			// Private members
			//----------------------------------------------

			std::string m_id;
			nfx::time::DateTimeOffset m_timeStamp;
		};

		//=====================================================================
		// TabularDataSet class
		//=====================================================================

		/**
		 * @brief Single tabular data set with timestamp, values, and quality
		 */
		class TabularDataSet final
		{
		public:
			//----------------------------------------------
			// Construction
			//----------------------------------------------

			/** @brief Default constructor */
			TabularDataSet() = delete;

			/**
			 * @brief Constructs TabularDataSet with capacity
			 * @param timeStamp Data timestamp
			 * @param capacity Initial capacity for value vector
			 */
			VISTA_SDK_CPP_INLINE TabularDataSet(
				nfx::time::DateTimeOffset timeStamp,
				size_t capacity = 0 );

			/**
			 * @brief Constructs TabularDataSet with required fields
			 * @param timeStamp Data timestamp
			 * @param value Data values
			 * @param quality Optional quality values
			 */
			VISTA_SDK_CPP_INLINE TabularDataSet(
				nfx::time::DateTimeOffset timeStamp,
				std::vector<std::string> value,
				std::optional<std::vector<std::string>> quality = std::nullopt );

			/** @brief Copy constructor */
			TabularDataSet( const TabularDataSet& ) = default;

			/** @brief Move constructor */
			TabularDataSet( TabularDataSet&& ) noexcept = default;

			/** @brief Destructor */
			~TabularDataSet() = default;

			//----------------------------------------------
			// Assignment operators
			//----------------------------------------------

			/** @brief Copy assignment operator */
			TabularDataSet& operator=( const TabularDataSet& ) = default;

			/** @brief Move assignment operator */
			TabularDataSet& operator=( TabularDataSet&& ) noexcept = default;

			//----------------------------------------------
			// Property access
			//----------------------------------------------

			/**
			 * @brief Get timestamp
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const nfx::time::DateTimeOffset& timeStamp() const noexcept;

			/**
			 * @brief Get values
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::vector<std::string>& value() const noexcept;

			/**
			 * @brief Get quality values
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<std::vector<std::string>>& quality() const noexcept;

			//----------------------------------------------
			// Setters
			//----------------------------------------------

			/**
			 * @brief Set timestamp
			 * @param timeStamp New timestamp
			 */
			VISTA_SDK_CPP_INLINE void setTimeStamp( nfx::time::DateTimeOffset timeStamp );

			/**
			 * @brief Set values
			 * @param value New data values
			 */
			VISTA_SDK_CPP_INLINE void setValue( std::vector<std::string> value );

			/**
			 * @brief Set quality values
			 * @param quality New optional quality values
			 */
			VISTA_SDK_CPP_INLINE void setQuality( std::optional<std::vector<std::string>> quality );

		private:
			//----------------------------------------------
			// Private members
			//----------------------------------------------

			nfx::time::DateTimeOffset m_timeStamp;
			std::vector<std::string> m_value;
			std::optional<std::vector<std::string>> m_quality;
		};

		//=====================================================================
		// EventDataSet class
		//=====================================================================

		/**
		 * @brief Single event data set with timestamp, channel ID, value, and quality
		 */
		class EventDataSet final
		{
		public:
			//----------------------------------------------
			// Construction
			//----------------------------------------------

			/** @brief Default constructor */
			EventDataSet() = delete;

			/**
			 * @brief Constructs EventDataSet with required fields
			 * @param timeStamp Event timestamp
			 * @param dataChannelId Channel identifier
			 * @param value Event value
			 * @param quality Optional quality value
			 */
			VISTA_SDK_CPP_INLINE EventDataSet(
				nfx::time::DateTimeOffset timeStamp,
				DataChannelId dataChannelId,
				std::string value,
				std::optional<std::string> quality = std::nullopt );

			/** @brief Copy constructor */
			EventDataSet( const EventDataSet& ) = default;

			/** @brief Move constructor */
			EventDataSet( EventDataSet&& ) noexcept = default;

			/** @brief Destructor */
			~EventDataSet() = default;

			//----------------------------------------------
			// Assignment operators
			//----------------------------------------------

			/** @brief Copy assignment operator */
			EventDataSet& operator=( const EventDataSet& ) = delete;

			/** @brief Move assignment operator */
			EventDataSet& operator=( EventDataSet&& ) noexcept = default;

			//----------------------------------------------
			// Property access
			//----------------------------------------------

			/**
			 * @brief Get timestamp
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const nfx::time::DateTimeOffset& timeStamp() const noexcept;

			/**
			 * @brief Get data channel ID
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const DataChannelId& dataChannelId() const noexcept;

			/**
			 * @brief Get value
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::string& value() const noexcept;

			/**
			 * @brief Get quality
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<std::string>& quality() const noexcept;

			//----------------------------------------------
			// Setters
			//----------------------------------------------

			/**
			 * @brief Set timestamp
			 * @param timeStamp New timestamp
			 */
			VISTA_SDK_CPP_INLINE void setTimeStamp( nfx::time::DateTimeOffset timeStamp );

			/**
			 * @brief Set data channel ID
			 * @param dataChannelId New channel identifier
			 */
			VISTA_SDK_CPP_INLINE void setDataChannelId( DataChannelId dataChannelId );

			/**
			 * @brief Set value
			 * @param value New event value
			 */
			VISTA_SDK_CPP_INLINE void setValue( std::string_view value );

			/**
			 * @brief Set quality
			 * @param quality New optional quality value
			 */
			VISTA_SDK_CPP_INLINE void setQuality( std::optional<std::string_view> quality );

		private:
			//----------------------------------------------
			// Private members
			//----------------------------------------------

			nfx::time::DateTimeOffset m_timeStamp;
			DataChannelId m_dataChannelId;
			std::string m_value;
			std::optional<std::string> m_quality;
		};

		//=====================================================================
		// TabularData class
		//=====================================================================

		/**
		 * @brief Tabular data with channel IDs and data sets
		 */
		class TabularData final
		{
		public:
			//----------------------------------------------
			// Construction
			//----------------------------------------------

			/** @brief Default constructor */
			TabularData() = delete;

			/**
			 * @brief Constructs TabularData with required fields
			 * @param dataChannelIds Channel identifiers
			 * @param dataSets Data sets
			 */
			VISTA_SDK_CPP_INLINE TabularData(
				std::vector<DataChannelId> dataChannelIds,
				std::vector<TabularDataSet> dataSets );

			/** @brief Copy constructor */
			TabularData( const TabularData& ) = default;

			/** @brief Move constructor */
			TabularData( TabularData&& ) noexcept = default;

			/** @brief Destructor */
			~TabularData() = default;

			//----------------------------------------------
			// Assignment operators
			//----------------------------------------------

			/** @brief Copy assignment operator */
			TabularData& operator=( const TabularData& ) = default;

			/** @brief Move assignment operator */
			TabularData& operator=( TabularData&& ) noexcept = default;

			//----------------------------------------------
			// Property access
			//----------------------------------------------

			/**
			 * @brief Get data channel IDs
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::vector<DataChannelId>& dataChannelIds() const noexcept;

			/**
			 * @brief Get data sets
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::vector<TabularDataSet>& dataSets() const noexcept;

			//----------------------------------------------
			// Setters
			//----------------------------------------------

			/**
			 * @brief Set data channel IDs
			 * @param dataChannelIds New channel identifiers
			 */
			VISTA_SDK_CPP_INLINE void setDataChannelIds( std::vector<DataChannelId> dataChannelIds );

			/**
			 * @brief Set data sets
			 * @param dataSets New data sets
			 */
			VISTA_SDK_CPP_INLINE void setDataSets( std::vector<TabularDataSet> dataSets );

			//----------------------------------------------
			// Validation
			//----------------------------------------------

			/**
			 * @brief Validate tabular data structure (instance method)
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] ValidateResult validate() const;

			/**
			 * @brief Static validation method
			 * @param table TabularData instance to validate
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] static ValidateResult Validate( const TabularData& table );

		private:
			//----------------------------------------------
			// Private members
			//----------------------------------------------

			std::vector<DataChannelId> m_dataChannelIds;
			std::vector<TabularDataSet> m_dataSets;
		};

		//=====================================================================
		// EventData class
		//=====================================================================

		/**
		 * @brief Event data collection
		 */
		class EventData final
		{
		public:
			//----------------------------------------------
			// Construction
			//----------------------------------------------

			/** @brief Default constructor with optional capacity */
			VISTA_SDK_CPP_INLINE explicit EventData( size_t dataSet = 0 );

			/**
			 * @brief Constructs EventData with data sets
			 * @param dataSet Event data sets vector
			 */
			VISTA_SDK_CPP_INLINE explicit EventData( std::vector<EventDataSet> dataSet );

			/** @brief Copy constructor */
			EventData( const EventData& ) = default;

			/** @brief Move constructor */
			EventData( EventData&& ) noexcept = default;

			/** @brief Destructor */
			~EventData() = default;

			//----------------------------------------------
			// Assignment operators
			//----------------------------------------------

			/** @brief Copy assignment operator */
			EventData& operator=( const EventData& ) = default;

			/** @brief Move assignment operator */
			EventData& operator=( EventData&& ) noexcept = default;

			//----------------------------------------------
			// Property access
			//----------------------------------------------

			/**
			 * @brief Get data set
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::vector<EventDataSet>& dataSet() const noexcept;

			//----------------------------------------------
			// Setters
			//----------------------------------------------

			/**
			 * @brief Set data set
			 * @param dataSet New event data sets
			 */
			VISTA_SDK_CPP_INLINE void setDataSet( std::vector<EventDataSet> dataSet );

			//----------------------------------------------
			// Collection interface
			//----------------------------------------------

			/**
			 * @brief Add event data set
			 * @param dataSet Event data set to add
			 */
			VISTA_SDK_CPP_INLINE void add( EventDataSet dataSet );

			/**
			 * @brief Clear all data sets
			 */
			VISTA_SDK_CPP_INLINE void clear();

		private:
			//----------------------------------------------
			// Private members
			//----------------------------------------------

			std::vector<EventDataSet> m_dataSet;
		};

		//=====================================================================
		// Header class
		//=====================================================================

		/**
		 * @brief Time series package header with metadata
		 */
		class Header final
		{
		public:
			//----------------------------------------------
			// Construction
			//----------------------------------------------

			/** @brief Default constructor */
			Header() = delete;

			/**
			 * @brief Constructs Header with required fields
			 * @param shipId Ship identifier
			 * @param timeRange Time range for the data
			 * @param author Author information
			 */
			VISTA_SDK_CPP_INLINE Header(
				ShipId shipId,
				timeseries::TimeRange timeRange,
				std::optional<std::string> author );

			/**
			 * @brief Constructs Header with all fields
			 * @param shipId Ship identifier
			 * @param timeRange Optional time range for the data
			 * @param dateCreated Optional creation timestamp
			 * @param dateModified Optional modification timestamp
			 * @param author Optional author information
			 * @param systemConfiguration Optional system configuration references
			 * @param customHeaders Optional custom headers dictionary
			 */
			VISTA_SDK_CPP_INLINE Header(
				ShipId shipId,
				std::optional<TimeRange> timeRange,
				std::optional<nfx::time::DateTimeOffset> dateCreated,
				std::optional<nfx::time::DateTimeOffset> dateModified,
				std::optional<std::string> author,
				std::optional<std::vector<ConfigurationReference>> systemConfiguration,
				std::optional<nfx::containers::StringMap<Value>> customHeaders );

			/** @brief Copy constructor */
			Header( const Header& ) = default;

			/** @brief Move constructor */
			Header( Header&& ) noexcept = default;

			/** @brief Destructor */
			~Header() = default;

			//----------------------------------------------
			// Assignment operators
			//----------------------------------------------

			/** @brief Copy assignment operator */
			Header& operator=( const Header& ) = default;

			/** @brief Move assignment operator */
			Header& operator=( Header&& ) noexcept = default;

			//----------------------------------------------
			// Property access
			//----------------------------------------------

			/**
			 * @brief Get ship ID
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const ShipId& shipId() const noexcept;

			/**
			 * @brief Get time range
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<TimeRange>& timeRange() const noexcept;

			/**
			 * @brief Get date created
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<nfx::time::DateTimeOffset>& dateCreated() const noexcept;

			/**
			 * @brief Get date modified
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<nfx::time::DateTimeOffset>& dateModified() const noexcept;

			/**
			 * @brief Get author
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<std::string>& author() const noexcept;

			/**
			 * @brief Get system configuration
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<std::vector<ConfigurationReference>>& systemConfiguration() const noexcept;

			/**
			 * @brief Get custom headers
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<nfx::containers::StringMap<Value>>& customHeaders() const noexcept;

			//----------------------------------------------
			// Setters
			//----------------------------------------------

			/**
			 * @brief Set ship ID
			 * @param shipId New ship identifier
			 */
			VISTA_SDK_CPP_INLINE void setShipId( ShipId shipId );

			/**
			 * @brief Set time range
			 * @param timeRange New optional time range
			 */
			VISTA_SDK_CPP_INLINE void setTimeRange( std::optional<TimeRange> timeRange );

			/**
			 * @brief Set date created
			 * @param dateCreated New optional creation timestamp
			 */
			VISTA_SDK_CPP_INLINE void setDateCreated( std::optional<nfx::time::DateTimeOffset> dateCreated );

			/**
			 * @brief Set date modified
			 * @param dateModified New optional modification timestamp
			 */
			VISTA_SDK_CPP_INLINE void setDateModified( std::optional<nfx::time::DateTimeOffset> dateModified );

			/**
			 * @brief Set author
			 * @param author New optional author string
			 */
			VISTA_SDK_CPP_INLINE void setAuthor( std::optional<std::string_view> author );

			/**
			 * @brief Set system configuration
			 * @param systemConfiguration New optional system configuration
			 */
			VISTA_SDK_CPP_INLINE void setSystemConfiguration( std::optional<std::vector<ConfigurationReference>> systemConfiguration );

			/**
			 * @brief Set custom headers
			 * @param customHeaders New optional custom headers
			 */
			VISTA_SDK_CPP_INLINE void setCustomHeaders( std::optional<nfx::containers::StringMap<Value>> customHeaders );

		private:
			//----------------------------------------------
			// Private members
			//----------------------------------------------

			ShipId m_shipId;
			std::optional<TimeRange> m_timeRange;
			std::optional<nfx::time::DateTimeOffset> m_dateCreated;
			std::optional<nfx::time::DateTimeOffset> m_dateModified;
			std::optional<std::string> m_author;
			std::optional<std::vector<ConfigurationReference>> m_systemConfiguration;
			std::optional<nfx::containers::StringMap<Value>> m_customHeaders;
		};

		//=====================================================================
		// Validation delegate type
		//=====================================================================

		/**
		 * @brief Validation function type for data validation
		 */
		using ValidateData = std::function<ValidateResult(
			const nfx::time::DateTimeOffset& timeStamp,
			const datachannel::DataChannel& dataChannel,
			const Value& value,
			const std::optional<std::string>& quality )>;

		//=====================================================================
		// TimeSeriesData class
		//=====================================================================

		/**
		 * @brief Time series data with validation capabilities
		 */
		class TimeSeriesData final
		{
		public:
			//----------------------------------------------
			// Construction
			//----------------------------------------------

			/** @brief Default constructor */
			TimeSeriesData() = delete;

			/**
			 * @brief Constructs TimeSeriesData with optional components
			 * @param dataConfiguration Optional data configuration reference
			 * @param tabularData Optional vector of tabular data sets
			 * @param eventData Optional event data collection
			 * @param customDataKinds Optional custom data kinds dictionary
			 */
			VISTA_SDK_CPP_INLINE TimeSeriesData(
				std::optional<ConfigurationReference> dataConfiguration,
				std::optional<std::vector<TabularData>> tabularData,
				std::optional<EventData> eventData,
				std::optional<nfx::containers::StringMap<Value>> customDataKinds );

			/** @brief Copy constructor */
			TimeSeriesData( const TimeSeriesData& ) = default;

			/** @brief Move constructor */
			TimeSeriesData( TimeSeriesData&& ) noexcept = default;

			/** @brief Destructor */
			~TimeSeriesData() = default;

			//----------------------------------------------
			// Assignment operators
			//----------------------------------------------

			/** @brief Copy assignment operator */
			TimeSeriesData& operator=( const TimeSeriesData& ) = default;

			/** @brief Move assignment operator */
			TimeSeriesData& operator=( TimeSeriesData&& ) noexcept = default;

			//----------------------------------------------
			// Property access
			//----------------------------------------------

			/**
			 * @brief Get data configuration
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<ConfigurationReference>& dataConfiguration() const noexcept;

			/**
			 * @brief Get tabular data
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<std::vector<TabularData>>& tabularData() const noexcept;

			/**
			 * @brief Get event data
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<EventData>& eventData() const noexcept;

			/**
			 * @brief Get custom data kinds
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<nfx::containers::StringMap<Value>>& customDataKinds() const noexcept;

			//----------------------------------------------
			// Setters
			//----------------------------------------------

			/**
			 * @brief Set data configuration
			 * @param dataConfiguration New optional data configuration
			 */
			VISTA_SDK_CPP_INLINE void setDataConfiguration( std::optional<ConfigurationReference> dataConfiguration );

			/**
			 * @brief Set tabular data
			 * @param tabularData New optional tabular data
			 */
			VISTA_SDK_CPP_INLINE void setTabularData( std::optional<std::vector<TabularData>> tabularData );

			/**
			 * @brief Set event data
			 * @param eventData New optional event data
			 */
			VISTA_SDK_CPP_INLINE void setEventData( std::optional<EventData> eventData );

			/**
			 * @brief Set custom data kinds
			 * @param customDataKinds New optional custom data kinds
			 */
			VISTA_SDK_CPP_INLINE void setCustomDataKinds( std::optional<nfx::containers::StringMap<Value>> customDataKinds );

			//----------------------------------------------
			// Validation
			//----------------------------------------------

			/**
			 * @brief Validate time series data against data channel package
			 * @param dcPackage Data channel list package for validation
			 * @param onTabularData Validation function for tabular data
			 * @param onEventData Validation function for event data
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] ValidateResult validate(
				const datachannel::DataChannelListPackage& dcPackage,
				ValidateData onTabularData,
				ValidateData onEventData ) const;

		private:
			//----------------------------------------------
			// Private members
			//----------------------------------------------

			std::optional<ConfigurationReference> m_dataConfiguration;
			std::optional<std::vector<TabularData>> m_tabularData;
			std::optional<EventData> m_eventData;
			std::optional<nfx::containers::StringMap<Value>> m_customDataKinds;
		};

		//=====================================================================
		// Package class
		//=====================================================================

		/**
		 * @brief Time series package containing header and data
		 */
		class Package final
		{
		public:
			//----------------------------------------------
			// Construction
			//----------------------------------------------

			/** @brief Default constructor */
			Package() = delete;

			/**
			 * @brief Constructs Package with required components
			 * @param header Package header with metadata
			 * @param timeSeriesData Collection of time series data
			 */
			VISTA_SDK_CPP_INLINE Package(
				std::optional<Header> header,
				std::vector<TimeSeriesData> timeSeriesData );

			/** @brief Copy constructor */
			Package( const Package& ) = default;

			/** @brief Move constructor */
			Package( Package&& ) noexcept = default;

			/** @brief Destructor */
			~Package() = default;

			//----------------------------------------------
			// Assignment operators
			//----------------------------------------------

			/** @brief Copy assignment operator */
			Package& operator=( const Package& ) = default;

			/** @brief Move assignment operator */
			Package& operator=( Package&& ) noexcept = default;

			//----------------------------------------------
			// Property access
			//----------------------------------------------

			/**
			 * @brief Get header
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::optional<Header>& header() const noexcept;

			/**
			 * @brief Get time series data
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const std::vector<TimeSeriesData>& timeSeriesData() const noexcept;

			//----------------------------------------------
			// Setters
			//----------------------------------------------

			/**
			 * @brief Set header
			 * @param header New optional package header
			 */
			VISTA_SDK_CPP_INLINE void setHeader( std::optional<Header> header );

			/**
			 * @brief Set time series data
			 * @param timeSeriesData New time series data vector
			 */
			VISTA_SDK_CPP_INLINE void setTimeSeriesData( std::vector<TimeSeriesData> timeSeriesData );

		private:
			//----------------------------------------------
			// Private members
			//----------------------------------------------

			std::optional<Header> m_header;
			std::vector<TimeSeriesData> m_timeSeriesData;
		};

		//=====================================================================
		// TimeSeriesDataPackage class
		//=====================================================================

		/**
		 * @brief Main time series data package container
		 * @details Top-level container for time series data transport
		 */
		class TimeSeriesDataPackage final
		{
		public:
			//----------------------------------------------
			// Construction
			//----------------------------------------------

			/** @brief Default constructor */
			TimeSeriesDataPackage() = delete;

			/**
			 * @brief Constructs TimeSeriesDataPackage with required package
			 * @param package Package containing header and time series data
			 */
			VISTA_SDK_CPP_INLINE explicit TimeSeriesDataPackage( Package package );

			/** @brief Copy constructor */
			TimeSeriesDataPackage( const TimeSeriesDataPackage& ) = default;

			/** @brief Move constructor */
			TimeSeriesDataPackage( TimeSeriesDataPackage&& ) noexcept = default;

			/** @brief Destructor */
			~TimeSeriesDataPackage() = default;

			//----------------------------------------------
			// Assignment operators
			//----------------------------------------------

			/** @brief Copy assignment operator */
			TimeSeriesDataPackage& operator=( const TimeSeriesDataPackage& ) = default;

			/** @brief Move assignment operator */
			TimeSeriesDataPackage& operator=( TimeSeriesDataPackage&& ) noexcept = default;

			//----------------------------------------------
			// Property access
			//----------------------------------------------

			/**
			 * @brief Get package
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE const Package& package() const noexcept;

			/**
			 * @brief Get package (mutable)
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] [[nodiscard]] VISTA_SDK_CPP_INLINE Package& package() noexcept;

			//----------------------------------------------
			// Setters
			//----------------------------------------------

			/**
			 * @brief Set package
			 * @param package New package
			 */
			VISTA_SDK_CPP_INLINE void setPackage( Package package );

		private:
			//----------------------------------------------
			// Private members
			//----------------------------------------------

			Package m_package;
		};
	}
}

#include "TimeSeriesData.inl"
