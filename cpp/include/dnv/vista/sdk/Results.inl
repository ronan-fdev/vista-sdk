/**
 * @file Results.inl
 * @brief Inline implementations for validation result types
 * @details Provides inline method implementations for ValidateResult classes
 */

namespace dnv::vista::sdk
{
	//=====================================================================
	// ValidateResult class
	//=====================================================================

	//-----------------------------
	// Construction from result
	//-----------------------------

	inline ValidateResult::Invalid::Invalid( std::vector<std::string> errors ) noexcept
		: m_errors{ std::move( errors ) }
	{
	}

	inline const std::vector<std::string>& ValidateResult::Invalid::errors() const noexcept
	{
		return m_errors;
	}

	inline ValidateResult::ValidateResult( Ok ok ) noexcept
		: m_value{ std::move( ok ) }
	{
	}

	inline ValidateResult::ValidateResult( Invalid invalid ) noexcept
		: m_value{ std::move( invalid ) }
	{
	}

	//-----------------------------
	// Type checking
	//-----------------------------

	inline bool ValidateResult::isOk() const noexcept
	{
		return std::holds_alternative<Ok>( m_value );
	}

	inline bool ValidateResult::isInvalid() const noexcept
	{
		return std::holds_alternative<Invalid>( m_value );
	}

	//----------------------------------------------
	// Value access
	//----------------------------------------------

	inline const ValidateResult::Ok& ValidateResult::ok() const
	{
		return std::get<Ok>( m_value );
	}

	inline const ValidateResult::Invalid& ValidateResult::invalid() const
	{
		return std::get<Invalid>( m_value );
	}
}
