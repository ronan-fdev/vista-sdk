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
 * @file TESTS_VIS.cpp
 * @brief Unit tests for VIS singleton
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

#include <algorithm>
#include <thread>
#include <vector>

namespace dnv::vista::sdk::test
{
	//=====================================================================
	// VIS tests
	//=====================================================================

	//----------------------------------------------
	// Instance
	//----------------------------------------------

	TEST( VISTests, InstanceReturnsSameObject )
	{
		const auto& visInstance1 = VIS::instance();
		const auto& visInstance2 = VIS::instance();

		EXPECT_EQ( &visInstance1, &visInstance2 );
	}

	//----------------------------------------------
	// Versions
	//----------------------------------------------

	TEST( VISTests, VisVersionsReturnsAllVersions )
	{
		const auto& allVisVersions = VIS::instance().versions();

		EXPECT_EQ( allVisVersions.size(), 6 );
		EXPECT_EQ( allVisVersions[0], VisVersion::v3_4a );
		EXPECT_EQ( allVisVersions[1], VisVersion::v3_5a );
		EXPECT_EQ( allVisVersions[2], VisVersion::v3_6a );
		EXPECT_EQ( allVisVersions[3], VisVersion::v3_7a );
		EXPECT_EQ( allVisVersions[4], VisVersion::v3_8a );
		EXPECT_EQ( allVisVersions[5], VisVersion::v3_9a );
	}

	TEST( VISTests, VisVersionsReturnsSameReference )
	{
		const auto& vis = VIS::instance();

		const auto& allVisVersions1 = vis.versions();
		const auto& allVisVersions2 = vis.versions();

		EXPECT_EQ( &allVisVersions1, &allVisVersions2 );
	}

	TEST( VISTests, VisVersionsReturnsOrdered )
	{
		const auto& versions = VIS::instance().versions();

		// Find indices of 3-4a and 3-9a
		auto it34 = std::find( versions.begin(), versions.end(), VisVersion::v3_4a );
		auto it39 = std::find( versions.begin(), versions.end(), VisVersion::v3_9a );

		ASSERT_NE( it34, versions.end() );
		ASSERT_NE( it39, versions.end() );

		// v3_4a should come before v3_9a
		EXPECT_LT( std::distance( versions.begin(), it34 ), std::distance( versions.begin(), it39 ) );
	}

	TEST( VISTests, LatestReturnsValidVersion )
	{
		auto latestVisVersion = VIS::instance().latest();

		EXPECT_EQ( latestVisVersion, VisVersion::v3_9a );
	}

	//----------------------------------------------
	// Thread safety
	//----------------------------------------------

	TEST( VISTests, ConcurrentSingletonAccess_ThreadSafety )
	{
		// Test that multiple threads can safely access the const singleton simultaneously
		constexpr int numThreads = 10;
		std::vector<std::thread> threads;
		std::vector<const VIS*> instances( numThreads, nullptr );

		// Launch threads that all try to access the const singleton at the same time
		for ( int i = 0; i < numThreads; ++i )
		{
			threads.emplace_back(
				[&instances, i]() {
					instances[i] = &VIS::instance();

					const auto& versions = instances[i]->versions();
					const auto& latest = instances[i]->latest();

					EXPECT_EQ( versions.size(), 6 );
					EXPECT_EQ( latest, VisVersion::v3_9a );
				} );
		}

		// Wait for all threads to complete
		for ( auto& thread : threads )
		{
			thread.join();
		}

		// All threads should have received the exact same const instance pointer
		for ( int i = 1; i < numThreads; ++i )
		{
			EXPECT_EQ( instances[0], instances[i] );
		}
	}
} // namespace dnv::vista::sdk::test
