/**
 * @file TESTS_StringBuilderPoolTests.cpp
 * @brief StringBuilderPool unit tests
 * @details Validates pooled string building functionality and resource management
 */

#include "dnv/vista/sdk/utils/StringBuilderPool.h"

namespace dnv::vista::sdk::utils
{
	TEST( StringBuilderPoolTests, Test_Roundtrip )
	{
		auto lease = StringBuilderPool::instance();

		lease.builder().append( "1" );

		ASSERT_EQ( "1", lease.toString() );
	}

	TEST( StringBuilderPoolTests, Test_Use_After_Move_Throws_Exception )
	{
		auto lease = StringBuilderPool::instance();
		auto movedLease = std::move( lease );

		ASSERT_THROW( [[maybe_unused]] auto result = lease.toString(), std::runtime_error );
		ASSERT_THROW( [[maybe_unused]] auto builder = lease.builder(), std::runtime_error );

		ASSERT_NO_THROW( [[maybe_unused]] auto validBuilder = movedLease.builder() );
	}

	TEST( StringBuilderPoolTests, Test_Builder_Is_Cleaned )
	{
		{
			auto lease = StringBuilderPool::instance();
			auto builder = lease.builder();
			builder.append( "a" );
			ASSERT_EQ( 1U, builder.length() );
		}

		{
			auto newLease = StringBuilderPool::instance();
			auto newBuilder = newLease.builder();

			ASSERT_EQ( 0U, newBuilder.length() );
		}
	}
}
