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
#include <dnv/vista/sdk/VISVersions.h>

namespace dnv::vista::sdk::test
{
	//=====================================================================
	// VIS tests
	//=====================================================================

	TEST( VISTests, InstanceReturnsSameObject )
	{
		const auto& visInstance1 = VIS::instance();
		const auto& visInstance2 = VIS::instance();

		EXPECT_EQ( &visInstance1, &visInstance2 );
	}

	TEST( VISTests, LatestReturnsValidVersion )
	{
		const auto& latestVisVersion = VIS::instance().latest();

		// Assert - Latest should be v3_10a
		EXPECT_EQ( latestVisVersion, VISVersion::v3_10a );
		EXPECT_EQ( latestVisVersion, VISVersions::latest() );
	}

	TEST( VISTests, VisVersionsReturnsAllVersions )
	{
		const auto& allVisVersions = VIS::instance().all();

		EXPECT_EQ( allVisVersions.size(), 7 );
		EXPECT_EQ( allVisVersions[0], VISVersion::v3_4a );
		EXPECT_EQ( allVisVersions[1], VISVersion::v3_5a );
		EXPECT_EQ( allVisVersions[2], VISVersion::v3_6a );
		EXPECT_EQ( allVisVersions[3], VISVersion::v3_7a );
		EXPECT_EQ( allVisVersions[4], VISVersion::v3_8a );
		EXPECT_EQ( allVisVersions[5], VISVersion::v3_9a );
		EXPECT_EQ( allVisVersions[6], VISVersion::v3_10a );
	}

	TEST( VISTests, VisVersionsReturnsSameReference )
	{
		const auto& vis = VIS::instance();

		const auto& allVisVersions1 = vis.all();
		const auto& allVisVersions2 = vis.all();

		EXPECT_EQ( &allVisVersions1, &allVisVersions2 );
	}
} // namespace dnv::vista::sdk::test
