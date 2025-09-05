/**
 * @file TimeSeriesData.cpp
 * @brief Implementation of TimeSeriesData validation and complex methods
 */

#include <nfx/string/StringBuilderPool.h>

#include "dnv/vista/sdk/transport/TimeSeriesData/TimeSeriesData.h"
#include "dnv/vista/sdk/transport/TimeSeriesData/DataChannelId.h"
#include "dnv/vista/sdk/transport/DataChannel/DataChannel.h"
#include "dnv/vista/sdk/Results.h"

namespace dnv::vista::sdk::transport::timeseries
{
	//=====================================================================
	// TimeRange class
	//=====================================================================

	//----------------------------------------------
	// Setters
	//----------------------------------------------

	void TimeRange::setStart( nfx::time::DateTimeOffset start )
	{
		if ( start > m_end )
		{
			throw std::invalid_argument{ "Start time must be before or equal to end time" };
		}

		m_start = std::move( start );
	}

	void TimeRange::setEnd( nfx::time::DateTimeOffset end )
	{
		if ( end < m_start )
		{
			throw std::invalid_argument{ "End time must be after or equal to start time" };
		}

		m_end = std::move( end );
	}

	//=====================================================================
	// TabularData class
	//=====================================================================

	//----------------------------------------------
	// Validation
	//----------------------------------------------

	ValidateResult TabularData::validate() const
	{
		return Validate( *this );
	}

	ValidateResult TabularData::Validate( const TabularData& table )
	{
		/* Ensure data channels are provided */
		if ( table.m_dataChannelIds.empty() )
		{
			return ValidateResult::Invalid{ { "Tabular data has no data channels" } };
		}

		/* Ensure data sets are provided */
		if ( table.m_dataSets.empty() )
		{
			return ValidateResult::Invalid{ { "Tabular data has no data" } };
		}

		/* Validate that all data sets have the correct number of values */
		const auto expectedChannelCount = table.m_dataChannelIds.size();
		for ( size_t i = 0; i < table.m_dataSets.size(); ++i )
		{
			const auto& dataSet = table.m_dataSets[i];
			if ( dataSet.value().size() != expectedChannelCount )
			{
				auto lease = nfx::string::StringBuilderPool::lease();
				auto builder = lease.builder();
				builder.append( "Tabular data set " );
				builder.append( std::to_string( i ) );
				builder.append( " expects " );
				builder.append( std::to_string( expectedChannelCount ) );
				builder.append( " values, but " );
				builder.append( std::to_string( dataSet.value().size() ) );
				builder.append( " values are provided" );

				return ValidateResult::Invalid{ { lease.toString() } };
			}
		}

		return ValidateResult::Ok{};
	}

	//=====================================================================
	// TimeSeriesData class
	//=====================================================================

	//----------------------------------------------
	// Validation
	//----------------------------------------------

	ValidateResult TimeSeriesData::validate(
		const datachannel::DataChannelListPackage& dcPackage,
		ValidateData onTabularData,
		ValidateData onEventData ) const
	{
		/* Check data configuration matches */
		if ( m_dataConfiguration.has_value() )
		{
			if ( dcPackage.package().header().dataChannelListId().id() != m_dataConfiguration->id() )
			{
				return ValidateResult::Invalid{ { "DataConfiguration Id does not match DataChannelList Id" } };
			}
		}

		/* Ensure we have some data */
		const bool hasTabularData = m_tabularData.has_value() && !m_tabularData->empty();
		const bool hasEventData = m_eventData.has_value() && !m_eventData->dataSet().empty();

		if ( !hasTabularData && !hasEventData )
		{
			return ValidateResult::Invalid{ { "Can't ingest timeseries data without data" } };
		}

		std::vector<std::string> errors;

		/* Validate tabular data*/
		if ( hasTabularData )
		{
			for ( const auto& table : *m_tabularData )
			{
				/* Validate the table structure */
				auto tableResult = TabularData::Validate( table );
				if ( tableResult.isInvalid() )
				{
					return tableResult;
				}

				/* Validate each data set against data channels */
				for ( size_t i = 0; i < table.dataSets().size(); ++i )
				{
					const auto& dataSet = table.dataSets()[i];

					for ( size_t j = 0; j < table.dataChannelIds().size(); ++j )
					{
						const auto& dataChannelId = table.dataChannelIds()[j];

						/* Find the data channel */
						const datachannel::DataChannel* dataChannel = nullptr;

						/* Try to find by LocalId or ShortId using the data channel list */
						if ( dataChannelId.isLocalId() )
						{
							auto localIdOpt = dataChannelId.localId();
							if ( localIdOpt.has_value() )
							{
								dataChannel = dcPackage.package().dataChannelList().tryGetByLocalId( localIdOpt.value() );
							}
						}
						else if ( dataChannelId.isShortId() )
						{
							auto shortIdOpt = dataChannelId.shortId();
							if ( shortIdOpt.has_value() )
							{
								std::string shortIdStr( shortIdOpt.value() );
								dataChannel = dcPackage.package().dataChannelList().tryGetByShortId( shortIdStr );
							}
						}

						if ( !dataChannel )
						{
							auto lease = nfx::string::StringBuilderPool::lease();
							auto builder = lease.builder();
							builder.append( "Data channel not found: " );
							builder.append( dataChannelId.toString() );
							errors.push_back( lease.toString() );
							continue;
						}

						/* Validate the value format */
						Value parsedValue;
						auto formatResult = dataChannel->property().format().validateValue( dataSet.value()[j], parsedValue );
						if ( formatResult.isInvalid() )
						{
							auto lease = nfx::string::StringBuilderPool::lease();
							auto builder = lease.builder();
							builder.append( "Invalid value format: " );
							builder.append( dataSet.value()[j] );
							errors.push_back( lease.toString() );
							continue;
						}

						/* Call user validation */
						const auto* quality = ( dataSet.quality().has_value() && j < dataSet.quality()->size() )
												  ? &( *dataSet.quality() )[j]
												  : nullptr;

						std::optional<std::string> qualityOpt = quality
																	? std::optional<std::string>( *quality )
																	: std::nullopt;

						auto userResult = onTabularData( dataSet.timeStamp(), *dataChannel, parsedValue, qualityOpt );
						if ( userResult.isInvalid() )
						{
							errors.push_back( "User validation failed for data channel" );
						}
					}
				}
			}
		}

		/* Validate event data */
		if ( hasEventData )
		{
			for ( const auto& eventDataSet : m_eventData->dataSet() )
			{
				/* Find the data channel */
				const datachannel::DataChannel* dataChannel = nullptr;

				/* Try to find by LocalId or ShortId */
				if ( eventDataSet.dataChannelId().isLocalId() )
				{
					auto localIdOpt = eventDataSet.dataChannelId().localId();
					if ( localIdOpt.has_value() )
					{
						dataChannel = dcPackage.package().dataChannelList().tryGetByLocalId( localIdOpt.value() );
					}
				}
				else if ( eventDataSet.dataChannelId().isShortId() )
				{
					auto shortIdOpt = eventDataSet.dataChannelId().shortId();
					if ( shortIdOpt.has_value() )
					{
						std::string shortIdStr( shortIdOpt.value() );
						dataChannel = dcPackage.package().dataChannelList().tryGetByShortId( shortIdStr );
					}
				}

				if ( !dataChannel )
				{
					auto lease = nfx::string::StringBuilderPool::lease();
					auto builder = lease.builder();
					builder.append( "Data channel not found: " );
					builder.append( eventDataSet.dataChannelId().toString() );
					errors.push_back( lease.toString() );
					continue;
				}

				/* Validate the value format */
				Value parsedValue;
				auto formatResult = dataChannel->property().format().validateValue( eventDataSet.value(), parsedValue );
				if ( formatResult.isInvalid() )
				{
					auto lease = nfx::string::StringBuilderPool::lease();
					auto builder = lease.builder();
					builder.append( "Invalid value format: " );
					builder.append( eventDataSet.value() );
					errors.push_back( lease.toString() );

					continue;
				}

				/* Call user validation */
				auto userResult = onEventData( eventDataSet.timeStamp(), *dataChannel, parsedValue, eventDataSet.quality() );
				if ( userResult.isInvalid() )
				{
					errors.push_back( "User validation failed for event data channel" );
				}
			}
		}

		if ( !errors.empty() )
		{
			return ValidateResult::Invalid{ std::move( errors ) };
		}

		return ValidateResult::Ok{};
	}
}
