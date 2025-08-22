/**
 * @file ShipId.h
 * @brief Ship identifier implementation using discriminated union pattern
 * @details Represents either an IMO number or other string identifier for vessels
 *          following the pattern established in the C# Vista SDK
 */

#pragma once

#include "dnv/vista/sdk/config/Platform.h"

#include "dnv/vista/sdk/ImoNumber.h"
#include "dnv/vista/sdk/Exceptions.h"

namespace dnv::vista::sdk::transport
{
	//=====================================================================
	// ShipId class
	//=====================================================================

	/**
	 * @brief Ship identifier using discriminated union pattern
	 * @details Immutable structure that can represent either an IMO number
	 *          or other string-based ship identifier. Uses a tag-based
	 *          discriminated union pattern similar to std::variant but with
	 *          explicit member access for performance.
	 */
	class ShipId final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructs ShipId from IMO number
		 * @param imoNumber Valid IMO number
		 */
		explicit ShipId( ImoNumber imoNumber ) noexcept;

		/**
		 * @brief Constructs ShipId from string_view identifier
		 * @param otherId String-based ship identifier
		 * @throws std::invalid_argument If otherId is empty
		 */
		explicit ShipId( std::string_view otherId );

		/** @brief Default constructor */
		ShipId() = delete;

		/** @brief Copy constructor */
		ShipId( const ShipId& ) = default;

		/** @brief Move constructor */
		ShipId( ShipId&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~ShipId() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		ShipId& operator=( const ShipId& ) = default;

		/** @brief Move assignment operator */
		ShipId& operator=( ShipId&& ) noexcept = default;

		//----------------------------------------------
		// Equality operators
		//----------------------------------------------

		/**
		 * @brief Equality comparison operator
		 * @param other The ShipId to compare with
		 * @return True if both ShipIds represent the same identifier
		 */
		[[nodiscard]] inline bool operator==( const ShipId& other ) const noexcept;

		/**
		 * @brief Inequality comparison operator
		 * @param other The ShipId to compare with
		 * @return True if ShipIds represent different identifiers
		 */
		[[nodiscard]] inline bool operator!=( const ShipId& other ) const noexcept;

		//----------------------------------------------
		// Type checking
		//----------------------------------------------

		/**
		 * @brief Check if this ShipId contains an IMO number
		 * @return True if this contains an IMO number
		 */
		[[nodiscard]] inline bool isImoNumber() const noexcept;

		/**
		 * @brief Check if this ShipId contains an other identifier
		 * @return True if this contains an other identifier
		 */
		[[nodiscard]] inline bool isOtherId() const noexcept;

		//----------------------------------------------
		// Value access
		//----------------------------------------------

		/**
		 * @brief Get IMO number if present
		 * @return Optional containing IMO number if present, empty otherwise
		 */
		[[nodiscard]] inline std::optional<ImoNumber> imoNumber() const noexcept;

		/**
		 * @brief Get other identifier if present
		 * @return Optional containing other identifier if present, empty otherwise
		 */
		[[nodiscard]] inline std::optional<std::string_view> otherId() const noexcept;

		//----------------------------------------------
		// Pattern matching
		//----------------------------------------------

		/**
		 * @brief Pattern match on ShipId content
		 * @tparam T Return type
		 * @param onImoNumber Function to call if ShipId contains IMO number
		 * @param onOtherId Function to call if ShipId contains other identifier
		 * @return Result of the matched function
		 */
		template <typename T>
		[[nodiscard]] T matchOn(
			std::function<T( const ImoNumber& )> onImoNumber,
			std::function<T( std::string_view )> onOtherId ) const;

		/**
		 * @brief Pattern switch on ShipId content (void version)
		 * @param onImoNumber Function to call if ShipId contains IMO number
		 * @param onOtherId Function to call if ShipId contains other identifier
		 */
		void switchOn(
			std::function<void( const ImoNumber& )> onImoNumber,
			std::function<void( std::string_view )> onOtherId ) const;

		//----------------------------------------------
		// String conversion
		//----------------------------------------------

		/**
		 * @brief Convert to string representation
		 * @return String representation of the ship identifier
		 */
		[[nodiscard]] std::string toString() const;

		//----------------------------------------------
		// Static factory methods
		//----------------------------------------------

		/**
		 * @brief Parse ShipId from string representation
		 * @param value String to parse (may include "IMO" prefix for IMO numbers)
		 * @return Parsed ShipId
		 * @throws std::invalid_argument If value is empty
		 */
		[[nodiscard]] static ShipId parse( std::string_view value );

	private:
		//----------------------------------------------
		// Private members
		//----------------------------------------------

		/** @brief Tag to identify which union member is active (1=IMO, 2=other) */
		int m_tag;

		/** @brief IMO number storage (active when m_tag == 1) */
		std::optional<ImoNumber> m_imoNumber;

		/** @brief Other identifier storage (active when m_tag == 2) */
		std::optional<std::string> m_otherId;
	};
}

#include "ShipId.inl"
