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
 * @file TESTS_LocalIdQuery.cpp
 * @brief Unit tests for LocalIdQuery and LocalIdQueryBuilder functionality
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

#include <fstream>
#include <string>
#include <vector>

namespace dnv::vista::sdk::test
{
	using namespace dnv::vista::sdk;

	class LocalIdQueryTests : public ::testing::Test
	{
	};

	TEST_F( LocalIdQueryTests, EmptyQueryMatchesAll )
	{
		auto localIdStr = "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-power";
		auto localId = LocalId::fromString( localIdStr );
		ASSERT_TRUE( localId.has_value() );

		auto query = LocalIdQueryBuilder::create().build();
		EXPECT_TRUE( query.match( *localId ) );
	}

	TEST_F( LocalIdQueryTests, FromLocalId )
	{
		auto localIdStr = "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-power";
		auto localId = LocalId::fromString( localIdStr );
		ASSERT_TRUE( localId.has_value() );

		auto builder = LocalIdQueryBuilder::from( *localId );
		auto query = builder.build();
		EXPECT_TRUE( query.match( *localId ) );
	}

	TEST_F( LocalIdQueryTests, MatchGmodPathWithoutLocations )
	{
		const auto& vis = VIS::instance();
		auto version = VisVersion::v3_4a;
		const auto& gmod = vis.gmod( version );
		const auto& locations = vis.locations( version );

		auto path = GmodPath::fromString( "411.1/C101.31", gmod, locations );
		ASSERT_TRUE( path.has_value() );

		auto query = LocalIdQueryBuilder::create()
						 .withPrimaryItem( *path, []( GmodPathQueryBuilder::Path& pathBuilder ) {
							 return pathBuilder.withoutLocations().build();
						 } )
						 .build();

		auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-power" );
		ASSERT_TRUE( localId1.has_value() );
		EXPECT_TRUE( query.match( *localId1 ) );

		auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1-1/C101.31/meta/qty-power" );
		ASSERT_TRUE( localId2.has_value() );
		EXPECT_TRUE( query.match( *localId2 ) );

		auto localId3 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C102.31/meta/qty-power" );
		ASSERT_TRUE( localId3.has_value() );
		EXPECT_FALSE( query.match( *localId3 ) );
	}

	TEST_F( LocalIdQueryTests, MatchByMetadataTags )
	{
		const auto& vis = VIS::instance();
		auto version = VisVersion::v3_4a;
		const auto& codebooks = vis.codebooks( version );

		auto qtyTag = codebooks[CodebookName::Quantity].createTag( "power" );
		ASSERT_TRUE( qtyTag.has_value() );

		auto query = LocalIdQueryBuilder::create()
						 .withTags( [&qtyTag]( MetadataTagsQueryBuilder& tags ) {
							 return tags.withTag( *qtyTag ).build();
						 } )
						 .build();

		auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-power" );
		ASSERT_TRUE( localId1.has_value() );
		EXPECT_TRUE( query.match( *localId1 ) );

		auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-temperature" );
		ASSERT_TRUE( localId2.has_value() );
		EXPECT_FALSE( query.match( *localId2 ) );
	}

	TEST_F( LocalIdQueryTests, WithNodeQuery )
	{
		const auto& vis = VIS::instance();
		auto version = VisVersion::v3_4a;
		const auto& gmod = vis.gmod( version );

		auto nodeC101 = gmod.node( "C101" );
		ASSERT_TRUE( nodeC101.has_value() );

		auto query = LocalIdQueryBuilder::create()
						 .withPrimaryItem( [&nodeC101]( GmodPathQueryBuilder::Nodes& nodes ) {
							 return nodes.withNode( **nodeC101 ).build();
						 } )
						 .build();

		auto localId1 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C101.31/meta/qty-power" );
		ASSERT_TRUE( localId1.has_value() );
		EXPECT_TRUE( query.match( *localId1 ) );

		auto localId2 = LocalId::fromString( "/dnv-v2/vis-3-4a/411.1/C102.31/meta/qty-power" );
		ASSERT_TRUE( localId2.has_value() );
		EXPECT_FALSE( query.match( *localId2 ) );
	}

	TEST_F( LocalIdQueryTests, WithoutSecondaryItem )
	{
		auto baseLocalId = LocalId::fromString( "/dnv-v2/vis-3-9a/411.1/C101.31/meta/qty-power" );
		ASSERT_TRUE( baseLocalId.has_value() );

		auto queryBuilder = LocalIdQueryBuilder::from( *baseLocalId );
		auto otherLocalId = LocalId::fromString( "/dnv-v2/vis-3-9a/411.1/C101.31/sec/412.3/meta/qty-power" );
		ASSERT_TRUE( otherLocalId.has_value() );

		auto query = queryBuilder.build();
		EXPECT_TRUE( query.match( *baseLocalId ) );
		EXPECT_FALSE( query.match( *otherLocalId ) );

		query = queryBuilder.withoutSecondaryItem().build();
		EXPECT_FALSE( query.match( *otherLocalId ) );

		query = queryBuilder.withAnySecondaryItem().build();
		EXPECT_TRUE( query.match( *otherLocalId ) );
	}

	TEST_F( LocalIdQueryTests, Variations )
	{
		auto localId = LocalId::fromString(
			"/dnv-v2/vis-3-4a/1036.13i-1/C662/sec/411.1-2/C101/meta/qty-pressure/cnt-cargo/state-high.high/pos-stage-3/detail-discharge" );
		ASSERT_TRUE( localId.has_value() );

		const auto& vis = VIS::instance();
		auto version = VisVersion::v3_4a;
		const auto& gmod = vis.gmod( version );
		const auto& locations = vis.locations( version );

		auto primaryItem = GmodPath::fromString( "1036.13i-2/C662", gmod, locations );
		ASSERT_TRUE( primaryItem.has_value() );

		auto builder = LocalIdQueryBuilder::create().withPrimaryItem( *primaryItem );
		auto query = builder.build();
		EXPECT_FALSE( query.match( *localId ) );

		builder = builder.withPrimaryItem( *primaryItem, []( GmodPathQueryBuilder::Path& pathBuilder ) {
			return pathBuilder.withoutLocations().build();
		} );
		query = builder.build();
		EXPECT_TRUE( query.match( *localId ) );

		auto primaryItemNoLoc = primaryItem->withoutLocations();
		builder = LocalIdQueryBuilder::create().withPrimaryItem( primaryItemNoLoc );
		query = builder.build();
		EXPECT_FALSE( query.match( *localId ) );

		builder = builder.withPrimaryItem( primaryItemNoLoc, []( GmodPathQueryBuilder::Path& pathBuilder ) {
			return pathBuilder.withoutLocations().build();
		} );
		query = builder.build();
		EXPECT_TRUE( query.match( *localId ) );
	}

	TEST_F( LocalIdQueryTests, WithAnyNodeBefore )
	{
		const auto& vis = VIS::instance();
		auto version = VisVersion::v3_9a;
		const auto& codebooks = vis.codebooks( version );
		const auto& gmod = vis.gmod( version );
		const auto& locations = vis.locations( version );

		auto basePath = GmodPath::fromString( "411.1/C101.31", gmod, locations );
		ASSERT_TRUE( basePath.has_value() );

		auto baseLocalId = LocalId::fromString( "/dnv-v2/vis-3-9a/411.1/C101.31/sec/412.3/meta/qty-power" );
		ASSERT_TRUE( baseLocalId.has_value() );

		auto nodeC101 = gmod.node( "C101" );
		ASSERT_TRUE( nodeC101.has_value() );
		const auto* pNodeC101 = *nodeC101;

		auto specificQuery = LocalIdQueryBuilder::from( *baseLocalId )
								 .withPrimaryItem( baseLocalId->primaryItem(), [pNodeC101]( GmodPathQueryBuilder::Path& path ) {
									 return path.withAnyNodeBefore( [pNodeC101]( const std::unordered_map<std::string, const GmodNode*>& nodes ) {
													return pNodeC101;
												} )
										 .build();
								 } )
								 .build();

		auto qtyPowerTag = codebooks[CodebookName::Quantity].createTag( "power" );
		ASSERT_TRUE( qtyPowerTag.has_value() );

		auto generalQuery = LocalIdQueryBuilder::create()
								.withPrimaryItem( *basePath, [pNodeC101]( GmodPathQueryBuilder::Path& path ) {
									return path.withAnyNodeBefore( [pNodeC101]( const std::unordered_map<std::string, const GmodNode*>& nodes ) {
												   return pNodeC101;
											   } )
										.build();
								} )
								.withTags( [&qtyPowerTag]( MetadataTagsQueryBuilder& tags ) {
									return tags.withTag( *qtyPowerTag ).build();
								} )
								.build();

		EXPECT_TRUE( generalQuery.match( *baseLocalId ) );

		auto l2 = LocalId::fromString( "/dnv-v2/vis-3-9a/411.1/C101.31/sec/412.3/meta/qty-power" );
		ASSERT_TRUE( l2.has_value() );
		EXPECT_TRUE( specificQuery.match( *l2 ) );
		EXPECT_TRUE( generalQuery.match( *l2 ) );

		auto l3 = LocalId::fromString( "/dnv-v2/vis-3-9a/411.1/C101.31/meta/qty-power" );
		ASSERT_TRUE( l3.has_value() );
		EXPECT_FALSE( specificQuery.match( *l3 ) );
		EXPECT_TRUE( generalQuery.match( *l3 ) );

		auto l4 = LocalId::fromString( "/dnv-v2/vis-3-9a/411.1/C102.31/meta/qty-power" );
		ASSERT_TRUE( l4.has_value() );
		EXPECT_FALSE( generalQuery.match( *l4 ) );
		EXPECT_FALSE( specificQuery.match( *l4 ) );

		auto l5 = LocalId::fromString( "/dnv-v2/vis-3-9a/411.1/C101.31/sec/412.2/meta/qty-power" );
		ASSERT_TRUE( l5.has_value() );
		EXPECT_FALSE( specificQuery.match( *l5 ) );
		EXPECT_TRUE( generalQuery.match( *l5 ) );

		auto l6 = LocalId::fromString( "/dnv-v2/vis-3-9a/411.1/C101.31/sec/412.2/meta/qty-pressure" );
		ASSERT_TRUE( l6.has_value() );
		EXPECT_FALSE( specificQuery.match( *l6 ) );
		EXPECT_FALSE( generalQuery.match( *l6 ) );
	}

	TEST_F( LocalIdQueryTests, UseCase1 )
	{
		const auto& vis = VIS::instance();
		const auto& locations = vis.locations( vis.latest() );

		auto localId = LocalId::fromString(
			"/dnv-v2/vis-3-7a/433.1-P/C322/meta/qty-linear.vibration.amplitude/pos-driving.end/detail-iso.10816" );
		ASSERT_TRUE( localId.has_value() );

		auto locationP = locations.fromString( "P" );
		auto locationS = locations.fromString( "S" );
		ASSERT_TRUE( locationP.has_value() );
		ASSERT_TRUE( locationS.has_value() );

		const auto& gmod = vis.gmod( localId->version() );
		auto node433 = gmod.node( "433.1" );
		ASSERT_TRUE( node433.has_value() );
		const auto* pNode433 = *node433;

		std::vector<Location> locs = { *locationP, *locationS };

		auto query = LocalIdQueryBuilder::from( *localId )
						 .withPrimaryItem( localId->primaryItem(), [pNode433, &locs]( GmodPathQueryBuilder::Path& builder ) {
							 return builder.withNode( [pNode433]( const std::unordered_map<std::string, const GmodNode*>& nodes ) {
											   return pNode433;
										   },
											   locs )
								 .build();
						 } )
						 .build();

		EXPECT_TRUE( query.match( *localId ) );
		auto localId2 = LocalId::fromString(
			"/dnv-v2/vis-3-7a/433.1-S/C322/meta/qty-linear.vibration.amplitude/pos-driving.end/detail-iso.10816" );
		ASSERT_TRUE( localId2.has_value() );
		EXPECT_TRUE( query.match( *localId2 ) );
	}

	TEST_F( LocalIdQueryTests, UseCase2 )
	{
		const auto& vis = VIS::instance();

		auto localId = LocalId::fromString(
			"/dnv-v2/vis-3-7a/511.31/C121/meta/qty-linear.vibration.amplitude/pos-driving.end/detail-iso.10816" );
		ASSERT_TRUE( localId.has_value() );

		const auto& gmod = vis.gmod( localId->version() );

		auto node5113 = gmod.node( "511.3" );
		ASSERT_TRUE( node5113.has_value() );

		auto query = LocalIdQueryBuilder::from( *localId )
						 .withPrimaryItem( [&node5113]( GmodPathQueryBuilder::Nodes& builder ) {
							 return builder.withNode( **node5113, true ).build();
						 } )
						 .build();

		EXPECT_TRUE( query.match( *localId ) );

		auto node5114 = gmod.node( "511.4" );
		ASSERT_TRUE( node5114.has_value() );
		const auto* pNode5114 = *node5114;

		query = LocalIdQueryBuilder::create()
					.withPrimaryItem( [pNode5114]( GmodPathQueryBuilder::Nodes& builder ) {
						return builder.withNode( *pNode5114, true ).build();
					} )
					.build();

		EXPECT_FALSE( query.match( *localId ) );
	}

	TEST_F( LocalIdQueryTests, UseCase3 )
	{
		const auto& vis = VIS::instance();

		auto localId = LocalId::fromString( "/dnv-v2/vis-3-7a/433.1-S/C322.91/S205/meta/qty-conductivity/detail-relative" );
		ASSERT_TRUE( localId.has_value() );

		auto builder = localId->builder();
		auto visVersion = localId->version();

		auto query = LocalIdQueryBuilder::from( *localId )
						 .withTags( [&localId]( MetadataTagsQueryBuilder& tags ) {
							 auto tagsBuilder = tags;
							 for ( const auto& tag : localId->metadataTags() )
							 {
								 tagsBuilder = tagsBuilder.withTag( tag );
							 }
							 return tagsBuilder.withAllowOtherTags( false ).build();
						 } )
						 .build();

		EXPECT_TRUE( query.match( *localId ) );

		const auto& gmod = vis.gmod( visVersion );
		const auto& codebooks = vis.codebooks( visVersion );
		const auto& locations = vis.locations( visVersion );

		auto randomTag = codebooks[CodebookName::Content].createTag( "random" );
		ASSERT_TRUE( randomTag.has_value() );
		auto l1 = builder.withMetadataTag( *randomTag ).build();

		auto pathOpt = GmodPath::fromString( "433.1-1S", gmod, locations );
		ASSERT_TRUE( pathOpt.has_value() );
		auto l2 = builder.withPrimaryItem( *pathOpt ).build();

		EXPECT_FALSE( query.match( l1 ) );
		EXPECT_FALSE( query.match( l2 ) );
	}

	TEST_F( LocalIdQueryTests, UseCase4 )
	{
		auto localIdOpt = LocalId::fromString( "/dnv-v2/vis-3-7a/511.11/C101/meta/qty-pressure/cnt-lubricating.oil" );
		ASSERT_TRUE( localIdOpt.has_value() );

		auto queryOpt = LocalIdQueryBuilder::from( *localIdOpt );

		auto query = queryOpt.withPrimaryItem( localIdOpt->primaryItem(), []( GmodPathQueryBuilder::Path& builder ) {
								 return builder.withoutLocations().build();
							 } )
						 .build();

		auto other = LocalId::fromString( "/dnv-v2/vis-3-7a/511.11-1/C101/meta/qty-pressure/cnt-lubricating.oil" );
		ASSERT_TRUE( other.has_value() );

		EXPECT_TRUE( query.match( *other ) );
	}
} // namespace dnv::vista::sdk::test
