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
 * @file GmodPathQuery.h
 * @brief Query API for matching GmodPath instances
 * @details Provides fluent API for building queries to filter GmodPaths based on nodes and locations.
 *          Supports path-based queries (starting from existing path) and node-based queries (from scratch).
 */

#pragma once

#include "GmodPath.h"
#include "Locations.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace dnv::vista::sdk
{
	class GmodPathQueryBuilder;

	/**
	 * @class GmodPathQuery
	 * @brief Immutable query for matching GmodPath instances
	 * @details Created via GmodPathQueryBuilder. Provides match() method to test GmodPaths.
	 *
	 * @example
	 * @code
	 * auto basePath = GmodPath::fromString("411.1/C101.31-2", version).value();
	 * auto query = GmodPathQueryBuilder::from(basePath)
	 *     .withoutLocations()
	 *     .build();
	 *
	 * bool matches = query.match(otherPath);
	 * @endcode
	 */
	class GmodPathQuery final
	{
		friend class GmodPathQueryBuilder;

	private:
		explicit GmodPathQuery( const GmodPathQueryBuilder* builder );

	public:
		/** @brief Copy constructor */
		GmodPathQuery( const GmodPathQuery& other );

		/** @brief Move constructor */
		GmodPathQuery( GmodPathQuery&& ) noexcept = default;

		/** @brief Destructor */
		~GmodPathQuery() = default;

		/** @brief Copy assignment */
		GmodPathQuery& operator=( const GmodPathQuery& other );

		/** @brief Move assignment */
		GmodPathQuery& operator=( GmodPathQuery&& ) noexcept = default;

		/**
		 * @brief Match against a GmodPath
		 * @param other GmodPath to test (nullptr returns false)
		 * @return true if path matches the query criteria
		 */
		[[nodiscard]] bool match( const GmodPath* other ) const;

		/**
		 * @brief Match against a GmodPath
		 * @param other GmodPath to test
		 * @return true if path matches the query criteria
		 */
		[[nodiscard]] bool match( const GmodPath& other ) const;

		/**
		 * @brief Get the builder that created this query
		 * @return Pointer to builder
		 */
		[[nodiscard]] const GmodPathQueryBuilder* builder() const noexcept;

	private:
		std::unique_ptr<GmodPathQueryBuilder> m_builder;
	};

	/**
	 * @class GmodPathQueryBuilder
	 * @brief Abstract base class for building GmodPath queries
	 * @details Two concrete implementations:
	 *          - Path: Start from existing GmodPath, modify specific nodes
	 *          - Nodes: Build from scratch, specify individual nodes
	 */
	class GmodPathQueryBuilder
	{
		friend class GmodPathQuery;

	private:
		/**
		 * @struct NodeItem
		 * @brief Internal filter item for node matching
		 */
		struct NodeItem
		{
			GmodNode node;
			std::vector<Location> locations;
			bool matchAllLocations = false;
			bool ignoreInMatching = false;

			NodeItem() = default;
			NodeItem( const GmodNode& n, const std::vector<Location>& locs );
		};

	protected:
		GmodPathQueryBuilder() = default;

	public:
		class Path;
		class Nodes;

		/** @brief Virtual destructor */
		virtual ~GmodPathQueryBuilder() = default;

		/**
		 * @brief Create empty Nodes builder
		 * @return New Nodes builder
		 */
		[[nodiscard]] static Nodes empty();

		/**
		 * @brief Create Path builder from existing GmodPath
		 * @param path GmodPath to use as base
		 * @return New Path builder
		 */
		[[nodiscard]] static Path from( const GmodPath& path );

		/**
		 * @brief Build immutable query
		 * @return GmodPathQuery instance
		 */
		[[nodiscard]] GmodPathQuery build() const;

		/**
		 * @brief Clone this builder (polymorphic copy)
		 * @return Unique pointer to cloned builder
		 */
		[[nodiscard]] virtual std::unique_ptr<GmodPathQueryBuilder> clone() const = 0;

		/**
		 * @brief Internal match implementation
		 * @param other GmodPath to test (nullptr returns false)
		 * @return true if path matches the query criteria
		 */
		[[nodiscard]] virtual bool match( const GmodPath* other ) const = 0;

	protected:
		std::unordered_map<std::string, NodeItem> m_filter;
	};

	/**
	 * @class GmodPathQueryBuilder::Path
	 * @brief Path-based query builder
	 */
	class GmodPathQueryBuilder::Path final : public GmodPathQueryBuilder
	{
	public:
		/** @brief Default constructor */
		Path() = delete;

		/** @brief Construct from GmodPath */
		explicit Path( const GmodPath& path );

		/** @brief Copy constructor */
		Path( const Path& ) = default;

		/** @brief Move constructor */
		Path( Path&& ) noexcept = default;

		/** @brief Destructor */
		~Path() override = default;

		/** @brief Copy assignment */
		Path& operator=( const Path& ) = default;

		/** @brief Move assignment */
		Path& operator=( Path&& ) noexcept = default;

		/**
		 * @brief Get the base GmodPath
		 * @return Reference to the GmodPath
		 */
		[[nodiscard]] const GmodPath& path() const noexcept;

		/**
		 * @brief Configure node to match all locations
		 * @param select Function to select node from set nodes dictionary
		 * @param matchAllLocations If true, match any location individualization
		 * @return Reference to this builder for chaining
		 */
		Path& withNode( std::function<const GmodNode*( const std::unordered_map<std::string, const GmodNode*>& )> select,
			bool matchAllLocations = false );

		/**
		 * @brief Configure node with specific locations
		 * @param select Function to select node from set nodes dictionary
		 * @param locations Locations to match (empty = no locations)
		 * @return Reference to this builder for chaining
		 */
		Path& withNode( std::function<const GmodNode*( const std::unordered_map<std::string, const GmodNode*>& )> select,
			const std::vector<Location>& locations );

		/**
		 * @brief Ignore nodes before specified node in path
		 * @param select Function to select node from full path nodes
		 * @return Reference to this builder for chaining
		 * @throws std::invalid_argument if node not in path
		 */
		Path& withAnyNodeBefore( std::function<const GmodNode*( const std::unordered_map<std::string, const GmodNode*>& )> select );

		/**
		 * @brief Ignore all location individualizations
		 * @return Reference to this builder for chaining
		 */
		Path& withoutLocations();

		/** @brief Internal match implementation */
		[[nodiscard]] bool match( const GmodPath* other ) const override;

		/** @brief Clone this builder */
		[[nodiscard]] std::unique_ptr<GmodPathQueryBuilder> clone() const override;

	private:
		GmodPath m_gmodPath;
		std::unordered_map<std::string, const GmodNode*> m_setNodes;
		std::unordered_map<std::string, const GmodNode*> m_nodes;
	};

	/**
	 * @class GmodPathQueryBuilder::Nodes
	 * @brief Node-based query builder (build from scratch)
	 */
	class GmodPathQueryBuilder::Nodes final : public GmodPathQueryBuilder
	{
		friend class GmodPathQueryBuilder;

	private:
		/** @brief Default constructor */
		Nodes() = default;

	public:
		/** @brief Copy constructor */
		Nodes( const Nodes& ) = default;

		/** @brief Move constructor */
		Nodes( Nodes&& ) noexcept = default;

		/** @brief Destructor */
		~Nodes() override = default;

		/** @brief Copy assignment */
		Nodes& operator=( const Nodes& ) = default;

		/** @brief Move assignment */
		Nodes& operator=( Nodes&& ) noexcept = default;

		/**
		 * @brief Add node to match with location settings
		 * @param node Node to match
		 * @param matchAllLocations If true, match any location individualization
		 * @return Reference to this builder for chaining
		 */
		Nodes& withNode( const GmodNode& node, bool matchAllLocations = false );

		/**
		 * @brief Add node to match with specific locations
		 * @param node Node to match
		 * @param locations Locations to match (empty = no locations)
		 * @return Reference to this builder for chaining
		 */
		Nodes& withNode( const GmodNode& node, const std::vector<Location>& locations );

		/** @brief Internal match implementation */
		[[nodiscard]] bool match( const GmodPath* other ) const override;

		/** @brief Clone this builder */
		[[nodiscard]] std::unique_ptr<GmodPathQueryBuilder> clone() const override;
	};
} // namespace dnv::vista::sdk
