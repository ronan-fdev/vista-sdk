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
 * @file GmodNode.inl
 * @brief Inline implementations for GmodNodeMetadata and GmodNode
 */

#pragma once

namespace dnv::vista::sdk
{
	inline bool GmodNodeMetadata::operator==( const GmodNodeMetadata& other ) const
	{
		return m_category == other.m_category &&
			   m_type == other.m_type &&
			   m_name == other.m_name &&
			   m_commonName == other.m_commonName &&
			   m_definition == other.m_definition &&
			   m_commonDefinition == other.m_commonDefinition &&
			   m_installSubstructure == other.m_installSubstructure &&
			   m_normalAssignmentNames == other.m_normalAssignmentNames;
	}

	inline bool GmodNodeMetadata::operator!=( const GmodNodeMetadata& other ) const
	{
		return !( *this == other );
	}

	inline std::string_view GmodNodeMetadata::category() const noexcept
	{
		return m_category;
	}

	inline std::string_view GmodNodeMetadata::type() const noexcept
	{
		return m_type;
	}

	inline std::string_view GmodNodeMetadata::fullType() const noexcept
	{
		return m_fullType;
	}

	inline std::string_view GmodNodeMetadata::name() const noexcept
	{
		return m_name;
	}

	inline const std::optional<std::string>& GmodNodeMetadata::commonName() const noexcept
	{
		return m_commonName;
	}

	inline const std::optional<std::string>& GmodNodeMetadata::definition() const noexcept
	{
		return m_definition;
	}

	inline const std::optional<std::string>& GmodNodeMetadata::commonDefinition() const noexcept
	{
		return m_commonDefinition;
	}

	inline const std::optional<bool>& GmodNodeMetadata::installSubstructure() const noexcept
	{
		return m_installSubstructure;
	}

	inline const std::unordered_map<std::string, std::string, StringHash, std::equal_to<>>& GmodNodeMetadata::normalAssignmentNames() const noexcept
	{
		return m_normalAssignmentNames;
	}

	inline bool GmodNode::operator==( const GmodNode& other ) const noexcept
	{
		if ( m_code != other.m_code )
		{
			return false;
		}

		if ( !m_location.has_value() && !other.m_location.has_value() )
		{
			return true;
		}

		return m_location == other.m_location;
	}

	inline bool GmodNode::operator!=( const GmodNode& other ) const noexcept
	{
		return !( *this == other );
	}

	inline VisVersion GmodNode::version() const noexcept
	{
		return m_visVersion;
	}

	inline std::string_view GmodNode::code() const noexcept
	{
		return m_code;
	}

	inline const std::optional<Location>& GmodNode::location() const noexcept
	{
		return m_location;
	}

	inline const GmodNodeMetadata& GmodNode::metadata() const noexcept
	{
		return m_metadata;
	}

	inline const std::vector<GmodNode*>& GmodNode::children() const noexcept
	{
		return m_children;
	}

	inline const std::vector<GmodNode*>& GmodNode::parents() const noexcept
	{
		return m_parents;
	}

	inline bool GmodNode::isRoot() const noexcept
	{
		return m_code == "VE";
	}

	inline bool GmodNode::isChild( const GmodNode& node ) const noexcept
	{
		return isChild( node.m_code );
	}

	inline bool GmodNode::isChild( std::string_view code ) const noexcept
	{
		if ( !m_childrenSet )
		{
			return false;
		}
		return m_childrenSet->contains( code );
	}
} // namespace dnv::vista::sdk
