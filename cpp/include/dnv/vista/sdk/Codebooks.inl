/**
 * @file Codebooks.inl
 * @brief Inline implementation of Codebooks class methods
 */

#pragma once

namespace dnv::vista::sdk
{
	//=====================================================================
	// Codebooks class
	//=====================================================================

	//----------------------------------------------
	// Lookup operators
	//----------------------------------------------

	inline constexpr const Codebook& Codebooks::operator[]( CodebookName name ) const noexcept
	{
		return m_codebooks[static_cast<size_t>( name ) - 1];
	}

	//----------------------------------------------
	// Accessors
	//----------------------------------------------

	inline VisVersion Codebooks::visVersion() const noexcept
	{
		return m_visVersion;
	}

	inline const Codebook& Codebooks::codebook( CodebookName name ) const
	{
		const auto rawValue = static_cast<size_t>( name );

		if ( rawValue >= 1 && rawValue <= NUM_CODEBOOKS )
		{
			return m_codebooks[rawValue - 1];
		}

		throw std::invalid_argument( "Invalid codebook name: " + std::to_string( rawValue ) );
	}

	//----------------------------------------------
	// Range-based iterator
	//----------------------------------------------

	inline Codebooks::Iterator Codebooks::begin() const noexcept
	{
		return m_codebooks.data();
	}

	inline Codebooks::Iterator Codebooks::end() const noexcept
	{
		return m_codebooks.data() + NUM_CODEBOOKS;
	}

	//----------------------------------------------
	// Tag creation
	//----------------------------------------------

	inline std::optional<MetadataTag> Codebooks::tryCreateTag( CodebookName name, std::string_view value ) const
	{
		return m_codebooks[static_cast<size_t>( name ) - 1].tryCreateTag( value );
	}

	inline MetadataTag Codebooks::createTag( CodebookName name, const std::string& value ) const
	{
		return m_codebooks[static_cast<size_t>( name ) - 1].createTag( value );
	}
}
