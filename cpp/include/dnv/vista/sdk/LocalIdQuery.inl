/**
 * @file LocalIdQuery.inl
 * @brief Inline implementations for performance-critical LocalIdQuery operations
 */

namespace dnv::vista::sdk
{
	//=====================================================================
	// LocalIdQueryBuilder class
	//=====================================================================

	//----------------------------------------------
	// Factory methods
	//----------------------------------------------

	inline LocalIdQueryBuilder LocalIdQueryBuilder::empty()
	{
		return LocalIdQueryBuilder{};
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline std::optional<GmodPath> LocalIdQueryBuilder::primaryItem() const
	{
		if ( m_primaryItem.has_value() )
		{
			return m_primaryItem->sourcePath();
		}
		return std::nullopt;
	}

	inline std::optional<GmodPath> LocalIdQueryBuilder::secondaryItem() const
	{
		if ( m_secondaryItem.has_value() )
		{
			return m_secondaryItem->sourcePath();
		}
		return std::nullopt;
	}
}
