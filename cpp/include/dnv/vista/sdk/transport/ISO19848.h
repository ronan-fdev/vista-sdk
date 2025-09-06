/**
 * @file ISO19848.h
 * @brief ISO 19848 standard data channel types and format data types
 */

#pragma once

#include <cstdint>

#include <nfx/datatypes/Decimal.h>
#include <nfx/memory/MemoryCache.h>
#include <nfx/time/DateTime.h>

#include "dnv/vista/sdk/transport/ISO19848Dtos.h"
#include "dnv/vista/sdk/Exceptions.h"
#include "dnv/vista/sdk/Results.h"

namespace dnv::vista::sdk::transport
{
	//=====================================================================
	// Enumerations
	//=====================================================================

	/** @brief ISO 19848 standard versions */
	enum class ISO19848Version
	{
		v2018,
		v2024
	};

	//=====================================================================
	// Value class
	//=====================================================================

	/**
	 * @brief Typed value wrapper for format data type validation results
	 * @details Supports all ISO 19848 format data types with efficient variant storage
	 */
	class Value
	{
	public:
		//----------------------------------------------
		// Type enumeration
		//----------------------------------------------

		/** @brief Enumeration representing the variant types */
		enum class Type : std::uint8_t
		{
			String = 0,
			Char,
			Boolean,
			Integer,
			UnsignedInteger,
			Long,
			Double,
			Decimal,
			DateTime,
		};

		//----------------------------------------------
		// Value::String class
		//----------------------------------------------

		/**
		 * @brief String value wrapper for format data type validation
		 */
		class String final
		{
		public:
			/**
			 * @brief Construct string value from string view
			 * @param value The string value to store
			 */
			inline explicit String( std::string_view value ) noexcept;

			/** @brief Default constructor */
			String() = default;

			/** @brief Copy constructor */
			String( const String& ) = default;

			/** @brief Move constructor */
			String( String&& ) noexcept = default;

			/** @brief Destructor */
			~String() = default;

			/** @brief Copy assignment */
			String& operator=( const String& ) = default;

			/** @brief Move assignment */
			String& operator=( String&& ) noexcept = default;

			/**
			 * @brief Get the string value
			 * @return Reference to the internal string value
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const std::string& value() const noexcept;

		private:
			/** @brief Internal string storage */
			std::string m_value;
		};

		//----------------------------------------------
		// Value::Char class
		//----------------------------------------------

		/**
		 * @brief Character value wrapper for format data type validation
		 */
		class Char final
		{
		public:
			/**
			 * @brief Construct character value from char
			 * @param value The character value to store
			 */
			inline explicit Char( char value ) noexcept;

			/**
			 * @brief Get the character value
			 * @return The internal character value
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline char value() const noexcept;

		private:
			/** @brief Internal character storage */
			char m_value;
		};

		//----------------------------------------------
		// Value::Boolean class
		//----------------------------------------------

		/**
		 * @brief Boolean value wrapper for format data type validation
		 */
		class Boolean final
		{
		public:
			/**
			 * @brief Construct boolean value from bool
			 * @param value The boolean value to store
			 */
			inline explicit Boolean( bool value ) noexcept;

			/**
			 * @brief Get the boolean value
			 * @return The internal boolean value
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline bool value() const noexcept;

		private:
			/** @brief Internal boolean storage */
			bool m_value;
		};

		//----------------------------------------------
		// Value::Integer class
		//----------------------------------------------

		/**
		 * @brief Integer value wrapper for format data type validation
		 */
		class Integer final
		{
		public:
			/**
			 * @brief Construct integer value from int
			 * @param value The integer value to store
			 */
			inline explicit Integer( int value ) noexcept;

			/**
			 * @brief Get the integer value
			 * @return The internal integer value
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline int value() const noexcept;

		private:
			/** @brief Internal integer storage */
			int m_value;
		};

		//----------------------------------------------
		// Value::UnsignedInteger class
		//----------------------------------------------

		/**
		 * @brief Unsigned integer value wrapper for format data type validation
		 */
		class UnsignedInteger final
		{
		public:
			/**
			 * @brief Construct unsigned integer value from uint32_t
			 * @param value The unsigned integer value to store
			 */
			inline explicit UnsignedInteger( std::uint32_t value ) noexcept;

			/**
			 * @brief Get the unsigned integer value
			 * @return The internal unsigned integer value
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline std::uint32_t value() const noexcept;

		private:
			/** @brief Internal unsigned integer storage */
			std::uint32_t m_value;
		};

		//----------------------------------------------
		// Value::Long class
		//---------------------------------------------

		/**
		 * @brief Long integer value wrapper for format data type validation
		 */
		class Long final
		{
		public:
			/**
			 * @brief Construct long integer value from int64_t
			 * @param value The long integer value to store
			 */
			inline explicit Long( std::int64_t value ) noexcept;

			/**
			 * @brief Get the long integer value
			 * @return The internal long integer value
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline std::int64_t value() const noexcept;

		private:
			/** @brief Internal long integer storage */
			std::int64_t m_value;
		};

		//----------------------------------------------
		// Value::Double class
		//---------------------------------------------

		/**
		 * @brief Double precision floating-point value wrapper for format data type validation
		 */
		class Double final
		{
		public:
			/**
			 * @brief Construct double precision value from double
			 * @param value The double precision value to store
			 */
			inline explicit Double( double value ) noexcept;

			/**
			 * @brief Get the double precision value
			 * @return The internal double precision value
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline double value() const noexcept;

		private:
			/** @brief Internal double precision storage */
			double m_value;
		};

		//----------------------------------------------
		// Value::Decimal class
		//----------------------------------------------

		/**
		 * @brief High-precision decimal value wrapper for format data type validation
		 */
		class Decimal final
		{
		public:
			/**
			 * @brief Construct decimal value from high-precision decimal
			 * @param value The high-precision decimal value to store
			 */
			inline explicit Decimal( const nfx::datatypes::Decimal& value ) noexcept;

			/**
			 * @brief Construct decimal value from double
			 * @param value The double value to convert and store as decimal
			 */
			inline explicit Decimal( double value ) noexcept;

			/**
			 * @brief Get the high-precision decimal value
			 * @return Reference to the internal high-precision decimal value
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const nfx::datatypes::Decimal& value() const noexcept;

		private:
			/** @brief Internal high-precision decimal storage */
			nfx::datatypes::Decimal m_value;
		};

		//----------------------------------------------
		// Value::DateTime class
		//----------------------------------------------

		/**
		 * @brief Date and time value wrapper for format data type validation
		 */
		class DateTime final
		{
		public:
			/**
			 * @brief Construct date time value from DateTimeOffset
			 * @param value The date time offset value to store
			 */
			inline explicit DateTime( const nfx::time::DateTimeOffset& value ) noexcept;

			/**
			 * @brief Get the date and time value
			 * @return Reference to the internal date and time value
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const nfx::time::DateTimeOffset& value() const noexcept;

		private:
			/** @brief Internal date and time storage */
			nfx::time::DateTimeOffset m_value;
		};

		//----------------------------------------------
		// Construction from value types
		//----------------------------------------------

		Value() = default;

		//-----------------------------
		// Converting constructors
		//-----------------------------

		/**
		 * @brief Converting constructors with optimal move semantics
		 * @details These constructors use the "pass-by-value then move" idiom for optimal performance:
		 * - When called with lvalue: parameter is copy-constructed, then moved into variant
		 * - When called with rvalue: parameter is move-constructed, then moved into variant
		 * - Avoids constructor ambiguity that would arise with separate copy/move overloads
		 * - Provides single, efficient path for both copy and move scenarios
		 * - Implementation uses std::move() to forward into std::variant storage
		 *
		 * Performance benefits over traditional copy/move constructor pairs:
		 * - No ambiguous overload resolution
		 * - Optimal performance for both lvalue and rvalue arguments
		 * - Simpler API surface with single constructor per type
		 * - Perfect forwarding semantics without template complexity
		 */

		inline Value( String string ) noexcept;
		inline Value( Char charValue ) noexcept;
		inline Value( Boolean boolean ) noexcept;
		inline Value( Integer integer ) noexcept;
		inline Value( UnsignedInteger unsignedInteger ) noexcept;
		inline Value( Long longValue ) noexcept;
		inline Value( Double doubleValue ) noexcept;
		inline Value( Decimal decimal ) noexcept;
		inline Value( DateTime dateTime ) noexcept;

		//----------------------------------------------
		// Type checking
		//----------------------------------------------

		/**
		 * @brief Check if value is a string
		 * @return True if the value holds a string, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isString() const noexcept;

		/**
		 * @brief Check if value is a character
		 * @return True if the value holds a character, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isChar() const noexcept;

		/**
		 * @brief Check if value is a boolean
		 * @return True if the value holds a boolean, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isBoolean() const noexcept;

		/**
		 * @brief Check if value is an integer
		 * @return True if the value holds an integer, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isInteger() const noexcept;

		/**
		 * @brief Check if value is an unsigned integer
		 * @return True if the value holds an unsigned integer, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isUnsignedInteger() const noexcept;

		/**
		 * @brief Check if value is a long integer
		 * @return True if the value holds a long integer, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isLong() const noexcept;

		/**
		 * @brief Check if value is a double precision number
		 * @return True if the value holds a double, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isDouble() const noexcept;

		/**
		 * @brief Check if value is a high-precision decimal
		 * @return True if the value holds a decimal, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isDecimal() const noexcept;

		/**
		 * @brief Check if value is a date and time
		 * @return True if the value holds a datetime, false otherwise
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline bool isDateTime() const noexcept;

		//----------------------------------------------
		// Value access
		//----------------------------------------------

		/**
		 * @brief Get the string value
		 * @return Reference to the string value
		 * @throws std::bad_variant_access if value is not a string
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const String& string() const;

		/**
		 * @brief Get the character value
		 * @return Reference to the character value
		 * @throws std::bad_variant_access if value is not a character
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const Char& charValue() const;

		/**
		 * @brief Get the boolean value
		 * @return Reference to the boolean value
		 * @throws std::bad_variant_access if value is not a boolean
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const Boolean& boolean() const;

		/**
		 * @brief Get the integer value
		 * @return Reference to the integer value
		 * @throws std::bad_variant_access if value is not an integer
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const Integer& integer() const;

		/**
		 * @brief Get the unsigned integer value
		 * @return Reference to the unsigned integer value
		 * @throws std::bad_variant_access if value is not an unsigned integer
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const UnsignedInteger& unsignedInteger() const;

		/**
		 * @brief Get the long integer value
		 * @return Reference to the long integer value
		 * @throws std::bad_variant_access if value is not a long integer
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const Long& longValue() const;

		/**
		 * @brief Get the double precision value
		 * @return Reference to the double precision value
		 * @throws std::bad_variant_access if value is not a double
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const Double& doubleValue() const;

		/**
		 * @brief Get the high-precision decimal value
		 * @return Reference to the decimal value
		 * @throws std::bad_variant_access if value is not a decimal
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const Decimal& decimal() const;

		/**
		 * @brief Get the date and time value
		 * @return Reference to the datetime value
		 * @throws std::bad_variant_access if value is not a datetime
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const DateTime& dateTime() const;

		//----------------------------------------------
		// Variant index access
		//----------------------------------------------

		/**
		 * @brief Get the variant index for switch statements
		 * @return Index of the currently held type in the variant
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline std::size_t index() const noexcept;

		/**
		 * @brief Get the type as an enum for readable switch statements
		 * @return Type enum representing the currently held type
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline Type type() const noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Variant storing the typed value */
		std::variant<String, Char, Boolean, Integer, UnsignedInteger, Long, Double, Decimal, DateTime> m_value{ String{ std::string_view{ "" } } };
	};

	//=====================================================================
	// DataChannelTypeName class
	//=====================================================================

	/** @brief Single data channel type name with description */
	class DataChannelTypeName final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor
		 * @param type Type name
		 * @param description Description
		 */
		inline explicit DataChannelTypeName( std::string_view type, std::string_view description ) noexcept;

		/** @brief Default constructor */
		DataChannelTypeName() = default;

		/** @brief Copy constructor */
		DataChannelTypeName( const DataChannelTypeName& ) = default;

		/** @brief Move constructor */
		DataChannelTypeName( DataChannelTypeName&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~DataChannelTypeName() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment */
		DataChannelTypeName& operator=( const DataChannelTypeName& ) = default;

		/** @brief Move assignment */
		DataChannelTypeName& operator=( DataChannelTypeName&& ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get type name
		 * @return Type name
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::string& type() const noexcept;

		/**
		 * @brief Get description
		 * @return Description
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::string& description() const noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Type name identifier */
		std::string m_type;

		/** @brief Human-readable description */
		std::string m_description;
	};

	//=====================================================================
	// DataChannelTypeNames class
	//=====================================================================

	/** @brief Collection of data channel type names with parsing capability */
	class DataChannelTypeNames final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor with values
		 * @param values Collection of data channel type names
		 */
		inline explicit DataChannelTypeNames( std::vector<DataChannelTypeName> values ) noexcept;

		/** @brief Default constructor */
		DataChannelTypeNames() = default;

		/** @brief Copy constructor */
		DataChannelTypeNames( const DataChannelTypeNames& ) = default;

		/** @brief Move constructor */
		DataChannelTypeNames( DataChannelTypeNames&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~DataChannelTypeNames() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment */
		DataChannelTypeNames& operator=( const DataChannelTypeNames& ) = default;

		/** @brief Move assignment */
		DataChannelTypeNames& operator=( DataChannelTypeNames&& ) noexcept = default;

		//----------------------------------------------
		// DataChannelTypeNames::ParseResult class
		//----------------------------------------------

		/** @brief Result of parsing operation */
		class ParseResult
		{
		public:
			//----------------------------------------------
			// DataChannelTypeNames::ParseResult::Ok class
			//----------------------------------------------

			/**
			 * @brief Successful parse result containing a data channel type name
			 */
			class Ok final
			{
			public:
				/**
				 * @brief Construct successful parse result
				 * @param typeName The parsed data channel type name
				 */
				inline explicit Ok( DataChannelTypeName typeName ) noexcept;

				/**
				 * @brief Get the parsed data channel type name
				 * @return Reference to the data channel type name
				 * @note This function is marked [[nodiscard]] - the return value should not be ignored
				 */
				[[nodiscard]] inline const DataChannelTypeName& typeName() const noexcept;

			private:
				DataChannelTypeName m_typeName;
			};

			//----------------------------------------------
			// DataChannelTypeNames::ParseResult::Invalid class
			//----------------------------------------------

			/**
			 * @brief Failed parse result indicating invalid data channel type name
			 */
			class Invalid final
			{
			public:
				Invalid() = default;
			};

			//----------------------------------------------
			// Construction
			//----------------------------------------------

			/**
			 * @brief Construct parse result from successful result
			 * @param ok The successful parse result
			 */
			inline ParseResult( Ok ok ) noexcept;

			/**
			 * @brief Construct parse result from failed result
			 * @param invalid The failed parse result
			 */
			inline ParseResult( Invalid invalid ) noexcept;

			//----------------------------------------------
			// Type checking
			//----------------------------------------------

			/**
			 * @brief Check if parse result is successful
			 * @return True if parse was successful, false otherwise
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline bool isOk() const noexcept;

			/**
			 * @brief Check if parse result is invalid
			 * @return True if parse failed, false otherwise
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline bool isInvalid() const noexcept;

			//----------------------------------------------
			// Value access
			//----------------------------------------------

			/**
			 * @brief Get the successful parse result
			 * @return Reference to the successful result
			 * @throws std::bad_variant_access if result is not successful
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const Ok& ok() const;

			/**
			 * @brief Get the failed parse result
			 * @return Reference to the failed result
			 * @throws std::bad_variant_access if result is not failed
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const Invalid& invalid() const;

		private:
			/** @brief Variant storage for parse result types */
			std::variant<Ok, Invalid> m_value;
		};

		//----------------------------------------------
		// DataChannelTypeNames::Iterator support
		//----------------------------------------------

		using iterator = std::vector<DataChannelTypeName>::const_iterator;
		using const_iterator = std::vector<DataChannelTypeName>::const_iterator;

		//----------------------------------------------
		// Parsing
		//----------------------------------------------

		/**
		 * @brief Parse type name from string
		 * @param type Type string to parse
		 * @return Parse result
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] ParseResult parse( std::string_view type ) const;

		//----------------------------------------------
		// Iteration
		//----------------------------------------------

		/**
		 * @brief Get iterator to beginning of data channel type names
		 * @return Iterator pointing to the first data channel type name
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline iterator begin() const noexcept;

		/**
		 * @brief Get iterator to end of data channel type names
		 * @return Iterator pointing past the last data channel type name
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline iterator end() const noexcept;

		/**
		 * @brief Get const iterator to beginning of data channel type names
		 * @return Const iterator pointing to the first data channel type name
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const_iterator cbegin() const noexcept;

		/**
		 * @brief Get const iterator to end of data channel type names
		 * @return Const iterator pointing past the last data channel type name
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const_iterator cend() const noexcept;

	private:
		std::vector<DataChannelTypeName> m_values;
	};

	//=====================================================================
	// FormatDataType class
	//=====================================================================

	/** @brief Single format data type with validation capability */
	class FormatDataType final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor
		 * @param type Type name
		 * @param description Description
		 */
		inline explicit FormatDataType( std::string_view type, std::string_view description ) noexcept;

		/** @brief Default constructor */
		FormatDataType() = default;

		/** @brief Copy constructor */
		FormatDataType( const FormatDataType& ) = default;

		/** @brief Move constructor */
		FormatDataType( FormatDataType&& ) noexcept = default;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~FormatDataType() = default;

		//-----------------------------
		// Assignment operators
		//-----------------------------

		/** @brief Copy assignment */
		FormatDataType& operator=( const FormatDataType& ) = default;

		/** @brief Move assignment */
		FormatDataType& operator=( FormatDataType&& ) noexcept = default;

		//----------------------------------------------
		// Accessors
		//----------------------------------------------

		/**
		 * @brief Get type name
		 * @return Type name
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::string& type() const noexcept;

		/**
		 * @brief Get description
		 * @return Description
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const std::string& description() const noexcept;

		//----------------------------------------------
		// Validation
		//----------------------------------------------

		/**
		 * @brief Validate value according to format type
		 * @param value String value to validate
		 * @param outValue Output typed value
		 * @return Validation result
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] ValidateResult validate( std::string_view value, Value& outValue ) const;

		//----------------------------------------------
		// Pattern matching methods
		//----------------------------------------------

		/**
		 * @brief Action-based pattern matching on validated value
		 * @param value String value to validate and switch on
		 * @param onDecimal Action for decimal values
		 * @param onInteger Action for integer values
		 * @param onBoolean Action for boolean values
		 * @param onString Action for string values
		 * @param onDateTime Action for datetime values
		 * @throws ValidationException if validation fails
		 */
		template <typename DecimalFunc, typename IntegerFunc, typename BooleanFunc, typename StringFunc, typename DateTimeFunc>
		inline void switchOn( std::string_view value, DecimalFunc&& onDecimal, IntegerFunc&& onInteger, BooleanFunc&& onBoolean, StringFunc&& onString,
			DateTimeFunc&& onDateTime ) const;

		/**
		 * @brief Function-based pattern matching on validated value with return value
		 * @tparam T Return type
		 * @param value String value to validate and match on
		 * @param onDecimal Function for decimal values
		 * @param onInteger Function for integer values
		 * @param onBoolean Function for boolean values
		 * @param onString Function for string values
		 * @param onDateTime Function for datetime values
		 * @return Result of matched function
		 * @throws ValidationException if validation fails
		 */
		template <typename T, typename DecimalFunc, typename IntegerFunc, typename BooleanFunc, typename StringFunc, typename DateTimeFunc>
		inline T matchOn( std::string_view value, DecimalFunc&& onDecimal, IntegerFunc&& onInteger, BooleanFunc&& onBoolean, StringFunc&& onString,
			DateTimeFunc&& onDateTime ) const;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Format data type name identifier */
		std::string m_type;

		/** @brief Human-readable description of the format data type */
		std::string m_description;
	};

	//=====================================================================
	// FormatDataTypes class
	//=====================================================================

	/**
	 * @brief Collection of format data types with parsing capability
	 */
	class FormatDataTypes final
	{
	public:
		//----------------------------------------------
		// Construction
		//----------------------------------------------

		/**
		 * @brief Constructor with values
		 * @param values Collection of format data types
		 */
		inline explicit FormatDataTypes( std::vector<FormatDataType> values ) noexcept;

		/** @brief Default constructor */
		FormatDataTypes() = default;

		/** @brief Copy constructor */
		FormatDataTypes( const FormatDataTypes& ) = default;

		/** @brief Move constructor */
		FormatDataTypes( FormatDataTypes&& ) noexcept = default;

		/** @brief Copy assignment */
		FormatDataTypes& operator=( const FormatDataTypes& ) = default;

		/** @brief Move assignment */
		FormatDataTypes& operator=( FormatDataTypes&& ) noexcept = default;

		//----------------------------------------------
		// FormatDataType::ParseResult class
		//----------------------------------------------

		/** @brief Result of parsing operation */
		class ParseResult
		{
		public:
			/** @brief Successful parse result */
			class Ok final
			{
			public:
				/**
				 * @brief Construct successful parse result
				 * @param typeName The parsed format data type
				 */
				inline explicit Ok( FormatDataType typeName ) noexcept;

				/**
				 * @brief Get the parsed format data type
				 * @return Reference to the format data type
				 * @note This function is marked [[nodiscard]] - the return value should not be ignored
				 */
				[[nodiscard]] inline const FormatDataType& typeName() const noexcept;

			private:
				FormatDataType m_typeName;
			};

			/** @brief Failed parse result */
			class Invalid final
			{
			public:
				/**
				 * @brief Default constructor for failed parse result
				 */
				Invalid() = default;
			};

			//-----------------------------
			// Construction from result
			//-----------------------------

			/**
			 * @brief Construct parse result from successful result
			 * @param ok The successful parse result
			 */
			inline ParseResult( Ok ok ) noexcept;

			/**
			 * @brief Construct parse result from failed result
			 * @param invalid The failed parse result
			 */
			inline ParseResult( Invalid invalid ) noexcept;

			//-----------------------------
			// Type checking
			//-----------------------------

			/**
			 * @brief Check if parse result is successful
			 * @return True if parse was successful, false otherwise
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline bool isOk() const noexcept;

			/**
			 * @brief Check if parse result is invalid
			 * @return True if parse failed, false otherwise
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline bool isInvalid() const noexcept;

			//-----------------------------
			// Value access
			//-----------------------------

			/**
			 * @brief Get the successful parse result
			 * @return Reference to the successful result
			 * @throws std::bad_variant_access if result is not successful
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const Ok& ok() const;

			/**
			 * @brief Get the failed parse result
			 * @return Reference to the failed result
			 * @throws std::bad_variant_access if result is not failed
			 * @note This function is marked [[nodiscard]] - the return value should not be ignored
			 */
			[[nodiscard]] inline const Invalid& invalid() const;

		private:
			/** @brief Variant storage for parse result types */
			std::variant<Ok, Invalid> m_value;
		};

		//----------------------------------------------
		// FormatDataTypes::Iterator support
		//----------------------------------------------

		using iterator = std::vector<FormatDataType>::const_iterator;
		using const_iterator = std::vector<FormatDataType>::const_iterator;

		//----------------------------------------------
		// Parsing
		//----------------------------------------------

		/**
		 * @brief Parse type name from string
		 * @param type Type string to parse
		 * @return Parse result
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] ParseResult parse( std::string_view type ) const;

		//----------------------------------------------
		// Iteration
		//----------------------------------------------

		/**
		 * @brief Get iterator to beginning of format data types
		 * @return Iterator pointing to the first format data type
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline iterator begin() const noexcept;

		/**
		 * @brief Get iterator to end of format data types
		 * @return Iterator pointing past the last format data type
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline iterator end() const noexcept;

		/**
		 * @brief Get const iterator to beginning of format data types
		 * @return Const iterator pointing to the first format data type
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const_iterator cbegin() const noexcept;

		/**
		 * @brief Get const iterator to end of format data types
		 * @return Const iterator pointing past the last format data type
		 * @note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] inline const_iterator cend() const noexcept;

	private:
		//----------------------------------------------
		// Private member variables
		//----------------------------------------------

		/** @brief Collection of format data types */
		std::vector<FormatDataType> m_values;
	};

	//=====================================================================
	// ISO19848 class
	//=====================================================================

	/** @brief ISO 19848 standard data access */
	class ISO19848 final
	{
	public:
		//----------------------------------------------
		// Static constants
		//----------------------------------------------

		static constexpr ISO19848Version LatestVersion = ISO19848Version::v2024;

		//----------------------------------------------
		// Singleton access
		//----------------------------------------------

		/**
		 * @brief Get singleton instance
		 * @return Reference to singleton instance
		 */
		[[nodiscard]] static ISO19848& instance() noexcept;

		//----------------------------------------------
		// Public interface
		//----------------------------------------------

		/**
		 * @brief Get data channel type names for specified version
		 * @param version ISO 19848 version
		 * @return Data channel type names collection
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DataChannelTypeNames dataChannelTypeNames( ISO19848Version version );

		/**
		 * @brief Get format data types for specified version
		 * @param version ISO 19848 version
		 * @return Format data types collection
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] FormatDataTypes formatDataTypes( ISO19848Version version );

	private:
		//----------------------------------------------
		// Singleton construction
		//----------------------------------------------

		/** @brief Default constructor. */
		ISO19848() noexcept;

		/** @brief Copy constructor */
		ISO19848( const ISO19848& ) = delete;

		/** @brief Move constructor */
		ISO19848( ISO19848&& ) = delete;

		//----------------------------------------------
		// Destruction
		//----------------------------------------------

		/** @brief Destructor */
		~ISO19848() = default;

		//----------------------------------------------
		// Assignment operators
		//----------------------------------------------

		/** @brief Copy assignment operator */
		ISO19848& operator=( const ISO19848& ) = delete;

		/** @brief Move assignment operator */
		ISO19848& operator=( ISO19848&& ) = delete;

		//----------------------------------------------
		// Cache instances
		//----------------------------------------------

		nfx::memory::MemoryCache<ISO19848Version, DataChannelTypeNamesDto> m_dataChannelTypeNamesDtoCache;
		nfx::memory::MemoryCache<ISO19848Version, DataChannelTypeNames> m_dataChannelTypeNamesCache;
		nfx::memory::MemoryCache<ISO19848Version, FormatDataTypesDto> m_formatDataTypesDtoCache;
		nfx::memory::MemoryCache<ISO19848Version, FormatDataTypes> m_formatDataTypesCache;

		//----------------------------------------------
		// DTO access
		//----------------------------------------------

		/**
		 * @brief Get data channel type names DTO (internal implementation)
		 * @param version ISO 19848 version
		 * @return Data channel type names DTO
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] DataChannelTypeNamesDto dataChannelTypeNamesDto( ISO19848Version version );

		/**
		 * @brief Get format data types DTO (internal implementation)
		 * @param version ISO 19848 version
		 * @return Format data types DTO
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] FormatDataTypesDto formatDataTypesDto( ISO19848Version version );

		//----------------------------------------------
		// Loading
		//----------------------------------------------

		/**
		 * @brief Load data channel type names DTO from resources
		 * @param version ISO 19848 version
		 * @return Data channel type names DTO if found
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static std::optional<DataChannelTypeNamesDto> loadDataChannelTypeNamesDto( ISO19848Version version );

		/**
		 * @brief Load format data types DTO from resources
		 * @param version ISO 19848 version
		 * @return Format data types DTO if found
		 *@note This function is marked [[nodiscard]] - the return value should not be ignored
		 */
		[[nodiscard]] static std::optional<FormatDataTypesDto> loadFormatDataTypesDto( ISO19848Version version );
	};
}

#include "ISO19848.inl"
