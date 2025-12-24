/**
 * @file LocalId.h
 * @brief MQTT-compatible LocalId implementation with inheritance from main LocalId
 */

#pragma once

#include "dnv/vista/sdk/LocalId.h"
#include "dnv/vista/sdk/LocalIdBuilder.h"

namespace dnv::vista::sdk::mqtt
{
	/**
	 * @brief MQTT-compatible LocalId implementation inheriting from main LocalId.
	 * @details This class extends the base LocalId with MQTT-specific formatting capabilities:
	 *          - Underscores instead of slashes in GMOD paths for topic compatibility
	 *          - No leading slash to match MQTT topic conventions
	 *          - No "meta/" prefix section for cleaner IoT topics
	 *          - Placeholder handling for missing components
	 */
	class LocalId final : public sdk::LocalId
	{
	public:
		/**
		 * @brief Constructs MQTT LocalId from validated LocalIdBuilder.
		 * @param[in] builder Valid LocalIdBuilder instance.
		 * @throws std::invalid_argument If builder is invalid or empty.
		 */
		explicit LocalId( const sdk::LocalIdBuilder& builder );

		/** @brief Default constructor */
		LocalId() = delete;

		/**
		 * @brief Copy constructor
		 * @param other The MQTT LocalId to copy from
		 */
		LocalId( const LocalId& other ) = default;

		/**
		 * @brief Move constructor
		 * @param other The MQTT LocalId to move from
		 */
		LocalId( LocalId&& other ) noexcept = default;

		/** @brief Destructor */
		virtual ~LocalId() = default;

		/**
		 * @brief Copy assignment operator
		 */
		LocalId& operator=( const LocalId& ) = delete;

		/**
		 * @brief Move assignment operator
		 * @param other The MQTT LocalId to move from
		 * @return Reference to this LocalId
		 */
		LocalId& operator=( LocalId&& other ) noexcept = default;

		/**
		 * @brief Converts LocalId to MQTT-compatible topic string.
		 * @details Provides MQTT-specific formatting:
		 *          - No leading slash
		 *          - Underscores instead of slashes in paths
		 *          - No "meta/" section
		 *          - Placeholders for missing components
		 * @return MQTT-compatible Local ID topic string.
		 */
		[[nodiscard]] virtual std::string toString() const override;
	};
} // namespace dnv::vista::sdk::mqtt
