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
 * @file TESTS_GmodVersioning.cpp
 * @brief Unit tests for GmodVersioning functionality - maps 1:1 with GmodVersioningTests.cs
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

#include <EmbeddedTestData.h>

#include <SDK/versioning/GmodVersioning.h>
#include <SDK/dto/GmodVersioningDto.h>

#include <nfx/serialization/json/Document.h>
#include <nfx/serialization/json/Serializer.h>

#include <string>
#include <vector>

namespace dnv::vista::sdk::test
{
	using nfx::serialization::json::Document;
	using internal::GmodVersioning;

	struct ConvertPathTestData
	{
		std::string inputPath;
		std::string expectedPath;
		VisVersion sourceVersion;
		VisVersion targetVersion;
	};

	class GmodVersioningConvertPathTest : public ::testing::TestWithParam<ConvertPathTestData>
	{
	};

	TEST_P( GmodVersioningConvertPathTest, ConvertPath )
	{
		const auto& params = GetParam();
		const auto& vis = VIS::instance();

		const auto& sourceGmod = vis.gmod( params.sourceVersion );
		const auto& targetGmod = vis.gmod( params.targetVersion );
		const auto& sourceLocations = vis.locations( params.sourceVersion );
		const auto& targetLocations = vis.locations( params.targetVersion );

		ParsingErrors sourceErrors;
		ParsingErrors targetErrors;

		auto sourcePath = GmodPath::fromString( params.inputPath, sourceGmod, sourceLocations, sourceErrors );
		ASSERT_TRUE( sourcePath.has_value() ) << "Failed to parse source path: " << params.inputPath
											  << "\nErrors: " << sourceErrors.toString();

		auto parsedTargetPath = GmodPath::fromString( params.expectedPath, targetGmod, targetLocations, targetErrors );
		ASSERT_TRUE( parsedTargetPath.has_value() ) << "Failed to parse expected path: " << params.expectedPath
													<< "\nErrors: " << targetErrors.toString();

		auto targetPath = vis.convertPath( params.sourceVersion, *sourcePath, params.targetVersion );

		ASSERT_TRUE( targetPath.has_value() ) << "Failed to convert path from "
											  << params.inputPath << " (v" << VisVersions::toString( params.sourceVersion )
											  << ") to v" << VisVersions::toString( params.targetVersion );

		EXPECT_EQ( params.inputPath, sourcePath->toString() );
		EXPECT_EQ( params.expectedPath, parsedTargetPath->toString() );
		EXPECT_EQ( params.expectedPath, targetPath->toString() );
	}

	INSTANTIATE_TEST_SUITE_P(
		GmodVersioningTests,
		GmodVersioningConvertPathTest,
		::testing::Values(
			ConvertPathTestData{ "411.1/C101.72/I101", "411.1/C101.72/I101", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "323.51/H362.1", "323.61/H362.1", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "321.38/C906", "321.39/C906", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "511.331/C221", "511.31/C121.31/C221", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "511.11/C101.663i/C663.5/CS6d", "511.11/C101.663i/C663.6/CS6d", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "511.11-1/C101.663i/C663.5/CS6d", "511.11-1/C101.663i/C663.6/CS6d", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "1012.21/C1147.221/C1051.7/C101.22", "1012.21/C1147.221/C1051.7/C101.93", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "1012.21/C1147.221/C1051.7/C101.61/S203.6", "1012.21/C1147.221/C1051.7/C101.311/C467.5", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "001", "001", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "038.7/F101.2/F71", "038.7/F101.2/F71", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "1012.21/C1147.221/C1051.7/C101.61/S203.6/S61", "1012.21/C1147.221/C1051.7/C101.311/C467.5/S61", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "000a", "000a", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "1012.21/C1147.221/C1051.7/C101.61/S203.2/S101", "1012.21/C1147.221/C1051.7/C101.61/S203.3/S110.1/S101", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "1012.21/C1147.221/C1051.7/C101.661i/C624", "1012.21/C1147.221/C1051.7/C101.661i/C621", VisVersion::v3_4a, VisVersion::v3_6a },
			// ConvertPathTestData{ "1012.22/S201.1/C151.2/S110.2/C101.61/S203.2/S101", "1012.22/S201.1/C151.2/S110.2/C101.61/S203.3/S110.1/S101", VisVersion::v3_4a, VisVersion::v3_6a }, // Parent code change and different depth, which introduced cycle
			ConvertPathTestData{ "1012.22/S201.1/C151.2/S110.2/C101.64i", "1012.22/S201.1/C151.2/S110.2/C101.64", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "632.32i/S110.2/C111.42/G203.31/S90.5/C401", "632.32i/S110.2/C111.42/G203.31/S90.5/C401", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "864.11/G71.21/C101.64i/S201.1/C151.31/S110.2/C111.42/G204.41/S90.2/S51", "864.11/G71.21/C101.64/S201.1/C151.31/S110.2/C111.42/G204.41/S90.2/S51", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "864.11/G71.21/C101.64i/S201.1/C151.31/S110.2/C111.41/G240.1/G242.2/S90.5/C401", "864.11/G71.21/C101.64/S201.1/C151.31/S110.2/C111.41/G240.1/G242.2/S90.5/C401", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "221.31/C1141.41/C664.2/C471", "221.31/C1141.41/C664.2/C471", VisVersion::v3_4a, VisVersion::v3_6a },
			// ConvertPathTestData{ "354.2/C1096", "354.2/C1096", VisVersion::v3_4a, VisVersion::v3_6a }, // Was deleted, as path to root is gone
			ConvertPathTestData{ "514/E15", "514", VisVersion::v3_4a, VisVersion::v3_6a },
			ConvertPathTestData{ "1346/S201.1/C151.31/S110.2/C111.1/C109.16/C509", "1346/S201.1/C151.31/S110.2/C111.1/C109.126/C509", VisVersion::v3_7a, VisVersion::v3_8a }
			// ConvertPathTestData{ "851/H231", "851", VisVersion::v3_6a, VisVersion::v3_7a } // Effect of SD - no idea how to version this
			) );

	struct ConvertFullPathTestData
	{
		std::string inputPath;
		std::string expectedPath;
	};

	class GmodVersioningConvertFullPathTest : public ::testing::TestWithParam<ConvertFullPathTestData>
	{
	};

	TEST_P( GmodVersioningConvertFullPathTest, ConvertFullPath )
	{
		const auto& params = GetParam();
		const auto& vis = VIS::instance();

		const auto& sourceGmod = vis.gmod( VisVersion::v3_4a );
		const auto& targetGmod = vis.gmod( VisVersion::v3_6a );
		const auto& sourceLocations = vis.locations( VisVersion::v3_4a );
		const auto& targetLocations = vis.locations( VisVersion::v3_6a );

		ParsingErrors sourceErrors;
		ParsingErrors targetErrors;

		auto sourcePath = GmodPath::fromFullPathString( params.inputPath, sourceGmod, sourceLocations, sourceErrors );
		ASSERT_TRUE( sourcePath.has_value() ) << "Failed to parse source path: " << params.inputPath
											  << "\nErrors: " << sourceErrors.toString();

		auto parsedTargetPath = GmodPath::fromFullPathString( params.expectedPath, targetGmod, targetLocations, targetErrors );
		ASSERT_TRUE( parsedTargetPath.has_value() ) << "Failed to parse expected path: " << params.expectedPath
													<< "\nErrors: " << targetErrors.toString();

		auto targetPath = vis.convertPath( VisVersion::v3_4a, *sourcePath, VisVersion::v3_6a );

		ASSERT_TRUE( targetPath.has_value() ) << "Failed to convert full path";

		EXPECT_EQ( params.inputPath, sourcePath->toFullPathString() );
		EXPECT_EQ( params.expectedPath, parsedTargetPath->toFullPathString() );
		EXPECT_EQ( params.expectedPath, targetPath->toFullPathString() );
	}

	INSTANTIATE_TEST_SUITE_P(
		GmodVersioningTests,
		GmodVersioningConvertFullPathTest,
		::testing::Values(
			ConvertFullPathTestData{ "VE/600a/630/632/632.3/632.32/632.32i-2/S110", "VE/600a/630/632/632.3/632.32/632.32i-2/SS5/S110" } ) );

	TEST( GmodVersioningTests, ConversionExceptions )
	{
		const auto& vis = VIS::instance();
		const auto& sourceGmod = vis.gmod( VisVersion::v3_7a );
		const auto& sourceLocations = vis.locations( VisVersion::v3_7a );

		ParsingErrors errors;
		auto sourcePath = GmodPath::fromString( "244.1i/H101.111/H401", sourceGmod, sourceLocations, errors );
		ASSERT_TRUE( sourcePath.has_value() );

		// Uncovered cases due to missing conversion codes, e.g. merge + normal assignment change
		EXPECT_THROW( vis.convertPath( VisVersion::v3_7a, *sourcePath, VisVersion::v3_8a ), std::runtime_error );
	}

	TEST( GmodVersioningTests, FindsPath )
	{
		const auto& vis = VIS::instance();
		const auto& gmod = vis.gmod( VisVersion::v3_4a );

		bool completed = gmod.traverse(
			[&]( const std::vector<const GmodNode*>& parents, const GmodNode& node ) -> TraversalHandlerResult {
				if ( parents.empty() )
				{
					return TraversalHandlerResult::Continue;
				}

				std::vector<GmodNode> parentNodes;
				parentNodes.reserve( parents.size() );
				for ( const GmodNode* p : parents )
				{
					parentNodes.push_back( *p );
				}
				GmodPath path{ std::move( parentNodes ), node };
				if ( path.toString() == "1012.22/S201.1/C151.2/S110.2/C101.61/S203.2/S101" )
				{
					return TraversalHandlerResult::Stop;
				}

				return TraversalHandlerResult::Continue;
			} );

		EXPECT_FALSE( completed );
	}

	TEST( GmodVersioningTests, OnePathToRootForAssetFunctions )
	{
		auto onePathToRoot = []( const GmodNode& node, auto& self ) -> bool {
			if ( node.isRoot() )
				return true;
			const auto& parents = node.parents();
			return parents.size() == 1 && self( *parents[0], self );
		};

		const auto& vis = VIS::instance();
		const auto& versions = vis.versions();
		for ( const auto& version : versions )
		{
			const auto& gmod = vis.gmod( version );
			for ( const auto& [code, node] : gmod )
			{
				if ( !node.isAssetFunctionNode() )
				{
					continue;
				}

				EXPECT_TRUE( onePathToRoot( node, onePathToRoot ) );
			}
		}
	}

	struct ConvertNodeTestData
	{
		std::string inputCode;
		std::optional<std::string> location;
		std::string expectedCode;
	};

	class GmodVersioningConvertNodeTest : public ::testing::TestWithParam<ConvertNodeTestData>
	{
	};

	TEST_P( GmodVersioningConvertNodeTest, ConvertNode )
	{
		const auto& params = GetParam();
		const auto& vis = VIS::instance();

		const auto& sourceGmod = vis.gmod( VisVersion::v3_4a );
		const auto& targetGmod = vis.gmod( VisVersion::v3_6a );

		const auto& sourceNode = sourceGmod[params.inputCode];
		const auto& expectedNode = targetGmod[params.expectedCode];

		auto targetNode = vis.convertNode( VisVersion::v3_4a, sourceNode, VisVersion::v3_6a );

		ASSERT_TRUE( targetNode.has_value() );
		EXPECT_EQ( expectedNode.code(), targetNode->code() );
	}

	TEST( GmodVersioningTests, ConvertNodePreservesLocation )
	{
		const auto& vis = VIS::instance();
		const auto& sourceGmod = vis.gmod( VisVersion::v3_7a );
		const auto& targetGmod = vis.gmod( VisVersion::v3_9a );
		const auto& sourceLocations = vis.locations( VisVersion::v3_7a );
		const auto& targetLocations = vis.locations( VisVersion::v3_9a );

		ParsingErrors errors;
		auto sourcePath = GmodPath::fromString( "691.811i-A", sourceGmod, sourceLocations, errors );
		ASSERT_TRUE( sourcePath.has_value() );
		ASSERT_TRUE( sourcePath->node().location().has_value() );

		auto convertedPath = vis.convertPath( VisVersion::v3_7a, *sourcePath, VisVersion::v3_9a );
		ASSERT_TRUE( convertedPath.has_value() );

		ASSERT_TRUE( convertedPath->node().location().has_value() );
		EXPECT_EQ( sourcePath->node().location()->value(), convertedPath->node().location()->value() );
	}

	INSTANTIATE_TEST_SUITE_P(
		GmodVersioningTests,
		GmodVersioningConvertNodeTest,
		::testing::Values(
			ConvertNodeTestData{ "1014.211", std::nullopt, "1014.211" },
			ConvertNodeTestData{ "323.5", std::nullopt, "323.6" },
			ConvertNodeTestData{ "412.72", std::nullopt, "412.7i" },
			ConvertNodeTestData{ "323.4", std::nullopt, "323.5" },
			ConvertNodeTestData{ "323.51", std::nullopt, "323.61" },
			ConvertNodeTestData{ "323.6", std::nullopt, "323.7" },
			ConvertNodeTestData{ "C101.212", std::nullopt, "C101.22" },
			ConvertNodeTestData{ "C101.22", std::nullopt, "C101.93" },
			ConvertNodeTestData{ "511.31", std::nullopt, "C121.1" },
			ConvertNodeTestData{ "C101.31", "5", "C101.31" } ) );

	struct ConvertLocalIdTestData
	{
		std::string sourceLocalIdStr;
		std::string targetLocalIdStr;
	};

	class GmodVersioningConvertLocalIdTest : public ::testing::TestWithParam<ConvertLocalIdTestData>
	{
	};

	TEST_P( GmodVersioningConvertLocalIdTest, ConvertLocalId )
	{
		const auto& params = GetParam();
		const auto& vis = VIS::instance();

		auto sourceLocalId = LocalId::fromString( params.sourceLocalIdStr );
		ASSERT_TRUE( sourceLocalId.has_value() ) << "Failed to parse source LocalId: " << params.sourceLocalIdStr;

		auto targetLocalId = LocalId::fromString( params.targetLocalIdStr );
		ASSERT_TRUE( targetLocalId.has_value() ) << "Failed to parse target LocalId: " << params.targetLocalIdStr;

		auto convertedLocalId = vis.convertLocalId( *sourceLocalId, targetLocalId->version() );

		ASSERT_TRUE( convertedLocalId.has_value() ) << "Failed to convert LocalId";
		EXPECT_EQ( targetLocalId->toString(), convertedLocalId->toString() );
	}

	INSTANTIATE_TEST_SUITE_P(
		GmodVersioningTests,
		GmodVersioningConvertLocalIdTest,
		::testing::Values(
			ConvertLocalIdTestData{
				"/dnv-v2/vis-3-4a/411.1/C101/sec/411.1/C101.64i/S201/meta/cnt-condensate",
				"/dnv-v2/vis-3-5a/411.1/C101/sec/411.1/C101.64/S201/meta/cnt-condensate" },
			ConvertLocalIdTestData{
				"/dnv-v2/vis-3-4a/411.1/C101.64i-1/S201.1/C151.2/S110/meta/cnt-hydraulic.oil/state-running",
				"/dnv-v2/vis-3-9a/411.1/C101.64-1/S201.1/C151.2/S110/meta/cnt-hydraulic.oil/state-running" } ) );

	struct GmodPathTestItem
	{
		std::string path;
		std::string visVersion;
	};

	static std::vector<GmodPathTestItem> LoadValidGmodPaths()
	{
		auto doc = EmbeddedTestData::load<Document>( "GmodPaths.json" ).value();
		auto array = doc.get<Document::Array>( "Valid" ).value();

		std::vector<GmodPathTestItem> result;
		result.reserve( array.size() );

		for ( size_t i = 0; i < array.size(); ++i )
		{
			auto item = array.get<Document::Object>( i ).value();

			GmodPathTestItem data;
			data.path = item.get<std::string>( "path" ).value();
			data.visVersion = item.get<std::string>( "visVersion" ).value();

			result.push_back( data );
		}
		return result;
	}

	class GmodVersioningValidPathToLatestTest : public ::testing::TestWithParam<GmodPathTestItem>
	{
	};

	TEST_P( GmodVersioningValidPathToLatestTest, Test_Valid_GmodPath_To_Latest )
	{
		const auto& params = GetParam();
		const auto& vis = VIS::instance();

		auto sourceVersionOpt = VisVersions::fromString( params.visVersion );
		ASSERT_TRUE( sourceVersionOpt.has_value() );
		auto sourceVersion = *sourceVersionOpt;

		const auto& sourceGmod = vis.gmod( sourceVersion );
		const auto& sourceLocations = vis.locations( sourceVersion );

		ParsingErrors errors;
		auto sourcePath = GmodPath::fromString( params.path, sourceGmod, sourceLocations, errors );
		ASSERT_TRUE( sourcePath.has_value() ) << "Failed to parse: " << params.path;

		auto targetPath = vis.convertPath( sourceVersion, *sourcePath, vis.latest() );
		EXPECT_TRUE( targetPath.has_value() ) << "Failed to convert: " << params.path << " from " << params.visVersion;
	}

	INSTANTIATE_TEST_SUITE_P( GmodVersioningTests, GmodVersioningValidPathToLatestTest,
		::testing::ValuesIn( LoadValidGmodPaths() ) );

	TEST( GmodVersioningTests, ConvertGmodPathWithLocation )
	{
		const auto& vis = VIS::instance();
		const auto& sourceGmod = vis.gmod( VisVersion::v3_7a );
		const auto& targetGmod = vis.gmod( VisVersion::v3_9a );
		const auto& sourceLocations = vis.locations( VisVersion::v3_7a );
		const auto& targetLocations = vis.locations( VisVersion::v3_9a );

		ParsingErrors sourceErrors;
		ParsingErrors targetErrors;

		auto sourcePath = GmodPath::fromString( "691.811i-A/H101.11-1", sourceGmod, sourceLocations, sourceErrors );
		ASSERT_TRUE( sourcePath.has_value() );

		auto expectedPath = GmodPath::fromString( "691.83111i-A/H101.11-1", targetGmod, targetLocations, targetErrors );
		ASSERT_TRUE( expectedPath.has_value() );

		auto convertedPath = vis.convertPath( VisVersion::v3_7a, *sourcePath, VisVersion::v3_9a );
		ASSERT_TRUE( convertedPath.has_value() );

		EXPECT_EQ( expectedPath->toString(), convertedPath->toString() );
	}
} // namespace dnv::vista::sdk::test
