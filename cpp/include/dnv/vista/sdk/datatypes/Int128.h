/**
 * @file Int128.h
 * @brief Cross-platform 128-bit integer arithmetic type
 * @details Provides portable 128-bit signed integer operations with
 *          native __int128 on GCC/Clang and manual implementation on MSVC
 */

#pragma once

#include "dnv/vista/sdk/config/Platform.h"

namespace dnv::vista::sdk::datatypes
{
	//=====================================================================
	// Int128 class
	//=====================================================================

	/**
	 * @brief Cross-platform 128-bit signed integer type
	 * @details Implements portable 128-bit signed integer arithmetic with:
	 *          - Native __int128 support on GCC/Clang for maximum performance
	 *          - Manual implementation on MSVC using two 64-bit words
	 *          - Full arithmetic and comparison operator support
	 *          - Optimized for decimal arithmetic operations
	 *
	 *          Performance characteristics:
	 *          - GCC/Clang: Direct hardware 128-bit operations where available
	 *          - MSVC: Optimized multi-precision arithmetic using 64-bit operations
	 */
	class Int128 final
	{
		friend class Decimal128;

	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Default constructor
		 */
		constexpr Int128() noexcept;

		/**
		 * @brief Construct from two 64-bit words
		 * @param low Lower 64 bits
		 * @param high Upper 64 bits
		 */
		constexpr Int128( std::uint64_t low, std::uint64_t high ) noexcept;

		/**
		 * @brief Construct from unsigned 64-bit integer
		 * @param val 64-bit unsigned value
		 */
		explicit constexpr Int128( std::uint64_t val ) noexcept;

		/**
		 * @brief Construct from signed 64-bit integer
		 * @param val 64-bit signed value
		 */
		explicit constexpr Int128( std::int64_t val ) noexcept;

		/**
		 * @brief Construct from unsigned 32-bit integer
		 * @param val 32-bit unsigned value
		 */
		explicit constexpr Int128( std::uint32_t val ) noexcept;

		/**
		 * @brief Construct from signed 32-bit integer
		 * @param val 32-bit signed value
		 */
		explicit constexpr Int128( int val ) noexcept;

#if VISTA_SDK_CPP_HAS_INT128
		/**
		 * @brief Construct from native __int128 (GCC/Clang only)
		 * @param val Native 128-bit value
		 */
		explicit constexpr Int128( VISTA_SDK_CPP_INT128 val ) noexcept;
#endif

		/**
		 * @brief Copy constructor
		 */
		constexpr Int128( const Int128& other ) noexcept = default;

		/**
		 * @brief Move constructor
		 */
		constexpr Int128( Int128&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~Int128() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 */
		constexpr Int128& operator=( const Int128& other ) noexcept = default;

		/**
		 * @brief Move assignment operator
		 */
		constexpr Int128& operator=( Int128&& other ) noexcept = default;

		//----------------------------------------------
		// Arithmetic operators
		//----------------------------------------------

		/**
		 * @brief Addition operator
		 * @param other Right operand
		 * @return Sum of this and other
		 */
		Int128 operator+( const Int128& other ) const noexcept;

		/**
		 * @brief Subtraction operator
		 * @param other Right operand
		 * @return Difference of this and other
		 */
		Int128 operator-( const Int128& other ) const noexcept;

		/**
		 * @brief Multiplication operator
		 * @param other Right operand
		 * @return Product of this and other
		 */
		Int128 operator*( const Int128& other ) const noexcept;

		/**
		 * @brief Division operator
		 * @param other Divisor
		 * @return Result of division
		 * @throws std::overflow_error if divisor is zero
		 */
		Int128 operator/( const Int128& other ) const;

		/**
		 * @brief Modulo operator
		 * @param other Divisor
		 * @return Remainder of division
		 * @throws std::overflow_error if divisor is zero
		 */
		Int128 operator%( const Int128& other ) const;

		/**
		 * @brief Unary minus operator
		 * @return Negated value
		 */
		Int128 operator-() const noexcept;

		//----------------------------------------------
		// Comparison operators
		//----------------------------------------------

		/**
		 * @brief Equality operator
		 * @param other Right operand
		 * @return true if values are equal
		 */
		bool operator==( const Int128& other ) const noexcept;

		/**
		 * @brief Inequality operator
		 * @param other Right operand
		 * @return true if values are not equal
		 */
		bool operator!=( const Int128& other ) const noexcept;

		/**
		 * @brief Less than operator
		 * @param other Right operand
		 * @return true if this is less than other
		 */
		bool operator<( const Int128& other ) const noexcept;

		/**
		 * @brief Less than or equal operator
		 * @param other Right operand
		 * @return true if this is less than or equal to other
		 */
		bool operator<=( const Int128& other ) const noexcept;

		/**
		 * @brief Greater than operator
		 * @param other Right operand
		 * @return true if this is greater than other
		 */
		bool operator>( const Int128& other ) const noexcept;

		/**
		 * @brief Greater than or equal operator
		 * @param other Right operand
		 * @return true if this is greater than or equal to other
		 */
		bool operator>=( const Int128& other ) const noexcept;

		//----------------------------------------------
		// State checking
		//----------------------------------------------

		/**
		 * @brief Check if value is zero
		 * @return true if zero
		 */
		[[nodiscard]] bool isZero() const noexcept;

		/**
		 * @brief Check if value is negative
		 * @return true if negative
		 */
		[[nodiscard]] bool isNegative() const noexcept;

		//----------------------------------------------
		// Mathematical operations
		//----------------------------------------------

		/**
		 * @brief Get absolute value
		 * @return Absolute value of the integer
		 */
		[[nodiscard]] Int128 abs() const noexcept;

		//----------------------------------------------
		// Access operations
		//----------------------------------------------

		/**
		 * @brief Get lower 64 bits
		 * @return Lower 64 bits as unsigned integer
		 */
		[[nodiscard]] std::uint64_t toLow() const noexcept;

		/**
		 * @brief Get upper 64 bits
		 * @return Upper 64 bits as unsigned integer
		 */
		[[nodiscard]] std::uint64_t toHigh() const noexcept;

	private:
		//----------------------------------------------
		// Platform-specific storage
		//----------------------------------------------

#if VISTA_SDK_CPP_HAS_INT128
		/** @brief Native 128-bit integer storage (GCC/Clang) */
		VISTA_SDK_CPP_INT128 m_value;
#else
		/** @brief Manual 128-bit storage using two 64-bit words (MSVC) */
		std::uint64_t m_lower64bits;
		std::uint64_t m_upper64bits;
#endif
	};

	//=====================================================================
	// Stream operators
	//=====================================================================

	/**
	 * @brief Output stream operator
	 * @param os Output stream
	 * @param value Int128 value to output
	 * @return Reference to output stream
	 */
	std::ostream& operator<<( std::ostream& os, const Int128& value );
}

#include "Int128.inl"
