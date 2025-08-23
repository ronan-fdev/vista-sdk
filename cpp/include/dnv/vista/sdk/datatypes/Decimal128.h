/**
 * @file Decimal128.h
 * @brief Cross-platform high-precision decimal arithmetic type
 * @details Provides 128-bit decimal arithmetic with 28-29 significant digits,
 *          exact decimal arithmetic with no floating-point rounding errors.
 *
 *          IEEE 754-2008 Input Compatibility:
 *          - Construction from double uses IEEE 754-2008 std::isnan/std::isinf functions
 *          - Preserves IEEE 754 binary64 precision limits (~15-17 digits)
 *          - NaN and Infinity from double are converted to zero
 *          - String construction provides exact decimal precision (up to 28 digits)
 *
 * @note This is NOT IEEE 754 decimal128 arithmetic - it implements
 *       exact decimal arithmetic without floating-point rounding errors.
 *
 * @note Design inspired by .NET System.Decimal semantics
 *       (96-bit mantissa + scale, 28–29 digits, banker's rounding) to match
 *       the original C# SDK API.
 */

#pragma once

#include "dnv/vista/sdk/config/Platform.h"

#include "Int128.h"

namespace dnv::vista::sdk::datatypes
{
	//=====================================================================
	// Decimal128 class
	//=====================================================================

	/**
	 * @brief Cross-platform 128-bit high-precision decimal type
	 * @details Implements exact decimal arithmetic with:
	 *          - 128-bit storage (96-bit mantissa + 32-bit scale/sign)
	 *          - 28-29 significant decimal digits
	 *          - Range: ±1.0 × 10^-28 to ±7.9228162514264337593543950335 × 10^28
	 *          - Exact decimal arithmetic (no floating-point rounding errors)
	 *
	 *          IEEE 754-2008 Input Compatibility:
	 *          - Double constructor uses IEEE 754-2008 std::isnan/std::isinf functions
	 *          - Respects IEEE 754 binary64 precision limits (~15-17 digits)
	 *          - NaN and Infinity from double input are converted to zero
	 *          - No internal NaN/Infinity states (division by zero throws std::overflow_error)
	 *          - Use string constructor for values requiring exact decimal precision
	 *
	 *          Note: This implements exact decimal arithmetic, NOT IEEE 754 decimal128.
	 */
	class Decimal128 final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Default constructor (zero value)
		 */
		constexpr Decimal128() noexcept;

		/**
		 * @brief Construct from double with IEEE 754-2008 input compatibility
		 * @param value Double value to convert
		 * @note Limited to IEEE 754 binary64 precision (~15-17 significant digits).
		 *       Uses IEEE 754-2008 std::isnan/std::isinf for special value detection.
		 *       NaN and Infinity values are converted to zero.
		 *       For exact precision, use string constructor instead.
		 */
		explicit Decimal128( double value ) noexcept;

		/**
		 * @brief Construct from integer
		 * @param value Integer value
		 */
		explicit Decimal128( std::int32_t value ) noexcept;
		explicit Decimal128( std::int64_t value ) noexcept;
		explicit Decimal128( std::uint32_t value ) noexcept;
		explicit Decimal128( std::uint64_t value ) noexcept;

		/**
		 * @brief Construct from string (exact parsing)
		 * @param str String representation (e.g., "123.456")
		 * @throws std::invalid_argument if string is not a valid decimal
		 */
		explicit Decimal128( std::string_view str );

		/**
		 * @brief Copy constructor
		 */
		Decimal128( const Decimal128& other ) noexcept = default;

		/**
		 * @brief Move constructor
		 */
		Decimal128( Decimal128&& other ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~Decimal128() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/**
		 * @brief Copy assignment operator
		 */
		Decimal128& operator=( const Decimal128& other ) noexcept = default;

		/**
		 * @brief Move assignment operator
		 */
		Decimal128& operator=( Decimal128&& other ) noexcept = default;

		//----------------------------------------------
		// Decimal constants
		//----------------------------------------------

		/**
		 * @brief Zero value constant
		 * @return Decimal representing zero
		 */
		[[nodiscard]] static constexpr Decimal128 zero() noexcept;

		/**
		 * @brief One value constant
		 * @return Decimal representing one
		 */
		[[nodiscard]] static constexpr Decimal128 one() noexcept;

		/**
		 * @brief Minimum finite value constant
		 * @return Smallest representable positive decimal
		 */
		[[nodiscard]] static constexpr Decimal128 minValue() noexcept;

		/**
		 * @brief Maximum finite value constant
		 * @return Largest representable decimal
		 */
		[[nodiscard]] static constexpr Decimal128 maxValue() noexcept;

		//----------------------------------------------
		// Static mathematical operations
		//----------------------------------------------

		/**
		 * @brief Remove fractional part
		 * @param value Decimal to truncate
		 * @return Decimal with fractional part removed
		 */
		[[nodiscard]] static Decimal128 truncate( const Decimal128& value ) noexcept;

		/**
		 * @brief Round down to nearest integer
		 * @param value Decimal to floor
		 * @return Largest integer less than or equal to value
		 */
		[[nodiscard]] static Decimal128 floor( const Decimal128& value ) noexcept;

		/**
		 * @brief Round up to nearest integer
		 * @param value Decimal to ceiling
		 * @return Smallest integer greater than or equal to value
		 */
		[[nodiscard]] static Decimal128 ceiling( const Decimal128& value ) noexcept;

		/**
		 * @brief Round to nearest integer
		 * @param value Decimal to round
		 * @return Rounded decimal value
		 */
		[[nodiscard]] static Decimal128 round( const Decimal128& value ) noexcept;

		/**
		 * @brief Round to specified decimal places
		 * @param value Decimal to round
		 * @param decimalsPlacesCount Number of decimal places to round to
		 * @return Rounded decimal value
		 */
		[[nodiscard]] static Decimal128 round( const Decimal128& value, std::int32_t decimalsPlacesCount ) noexcept;

		/**
		 * @brief Get absolute value
		 * @param value Decimal to get absolute value of
		 * @return Absolute value of the decimal
		 */
		[[nodiscard]] static Decimal128 abs( const Decimal128& value ) noexcept;

		//----------------------------------------------
		// Arithmetic operators
		//----------------------------------------------

		Decimal128 operator+( const Decimal128& other ) const;
		Decimal128 operator-( const Decimal128& other ) const;
		Decimal128 operator*( const Decimal128& other ) const;

		/**
		 * @brief Division operator
		 * @param other Divisor
		 * @return Result of division
		 * @throws std::overflow_error if divisor is zero (no NaN/Infinity representation)
		 */
		Decimal128 operator/( const Decimal128& other ) const;

		Decimal128& operator+=( const Decimal128& other );
		Decimal128& operator-=( const Decimal128& other );
		Decimal128& operator*=( const Decimal128& other );

		/**
		 * @brief Division assignment operator
		 * @param other Divisor
		 * @return Reference to this after division
		 * @throws std::overflow_error if divisor is zero (no NaN/Infinity representation)
		 */
		Decimal128& operator/=( const Decimal128& other );

		Decimal128 operator-() const noexcept;

		//----------------------------------------------
		// Comparison operators
		//----------------------------------------------

		bool operator==( const Decimal128& other ) const noexcept;
		bool operator!=( const Decimal128& other ) const noexcept;
		bool operator<( const Decimal128& other ) const noexcept;
		bool operator<=( const Decimal128& other ) const noexcept;
		bool operator>( const Decimal128& other ) const noexcept;
		bool operator>=( const Decimal128& other ) const noexcept;

		//----------------------------------------------
		// String parsing and conversion
		//----------------------------------------------

		/**
		 * @brief Parse high-precision decimal from string
		 * @param str String representation of decimal number (e.g., "123.456", "-0.001", "999999999999999999999999999.999")
		 * @return Parsed Decimal128 value with exact precision
		 * @throws std::invalid_argument if string format is invalid or represents value outside Decimal128 range
		 * @note Provides exact decimal parsing up to 28-29 significant digits without floating-point rounding errors
		 */
		[[nodiscard]] static Decimal128 parse( std::string_view str );

		/**
		 * @brief Parse string to decimal with error handling
		 * @param str String to parse
		 * @param result Output decimal value
		 * @return true if parsing succeeded, false otherwise
		 */
		[[nodiscard]] static bool tryParse( std::string_view str, Decimal128& result ) noexcept;

		//----------------------------------------------
		// Type conversion
		//----------------------------------------------

		/**
		 * @brief Convert to double (may lose precision)
		 * @return Double representation
		 */
		[[nodiscard]] double toDouble() const noexcept;

		/**
		 * @brief Convert to string with exact precision
		 * @return String representation
		 */
		[[nodiscard]] std::string toString() const;

		/**
		 * @brief Get internal 32-bit representation
		 * @return Array of 4 32-bit integers representing the decimal
		 */
		[[nodiscard]] std::array<std::int32_t, 4> toBits() const noexcept;

		//----------------------------------------------
		// Property accessors
		//----------------------------------------------

		/**
		 * @brief Get decimal scale (number of decimal places)
		 * @return Scale value (0-28)
		 */
		[[nodiscard]] std::uint8_t scale() const noexcept;

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
		 * @brief Remove fractional part
		 * @return Decimal with fractional part removed
		 */
		[[nodiscard]] Decimal128 truncate() const noexcept;

		/**
		 * @brief Round down to nearest integer
		 * @return Largest integer less than or equal to value
		 */
		[[nodiscard]] Decimal128 floor() const noexcept;

		/**
		 * @brief Round up to nearest integer
		 * @return Smallest integer greater than or equal to value
		 */
		[[nodiscard]] Decimal128 ceiling() const noexcept;

		/**
		 * @brief Round to nearest integer
		 * @return Rounded decimal value
		 */
		[[nodiscard]] Decimal128 round() const noexcept;

		/**
		 * @brief Round to specified decimal places
		 * @param decimalsPlacesCount Number of decimal places to round to
		 * @return Rounded decimal value
		 */
		[[nodiscard]] Decimal128 round( std::int32_t decimalsPlacesCount ) const noexcept;

		/**
		 * @brief Get absolute value
		 * @return Absolute value of the decimal
		 */
		[[nodiscard]] Decimal128 abs() const noexcept;

	private:
		//----------------------------------------------
		// Internal representation
		//----------------------------------------------

		/** @brief 96-bit mantissa (3 x 32-bit) */
		std::array<std::uint32_t, 3> m_mantissa;

		/** @brief Scale (bits 16-23) + Sign (bit 31) */
		std::uint32_t m_flags;

		//----------------------------------------------
		// Internal helper methods
		//----------------------------------------------

		/**
		 * @brief Normalize decimal (remove trailing zeros, adjust scale)
		 */
		void normalize() noexcept;

		/**
		 * @brief Scale values to same decimal places for arithmetic
		 * @param other Other decimal to align with
		 * @return Pair of aligned mantissas and common scale
		 */
		std::pair<Int128, Int128> alignScale( const Decimal128& other ) const;

		/**
		 * @brief Convert mantissa array to Int128
		 * @return 128-bit representation of mantissa
		 */
		[[nodiscard]] Int128 getMantissa() const noexcept;

		/**
		 * @brief Set mantissa from Int128
		 * @param value 128-bit mantissa value
		 */
		void setMantissa( const Int128& value ) noexcept;

		/**
		 * @brief Multiply by power of 10 (for scaling)
		 * @param power Power of 10 to multiply by
		 */
		void multiplyByPowerOf10( std::uint8_t power );

		/**
		 * @brief Divide by power of 10 (for scaling)
		 * @param power Power of 10 to divide by
		 */
		void divideByPowerOf10( std::uint8_t power );
	};

	//=====================================================================
	// Stream operators
	//=====================================================================

	/**
	 * @brief Output stream operator
	 * @param os Output stream
	 * @param decimal Decimal value to output
	 * @return Reference to output stream
	 */
	std::ostream& operator<<( std::ostream& os, const Decimal128& decimal );

	/**
	 * @brief Input stream operator
	 * @param is Input stream
	 * @param decimal Decimal value to input
	 * @return Reference to input stream
	 */
	std::istream& operator>>( std::istream& is, Decimal128& decimal );
}

#include "Decimal128.inl"
