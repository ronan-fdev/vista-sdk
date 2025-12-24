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
 * @file TESTS_LocalId.cpp
 * @brief Unit tests for LocalId and LocalIdBuilder functionality
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

#include <EmbeddedTestData.h>

#include <fstream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

namespace dnv::vista::sdk::test
{
	//=====================================================================
	// Test data structures
	//=====================================================================

	/**
	 * @brief Input data for LocalId building tests
	 */
	struct LocalIdInput
	{
		std::string primaryItem;
		std::optional<std::string> secondaryItem;
		std::optional<std::string> quantity;
		std::optional<std::string> content;
		std::optional<std::string> position;
		VisVersion visVersion = VisVersion::v3_4a;
		bool verbose = false;
	};

	/**
	 * @brief Test case for LocalId building
	 */
	struct LocalIdTestCase
	{
		LocalIdInput input;
		std::string expectedOutput;
	};

	//=====================================================================
	// Test data providers
	//=====================================================================

	static std::vector<LocalIdTestCase> validTestData()
	{
		return {
			// Test case 1: Basic primary item only
			{ LocalIdInput{ "411.1/C101.31-2" },
				"/dnv-v2/vis-3-4a/411.1/C101.31-2/meta" },

			// Test case 2: Primary item with metadata tags
			{ LocalIdInput{ "411.1/C101.31-2", std::nullopt, "temperature", "exhaust.gas", "inlet" },
				"/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" },

			// Test case 3: Verbose mode with locations
			{ LocalIdInput{ "411.1/C101.63/S206", std::nullopt, "temperature", "exhaust.gas", "inlet",
				  VisVersion::v3_4a, true },
				"/dnv-v2/vis-3-4a/411.1/C101.63/S206/~propulsion.engine/~cooling.system/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" },

			// Test case 4: With secondary item and verbose
			{ LocalIdInput{ "411.1/C101.63/S206", "411.1/C101.31-5", "temperature", "exhaust.gas", "inlet",
				  VisVersion::v3_4a, true },
				"/dnv-v2/vis-3-4a/411.1/C101.63/S206/sec/411.1/C101.31-5/~propulsion.engine/~cooling.system/~for.propulsion.engine/~cylinder.5/meta/qty-temperature/cnt-exhaust.gas/pos-inlet" },

			// Test case 5: Different VIS version
			{ LocalIdInput{ "511.11/C101.67/S208", std::nullopt, "pressure", "starting.air", "inlet",
				  VisVersion::v3_6a, true },
				"/dnv-v2/vis-3-6a/511.11/C101.67/S208/~main.generator.engine/~starting.system.pneumatic/meta/qty-pressure/cnt-starting.air/pos-inlet" } };
	}

	static std::vector<std::string> validParsingData()
	{
		return {
			"/dnv-v2/vis-3-4a/1031/meta/cnt-refrigerant/state-leaking",
			"/dnv-v2/vis-3-4a/1021.1i-6P/H123/meta/qty-volume/cnt-cargo/pos~percentage",
			"/dnv-v2/vis-3-4a/652.31/S90.3/S61/sec/652.1i-1P/meta/cnt-sea.water/state-opened",
			"/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
			"/dnv-v2/vis-3-4a/411.1/C101.63/S206/~propulsion.engine/~cooling.system/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
			"/dnv-v2/vis-3-4a/411.1/C101.63/S206/sec/411.1/C101.31-5/~propulsion.engine/~cooling.system/~for.propulsion.engine/~cylinder.5/meta/qty-temperature/cnt-exhaust.gas/pos-inlet",
			"/dnv-v2/vis-3-4a/511.11-21O/C101.67/S208/meta/qty-pressure/cnt-air/state-low" };
	}

	//=====================================================================
	// LocalId tests
	//=====================================================================

	//----------------------------------------------
	// Build valid LocalIds
	//----------------------------------------------

	class LocalIdBuildValidTest : public ::testing::TestWithParam<LocalIdTestCase>
	{
	};

	TEST_P( LocalIdBuildValidTest, Test_LocalId_Build_Valid )
	{
		auto testCase = GetParam();
		const auto& input = testCase.input;

		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( input.visVersion );
		const auto& codebooks = vis.codebooks( input.visVersion );

		// Parse primary and secondary items
		ParsingErrors primaryErrors;
		auto primaryItem = GmodPath::fromString( input.primaryItem, gmod, vis.locations( input.visVersion ), primaryErrors );
		ASSERT_TRUE( primaryItem.has_value() ) << "Failed to parse primary item: " << input.primaryItem;

		std::optional<GmodPath> secondaryItem;
		if ( input.secondaryItem.has_value() )
		{
			ParsingErrors secondaryErrors;
			secondaryItem = GmodPath::fromString( *input.secondaryItem, gmod, vis.locations( input.visVersion ), secondaryErrors );
			ASSERT_TRUE( secondaryItem.has_value() ) << "Failed to parse secondary item: " << *input.secondaryItem;
		}

		// Build LocalId
		auto builder = LocalIdBuilder::create( input.visVersion ).withPrimaryItem( *primaryItem );

		if ( input.verbose )
		{
			builder = builder.withVerboseMode( true );
		}

		if ( secondaryItem.has_value() )
		{
			builder = builder.withSecondaryItem( *secondaryItem );
		}

		if ( input.quantity.has_value() )
		{
			auto tag = codebooks[CodebookName::Quantity].createTag( *input.quantity );
			ASSERT_TRUE( tag.has_value() );
			builder = builder.withMetadataTag( *tag );
		}

		if ( input.content.has_value() )
		{
			auto tag = codebooks[CodebookName::Content].createTag( *input.content );
			ASSERT_TRUE( tag.has_value() );
			builder = builder.withMetadataTag( *tag );
		}

		if ( input.position.has_value() )
		{
			auto tag = codebooks[CodebookName::Position].createTag( *input.position );
			ASSERT_TRUE( tag.has_value() );
			builder = builder.withMetadataTag( *tag );
		} // Verify output
		auto localIdStr = builder.toString();
		EXPECT_EQ( testCase.expectedOutput, localIdStr );
	}

	INSTANTIATE_TEST_SUITE_P(
		LocalIdTests,
		LocalIdBuildValidTest,
		::testing::ValuesIn( validTestData() ) );

	//----------------------------------------------
	// Build with all without methods
	//----------------------------------------------

	TEST( LocalIdTests, Test_LocalId_Build_AllWithout )
	{
		const auto& vis = VIS::instance();
		const auto visVersion = VisVersion::v3_4a;
		const auto& gmod = vis.gmod( visVersion );
		const auto& codebooks = vis.codebooks( visVersion );

		// Parse items
		ParsingErrors errors;
		auto primaryItem = GmodPath::fromString( "411.1/C101.31-2", gmod, vis.locations( visVersion ), errors );
		ASSERT_TRUE( primaryItem.has_value() );

		auto secondaryItem = GmodPath::fromString( "411.1/C101.31-5", gmod, vis.locations( visVersion ), errors );
		ASSERT_TRUE( secondaryItem.has_value() );

		// Create tags
		auto quantityTag = codebooks[CodebookName::Quantity].createTag( "temperature" );
		ASSERT_TRUE( quantityTag.has_value() );

		auto contentTag = codebooks[CodebookName::Content].createTag( "exhaust.gas" );
		ASSERT_TRUE( contentTag.has_value() );

		auto positionTag = codebooks[CodebookName::Position].createTag( "inlet" );
		ASSERT_TRUE( positionTag.has_value() );

		auto stateTag = codebooks[CodebookName::State].createTag( "running" );
		ASSERT_TRUE( stateTag.has_value() );

		auto calculationTag = codebooks[CodebookName::Calculation].createTag( "average" );
		ASSERT_TRUE( calculationTag.has_value() );

		// Build LocalId with all properties
		auto localId = LocalIdBuilder::create( visVersion )
						   .withPrimaryItem( *primaryItem )
						   .withSecondaryItem( *secondaryItem )
						   .withVerboseMode( true )
						   .withMetadataTag( *quantityTag )
						   .withMetadataTag( *contentTag )
						   .withMetadataTag( *positionTag )
						   .withMetadataTag( *stateTag )
						   .withMetadataTag( *calculationTag );

		EXPECT_TRUE( localId.isValid() );

		// Remove all properties
		auto allWithout = localId.withoutPrimaryItem()
							  .withoutSecondaryItem()
							  .withoutVisVersion()
							  .withoutMetadataTag( CodebookName::Quantity )
							  .withoutMetadataTag( CodebookName::Position )
							  .withoutMetadataTag( CodebookName::State )
							  .withoutMetadataTag( CodebookName::Content )
							  .withoutMetadataTag( CodebookName::Calculation );

		EXPECT_TRUE( allWithout.isEmpty() );
	}

	//----------------------------------------------
	// Equality tests
	//----------------------------------------------

	class LocalIdEqualityTest : public ::testing::TestWithParam<LocalIdTestCase>
	{
	};

	TEST_P( LocalIdEqualityTest, Test_LocalId_Equality )
	{
		auto testCase = GetParam();
		const auto& input = testCase.input;

		const auto& vis = VIS::instance();
		const auto visVersion = VisVersion::v3_4a;
		const auto& gmod = vis.gmod( visVersion );
		const auto& codebooks = vis.codebooks( visVersion );

		// Parse items
		ParsingErrors errors;
		auto primaryItem = GmodPath::fromString( input.primaryItem, gmod, vis.locations( visVersion ), errors );
		ASSERT_TRUE( primaryItem.has_value() );

		std::optional<GmodPath> secondaryItem;
		if ( input.secondaryItem.has_value() )
		{
			secondaryItem = GmodPath::fromString( *input.secondaryItem, gmod, vis.locations( visVersion ), errors );
			ASSERT_TRUE( secondaryItem.has_value() );
		}

		// Build LocalIdBuilder
		auto localId = LocalIdBuilder::create( visVersion ).withPrimaryItem( *primaryItem );

		if ( secondaryItem.has_value() )
		{
			localId = localId.withSecondaryItem( *secondaryItem );
		}

		if ( input.quantity.has_value() )
		{
			auto tag = codebooks[CodebookName::Quantity].createTag( *input.quantity );
			if ( tag.has_value() )
			{
				localId = localId.withMetadataTag( *tag );
			}
		}

		if ( input.content.has_value() )
		{
			auto tag = codebooks[CodebookName::Content].createTag( *input.content );
			if ( tag.has_value() )
			{
				localId = localId.withMetadataTag( *tag );
			}
		}

		if ( input.position.has_value() )
		{
			auto tag = codebooks[CodebookName::Position].createTag( *input.position );
			if ( tag.has_value() )
			{
				localId = localId.withMetadataTag( *tag );
			}
		}

		// Test self-equality (copy constructor)
		auto otherLocalId = localId;
		EXPECT_EQ( localId, otherLocalId );

		// Test inequality with different position tag
		auto eqTestTag = codebooks[CodebookName::Position].createTag( "upper" );
		ASSERT_TRUE( eqTestTag.has_value() );
		otherLocalId = localId.withMetadataTag( *eqTestTag );
		EXPECT_NE( localId, otherLocalId );

		// Test equality with reconstructed primary item and position
		if ( localId.primaryItem().has_value() )
		{
			otherLocalId = localId.withPrimaryItem( *localId.primaryItem() );
			if ( input.position.has_value() )
			{
				auto tag = codebooks[CodebookName::Position].createTag( *input.position );
				if ( tag.has_value() )
				{
					otherLocalId = otherLocalId.withMetadataTag( *tag );
				}
			}
			EXPECT_EQ( localId, otherLocalId );
		}
	}

	INSTANTIATE_TEST_SUITE_P(
		LocalIdTests,
		LocalIdEqualityTest,
		::testing::ValuesIn( validTestData() ) );

	//----------------------------------------------
	// Parsing tests
	//----------------------------------------------

	class LocalIdParsingTest : public ::testing::TestWithParam<std::string>
	{
	};

	TEST_P( LocalIdParsingTest, Test_Parsing )
	{
		auto localIdStr = GetParam();

		ParsingErrors errors;
		auto parsed = LocalIdBuilder::fromString( localIdStr, errors );

		ASSERT_TRUE( parsed.has_value() ) << "Failed to parse: " << localIdStr
										  << "\nErrors: " << errors.toString();
		EXPECT_EQ( localIdStr, parsed->toString() );
	}

	INSTANTIATE_TEST_SUITE_P(
		LocalIdTests,
		LocalIdParsingTest,
		::testing::ValuesIn( validParsingData() ) );

	//----------------------------------------------
	// Simple parsing test
	//----------------------------------------------

	TEST( LocalIdTests, Test )
	{
		auto localIdAsString = "/dnv-v2/vis-3-4a/411.1/C101.31-2/meta/qty-temperature/cnt-exhaust.gas/pos-inlet";

		auto localId = LocalIdBuilder::fromString( localIdAsString );
		EXPECT_TRUE( localId.has_value() );
	}

	//----------------------------------------------
	// Smoke test with file
	//----------------------------------------------

	TEST( LocalIdTests, SmokeTest_Parsing )
	{
		// Debug: List all available embedded test files
		auto availableFiles = EmbeddedTestData::listFiles();
		std::ostringstream fileList;
		fileList << "Available embedded test files (" << availableFiles.size() << "):\n";
		for ( const auto& file : availableFiles )
		{
			fileList << "  - " << file << "\n";
		}

		auto testDataOpt = EmbeddedTestData::load<std::string>( "LocalIds.txt" );
		ASSERT_TRUE( testDataOpt.has_value() ) << "Failed to load LocalIds.txt\n"
											   << fileList.str();

		std::istringstream stream( *testDataOpt );
		std::string localIdStr;

		struct ErrorEntry
		{
			std::string localIdStr;
			std::optional<LocalId> localId;
			ParsingErrors parsingErrors;
		};

		std::vector<ErrorEntry> errored;

		while ( std::getline( stream, localIdStr ) )
		{
			// Quick fix to skip invalid metadata tags "qty-content"
			if ( localIdStr.find( "qty-content" ) != std::string::npos )
				continue;

			ParsingErrors errors;
			auto localId = LocalIdBuilder::fromString( localIdStr, errors );

			if ( !localId.has_value() )
			{
				errored.push_back( { localIdStr, std::nullopt, std::move( errors ) } );
			}
		}

		// Report errors
		if ( !errored.empty() )
		{
			std::ostringstream errorMsg;
			errorMsg << "Found " << errored.size() << " parsing errors:\n";
			for ( const auto& entry : errored )
			{
				errorMsg << "  LocalId: " << entry.localIdStr << "\n";
				if ( entry.parsingErrors.hasErrors() )
				{
					errorMsg << "  Errors: " << entry.parsingErrors.toString() << "\n";
				}
			}
			FAIL() << errorMsg.str();
		}

		EXPECT_TRUE( errored.empty() );
	}
} // namespace dnv::vista::sdk::test
