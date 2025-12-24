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
 * @file LocalId.inl
 * @brief Inline implementations for LocalId class
 * @details Provides inline implementations for LocalId accessor methods.
 *          All accessors delegate to the internal builder.
 */

namespace dnv::vista::sdk
{
	inline bool LocalId::operator==( const LocalId& other ) const noexcept
	{
		return m_builder == other.m_builder;
	}

	inline bool LocalId::operator!=( const LocalId& other ) const noexcept
	{
		return m_builder != other.m_builder;
	}

	inline VisVersion LocalId::version() const noexcept
	{
		return *m_builder.version();
	}

	inline bool LocalId::isVerboseMode() const noexcept
	{
		return m_builder.isVerboseMode();
	}

	inline const GmodPath& LocalId::primaryItem() const noexcept
	{
		return *m_builder.primaryItem();
	}

	inline const std::optional<GmodPath>& LocalId::secondaryItem() const noexcept
	{
		return m_builder.secondaryItem();
	}

	inline const std::optional<MetadataTag>& LocalId::quantity() const noexcept
	{
		return m_builder.quantity();
	}

	inline const std::optional<MetadataTag>& LocalId::content() const noexcept
	{
		return m_builder.content();
	}

	inline const std::optional<MetadataTag>& LocalId::calculation() const noexcept
	{
		return m_builder.calculation();
	}

	inline const std::optional<MetadataTag>& LocalId::state() const noexcept
	{
		return m_builder.state();
	}

	inline const std::optional<MetadataTag>& LocalId::command() const noexcept
	{
		return m_builder.command();
	}

	inline const std::optional<MetadataTag>& LocalId::type() const noexcept
	{
		return m_builder.type();
	}

	inline const std::optional<MetadataTag>& LocalId::position() const noexcept
	{
		return m_builder.position();
	}

	inline const std::optional<MetadataTag>& LocalId::detail() const noexcept
	{
		return m_builder.detail();
	}

	inline std::vector<MetadataTag> LocalId::metadataTags() const
	{
		return m_builder.metadataTags();
	}

	inline bool LocalId::hasCustomTag() const noexcept
	{
		return m_builder.hasCustomTag();
	}

	inline const LocalIdBuilder& LocalId::builder() const noexcept
	{
		return m_builder;
	}
} // namespace dnv::vista::sdk
