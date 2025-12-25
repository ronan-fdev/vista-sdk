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
 * @file TESTS_UniversalId.cpp
 * @brief Unit tests for UniversalId and UniversalIdBuilder functionality
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

#include <EmbeddedTestData.h>

namespace dnv::vista::sdk::test
{
	using namespace std::string_view_literals;

	//=====================================================================
	// Test data
	//=====================================================================

	static constexpr std::string_view testCase1 =
		"data.dnv.com/IMO1234567/dnv-v2/vis-3-4a/621.21/S90/sec/411.1/C101/meta/qty-mass/cnt-fuel.oil/pos-inlet";
	static constexpr std::string_view testCase2 =
		"data.dnv.com/IMO1234567/dnv-v2/vis-3-7a/612.21/C701.23/C633/meta/calc~accumulate";

	//=====================================================================
	// UniversalId tests
	//=====================================================================

	//----------------------------------------------
	// Parsing tests
	//----------------------------------------------

	TEST( UniversalIdTests, TryParsing_TestCase1 )
	{
		ParsingErrors errors;
		auto universalId = UniversalIdBuilder::fromString( testCase1, errors );

		EXPECT_TRUE( universalId.has_value() ) << "Parsing failed: " << errors.toString();
		EXPECT_FALSE( errors.hasErrors() );
	}

	TEST( UniversalIdTests, TryParsing_TestCase2 )
	{
		ParsingErrors errors;
		auto universalId = UniversalIdBuilder::fromString( testCase2, errors );

		EXPECT_TRUE( universalId.has_value() ) << "Parsing failed: " << errors.toString();
		EXPECT_FALSE( errors.hasErrors() );
	}

	TEST( UniversalIdTests, Parsing_TestCase1 )
	{
		auto universalId = UniversalId::fromString( testCase1 );

		ASSERT_TRUE( universalId.has_value() );
		EXPECT_EQ( universalId->imoNumber(), ImoNumber{ 1234567 } );
	}

	TEST( UniversalIdTests, Parsing_TestCase2 )
	{
		auto universalId = UniversalId::fromString( testCase2 );

		ASSERT_TRUE( universalId.has_value() );
		EXPECT_EQ( universalId->imoNumber(), ImoNumber{ 1234567 } );
	}

	//----------------------------------------------
	// ToString tests
	//----------------------------------------------

	TEST( UniversalIdTests, ToString_TestCase1 )
	{
		auto universalId = UniversalIdBuilder::fromString( testCase1 );

		ASSERT_TRUE( universalId.has_value() );
		const auto universalIdString = universalId->toString();
		EXPECT_EQ( testCase1, universalIdString );
	}

	TEST( UniversalIdTests, ToString_TestCase2 )
	{
		auto universalId = UniversalIdBuilder::fromString( testCase2 );

		ASSERT_TRUE( universalId.has_value() );
		const auto universalIdString = universalId->toString();
		EXPECT_EQ( testCase2, universalIdString );
	}

	//----------------------------------------------
	// Builder tests - Add and remove
	//----------------------------------------------

	TEST( UniversalIdTests, UniversalBuilder_AddAndRemoveAll_TestCase1 )
	{
		auto universalId = UniversalIdBuilder::fromString( testCase1 );

		ASSERT_TRUE( universalId.has_value() );

		const auto& builder = universalId->builder();
		EXPECT_TRUE( builder.localId().has_value() );
		EXPECT_TRUE( builder.imoNumber().has_value() );

		auto emptyBuilder = builder.withoutImoNumber().withoutLocalId();

		EXPECT_FALSE( emptyBuilder.localId().has_value() );
		EXPECT_FALSE( emptyBuilder.imoNumber().has_value() );
	}

	TEST( UniversalIdTests, UniversalBuilder_AddAndRemoveAll_TestCase2 )
	{
		auto universalId = UniversalIdBuilder::fromString( testCase2 );

		ASSERT_TRUE( universalId.has_value() );

		const auto& builder = universalId->builder();
		EXPECT_TRUE( builder.localId().has_value() );
		EXPECT_TRUE( builder.imoNumber().has_value() );

		auto emptyBuilder = builder.withoutImoNumber().withoutLocalId();

		EXPECT_FALSE( emptyBuilder.localId().has_value() );
		EXPECT_FALSE( emptyBuilder.imoNumber().has_value() );
	}

	TEST( UniversalIdTests, UniversalBuilder_WithOptional_Nullopt )
	{
		auto builder = UniversalIdBuilder::create( VisVersion::v3_4a );

		// Test that passing nullopt throws
		EXPECT_THROW( (void)builder.withLocalId( std::nullopt ), std::invalid_argument );
		EXPECT_THROW( (void)builder.withImoNumber( std::nullopt ), std::invalid_argument );
	}

	//----------------------------------------------
	// Programmatic construction
	//----------------------------------------------

	TEST( UniversalIdTests, BuildUniversalId_Programmatic )
	{
		const auto& vis = VIS::instance();
		const auto& codebooks = vis.codebooks( VisVersion::v3_4a );

		// Create metadata tags
		auto qtyTag = codebooks[CodebookName::Quantity].createTag( "mass" );
		auto cntTag = codebooks[CodebookName::Content].createTag( "fuel.oil" );
		auto posTag = codebooks[CodebookName::Position].createTag( "inlet" );

		ASSERT_TRUE( qtyTag.has_value() );
		ASSERT_TRUE( cntTag.has_value() );
		ASSERT_TRUE( posTag.has_value() );

		// Build LocalIdBuilder step by step
		auto localIdBuilder = LocalIdBuilder::create( VisVersion::v3_4a )
								  .withPrimaryItem( GmodPath::fromString( "621.21/S90", VisVersion::v3_4a ).value() )
								  .withSecondaryItem( GmodPath::fromString( "411.1/C101", VisVersion::v3_4a ).value() )
								  .withMetadataTag( *qtyTag )
								  .withMetadataTag( *cntTag )
								  .withMetadataTag( *posTag );

		// Build UniversalIdBuilder with the LocalIdBuilder
		auto universalIdBuilder = UniversalIdBuilder::create( VisVersion::v3_4a )
									  .withImoNumber( ImoNumber{ 1234567 } )
									  .withLocalId( localIdBuilder );

		EXPECT_TRUE( universalIdBuilder.isValid() );

		auto universalId = universalIdBuilder.build();

		EXPECT_EQ( universalId.imoNumber(), ImoNumber{ 1234567 } );
		EXPECT_TRUE( universalId.localId().builder().isValid() );

		const auto str = universalId.toString();
		EXPECT_EQ( testCase1, str );
	}

	//----------------------------------------------
	// Equality tests
	//----------------------------------------------

	TEST( UniversalIdTests, Equality_SameUniversalIds )
	{
		auto universalId1 = UniversalId::fromString( testCase1 );
		auto universalId2 = UniversalId::fromString( testCase1 );

		ASSERT_TRUE( universalId1.has_value() );
		ASSERT_TRUE( universalId2.has_value() );

		EXPECT_EQ( *universalId1, *universalId2 );
		EXPECT_FALSE( *universalId1 != *universalId2 );
	}

	TEST( UniversalIdTests, Equality_DifferentUniversalIds )
	{
		auto universalId1 = UniversalId::fromString( testCase1 );
		auto universalId2 = UniversalId::fromString( testCase2 );

		ASSERT_TRUE( universalId1.has_value() );
		ASSERT_TRUE( universalId2.has_value() );

		EXPECT_NE( *universalId1, *universalId2 );
		EXPECT_FALSE( *universalId1 == *universalId2 );
	}

	//----------------------------------------------
	// Invalid construction
	//----------------------------------------------

	TEST( UniversalIdTests, InvalidConstruction_MissingImoNumber )
	{
		auto builder = UniversalIdBuilder::create( VisVersion::v3_4a )
						   .withLocalId( LocalIdBuilder::create( VisVersion::v3_4a )
								   .withPrimaryItem( GmodPath::fromString( "411.1", VisVersion::v3_4a ).value() ) );

		// Without IMO number, builder is invalid
		EXPECT_FALSE( builder.isValid() );

		// Building should throw
		EXPECT_THROW( (void)builder.build(), std::invalid_argument );
	}

	TEST( UniversalIdTests, InvalidConstruction_MissingLocalId )
	{
		auto builder = UniversalIdBuilder::create( VisVersion::v3_4a ).withImoNumber( ImoNumber{ 1234567 } ).withoutLocalId();

		// Without LocalId, builder is invalid
		EXPECT_FALSE( builder.isValid() );

		// Building should throw
		EXPECT_THROW( (void)builder.build(), std::invalid_argument );
	}

	//----------------------------------------------
	// Parsing error handling
	//----------------------------------------------

	TEST( UniversalIdTests, ParsingErrors_InvalidFormat )
	{
		ParsingErrors errors;
		auto universalId = UniversalIdBuilder::fromString( "invalid-format", errors );

		EXPECT_FALSE( universalId.has_value() );
		EXPECT_TRUE( errors.hasErrors() );
	}

	TEST( UniversalIdTests, ParsingErrors_MissingNamingEntity )
	{
		ParsingErrors errors;
		auto universalId = UniversalIdBuilder::fromString( "wrong.entity.com/IMO1234567/dnv-v2/vis-3-4a/411.1", errors );

		EXPECT_FALSE( universalId.has_value() );
		EXPECT_TRUE( errors.hasErrors() );
	}

	TEST( UniversalIdTests, ParsingErrors_InvalidImoNumber )
	{
		ParsingErrors errors;
		auto universalId = UniversalIdBuilder::fromString( "data.dnv.com/INVALID/dnv-v2/vis-3-4a/411.1", errors );

		EXPECT_FALSE( universalId.has_value() );
		EXPECT_TRUE( errors.hasErrors() );
	}

	TEST( UniversalIdTests, ParsingErrors_EmptyString )
	{
		ParsingErrors errors;
		auto universalId = UniversalIdBuilder::fromString( "", errors );

		EXPECT_FALSE( universalId.has_value() );
		EXPECT_TRUE( errors.hasErrors() );
	}

	TEST( UniversalIdTests, ParsingErrors_MissingLocalId )
	{
		ParsingErrors errors;
		auto universalId = UniversalIdBuilder::fromString( "data.dnv.com/IMO1234567", errors );

		EXPECT_FALSE( universalId.has_value() );
		EXPECT_TRUE( errors.hasErrors() );
	}

	//----------------------------------------------
	// Accessor tests
	//----------------------------------------------

	TEST( UniversalIdTests, Accessors_ImoNumber )
	{
		auto universalId = UniversalId::fromString( testCase1 );

		ASSERT_TRUE( universalId.has_value() );
		EXPECT_EQ( universalId->imoNumber(), ImoNumber{ 1234567 } );
		EXPECT_EQ( universalId->imoNumber().toString(), "IMO1234567" );
	}

	TEST( UniversalIdTests, Accessors_LocalId )
	{
		auto universalId = UniversalId::fromString( testCase1 );

		ASSERT_TRUE( universalId.has_value() );

		const auto& localId = universalId->localId();
		const auto& builder = localId.builder();

		EXPECT_TRUE( builder.isValid() );
		EXPECT_FALSE( builder.isEmpty() );

		// LocalId should contain expected path
		const auto localIdStr = localId.toString();
		EXPECT_TRUE( localIdStr.find( "621.21" ) != std::string::npos );
		EXPECT_TRUE( localIdStr.find( "qty-mass" ) != std::string::npos );
	}

	TEST( UniversalIdTests, Accessors_Builder )
	{
		auto universalId = UniversalId::fromString( testCase1 );

		ASSERT_TRUE( universalId.has_value() );

		const auto& builder = universalId->builder();
		EXPECT_TRUE( builder.isValid() );
		EXPECT_TRUE( builder.imoNumber().has_value() );
		EXPECT_TRUE( builder.localId().has_value() );
	}

	//----------------------------------------------
	// Copy and move semantics
	//----------------------------------------------

	TEST( UniversalIdTests, CopyConstructor )
	{
		auto universalId1 = UniversalId::fromString( testCase1 );
		ASSERT_TRUE( universalId1.has_value() );

		UniversalId universalId2 = *universalId1;

		EXPECT_EQ( universalId1->imoNumber(), universalId2.imoNumber() );
		EXPECT_EQ( universalId1->toString(), universalId2.toString() );
	}

	TEST( UniversalIdTests, CopyAssignment )
	{
		auto universalId1 = UniversalId::fromString( testCase1 );
		auto universalId2 = UniversalId::fromString( testCase2 );

		ASSERT_TRUE( universalId1.has_value() );
		ASSERT_TRUE( universalId2.has_value() );

		*universalId2 = *universalId1;

		EXPECT_EQ( universalId1->imoNumber(), universalId2->imoNumber() );
		EXPECT_EQ( universalId1->toString(), universalId2->toString() );
	}

	TEST( UniversalIdTests, MoveConstructor )
	{
		auto universalId1 = UniversalId::fromString( testCase1 );
		ASSERT_TRUE( universalId1.has_value() );

		const auto expectedStr = universalId1->toString();
		const auto expectedImo = universalId1->imoNumber();

		UniversalId universalId2 = std::move( *universalId1 );

		EXPECT_EQ( expectedImo, universalId2.imoNumber() );
		EXPECT_EQ( expectedStr, universalId2.toString() );
	}

	TEST( UniversalIdTests, MoveAssignment )
	{
		auto universalId1 = UniversalId::fromString( testCase1 );
		auto universalId2 = UniversalId::fromString( testCase2 );

		ASSERT_TRUE( universalId1.has_value() );
		ASSERT_TRUE( universalId2.has_value() );

		const auto expectedStr = universalId1->toString();
		const auto expectedImo = universalId1->imoNumber();

		*universalId2 = std::move( *universalId1 );

		EXPECT_EQ( expectedImo, universalId2->imoNumber() );
		EXPECT_EQ( expectedStr, universalId2->toString() );
	}
} // namespace dnv::vista::sdk::test
