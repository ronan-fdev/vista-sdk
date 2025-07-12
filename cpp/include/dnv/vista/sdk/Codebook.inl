/**
 * @file Codebook.inl
 * @brief Inline implementations for performance-critical codebook operations
 * @details Contains performance-critical inline implementations for
 *          fast codebook access operations.
 */

namespace dnv::vista::sdk
{
	//=====================================================================
	// CodebookStandardValues class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline CodebookStandardValues::CodebookStandardValues( CodebookName name, StringSet&& standardValues ) noexcept
		: m_name{ name },
		  m_standardValues{ std::move( standardValues ) }
	{
	}

	//----------------------------------------------
	// Public methods
	//----------------------------------------------

	inline size_t CodebookStandardValues::count() const noexcept
	{
		return m_standardValues.size();
	}

	inline bool CodebookStandardValues::contains( std::string_view tagValue ) const noexcept
	{
		if ( m_standardValues.contains( tagValue ) )
		{
			return true;
		}

		if ( m_name == CodebookName::Position )
		{
			constexpr auto isDigit = []( char c ) noexcept {
				return static_cast<unsigned char>( c - '0' ) <= 9u;
			};
			return !tagValue.empty() && std::all_of( tagValue.begin(), tagValue.end(), isDigit );
		}

		return false;
	}

	//----------------------------------------------
	// Iteration
	//----------------------------------------------

	inline CodebookStandardValues::Iterator CodebookStandardValues::begin() const noexcept
	{
		return m_standardValues.begin();
	}

	inline CodebookStandardValues::Iterator CodebookStandardValues::end() const noexcept
	{
		return m_standardValues.end();
	}

	//=====================================================================
	// CodebookGroups class
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	inline CodebookGroups::CodebookGroups( StringSet&& groups ) noexcept
		: m_groups{ std::move( groups ) }
	{
	}

	//----------------------------------------------
	// Public methods
	//----------------------------------------------

	inline size_t CodebookGroups::count() const noexcept
	{
		return m_groups.size();
	}

	inline bool CodebookGroups::contains( std::string_view group ) const noexcept
	{
		return m_groups.contains( group );
	}

	//----------------------------------------------
	// Iteration
	//----------------------------------------------

	inline CodebookGroups::Iterator CodebookGroups::begin() const noexcept
	{
		return m_groups.begin();
	}

	inline CodebookGroups::Iterator CodebookGroups::end() const noexcept
	{
		return m_groups.end();
	}

	//=====================================================================
	// Codebook class
	//=====================================================================

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	constexpr inline CodebookName Codebook::name() const noexcept
	{
		return m_name;
	}

	inline const CodebookGroups& Codebook::groups() const noexcept
	{
		return m_groups;
	}

	inline const CodebookStandardValues& Codebook::standardValues() const noexcept
	{
		return m_standardValues;
	}

	inline const StringMap<std::vector<std::string>>& Codebook::rawData() const noexcept
	{
		return m_rawData;
	}

	//----------------------------------------------
	// State inspection methods
	//----------------------------------------------

	inline bool Codebook::hasGroup( std::string_view group ) const noexcept
	{
		return m_groups.contains( group );
	}

	inline bool Codebook::hasStandardValue( std::string_view value ) const noexcept
	{
		return m_standardValues.contains( value );
	}
}
