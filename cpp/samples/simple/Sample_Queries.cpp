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
 * @file Sample_Queries.cpp
 * @brief Demonstrates VIS Query APIs: MetadataTagsQuery, GmodPathQuery, and LocalIdQuery
 * @details Shows how to use the three query classes to match LocalIds, GmodPaths, and metadata tags.
 *          These queries enable declarative pattern matching for filtering and searching VIS data.
 */

#include <dnv/vista/sdk/VIS.h>

#include <iomanip>
#include <iostream>
#include <string>

int main()
{
	using namespace dnv::vista::sdk;

	std::cout << "=== vista-sdk-cpp Query APIs Sample ===\n\n";

	//=====================================================================
	// 1. MetadataTagsQuery: Matching LocalIds by metadata tags
	//=====================================================================
	{
		std::cout << "1. MetadataTagsQuery: Matching LocalIds by metadata tags\n";
		std::cout << "-----------------------------------------------------------\n";

		// Build a query for LocalIds with quantity='temperature'
		auto query = MetadataTagsQueryBuilder::empty()
						 .withTag( CodebookName::Quantity, "temperature" )
						 .build();

		// Test against some LocalIds
		auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas" );
		auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.63/meta/qty-pressure/cnt-air" );

		std::cout << "Query: Match LocalIds with qty-temperature tag\n\n";
		std::cout << "Test LocalId 1: " << localId1->toString() << "\n";
		std::cout << "  Matches: " << std::boolalpha << query.match( *localId1 ) << "\n\n";

		std::cout << "Test LocalId 2: " << localId2->toString() << "\n";
		std::cout << "  Matches: " << std::boolalpha << query.match( *localId2 ) << "\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 2. MetadataTagsQuery: Multiple tags with AND semantics
	//=====================================================================
	{
		std::cout << "2. MetadataTagsQuery: Multiple tags with AND semantics\n";
		std::cout << "---------------------------------------------------------\n";

		// Build query requiring BOTH tags
		auto query = MetadataTagsQueryBuilder::empty()
						 .withTag( CodebookName::Quantity, "volume" )
						 .withTag( CodebookName::Content, "cargo" )
						 .build();

		auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage" );
		auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-ballast.water" );

		std::cout << "Query: Match LocalIds with qty-volume AND cnt-cargo\n\n";
		std::cout << "Test LocalId 1: " << localId1->toString() << "\n";
		std::cout << "  Has qty-volume: true, Has cnt-cargo: true\n";
		std::cout << "  Matches: " << std::boolalpha << query.match( *localId1 ) << "\n\n";

		std::cout << "Test LocalId 2: " << localId2->toString() << "\n";
		std::cout << "  Has qty-volume: true, Has cnt-cargo: false\n";
		std::cout << "  Matches: " << std::boolalpha << query.match( *localId2 ) << "\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 3. MetadataTagsQuery: Subset vs Exact matching
	//=====================================================================
	{
		std::cout << "3. MetadataTagsQuery: Subset vs Exact matching\n";
		std::cout << "-------------------------------------------------\n";

		// Subset matching (default): LocalId can have EXTRA tags beyond those in query
		auto subsetQuery = MetadataTagsQueryBuilder::empty()
							   .withTag( CodebookName::Quantity, "pressure" )
							   .withAllowOtherTags( true ) // Allow extra tags (this is default)
							   .build();

		// Exact matching: LocalId must have EXACTLY the tags in query (no more, no less)
		auto exactQuery = MetadataTagsQueryBuilder::empty()
							  .withTag( CodebookName::Quantity, "pressure" )
							  .withAllowOtherTags( false ) // No extra tags allowed
							  .build();

		auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.63/meta/qty-pressure" );
		auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.63/meta/qty-pressure/cnt-air" );

		std::cout << "Query: Match LocalIds with qty-pressure\n\n";

		std::cout << "Test LocalId 1 (only qty-pressure): " << localId1->toString() << "\n";
		std::cout << "  Subset query matches: " << std::boolalpha << subsetQuery.match( *localId1 ) << "\n";
		std::cout << "  Exact query matches: " << std::boolalpha << exactQuery.match( *localId1 ) << "\n\n";

		std::cout << "Test LocalId 2 (qty-pressure + cnt-air): " << localId2->toString() << "\n";
		std::cout << "  Subset query matches: " << std::boolalpha << subsetQuery.match( *localId2 ) << " (allows extra tags)\n";
		std::cout << "  Exact query matches: " << std::boolalpha << exactQuery.match( *localId2 ) << " (no extra tags allowed)\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 4. GmodPathQuery: Matching paths without locations
	//=====================================================================
	{
		std::cout << "4. GmodPathQuery: Matching paths without locations\n";
		std::cout << "-----------------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto version = vis.latest();
		const auto& gmod = vis.gmod( version );
		const auto& locations = vis.locations( version );

		// Create base path and query that ignores locations
		auto basePath = GmodPath::fromString( "411.1/C101.31", gmod, locations );
		auto query = GmodPathQueryBuilder::Path::from( *basePath )
						 .withoutLocations()
						 .build();

		// Test against paths with different locations
		auto path1 = GmodPath::fromString( "411.1/C101.31-2", gmod, locations );
		auto path2 = GmodPath::fromString( "411.1/C101.31-5", gmod, locations );
		auto path3 = GmodPath::fromString( "411.1/C101.63", gmod, locations );

		std::cout << "Query: Match '411.1/C101.31' ignoring locations\n\n";

		std::cout << "Test path 1: " << path1->toString() << "\n";
		std::cout << "  Matches: " << std::boolalpha << query.match( *path1 ) << "\n\n";

		std::cout << "Test path 2: " << path2->toString() << "\n";
		std::cout << "  Matches: " << std::boolalpha << query.match( *path2 ) << "\n\n";

		std::cout << "Test path 3: " << path3->toString() << "\n";
		std::cout << "  Matches: " << std::boolalpha << query.match( *path3 ) << " (different node)\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 5. GmodPathQuery: Matching specific nodes
	//=====================================================================
	{
		std::cout << "5. GmodPathQuery: Matching specific nodes\n";
		std::cout << "--------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto version = vis.latest();
		const auto& gmod = vis.gmod( version );
		const auto& locations = vis.locations( version );

		// Get nodes to query
		auto node411Opt = gmod.node( "411.1" );
		auto nodeC101Opt = gmod.node( "C101" );

		if ( node411Opt.has_value() && nodeC101Opt.has_value() )
		{
			const auto* node411 = *node411Opt;
			const auto* nodeC101 = *nodeC101Opt;

			// Build query for paths containing these nodes (any locations)
			auto nodes = GmodPathQueryBuilder::empty();
			auto query = nodes
							 .withNode( *node411, true )
							 .withNode( *nodeC101, true )
							 .build();

			// Test paths
			auto path1 = GmodPath::fromString( "411.1-1P/C101.31-2", gmod, locations );
			auto path2 = GmodPath::fromString( "411.1-2P/C101.63", gmod, locations );
			auto path3 = GmodPath::fromString( "412.1/C101.31", gmod, locations );

			std::cout << "Query: Match paths containing nodes '411.1' AND 'C101' (any locations)\n\n";

			std::cout << "Test path 1: " << path1->toString() << "\n";
			std::cout << "  Matches: " << std::boolalpha << query.match( *path1 ) << "\n\n";

			std::cout << "Test path 2: " << path2->toString() << "\n";
			std::cout << "  Matches: " << std::boolalpha << query.match( *path2 ) << "\n\n";

			std::cout << "Test path 3: " << ( path3 ? path3->toString() : "invalid path" ) << "\n";
			std::cout << "  Matches: " << std::boolalpha << ( path3 ? query.match( *path3 ) : false ) << " (different first node)\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 6. LocalIdQuery: Matching by primary item
	//=====================================================================
	{
		std::cout << "6. LocalIdQuery: Matching by primary item\n";
		std::cout << "--------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto version = vis.latest();
		const auto& gmod = vis.gmod( version );
		const auto& locations = vis.locations( version );

		// Build query for specific primary item (ignoring locations)
		auto primaryPath = GmodPath::fromString( "411.1/C101.31", gmod, locations );
		auto pathQuery = GmodPathQueryBuilder::Path::from( *primaryPath )
							 .withoutLocations()
							 .build();

		auto query = LocalIdQueryBuilder::create()
						 .withPrimaryItem( pathQuery )
						 .build();

		// Test LocalIds
		auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature" );
		auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-5/meta/qty-pressure" );
		auto localId3 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.63/meta/qty-temperature" );

		std::cout << "Query: Match LocalIds with primary item '411.1/C101.31' (any location)\n\n";

		std::cout << "Test LocalId 1: " << localId1->toString() << "\n";
		std::cout << "  Matches: " << std::boolalpha << query.match( *localId1 ) << "\n\n";

		std::cout << "Test LocalId 2: " << localId2->toString() << "\n";
		std::cout << "  Matches: " << std::boolalpha << query.match( *localId2 ) << "\n\n";

		std::cout << "Test LocalId 3: " << localId3->toString() << "\n";
		std::cout << "  Matches: " << std::boolalpha << query.match( *localId3 ) << " (different node)\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 7. LocalIdQuery: Combined query (primary item + tags)
	//=====================================================================
	{
		std::cout << "7. LocalIdQuery: Combined query (primary item + tags)\n";
		std::cout << "-------------------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto version = vis.latest();
		const auto& gmod = vis.gmod( version );
		const auto& locations = vis.locations( version );

		// Get nodes for path query
		auto node411Opt = gmod.node( "411.1" );
		auto nodeC101Opt = gmod.node( "C101" );

		if ( node411Opt.has_value() && nodeC101Opt.has_value() )
		{
			const auto* node411 = *node411Opt;
			const auto* nodeC101 = *nodeC101Opt;

			// Build path query
			auto nodes = GmodPathQueryBuilder::empty();
			auto pathQuery = nodes
								 .withNode( *node411, true )
								 .withNode( *nodeC101, true )
								 .build();

			// Build tags query
			auto tagsQuery = MetadataTagsQueryBuilder::empty()
								 .withTag( CodebookName::Quantity, "temperature" )
								 .build();

			// Combine in LocalIdQuery
			auto query = LocalIdQueryBuilder::create()
							 .withPrimaryItem( pathQuery )
							 .withTags( tagsQuery )
							 .build();

			// Test LocalIds
			auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature" );
			auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-pressure" );

			std::cout << "Query: Match LocalIds with primary='411.1/C101' AND qty-temperature\n\n";

			std::cout << "Test LocalId 1: " << localId1->toString() << "\n";
			std::cout << "  Matches: " << std::boolalpha << query.match( *localId1 ) << " (has both)\n\n";

			std::cout << "Test LocalId 2: " << localId2->toString() << "\n";
			std::cout << "  Matches: " << std::boolalpha << query.match( *localId2 ) << " (missing tag)\n\n";

			auto localId3 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.63/meta/qty-temperature" );
			std::cout << "Test LocalId 3: " << localId3->toString() << "\n";
			std::cout << "  Matches: " << std::boolalpha << query.match( *localId3 ) << " (wrong node)\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 8. LocalIdQuery: Secondary item requirement
	//=====================================================================
	{
		std::cout << "8. LocalIdQuery: Secondary item requirement\n";
		std::cout << "----------------------------------------------\n";

		// Query requiring NO secondary item
		auto query1 = LocalIdQueryBuilder::create()
						  .withoutSecondaryItem()
						  .build();

		auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature" );
		auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/621.21/S90/sec/411.1/C101/meta/qty-mass" );

		std::cout << "Query: Match LocalIds WITHOUT secondary item\n\n";
		std::cout << "Test LocalId 1 (no secondary): " << localId1->toString() << "\n";
		std::cout << "  Matches: " << std::boolalpha << query1.match( *localId1 ) << "\n\n";

		std::cout << "Test LocalId 2 (has secondary): " << localId2->toString() << "\n";
		std::cout << "  Matches: " << std::boolalpha << query1.match( *localId2 ) << "\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 9. Building queries from existing LocalIds
	//=====================================================================
	{
		std::cout << "9. Building queries from existing LocalIds\n";
		std::cout << "---------------------------------------------\n";

		// Parse a reference LocalId
		auto referenceLocalId = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );

		// Build query that matches this exact LocalId
		auto query = LocalIdQueryBuilder::from( *referenceLocalId ).build();

		// Test against various LocalIds
		auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
		auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-5/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" );
		auto localId3 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-pressure/cnt-exhaust.gas/pos-inlet" );

		std::cout << "Reference LocalId: " << referenceLocalId->toString() << "\n\n";
		std::cout << "Query: Match exact copy of reference LocalId\n\n";

		std::cout << "Test LocalId 1 (exact match): \n  Matches: " << std::boolalpha << query.match( *localId1 ) << "\n\n";
		std::cout << "Test LocalId 2 (different location): \n  Matches: " << std::boolalpha << query.match( *localId2 ) << "\n\n";
		std::cout << "Test LocalId 3 (different tag): \n  Matches: " << std::boolalpha << query.match( *localId3 ) << "\n";

		std::cout << "\n";
	}

	//=====================================================================
	// 10. LocalIdQuery: withPrimaryItem using Nodes builder lambda
	//=====================================================================
	{
		std::cout << "10. LocalIdQuery: withPrimaryItem using Nodes builder lambda\n";
		std::cout << "--------------------------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto version = vis.latest();
		const auto& gmod = vis.gmod( version );

		// Get nodes
		auto node411Opt = gmod.node( "411.1" );
		auto nodeC101Opt = gmod.node( "C101" );

		if ( node411Opt.has_value() && nodeC101Opt.has_value() )
		{
			const auto* node411 = *node411Opt;
			const auto* nodeC101 = *nodeC101Opt;

			// Build query using Nodes builder lambda (builds path from scratch)
			auto query = LocalIdQueryBuilder::create()
							 .withPrimaryItem( [node411, nodeC101]( GmodPathQueryBuilder::Nodes& builder ) {
								 return builder
									 .withNode( *node411, true )
									 .withNode( *nodeC101, true )
									 .build();
							 } )
							 .build();

			auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature" );
			auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.63/meta/qty-pressure" );
			auto localId3 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C102.31/meta/qty-temperature" );

			std::cout << "Query: Match LocalIds with primary containing nodes 411.1 AND C101 (any locations)\n\n";

			std::cout << "Test LocalId 1: " << localId1->toString() << "\n";
			std::cout << "  Matches: " << std::boolalpha << query.match( *localId1 ) << "\n\n";

			std::cout << "Test LocalId 2: " << localId2->toString() << "\n";
			std::cout << "  Matches: " << std::boolalpha << query.match( *localId2 ) << "\n\n";

			std::cout << "Test LocalId 3: " << localId3->toString() << "\n";
			std::cout << "  Matches: " << std::boolalpha << query.match( *localId3 ) << " (wrong second node)\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 11. LocalIdQuery: withPrimaryItem using base path + Path builder lambda
	//=====================================================================
	{
		std::cout << "11. LocalIdQuery: withPrimaryItem using base path + Path builder lambda\n";
		std::cout << "------------------------------------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto version = vis.latest();
		const auto& gmod = vis.gmod( version );
		const auto& locations = vis.locations( version );

		// Parse base path
		auto basePath = GmodPath::fromString( "411.1/C101.31", gmod, locations );

		if ( basePath.has_value() )
		{
			// Build query using base path + Path builder lambda (modifies existing path)
			auto query = LocalIdQueryBuilder::create()
							 .withPrimaryItem( *basePath, []( GmodPathQueryBuilder::Path& builder ) {
								 return builder.withoutLocations().build();
							 } )
							 .build();

			auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature" );
			auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-5/meta/qty-pressure" );
			auto localId3 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.63/meta/qty-temperature" );

			std::cout << "Query: Match LocalIds with primary '411.1/C101.31' (any location) using Path lambda\n\n";

			std::cout << "Test LocalId 1: " << localId1->toString() << "\n";
			std::cout << "  Matches: " << std::boolalpha << query.match( *localId1 ) << "\n\n";

			std::cout << "Test LocalId 2: " << localId2->toString() << "\n";
			std::cout << "  Matches: " << std::boolalpha << query.match( *localId2 ) << "\n\n";

			std::cout << "Test LocalId 3: " << localId3->toString() << "\n";
			std::cout << "  Matches: " << std::boolalpha << query.match( *localId3 ) << " (different node)\n";
		}

		std::cout << "\n";
	}

	//=====================================================================
	// 12. LocalIdQuery: withAnyNodeBefore usage
	//=====================================================================
	{
		std::cout << "12. LocalIdQuery: withAnyNodeBefore usage\n";
		std::cout << "--------------------------------------------\n";

		const auto& vis = VIS::instance();
		const auto version = vis.latest();
		const auto& gmod = vis.gmod( version );
		const auto& locations = vis.locations( version );

		// Get target node
		auto nodeC101Opt = gmod.node( "C101" );

		if ( nodeC101Opt.has_value() )
		{
			const auto* nodeC101 = *nodeC101Opt;

			// Parse base path
			auto basePath = GmodPath::fromString( "411.1/C101.31", gmod, locations );

			if ( basePath.has_value() )
			{
				// Build query that matches paths with ANY node before C101
				auto query = LocalIdQueryBuilder::create()
								 .withPrimaryItem( *basePath, [nodeC101]( GmodPathQueryBuilder::Path& builder ) {
									 return builder
										 .withAnyNodeBefore( [nodeC101]( const std::unordered_map<std::string, const GmodNode*>& nodes ) -> const GmodNode* {
											 // Return C101 node - any node before it will be ignored
											 return nodeC101;
										 } )
										 .withoutLocations()
										 .build();
								 } )
								 .build();

				auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature" );
				auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/E202.1/E31/meta/qty-pressure" );
				auto localId3 = LocalId::fromString( "/dnv-v2/vis-3-4a/621.21/S90/meta/qty-temperature" );

				std::cout << "Query: Match LocalIds with any node before C101 (ignoring nodes before C101)\n\n";

				std::cout << "Test LocalId 1 (411.1): " << localId1->toString() << "\n";
				std::cout << "  Matches: " << std::boolalpha << query.match( *localId1 ) << "\n\n";

				std::cout << "Test LocalId 2 (411.1/E202.1): " << localId2->toString() << "\n";
				std::cout << "  Matches: " << std::boolalpha << query.match( *localId2 ) << " (no C101)\n\n";

				std::cout << "Test LocalId 3 (621.21): " << localId3->toString() << "\n";
				std::cout << "  Matches: " << std::boolalpha << query.match( *localId3 ) << " (no C101)\n";
			}
		}

		std::cout << "\n";
	}
	return 0;
}
