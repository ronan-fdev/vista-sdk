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
 * @file TESTS_EmbeddedSchemas.cpp
 * @brief Unit tests for embedded schema resources
 */

#include <gtest/gtest.h>

#include <EmbeddedSchemas/EmbeddedSchemas.h>

#include <algorithm>
#include <string>

using namespace dnv::vista::sdk::schemas;

TEST( EmbeddedSchemasTests, AllSchemasLoaded )
{
	const auto& schemas = all();

	EXPECT_FALSE( schemas.empty() ) << "Should have embedded schemas";

	EXPECT_GT( schemas.size(), 5 ) << "Expected multiple schema files";
}

TEST( EmbeddedSchemasTests, SchemasHaveValidData )
{
	const auto& schemas = all();

	for ( const auto& schema : schemas )
	{
		EXPECT_FALSE( schema.name.empty() ) << "Schema name should not be empty";
		EXPECT_NE( schema.data, nullptr ) << "Schema " << schema.name << " data should not be null";
		EXPECT_GT( schema.size, 0 ) << "Schema " << schema.name << " size should be > 0";
	}
}

TEST( EmbeddedSchemasTests, ExpectedSchemasPresent )
{
	const auto& schemas = all();

	auto find_schema = [&schemas]( std::string_view name ) {
		return std::find_if( schemas.begin(), schemas.end(),
				   [name]( const SchemaResource& res ) {
					   return res.name.find( name ) != std::string_view::npos;
				   } ) != schemas.end();
	};

	EXPECT_TRUE( find_schema( "DataChannelList.schema.json" ) )
		<< "DataChannelList schema should be embedded";
	EXPECT_TRUE( find_schema( "TimeSeriesData.schema.json" ) )
		<< "TimeSeriesData schema should be embedded";

	EXPECT_TRUE( find_schema( "DataChannelList.xsd" ) )
		<< "DataChannelList XSD should be embedded";
	EXPECT_TRUE( find_schema( "TimeSeriesData.xsd" ) )
		<< "TimeSeriesData XSD should be embedded";
}

TEST( EmbeddedSchemasTests, SampleFilesPresent )
{
	const auto& schemas = all();

	auto find_schema = [&schemas]( std::string_view name ) {
		return std::find_if( schemas.begin(), schemas.end(),
				   [name]( const SchemaResource& res ) {
					   return res.name.find( name ) != std::string_view::npos;
				   } ) != schemas.end();
	};

	EXPECT_TRUE( find_schema( "DataChannelList.sample.json" ) )
		<< "Sample files should be embedded";
}

TEST( EmbeddedSchemasTests, SchemaPathsPreserved )
{
	const auto& schemas = all();

	auto has_json_subdir = std::any_of( schemas.begin(), schemas.end(),
		[]( const SchemaResource& res ) {
			return res.name.find( "json/" ) != std::string_view::npos;
		} );

	auto has_xml_subdir = std::any_of( schemas.begin(), schemas.end(),
		[]( const SchemaResource& res ) {
			return res.name.find( "xml/" ) != std::string_view::npos;
		} );

	EXPECT_TRUE( has_json_subdir ) << "Should preserve json/ subdirectory in paths";
	EXPECT_TRUE( has_xml_subdir ) << "Should preserve xml/ subdirectory in paths";
}

TEST( EmbeddedSchemasTests, ExperimentalSchemasPresent )
{
	const auto& schemas = all();

	auto has_experimental = std::any_of( schemas.begin(), schemas.end(),
		[]( const SchemaResource& res ) {
			return res.name.find( "experimental/" ) != std::string_view::npos;
		} );

	EXPECT_TRUE( has_experimental ) << "Should include experimental schemas";
}
