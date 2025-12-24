/**
 * @file LocalId.cpp
 * @brief MQTT-compatible LocalId implementation
 */

#include "dnv/vista/sdk/mqtt/LocalId.h"

#include "dnv/vista/sdk/GmodPath.h"
#include "dnv/vista/sdk/MetadataTag.h"
#include "dnv/vista/sdk/VisVersions.h"

namespace dnv::vista::sdk::mqtt
{
	namespace internal
	{
		/** @brief Internal separator for MQTT paths */
		static constexpr char INTERNAL_SEPARATOR = '_';

		/**
		 * @brief Appends GMOD path to builder with MQTT formatting.
		 * @param[in,out] builder StringBuilder to append to.
		 * @param[in] path GmodPath to append with underscore separators.
		 */
		static void appendPath( StringBuilder& builder, const GmodPath& path )
		{
			// Convert path to string with MQTT separator (underscore instead of slash)
			path.toString( builder, INTERNAL_SEPARATOR );
			builder.append( "/" );
		}

		/**
		 * @brief Appends primary item to builder in MQTT format.
		 * @param[in] localIdBuilder LocalIdBuilder containing the primary item.
		 * @param[in,out] builder StringBuilder to append to.
		 */
		static void appendPrimaryItem( const LocalIdBuilder& localIdBuilder, StringBuilder& builder )
		{
			appendPath( builder, localIdBuilder.primaryItem().value() );
		}

		/**
		 * @brief Appends secondary item or placeholder to builder in MQTT format.
		 * @param[in] localIdBuilder LocalIdBuilder containing the optional secondary item.
		 * @param[in,out] builder StringBuilder to append to.
		 */
		static void appendSecondaryItem( const LocalIdBuilder& localIdBuilder, StringBuilder& builder )
		{
			if ( localIdBuilder.secondaryItem().has_value() )
			{
				appendPath( builder, localIdBuilder.secondaryItem().value() );
			}
			else
			{
				// No secondary item - use placeholder
				builder.append( "_/" );
			}
		}

		/**
		 * @brief Appends metadata tag or placeholder to builder in MQTT format.
		 * @details Appends the metadata tag value if present, otherwise appends an underscore placeholder.
		 *          Always appends a trailing forward slash for MQTT topic formatting.
		 * @param[in,out] builder StringBuilder to append to.
		 * @param[in] tag Optional metadata tag to append (or placeholder if empty).
		 */
		static void appendMeta( StringBuilder& builder, const std::optional<MetadataTag>& tag )
		{
			if ( !tag.has_value() )
			{
				builder.append( "_/" );
			}
			else
			{
				tag.value().toString( builder );
			}
		}
	} // namespace internal

	LocalId::LocalId( const sdk::LocalIdBuilder& builder )
		: sdk::LocalId{ builder }
	{
	}

	std::string LocalId::toString() const
	{
		StringBuilder sb;

		// Naming rule (NO leading slash for MQTT): "dnv-v2/"
		sb.append( NamingRule );
		sb.append( "/" );

		sb.append( "vis-" );
		sb.append( VisVersions::toString( version() ) );
		sb.append( "/" );

		internal::appendPrimaryItem( m_builder, sb );

		internal::appendSecondaryItem( m_builder, sb );

		internal::appendMeta( sb, quantity() );
		internal::appendMeta( sb, content() );
		internal::appendMeta( sb, calculation() );
		internal::appendMeta( sb, state() );
		internal::appendMeta( sb, command() );
		internal::appendMeta( sb, type() );
		internal::appendMeta( sb, position() );
		internal::appendMeta( sb, detail() );

		auto result = sb.toString();
		if ( !result.empty() && result.back() == '/' )
		{
			result.pop_back();
		}

		return result;
	}
} // namespace dnv::vista::sdk::mqtt
