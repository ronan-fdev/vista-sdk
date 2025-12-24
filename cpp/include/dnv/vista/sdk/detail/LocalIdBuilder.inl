/*
 * MIT License
 *
 * Copyright (c) 2025 DNV
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file LocalIdBuilder.inl
 * @brief Inline implementations for LocalIdBuilder class
 * @details Provides inline implementations for simple accessor methods.
 */

namespace dnv::vista::sdk
{
	inline bool LocalIdBuilder::operator==( const LocalIdBuilder& other ) const noexcept
	{
		if ( m_visVersion != other.m_visVersion )
		{
			return false;
		}

		return m_verboseMode == other.m_verboseMode &&
			   m_primaryItem == other.m_primaryItem && m_secondaryItem == other.m_secondaryItem &&
			   m_quantity == other.m_quantity && m_content == other.m_content &&
			   m_calculation == other.m_calculation && m_state == other.m_state &&
			   m_command == other.m_command && m_type == other.m_type &&
			   m_position == other.m_position && m_detail == other.m_detail;
	}

	inline bool LocalIdBuilder::operator!=( const LocalIdBuilder& other ) const noexcept
	{
		return !( *this == other );
	}

	inline std::optional<VisVersion> LocalIdBuilder::version() const noexcept
	{
		return m_visVersion;
	}

	inline bool LocalIdBuilder::isVerboseMode() const noexcept
	{
		return m_verboseMode;
	}

	inline bool LocalIdBuilder::isValid() const noexcept
	{
		return m_visVersion.has_value() && m_primaryItem.has_value() &&
			   ( m_quantity.has_value() || m_content.has_value() || m_calculation.has_value() ||
				   m_state.has_value() || m_command.has_value() || m_type.has_value() ||
				   m_position.has_value() || m_detail.has_value() );
	}

	inline bool LocalIdBuilder::isEmpty() const noexcept
	{
		return !m_visVersion.has_value() && !m_primaryItem.has_value() &&
			   !m_secondaryItem.has_value() && isEmptyMetadata();
	}

	inline bool LocalIdBuilder::isEmptyMetadata() const noexcept
	{
		return !m_quantity.has_value() && !m_content.has_value() && !m_calculation.has_value() &&
			   !m_state.has_value() && !m_command.has_value() && !m_type.has_value() &&
			   !m_position.has_value() && !m_detail.has_value();
	}

	inline bool LocalIdBuilder::hasCustomTag() const noexcept
	{
		return ( m_quantity.has_value() && m_quantity->isCustom() ) ||
			   ( m_content.has_value() && m_content->isCustom() ) ||
			   ( m_calculation.has_value() && m_calculation->isCustom() ) ||
			   ( m_state.has_value() && m_state->isCustom() ) ||
			   ( m_command.has_value() && m_command->isCustom() ) ||
			   ( m_type.has_value() && m_type->isCustom() ) ||
			   ( m_position.has_value() && m_position->isCustom() ) ||
			   ( m_detail.has_value() && m_detail->isCustom() );
	}

	inline const std::optional<GmodPath>& LocalIdBuilder::primaryItem() const noexcept
	{
		return m_primaryItem;
	}

	inline const std::optional<GmodPath>& LocalIdBuilder::secondaryItem() const noexcept
	{
		return m_secondaryItem;
	}

	inline std::vector<MetadataTag> LocalIdBuilder::metadataTags() const noexcept
	{
		std::vector<MetadataTag> tags;
		tags.reserve( 8 );

		if ( m_quantity.has_value() )
		{
			tags.push_back( *m_quantity );
		}
		if ( m_content.has_value() )
		{
			tags.push_back( *m_content );
		}
		if ( m_calculation.has_value() )
		{
			tags.push_back( *m_calculation );
		}
		if ( m_state.has_value() )
		{
			tags.push_back( *m_state );
		}
		if ( m_command.has_value() )
		{
			tags.push_back( *m_command );
		}
		if ( m_type.has_value() )
		{
			tags.push_back( *m_type );
		}
		if ( m_position.has_value() )
		{
			tags.push_back( *m_position );
		}
		if ( m_detail.has_value() )
		{
			tags.push_back( *m_detail );
		}
		return tags;
	}

	inline const std::optional<MetadataTag>& LocalIdBuilder::quantity() const noexcept
	{
		return m_quantity;
	}

	inline const std::optional<MetadataTag>& LocalIdBuilder::content() const noexcept
	{
		return m_content;
	}

	inline const std::optional<MetadataTag>& LocalIdBuilder::calculation() const noexcept
	{
		return m_calculation;
	}

	inline const std::optional<MetadataTag>& LocalIdBuilder::state() const noexcept
	{
		return m_state;
	}

	inline const std::optional<MetadataTag>& LocalIdBuilder::command() const noexcept
	{
		return m_command;
	}

	inline const std::optional<MetadataTag>& LocalIdBuilder::type() const noexcept
	{
		return m_type;
	}

	inline const std::optional<MetadataTag>& LocalIdBuilder::position() const noexcept
	{
		return m_position;
	}

	inline const std::optional<MetadataTag>& LocalIdBuilder::detail() const noexcept
	{
		return m_detail;
	}
} // namespace dnv::vista::sdk
