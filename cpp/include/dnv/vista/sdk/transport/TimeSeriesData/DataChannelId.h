/**
 * @file DataChannelId.h
 * @brief Data channel identifier implementation using discriminated union pattern
 * @details Represents either a LocalId or a short string identifier for data channels
 */

#pragma once

#include "dnv/vista/sdk/LocalId.h"

namespace dnv::vista::sdk::transport
{
	//=====================================================================
	// DataChannelId class
	//=====================================================================

	/**
	 * @brief Data channel identifier using discriminated union pattern
	 * @details Immutable structure that can represent either a LocalId
	 *          or a short string-based identifier. Uses a tag-based
	 *          discriminated union pattern similar to std::variant but with
	 *          explicit member access for performance.
	 */
	class DataChannelId final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs DataChannelId from LocalId
		 * @param localId Valid LocalId
		 */
		inline explicit DataChannelId( const LocalId& localId ) noexcept;

		/**
		 * @brief Constructs DataChannelId from string_view identifier
		 * @param shortId String-based channel identifier
		 */
		inline explicit DataChannelId( std::string_view shortId ) noexcept;

		/** @brief Default constructor */
		DataChannelId() = delete;

		/** @brief Copy constructor */
		DataChannelId( const DataChannelId& ) = default;

		/** @brief Move constructor */
		DataChannelId( DataChannelId&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~DataChannelId() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		DataChannelId& operator=( const DataChannelId& ) = delete;

		/** @brief Move assignment operator */
		DataChannelId& operator=( DataChannelId&& ) noexcept = default;

		//----------------------------------------------
		// Equality operators
		//----------------------------------------------

		/**
		 * @brief Equality comparison operator
		 * @param other The DataChannelId to compare with
		 * @return True if both DataChannelIds represent the same identifier
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator==( const DataChannelId& other ) const noexcept;

		/**
		 * @brief Inequality comparison operator
		 * @param other The DataChannelId to compare with
		 * @return True if DataChannelIds represent different identifiers
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool operator!=( const DataChannelId& other ) const noexcept;

		//----------------------------------------------
		// Type checking
		//----------------------------------------------

		/**
		 * @brief Check if this DataChannelId contains a LocalId
		 * @return True if this contains a LocalId
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isLocalId() const noexcept;

		/**
		 * @brief Check if this DataChannelId contains a short ID
		 * @return True if this contains a short ID
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isShortId() const noexcept;

		//----------------------------------------------
		// Value access
		//----------------------------------------------

		/**
		 * @brief Get LocalId if present
		 * @return Optional containing LocalId if present, empty otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::optional<LocalId> localId() const noexcept;

		/**
		 * @brief Get short identifier if present
		 * @return Optional containing short identifier if present, empty otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::optional<std::string_view> shortId() const noexcept;

		//----------------------------------------------
		// Pattern matching
		//----------------------------------------------

		/**
		 * @brief Pattern match on DataChannelId content
		 * @tparam T Return type
		 * @tparam LocalIdFunc Callable type for LocalId case
		 * @tparam ShortIdFunc Callable type for short identifier case
		 * @param onLocalId Function to call if DataChannelId contains LocalId
		 * @param onShortId Function to call if DataChannelId contains short identifier
		 * @return Result of the matched function
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		template <typename T, typename LocalIdFunc, typename ShortIdFunc>
		[[nodiscard]] inline T matchOn( LocalIdFunc&& onLocalId, ShortIdFunc&& onShortId ) const;

		/**
		 * @brief Pattern switch on DataChannelId content
		 * @tparam LocalIdFunc Callable type for LocalId case
		 * @tparam ShortIdFunc Callable type for short identifier case
		 * @param onLocalId Function to call if DataChannelId contains LocalId
		 * @param onShortId Function to call if DataChannelId contains short identifier
		 */
		template <typename LocalIdFunc, typename ShortIdFunc>
		inline void switchOn( LocalIdFunc&& onLocalId, ShortIdFunc&& onShortId ) const;

		//----------------------------------------------
		// String conversion
		//----------------------------------------------

		/**
		 * @brief Convert to string representation
		 * @return String representation of the data channel identifier
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] std::string toString() const;

		//----------------------------------------------
		// Static factory methods
		//----------------------------------------------

		/**
		 * @brief Parse DataChannelId from string representation
		 * @param value String to parse (tries LocalId first, then uses as short ID)
		 * @return Parsed DataChannelId
		 * @throws std::invalid_argument If value is empty
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static DataChannelId parse( std::string_view value );

	private:
	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		/** @brief Tag enumeration for discriminated union */
		enum class Tag : bool
		{
			LocalId = 0,
			ShortId
		};

		/** @brief Tag to identify which union member is active */
		Tag m_tag;

		/** @brief LocalId storage (active when m_tag == Tag::LocalId) */
		std::optional<LocalId> m_localId;

		/** @brief Short identifier storage (active when m_tag == Tag::ShortId) */
		std::optional<std::string> m_shortId;
	};
}

#include "DataChannelId.inl"
