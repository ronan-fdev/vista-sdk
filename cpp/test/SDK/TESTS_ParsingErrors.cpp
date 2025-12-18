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
 * @file TESTS_ParsingErrors.cpp
 * @brief Unit tests for ParsingErrors public API
 */

#include <gtest/gtest.h>

#include <dnv/vista/sdk/VIS.h>

#include <algorithm>
#include <sstream>

namespace dnv::vista::sdk::test
{
	//=====================================================================
	// Test fixture
	//=====================================================================

	class ParsingErrorsTest : public ::testing::Test
	{
	protected:
		// Helper to create errors
		static ParsingErrors createErrors( std::vector<ParsingErrors::ErrorEntry> entries )
		{
			return ParsingErrors{ std::move( entries ) };
		}
	};

	//=====================================================================
	// Construction tests
	//=====================================================================

	TEST_F( ParsingErrorsTest, DefaultConstructor_CreatesEmpty )
	{
		ParsingErrors errors;

		EXPECT_EQ( errors.count(), 0 );
		EXPECT_FALSE( errors.hasErrors() );
	}

	TEST_F( ParsingErrorsTest, EmptySingleton_ReturnsEmptyInstance )
	{
		const auto& empty1 = ParsingErrors::empty();
		const auto& empty2 = ParsingErrors::empty();

		EXPECT_EQ( empty1.count(), 0 );
		EXPECT_FALSE( empty1.hasErrors() );
		EXPECT_EQ( &empty1, &empty2 ); // Same instance
	}

	TEST_F( ParsingErrorsTest, VectorConstructor_MovesErrors )
	{
		std::vector<ParsingErrors::ErrorEntry> entries = {
			{ "Invalid", "First error" },
			{ "InvalidCode", "Second error" },
		};

		auto errors = createErrors( std::move( entries ) );

		EXPECT_EQ( errors.count(), 2 );
		EXPECT_TRUE( errors.hasErrors() );
	}

	//=====================================================================
	// Copy and move semantics
	//=====================================================================

	TEST_F( ParsingErrorsTest, CopyConstructor_CopiesErrors )
	{
		auto original = createErrors( {
			{ "Invalid", "Error message" },
		} );

		ParsingErrors copy{ original };

		EXPECT_EQ( copy.count(), 1 );
		EXPECT_EQ( original, copy );
	}

	TEST_F( ParsingErrorsTest, MoveConstructor_MovesErrors )
	{
		auto original = createErrors( {
			{ "Invalid", "Error message" },
		} );

		ParsingErrors moved{ std::move( original ) };

		EXPECT_EQ( moved.count(), 1 );
		EXPECT_TRUE( moved.hasErrors() );
	}

	TEST_F( ParsingErrorsTest, CopyAssignment_CopiesErrors )
	{
		auto original = createErrors( {
			{ "Invalid", "Error message" },
		} );

		ParsingErrors copy;
		copy = original;

		EXPECT_EQ( copy.count(), 1 );
		EXPECT_EQ( original, copy );
	}

	TEST_F( ParsingErrorsTest, MoveAssignment_MovesErrors )
	{
		auto original = createErrors( {
			{ "Invalid", "Error message" },
		} );

		ParsingErrors moved;
		moved = std::move( original );

		EXPECT_EQ( moved.count(), 1 );
		EXPECT_TRUE( moved.hasErrors() );
	}

	//=====================================================================
	// Query methods
	//=====================================================================

	TEST_F( ParsingErrorsTest, Count_ReturnsErrorCount )
	{
		ParsingErrors empty;
		EXPECT_EQ( empty.count(), 0 );

		auto oneError = createErrors( {
			{ "Invalid", "Message" },
		} );
		EXPECT_EQ( oneError.count(), 1 );

		auto multipleErrors = createErrors( {
			{ "Invalid", "First" },
			{ "InvalidCode", "Second" },
			{ "InvalidOrder", "Third" },
		} );
		EXPECT_EQ( multipleErrors.count(), 3 );
	}

	TEST_F( ParsingErrorsTest, HasErrors_ReturnsCorrectState )
	{
		ParsingErrors empty;
		EXPECT_FALSE( empty.hasErrors() );

		auto withErrors = createErrors( {
			{ "Invalid", "Error" },
		} );
		EXPECT_TRUE( withErrors.hasErrors() );
	}

	TEST_F( ParsingErrorsTest, HasErrorType_FindsMatchingType )
	{
		auto errors = createErrors( {
			{ "Invalid", "First error" },
			{ "InvalidCode", "Second error" },
		} );

		EXPECT_TRUE( errors.hasErrorType( "Invalid" ) );
		EXPECT_TRUE( errors.hasErrorType( "InvalidCode" ) );
		EXPECT_FALSE( errors.hasErrorType( "InvalidOrder" ) );
		EXPECT_FALSE( errors.hasErrorType( "NonExistent" ) );
	}

	TEST_F( ParsingErrorsTest, HasErrorType_EmptyErrors_ReturnsFalse )
	{
		ParsingErrors empty;

		EXPECT_FALSE( empty.hasErrorType( "Invalid" ) );
		EXPECT_FALSE( empty.hasErrorType( "AnyType" ) );
	}

	//=====================================================================
	// Comparison operators
	//=====================================================================

	TEST_F( ParsingErrorsTest, Equality_EmptyErrors_AreEqual )
	{
		ParsingErrors empty1;
		ParsingErrors empty2;

		EXPECT_TRUE( empty1 == empty2 );
		EXPECT_FALSE( empty1 != empty2 );
	}

	TEST_F( ParsingErrorsTest, Equality_SameErrors_AreEqual )
	{
		auto errors1 = createErrors( {
			{ "Invalid", "Error message" },
			{ "InvalidCode", "Another error" },
		} );

		auto errors2 = createErrors( {
			{ "Invalid", "Error message" },
			{ "InvalidCode", "Another error" },
		} );

		EXPECT_TRUE( errors1 == errors2 );
		EXPECT_FALSE( errors1 != errors2 );
	}

	TEST_F( ParsingErrorsTest, Equality_DifferentErrors_AreNotEqual )
	{
		auto errors1 = createErrors( {
			{ "Invalid", "Error message" },
		} );

		auto errors2 = createErrors( {
			{ "InvalidCode", "Different error" },
		} );

		EXPECT_FALSE( errors1 == errors2 );
		EXPECT_TRUE( errors1 != errors2 );
	}

	TEST_F( ParsingErrorsTest, Equality_DifferentCount_AreNotEqual )
	{
		auto errors1 = createErrors( {
			{ "Invalid", "Error" },
		} );

		auto errors2 = createErrors( {
			{ "Invalid", "Error" },
			{ "InvalidCode", "Another" },
		} );

		EXPECT_FALSE( errors1 == errors2 );
		EXPECT_TRUE( errors1 != errors2 );
	}

	//=====================================================================
	// ErrorEntry comparison
	//=====================================================================

	TEST_F( ParsingErrorsTest, ErrorEntry_Equality_SameEntries_AreEqual )
	{
		ParsingErrors::ErrorEntry entry1{ "Invalid", "Message" };
		ParsingErrors::ErrorEntry entry2{ "Invalid", "Message" };

		EXPECT_TRUE( entry1 == entry2 );
	}

	TEST_F( ParsingErrorsTest, ErrorEntry_Equality_DifferentType_AreNotEqual )
	{
		ParsingErrors::ErrorEntry entry1{ "Invalid", "Message" };
		ParsingErrors::ErrorEntry entry2{ "InvalidCode", "Message" };

		EXPECT_FALSE( entry1 == entry2 );
	}

	TEST_F( ParsingErrorsTest, ErrorEntry_Equality_DifferentMessage_AreNotEqual )
	{
		ParsingErrors::ErrorEntry entry1{ "Invalid", "Message 1" };
		ParsingErrors::ErrorEntry entry2{ "Invalid", "Message 2" };

		EXPECT_FALSE( entry1 == entry2 );
	}

	//=====================================================================
	// toString() tests
	//=====================================================================

	TEST_F( ParsingErrorsTest, ToString_EmptyErrors_ReturnsSuccess )
	{
		ParsingErrors empty;

		EXPECT_EQ( empty.toString(), "Success" );
	}

	TEST_F( ParsingErrorsTest, ToString_SingleError_FormatsCorrectly )
	{
		auto errors = createErrors( {
			{ "Invalid", "Test error message" },
		} );

		std::string result = errors.toString();

		EXPECT_NE( result.find( "Parsing errors:" ), std::string::npos );
		EXPECT_NE( result.find( "Invalid" ), std::string::npos );
		EXPECT_NE( result.find( "Test error message" ), std::string::npos );
		EXPECT_NE( result.find( '\t' ), std::string::npos ); // Tab for indentation
	}

	TEST_F( ParsingErrorsTest, ToString_MultipleErrors_FormatsCorrectly )
	{
		auto errors = createErrors( {
			{ "Invalid", "First error" },
			{ "InvalidCode", "Second error" },
			{ "InvalidOrder", "Third error" },
		} );

		std::string result = errors.toString();

		EXPECT_NE( result.find( "Parsing errors:" ), std::string::npos );
		EXPECT_NE( result.find( "Invalid" ), std::string::npos );
		EXPECT_NE( result.find( "First error" ), std::string::npos );
		EXPECT_NE( result.find( "InvalidCode" ), std::string::npos );
		EXPECT_NE( result.find( "Second error" ), std::string::npos );
		EXPECT_NE( result.find( "InvalidOrder" ), std::string::npos );
		EXPECT_NE( result.find( "Third error" ), std::string::npos );
	}

	//=====================================================================
	// Iterator tests
	//=====================================================================

	TEST_F( ParsingErrorsTest, Iterators_EmptyErrors_BeginEqualsEnd )
	{
		ParsingErrors empty;

		EXPECT_EQ( empty.begin(), empty.end() );
	}

	TEST_F( ParsingErrorsTest, Iterators_RangeBasedFor_IteratesAllErrors )
	{
		auto errors = createErrors( {
			{ "Invalid", "First" },
			{ "InvalidCode", "Second" },
			{ "InvalidOrder", "Third" },
		} );

		size_t count = 0;
		for ( const auto& [type, message] : errors )
		{
			EXPECT_FALSE( type.empty() );
			EXPECT_FALSE( message.empty() );
			++count;
		}

		EXPECT_EQ( count, 3 );
	}

	TEST_F( ParsingErrorsTest, Iterators_ManualIteration_AccessesCorrectEntries )
	{
		auto errors = createErrors( {
			{ "Invalid", "First error" },
			{ "InvalidCode", "Second error" },
		} );

		auto it = errors.begin();
		ASSERT_NE( it, errors.end() );
		EXPECT_EQ( it->type, "Invalid" );
		EXPECT_EQ( it->message, "First error" );

		++it;
		ASSERT_NE( it, errors.end() );
		EXPECT_EQ( it->type, "InvalidCode" );
		EXPECT_EQ( it->message, "Second error" );

		++it;
		EXPECT_EQ( it, errors.end() );
	}

	TEST_F( ParsingErrorsTest, Iterators_STLAlgorithms_WorkCorrectly )
	{
		auto errors = createErrors( {
			{ "Invalid", "Error 1" },
			{ "InvalidCode", "Error 2" },
			{ "Invalid", "Error 3" },
		} );

		// Count errors of type "Invalid"
		size_t invalidCount = std::count_if(
			errors.begin(),
			errors.end(),
			[]( const auto& entry ) { return entry.type == "Invalid"; } );

		EXPECT_EQ( invalidCount, 2 );

		// Find first "InvalidCode" error
		auto it = std::find_if(
			errors.begin(),
			errors.end(),
			[]( const auto& entry ) { return entry.type == "InvalidCode"; } );

		ASSERT_NE( it, errors.end() );
		EXPECT_EQ( it->message, "Error 2" );
	}

	TEST_F( ParsingErrorsTest, Iterators_ConstCorrectness_CompilesToConstIterator )
	{
		auto errors = createErrors( {
			{ "Invalid", "Test" },
		} );

		// Should be const_iterator
		ParsingErrors::Iterator it = errors.begin();
		EXPECT_EQ( it->type, "Invalid" );

		// This should compile (const access)
		const auto& constErrors = errors;
		auto constIt = constErrors.begin();
		EXPECT_EQ( constIt->type, "Invalid" );
	}
} // namespace dnv::vista::sdk::test
