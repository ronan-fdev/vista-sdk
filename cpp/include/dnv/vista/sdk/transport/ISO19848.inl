/**
 * @file ISO19848.inl
 * @brief Inline implementations for ISO 19848 classes
 */

#pragma once

namespace dnv::vista::sdk::transport
{
	//=====================================================================
	// DataChannelTypeName class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline DataChannelTypeName::DataChannelTypeName( std::string_view type, std::string_view description ) noexcept
		: m_type{ type },
		  m_description{ description }
	{
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline const std::string& DataChannelTypeName::type() const noexcept
	{
		return m_type;
	}

	inline const std::string& DataChannelTypeName::description() const noexcept
	{
		return m_description;
	}

	//=====================================================================
	// DataChannelTypeNames class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline DataChannelTypeNames::DataChannelTypeNames( std::vector<DataChannelTypeName> values ) noexcept
		: m_values{ std::move( values ) }
	{
	}

	//----------------------------------------------
	// DataChannelTypeNames::ParseResult class
	//----------------------------------------------

	//-----------------------------
	// Construction
	//-----------------------------

	inline DataChannelTypeNames::ParseResult::Ok::Ok( DataChannelTypeName typeName ) noexcept
		: m_typeName{ std::move( typeName ) }
	{
	}

	inline const DataChannelTypeName& DataChannelTypeNames::ParseResult::Ok::typeName() const noexcept
	{
		return m_typeName;
	}

	inline DataChannelTypeNames::ParseResult::ParseResult( Ok ok ) noexcept
		: m_value{ std::move( ok ) }
	{
	}

	inline DataChannelTypeNames::ParseResult::ParseResult( Invalid invalid ) noexcept
		: m_value{ std::move( invalid ) }
	{
	}

	//----------------------------------------------
	// Type checking
	//----------------------------------------------

	inline bool DataChannelTypeNames::ParseResult::isOk() const noexcept
	{
		return std::holds_alternative<Ok>( m_value );
	}

	inline bool DataChannelTypeNames::ParseResult::isInvalid() const noexcept
	{
		return std::holds_alternative<Invalid>( m_value );
	}

	//----------------------------------------------
	// Value access
	//----------------------------------------------

	inline const DataChannelTypeNames::ParseResult::Ok& DataChannelTypeNames::ParseResult::ok() const
	{
		return std::get<Ok>( m_value );
	}

	inline const DataChannelTypeNames::ParseResult::Invalid& DataChannelTypeNames::ParseResult::invalid() const
	{
		return std::get<Invalid>( m_value );
	}

	//----------------------------------------------
	// Iteration
	//----------------------------------------------

	inline DataChannelTypeNames::iterator DataChannelTypeNames::begin() const noexcept
	{
		return m_values.begin();
	}

	inline DataChannelTypeNames::iterator DataChannelTypeNames::end() const noexcept
	{
		return m_values.end();
	}

	inline DataChannelTypeNames::const_iterator DataChannelTypeNames::cbegin() const noexcept
	{
		return m_values.cbegin();
	}

	inline DataChannelTypeNames::const_iterator DataChannelTypeNames::cend() const noexcept
	{
		return m_values.cend();
	}

	//=====================================================================
	// FormatDataType class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline FormatDataType::FormatDataType( std::string_view type, std::string_view description ) noexcept
		: m_type{ type },
		  m_description{ description }
	{
	}

	//----------------------------------------------
	// Construction from value types
	//----------------------------------------------

	//-----------------------------
	// Converting constructors
	//-----------------------------

	inline FormatDataType::Value::Value( String string ) noexcept
		: m_value{ std::move( string ) }
	{
	}

	inline FormatDataType::Value::Value( Char charValue ) noexcept
		: m_value{ std::move( charValue ) }
	{
	}

	inline FormatDataType::Value::Value( Boolean boolean ) noexcept
		: m_value{ std::move( boolean ) }
	{
	}

	inline FormatDataType::Value::Value( Integer integer ) noexcept
		: m_value{ std::move( integer ) }
	{
	}

	inline FormatDataType::Value::Value( UnsignedInteger unsignedInteger ) noexcept
		: m_value{ std::move( unsignedInteger ) }
	{
	}

	inline FormatDataType::Value::Value( Long longValue ) noexcept
		: m_value{ std::move( longValue ) }
	{
	}

	inline FormatDataType::Value::Value( Double doubleValue ) noexcept
		: m_value{ std::move( doubleValue ) }
	{
	}

	inline FormatDataType::Value::Value( Decimal decimal ) noexcept
		: m_value{ std::move( decimal ) }
	{
	}

	inline FormatDataType::Value::Value( DateTime dateTime ) noexcept
		: m_value{ std::move( dateTime ) }
	{
	}

	//----------------------------------------------
	// Value type constructors
	//----------------------------------------------

	inline FormatDataType::Value::String::String( std::string_view value ) noexcept
		: m_value{ value }
	{
	}

	inline FormatDataType::Value::String::String( std::string value ) noexcept
		: m_value{ std::move( value ) }
	{
	}

	inline FormatDataType::Value::Char::Char( char value ) noexcept
		: m_value{ value }
	{
	}

	inline FormatDataType::Value::Boolean::Boolean( bool value ) noexcept
		: m_value{ value }
	{
	}

	inline FormatDataType::Value::Integer::Integer( int value ) noexcept
		: m_value{ value }
	{
	}

	inline FormatDataType::Value::UnsignedInteger::UnsignedInteger( std::uint32_t value ) noexcept
		: m_value{ value }
	{
	}

	inline FormatDataType::Value::Long::Long( std::int64_t value ) noexcept
		: m_value{ value }
	{
	}

	inline FormatDataType::Value::Double::Double( double value ) noexcept
		: m_value{ value }
	{
	}

	inline FormatDataType::Value::Decimal::Decimal( const datatypes::Decimal128& value ) noexcept
		: m_value{ value }
	{
	}

	inline FormatDataType::Value::Decimal::Decimal( double value ) noexcept
		: m_value{ value }
	{
	}

	inline FormatDataType::Value::DateTime::DateTime( const datatypes::DateTimeOffset& value ) noexcept
		: m_value{ value }
	{
	}

	//----------------------------------------------
	// Type checking
	//----------------------------------------------

	inline bool FormatDataType::Value::isString() const noexcept
	{
		return std::holds_alternative<String>( m_value );
	}

	inline bool FormatDataType::Value::isChar() const noexcept
	{
		return std::holds_alternative<Char>( m_value );
	}

	inline bool FormatDataType::Value::isBoolean() const noexcept
	{
		return std::holds_alternative<Boolean>( m_value );
	}

	inline bool FormatDataType::Value::isInteger() const noexcept
	{
		return std::holds_alternative<Integer>( m_value );
	}

	inline bool FormatDataType::Value::isUnsignedInteger() const noexcept
	{
		return std::holds_alternative<UnsignedInteger>( m_value );
	}

	inline bool FormatDataType::Value::isLong() const noexcept
	{
		return std::holds_alternative<Long>( m_value );
	}

	inline bool FormatDataType::Value::isDouble() const noexcept
	{
		return std::holds_alternative<Double>( m_value );
	}

	inline bool FormatDataType::Value::isDecimal() const noexcept
	{
		return std::holds_alternative<Decimal>( m_value );
	}

	inline bool FormatDataType::Value::isDateTime() const noexcept
	{
		return std::holds_alternative<DateTime>( m_value );
	}

	//----------------------------------------------
	// Value access
	//----------------------------------------------

	inline const std::string& FormatDataType::Value::String::value() const noexcept
	{
		return m_value;
	}

	inline char FormatDataType::Value::Char::value() const noexcept
	{
		return m_value;
	}

	inline bool FormatDataType::Value::Boolean::value() const noexcept
	{
		return m_value;
	}

	inline int FormatDataType::Value::Integer::value() const noexcept
	{
		return m_value;
	}

	inline std::uint32_t FormatDataType::Value::UnsignedInteger::value() const noexcept
	{
		return m_value;
	}

	inline std::int64_t FormatDataType::Value::Long::value() const noexcept
	{
		return m_value;
	}

	inline double FormatDataType::Value::Double::value() const noexcept
	{
		return m_value;
	}

	inline const datatypes::Decimal128& FormatDataType::Value::Decimal::value() const noexcept
	{
		return m_value;
	}

	inline const datatypes::DateTimeOffset& FormatDataType::Value::DateTime::value() const noexcept
	{
		return m_value;
	}

	//----------------------------------------------
	// FormatDataType::ValidateResult class
	//----------------------------------------------

	inline FormatDataType::ValidateResult::Invalid::Invalid( std::vector<std::string> errors ) noexcept
		: m_errors{ std::move( errors ) }
	{
	}

	inline const std::vector<std::string>& FormatDataType::ValidateResult::Invalid::errors() const noexcept
	{
		return m_errors;
	}

	inline FormatDataType::ValidateResult::ValidateResult( Ok ok ) noexcept
		: m_value{ std::move( ok ) }
	{
	}

	inline FormatDataType::ValidateResult::ValidateResult( Invalid invalid ) noexcept
		: m_value{ std::move( invalid ) }
	{
	}

	inline const std::string& FormatDataType::type() const noexcept
	{
		return m_type;
	}

	inline const std::string& FormatDataType::description() const noexcept
	{
		return m_description;
	}

	//=====================================================================
	// FormatDataTypes class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline FormatDataTypes::FormatDataTypes( std::vector<FormatDataType> values ) noexcept
		: m_values{ std::move( values ) }
	{
	}

	//----------------------------------------------
	// FormatDataType::ParseResult class
	//----------------------------------------------

	inline FormatDataTypes::ParseResult::Ok::Ok( FormatDataType typeName ) noexcept
		: m_typeName{ std::move( typeName ) }
	{
	}

	inline const FormatDataType& FormatDataTypes::ParseResult::Ok::typeName() const noexcept
	{
		return m_typeName;
	}

	inline FormatDataTypes::ParseResult::ParseResult( Ok ok ) noexcept
		: m_value{ std::move( ok ) }
	{
	}

	inline FormatDataTypes::ParseResult::ParseResult( Invalid invalid ) noexcept
		: m_value{ std::move( invalid ) }
	{
	}

	inline bool FormatDataTypes::ParseResult::isOk() const noexcept
	{
		return std::holds_alternative<Ok>( m_value );
	}

	inline bool FormatDataTypes::ParseResult::isInvalid() const noexcept
	{
		return std::holds_alternative<Invalid>( m_value );
	}

	inline const FormatDataTypes::ParseResult::Ok& FormatDataTypes::ParseResult::ok() const
	{
		return std::get<Ok>( m_value );
	}

	inline const FormatDataTypes::ParseResult::Invalid& FormatDataTypes::ParseResult::invalid() const
	{
		return std::get<Invalid>( m_value );
	}

	//----------------------------------------------
	// Value access
	//----------------------------------------------

	inline const FormatDataType::Value::String& FormatDataType::Value::string() const
	{
		return std::get<String>( m_value );
	}

	inline const FormatDataType::Value::Char& FormatDataType::Value::charValue() const
	{
		return std::get<Char>( m_value );
	}

	inline const FormatDataType::Value::Boolean& FormatDataType::Value::boolean() const
	{
		return std::get<Boolean>( m_value );
	}

	inline const FormatDataType::Value::Integer& FormatDataType::Value::integer() const
	{
		return std::get<Integer>( m_value );
	}

	inline const FormatDataType::Value::UnsignedInteger& FormatDataType::Value::unsignedInteger() const
	{
		return std::get<UnsignedInteger>( m_value );
	}

	inline const FormatDataType::Value::Decimal& FormatDataType::Value::decimal() const
	{
		return std::get<Decimal>( m_value );
	}

	inline const FormatDataType::Value::Long& FormatDataType::Value::longValue() const
	{
		return std::get<Long>( m_value );
	}

	inline const FormatDataType::Value::Double& FormatDataType::Value::doubleValue() const
	{
		return std::get<Double>( m_value );
	}

	inline const FormatDataType::Value::DateTime& FormatDataType::Value::dateTime() const
	{
		return std::get<DateTime>( m_value );
	}

	//----------------------------------------------
	// Variant index access
	//----------------------------------------------

	inline std::size_t FormatDataType::Value::index() const noexcept
	{
		return m_value.index();
	}

	inline FormatDataType::Value::Type FormatDataType::Value::type() const noexcept
	{
		return static_cast<Type>( m_value.index() );
	}

	//----------------------------------------------
	// Iteration
	//----------------------------------------------

	inline FormatDataTypes::iterator FormatDataTypes::begin() const noexcept
	{
		return m_values.begin();
	}

	inline FormatDataTypes::iterator FormatDataTypes::end() const noexcept
	{
		return m_values.end();
	}

	inline FormatDataTypes::const_iterator FormatDataTypes::cbegin() const noexcept
	{
		return m_values.cbegin();
	}

	inline FormatDataTypes::const_iterator FormatDataTypes::cend() const noexcept
	{
		return m_values.cend();
	}

	//----------------------------------------------
	// FormatDataType::ValidateResult class
	//----------------------------------------------

	//-----------------------------
	// Type checking
	//-----------------------------

	inline bool FormatDataType::ValidateResult::isOk() const noexcept
	{
		return std::holds_alternative<Ok>( m_value );
	}

	inline bool FormatDataType::ValidateResult::isInvalid() const noexcept
	{
		return std::holds_alternative<Invalid>( m_value );
	}

	//----------------------------------------------
	// Value access
	//----------------------------------------------

	inline const FormatDataType::ValidateResult::Ok& FormatDataType::ValidateResult::ok() const
	{
		return std::get<Ok>( m_value );
	}

	inline const FormatDataType::ValidateResult::Invalid& FormatDataType::ValidateResult::invalid() const
	{
		return std::get<Invalid>( m_value );
	}

	//----------------------------------------------
	// Pattern matching methods
	//----------------------------------------------

	template <typename DecimalFunc, typename IntegerFunc, typename BooleanFunc, typename StringFunc, typename DateTimeFunc>
	inline void FormatDataType::switchOn(
		std::string_view value, DecimalFunc&& onDecimal, IntegerFunc&& onInteger,
		BooleanFunc&& onBoolean, StringFunc&& onString, DateTimeFunc&& onDateTime ) const
	{
		Value typedValue;
		auto result = validate( value, typedValue );

		if ( result.isInvalid() )
		{
			throw ValidationException( "Invalid value" );
		}

		switch ( typedValue.type() )
		{
			case Value::Type::String:
			{
				onString( typedValue.string().value() );
				break;
			}
			case Value::Type::Boolean:
			{
				onBoolean( typedValue.boolean().value() );
				break;
			}
			case Value::Type::Integer:
			{
				onInteger( typedValue.integer().value() );
				break;
			}
			case Value::Type::Decimal:
			{
				onDecimal( typedValue.decimal().value() );
				break;
			}
			case Value::Type::DateTime:
			{
				onDateTime( typedValue.dateTime().value() );
				break;
			}
			case Value::Type::Char:
			case Value::Type::UnsignedInteger:
			case Value::Type::Long:
			case Value::Type::Double:
			default:
			{
				throw std::runtime_error( "Unexpected value type in switch operation" );
			}
		}
	}

	template <typename T, typename DecimalFunc, typename IntegerFunc, typename BooleanFunc, typename StringFunc, typename DateTimeFunc>
	inline T FormatDataType::matchOn(
		std::string_view value, DecimalFunc&& onDecimal, IntegerFunc&& onInteger,
		BooleanFunc&& onBoolean, StringFunc&& onString, DateTimeFunc&& onDateTime ) const
	{
		Value typedValue;
		auto result = validate( value, typedValue );

		if ( result.isInvalid() )
		{
			throw ValidationException( "Invalid value" );
		}

		switch ( typedValue.type() )
		{
			case Value::Type::String:
			{
				return onString( typedValue.string().value() );
			}
			case Value::Type::Boolean:
			{
				return onBoolean( typedValue.boolean().value() );
			}
			case Value::Type::Integer:
			{
				return onInteger( typedValue.integer().value() );
			}
			case Value::Type::Decimal:
			{
				return onDecimal( typedValue.decimal().value() );
			}
			case Value::Type::DateTime:
			{
				return onDateTime( typedValue.dateTime().value() );
			}
			case Value::Type::Char:
			case Value::Type::UnsignedInteger:
			case Value::Type::Long:
			case Value::Type::Double:
			default:
			{
				throw std::runtime_error( "Unexpected value type in match operation" );
			}
		}
	}
}
