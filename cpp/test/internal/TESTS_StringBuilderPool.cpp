/**
 * @file TESTS_StringBuilderPool.cpp
 * @brief StringBuilderPool unit tests
 * @details Validates pooled string building functionality and resource management
 */

#include "dnv/vista/sdk/internal/StringBuilderPool.h"

namespace dnv::vista::sdk::tests
{
	//=====================================================================
	// DynamicStringBuffer class
	//=====================================================================

	//----------------------------------------------
	// Construction and Destruction
	//----------------------------------------------

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_DefaultConstruction )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		// Test initial state
		ASSERT_EQ( buffer.size(), 0 );
		ASSERT_GE( buffer.capacity(), 256 ); // Should have at least initial capacity (default is 256 for now)
		ASSERT_TRUE( buffer.isEmpty() );
		ASSERT_EQ( buffer.toString(), "" );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_CopyConstruction )
	{
		auto lease1 = internal::StringBuilderPool::lease();
		auto& buffer1 = lease1.buffer();
		buffer1.append( "Copy test data" );

		auto lease2 = internal::StringBuilderPool::lease();
		auto& buffer2 = lease2.buffer();

		buffer2 = buffer1;

		ASSERT_EQ( buffer2.size(), 14 );
		ASSERT_EQ( buffer2.toString(), "Copy test data" );
		ASSERT_EQ( buffer1.toString(), "Copy test data" ); // Original unchanged
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_MoveConstruction )
	{
		auto lease1 = internal::StringBuilderPool::lease();
		auto& buffer1 = lease1.buffer();
		buffer1.append( "Move test data" );

		auto lease2 = internal::StringBuilderPool::lease();
		auto& buffer2 = lease2.buffer();

		buffer2 = std::move( buffer1 );

		ASSERT_EQ( buffer2.size(), 14 );
		ASSERT_EQ( buffer2.toString(), "Move test data" );
	}

	//----------------------------------------------
	// Capacity and Size Management
	//----------------------------------------------

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_SizeAndCapacity )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		// Initial state
		ASSERT_EQ( buffer.size(), 0 );
		ASSERT_GE( buffer.capacity(), 256 );
		ASSERT_TRUE( buffer.isEmpty() );

		// Add content
		buffer.append( "test" );
		ASSERT_EQ( buffer.size(), 4 );
		ASSERT_FALSE( buffer.isEmpty() );
		ASSERT_GE( buffer.capacity(), 4 );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_Reserve )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		// Reserve larger capacity
		buffer.reserve( 1024 );
		ASSERT_GE( buffer.capacity(), 1024 );
		ASSERT_EQ( buffer.size(), 0 ); // Size unchanged

		// Reserve smaller capacity (should not shrink)
		buffer.reserve( 512 );
		ASSERT_GE( buffer.capacity(), 1024 ); // Capacity should not decrease
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_Resize )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		buffer.append( "Hello World" );
		ASSERT_EQ( buffer.size(), 11 );

		// Resize larger
		buffer.resize( 20 );
		ASSERT_EQ( buffer.size(), 20 );
		ASSERT_GE( buffer.capacity(), 20 );

		// Resize smaller
		buffer.resize( 5 );
		ASSERT_EQ( buffer.size(), 5 );
		ASSERT_EQ( buffer.toString(), "Hello" );

		// Resize to zero
		buffer.resize( 0 );
		ASSERT_EQ( buffer.size(), 0 );
		ASSERT_TRUE( buffer.isEmpty() );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_Clear )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		buffer.append( "Clear test data" );
		size_t originalCapacity = buffer.capacity();

		ASSERT_EQ( buffer.size(), 15 );
		ASSERT_FALSE( buffer.isEmpty() );

		buffer.clear();

		ASSERT_EQ( buffer.size(), 0 );
		ASSERT_TRUE( buffer.isEmpty() );
		ASSERT_EQ( buffer.capacity(), originalCapacity ); // Capacity preserved
		ASSERT_EQ( buffer.toString(), "" );
	}

	//----------------------------------------------
	// Data Access
	//----------------------------------------------

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_DataAccess )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		buffer.append( "Data access test" );

		// Test const data access
		const char* constData = buffer.data();
		ASSERT_NE( constData, nullptr );
		ASSERT_EQ( constData[0], 'D' );
		ASSERT_EQ( constData[4], ' ' );

		// Test mutable data access
		char* mutableData = buffer.data();
		ASSERT_NE( mutableData, nullptr );
		ASSERT_EQ( mutableData, constData ); // Should point to same memory

		// Modify through mutable pointer
		mutableData[0] = 'M';
		ASSERT_EQ( buffer.toString(), "Mata access test" );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_ArrayAccess )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		buffer.append( "Array" );

		// Test read access
		ASSERT_EQ( buffer[0], 'A' );
		ASSERT_EQ( buffer[1], 'r' );
		ASSERT_EQ( buffer[2], 'r' );
		ASSERT_EQ( buffer[3], 'a' );
		ASSERT_EQ( buffer[4], 'y' );

		// Test write access
		buffer[1] = 'R';
		buffer[2] = 'R';
		ASSERT_EQ( buffer.toString(), "ARRay" );

		// Test const access
		const auto& constBuffer = buffer;
		ASSERT_EQ( constBuffer[0], 'A' );
		ASSERT_EQ( constBuffer[1], 'R' );
	}

	//----------------------------------------------
	// Content Manipulation
	//----------------------------------------------

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_AppendStringView )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		std::string_view testStr = "StringView test";
		buffer.append( testStr );

		ASSERT_EQ( buffer.size(), 15 );
		ASSERT_EQ( buffer.toString(), "StringView test" );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_AppendStdString )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		std::string testStr = "StdString test";
		buffer.append( testStr );

		ASSERT_EQ( buffer.size(), 14 );
		ASSERT_EQ( buffer.toString(), "StdString test" );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_AppendCString )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		const char* testStr = "CString test";
		buffer.append( testStr );

		ASSERT_EQ( buffer.size(), 12 );
		ASSERT_EQ( buffer.toString(), "CString test" );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_AppendNullCString )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		// Should handle null gracefully
		buffer.append( static_cast<const char*>( nullptr ) );

		ASSERT_EQ( buffer.size(), 0 );
		ASSERT_EQ( buffer.toString(), "" );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_PushBack )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		buffer.push_back( 'A' );
		buffer.push_back( 'B' );
		buffer.push_back( 'C' );

		ASSERT_EQ( buffer.size(), 3 );
		ASSERT_EQ( buffer.toString(), "ABC" );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_MultipleAppends )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		buffer.append( "Hello" );
		buffer.append( std::string( ", " ) );
		buffer.append( std::string_view( "World" ) );
		buffer.push_back( '!' );

		ASSERT_EQ( buffer.size(), 13 );
		ASSERT_EQ( buffer.toString(), "Hello, World!" );
	}

	//----------------------------------------------
	// String Conversion
	//----------------------------------------------

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_ToString )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		// Empty buffer
		ASSERT_EQ( buffer.toString(), "" );

		// With content
		buffer.append( "ToString test" );
		std::string result = buffer.toString();
		ASSERT_EQ( result, "ToString test" );

		// Multiple calls should return same result
		ASSERT_EQ( buffer.toString(), "ToString test" );
		ASSERT_EQ( buffer.toString(), result );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_ToStringView )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		// Empty buffer
		std::string_view emptyView = buffer.toStringView();
		ASSERT_EQ( emptyView.size(), 0 );
		ASSERT_TRUE( emptyView.empty() );

		// With content
		buffer.append( "StringView test" );
		std::string_view view = buffer.toStringView();
		ASSERT_EQ( view.size(), 15 );
		ASSERT_EQ( view, "StringView test" );

		// View should reference same memory as buffer
		ASSERT_EQ( view.data(), buffer.data() );
	}

	//----------------------------------------------
	// Iterator Interface
	//----------------------------------------------

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_IteratorTypes )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();
		builder.append( "hello" );

		// Test iterator types
		static_assert( std::is_same_v<internal::StringBuilder::iterator, char*> );
		static_assert( std::is_same_v<internal::StringBuilder::const_iterator, const char*> );
		static_assert( std::is_same_v<internal::StringBuilder::value_type, char> );

		// Test begin/end iterators
		auto it_begin = builder.begin();
		auto it_end = builder.end();
		auto const_it_begin = builder.begin();
		auto const_it_end = builder.end();

		ASSERT_NE( it_begin, it_end );
		ASSERT_EQ( it_end - it_begin, 5 );
		ASSERT_EQ( const_it_end - const_it_begin, 5 );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_BeginEndIterators )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();
		buffer.append( "iterator" );

		// Test mutable iterators
		auto it_begin = buffer.begin();
		auto it_end = buffer.end();
		ASSERT_NE( it_begin, it_end );
		ASSERT_EQ( it_end - it_begin, 8 );

		// Test const iterators
		const auto& constBuffer = buffer;
		auto const_it_begin = constBuffer.begin();
		auto const_it_end = constBuffer.end();
		ASSERT_NE( const_it_begin, const_it_end );
		ASSERT_EQ( const_it_end - const_it_begin, 8 );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_RangeBasedFor )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();
		buffer.append( "range" );

		std::string result;
		for ( char c : buffer )
		{
			result += c;
		}

		ASSERT_EQ( result, "range" );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_STLAlgorithms )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();
		buffer.append( "algorithm" );

		// Test std::find
		auto it = std::find( buffer.begin(), buffer.end(), 'o' );
		ASSERT_NE( it, buffer.end() );
		ASSERT_EQ( *it, 'o' );

		// Test std::count
		auto count = std::count( buffer.begin(), buffer.end(), 'a' );
		ASSERT_EQ( count, 1 );

		// Test std::distance
		auto distance = std::distance( buffer.begin(), buffer.end() );
		ASSERT_EQ( distance, 9 );

		// Test std::reverse
		std::reverse( buffer.begin(), buffer.end() );
		ASSERT_EQ( buffer.toString(), "mhtirogla" );
	}

	//----------------------------------------------
	// Edge Cases and Special Scenarios
	//----------------------------------------------

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_EmptyOperations )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		// Empty string operations
		buffer.append( "" );
		ASSERT_EQ( buffer.size(), 0 );
		ASSERT_TRUE( buffer.isEmpty() );

		buffer.append( std::string() );
		ASSERT_EQ( buffer.size(), 0 );

		buffer.append( std::string_view() );
		ASSERT_EQ( buffer.size(), 0 );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_LargeContent )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		// Create large content (exceeds initial capacity)
		std::string largeContent( 5000, 'L' );
		buffer.append( largeContent );

		ASSERT_EQ( buffer.size(), 5000 );
		ASSERT_GE( buffer.capacity(), 5000 );
		ASSERT_EQ( buffer.toString(), largeContent );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_SpecialCharacters )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		// Test special characters
		buffer.append( "Special: \n\t\r\"\\'" ); // 8 + 1 + 1 + 1 + 1 + 1 + 1 + 1 = 15
		buffer.push_back( '\0' );				 // Null character = 1
		buffer.append( " End" );				 // 1 + 3 = 4

		ASSERT_EQ( buffer.size(), 20 ); // 15 + 1 + 4

		// Check specific characters
		ASSERT_EQ( buffer[9], '\n' );
		ASSERT_EQ( buffer[10], '\t' );
		ASSERT_EQ( buffer[11], '\r' );
		ASSERT_EQ( buffer[15], '\0' );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_CapacityGrowth )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		size_t initialCapacity = buffer.capacity();

		// Gradually grow content
		std::string content;
		for ( int i = 0; i < 100; ++i )
		{
			content += "growth";
			buffer.append( "growth" );
		}

		ASSERT_EQ( buffer.size(), 600 ); // 100 * 6
		ASSERT_GE( buffer.capacity(), 600 );
		ASSERT_GT( buffer.capacity(), initialCapacity );
		ASSERT_EQ( buffer.toString(), content );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_Performance )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		// Performance test: many small appends
		for ( int i = 0; i < 1000; ++i )
		{
			buffer.append( "perf" );
		}

		ASSERT_EQ( buffer.size(), 4000 );

		// Verify content pattern
		std::string result = buffer.toString();
		ASSERT_EQ( result.substr( 0, 8 ), "perfperf" );
		ASSERT_EQ( result.substr( 3992, 8 ), "perfperf" );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_MemoryConsistency )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		buffer.append( "consistency" );

		// Data pointer should remain stable during read operations
		const char* data1 = buffer.data();
		std::string toString1 = buffer.toString();
		std::string_view toView1 = buffer.toStringView();
		const char* data2 = buffer.data();

		ASSERT_EQ( data1, data2 );
		ASSERT_EQ( toString1, "consistency" );
		ASSERT_EQ( toView1, "consistency" );
		ASSERT_EQ( toView1.data(), data1 );
	}

	TEST( DynamicStringBufferTests, Test_DynamicStringBuffer_Assignment )
	{
		auto lease1 = internal::StringBuilderPool::lease();
		auto& buffer1 = lease1.buffer();
		buffer1.append( "Source buffer" );

		auto lease2 = internal::StringBuilderPool::lease();
		auto& buffer2 = lease2.buffer();
		buffer2.append( "Target buffer" );

		// Copy assignment
		buffer2 = buffer1;
		ASSERT_EQ( buffer2.toString(), "Source buffer" );
		ASSERT_EQ( buffer1.toString(), "Source buffer" ); // Source unchanged

		// Modify copy
		buffer2.append( " modified" );
		ASSERT_EQ( buffer2.toString(), "Source buffer modified" );
		ASSERT_EQ( buffer1.toString(), "Source buffer" ); // Source still unchanged
	}

	//=====================================================================
	// StringBuilder tests
	//=====================================================================

	//----------------------------------------------
	// Construction
	//----------------------------------------------

	TEST( StringBuilderTests, Test_StringBuilder_BasicConstruction )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		// Test initial state
		ASSERT_EQ( builder.length(), 0 );
	}

	TEST( StringBuilderTests, Test_StringBuilder_CopyConstructor )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder1 = lease.builder();
		builder1.append( "Original" );

		// Test copy constructor
		auto builder2 = builder1;
		ASSERT_EQ( builder2.length(), 8 );

		// Modify copy - should affect both (they share the same buffer)
		builder2.push_back( '!' );
		ASSERT_EQ( builder1.length(), 9 );
		ASSERT_EQ( builder2.length(), 9 );
		ASSERT_EQ( lease.toString(), "Original!" );
	}

	//----------------------------------------------
	// Array access operators
	//----------------------------------------------

	TEST( StringBuilderTests, Test_StringBuilder_ArrayAccess )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		builder.append( "test" );

		// Test read access
		ASSERT_EQ( builder[0], 't' );
		ASSERT_EQ( builder[1], 'e' );
		ASSERT_EQ( builder[2], 's' );
		ASSERT_EQ( builder[3], 't' );

		// Test write access
		builder[1] = 'a';
		builder[2] = 'x';
		builder[3] = 'i';

		ASSERT_EQ( lease.toString(), "taxi" );
	}

	TEST( StringBuilderTests, Test_StringBuilder_ConstArrayAccess )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();
		builder.append( "const" );

		const auto& constBuilder = builder;

		// Test const access
		ASSERT_EQ( constBuilder[0], 'c' );
		ASSERT_EQ( constBuilder[1], 'o' );
		ASSERT_EQ( constBuilder[2], 'n' );
		ASSERT_EQ( constBuilder[3], 's' );
		ASSERT_EQ( constBuilder[4], 't' );
	}

	//----------------------------------------------
	// String append & push_back
	//----------------------------------------------

	TEST( StringBuilderTests, Test_StringBuilder_AppendStringView )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		std::string_view testStr = "Hello, World!";
		builder.append( testStr );

		ASSERT_EQ( builder.length(), 13 );
		ASSERT_EQ( lease.toString(), "Hello, World!" );
	}

	TEST( StringBuilderTests, Test_StringBuilder_AppendStdString )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		std::string testStr = "C++ StringBuilder";
		builder.append( testStr );

		ASSERT_EQ( builder.length(), 17 );
		ASSERT_EQ( lease.toString(), "C++ StringBuilder" );
	}

	TEST( StringBuilderTests, Test_StringBuilder_AppendCString )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		const char* testStr = "Null-terminated";
		builder.append( testStr );

		ASSERT_EQ( builder.length(), 15 );
		ASSERT_EQ( lease.toString(), "Null-terminated" );
	}

	TEST( StringBuilderTests, Test_StringBuilder_AppendNullCString )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		// Should handle null pointer gracefully
		builder.append( static_cast<const char*>( nullptr ) );

		ASSERT_EQ( builder.length(), 0 );
		ASSERT_EQ( lease.toString(), "" );
	}

	TEST( StringBuilderTests, Test_StringBuilder_PushBackChar )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		builder.push_back( 'A' );
		builder.push_back( 'B' );
		builder.push_back( 'C' );

		ASSERT_EQ( builder.length(), 3 );
		ASSERT_EQ( lease.toString(), "ABC" );
	}

	TEST( StringBuilderTests, Test_StringBuilder_MultipleAppends )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		builder.append( "Hello" );
		builder.append( ", " );
		builder.append( std::string( "C++" ) );
		builder.push_back( '!' );

		ASSERT_EQ( builder.length(), 11 );
		ASSERT_EQ( lease.toString(), "Hello, C++!" );
	}

	//----------------------------------------------
	// Stream operator
	//----------------------------------------------

	TEST( StringBuilderTests, Test_StringBuilder_StreamOperator_StringView )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		std::string_view testStr = "Hello, World!";
		builder << testStr;

		ASSERT_EQ( builder.length(), 13 );
		ASSERT_EQ( lease.toString(), "Hello, World!" );
	}

	TEST( StringBuilderTests, Test_StringBuilder_StreamOperator_StdString )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		std::string testStr = "C++ StringBuilder";
		builder << testStr;

		ASSERT_EQ( builder.length(), 17 );
		ASSERT_EQ( lease.toString(), "C++ StringBuilder" );
	}

	TEST( StringBuilderTests, Test_StringBuilder_StreamOperator_CString )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		const char* testStr = "Null-terminated";
		builder << testStr;

		ASSERT_EQ( builder.length(), 15 );
		ASSERT_EQ( lease.toString(), "Null-terminated" );
	}

	TEST( StringBuilderTests, Test_StringBuilder_StreamOperator_Char )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		builder << 'A' << 'B' << 'C';

		ASSERT_EQ( builder.length(), 3 );
		ASSERT_EQ( lease.toString(), "ABC" );
	}

	TEST( StringBuilderTests, Test_StringBuilder_StreamOperator_Chaining )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		builder << "Hello" << ", " << std::string( "C++" ) << '!';

		ASSERT_EQ( builder.length(), 11 );
		ASSERT_EQ( lease.toString(), "Hello, C++!" );
	}

	TEST( StringBuilderTests, Test_StringBuilder_StreamOperator_Mixed )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		// Mix stream operators with regular methods
		builder << "Start";
		builder.append( " middle" );
		builder << " end";

		ASSERT_EQ( builder.length(), 16 );
		ASSERT_EQ( lease.toString(), "Start middle end" );
	}

	TEST( StringBuilderTests, Test_StringBuilder_StreamOperator_NullCString )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		// Should handle null pointer gracefully
		builder << static_cast<const char*>( nullptr );

		ASSERT_EQ( builder.length(), 0 );
		ASSERT_EQ( lease.toString(), "" );
	}

	//----------------------------------------------
	// Size and capacity management
	//----------------------------------------------

	TEST( StringBuilderTests, Test_StringBuilder_Resize )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		builder.append( "Hello" );
		ASSERT_EQ( builder.length(), 5 );

		// Resize larger
		builder.resize( 10 );
		ASSERT_EQ( builder.length(), 10 );

		// Resize smaller
		builder.resize( 3 );
		ASSERT_EQ( builder.length(), 3 );
		ASSERT_EQ( lease.toString(), "Hel" );
	}

	//----------------------------------------------
	// Edge cases and special scenarios
	//----------------------------------------------

	TEST( StringBuilderTests, Test_StringBuilder_EmptyString )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		builder.append( "" );
		ASSERT_EQ( builder.length(), 0 );
		ASSERT_EQ( lease.toString(), "" );

		builder.append( std::string() );
		ASSERT_EQ( builder.length(), 0 );
		ASSERT_EQ( lease.toString(), "" );
	}

	TEST( StringBuilderTests, Test_StringBuilder_LargeString )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		// Build a large string (> 256 initial capacity)
		std::string largeStr( 1000, 'A' );
		builder.append( largeStr );

		ASSERT_EQ( builder.length(), 1000 );
		ASSERT_EQ( lease.toString(), largeStr );
	}

	TEST( StringBuilderTests, Test_StringBuilder_SpecialCharacters )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		builder.append( "Special: \n\t\r\"\\'" );
		builder.push_back( '\0' ); // Null character
		builder.append( " More text" );

		ASSERT_EQ( builder.length(), 26 ); // 15 + 1 + 10

		// Check specific characters
		ASSERT_EQ( builder[9], '\n' );
		ASSERT_EQ( builder[10], '\t' );
		ASSERT_EQ( builder[11], '\r' );
		ASSERT_EQ( builder[15], '\0' );
	}

	//----------------------------------------------
	// perf
	//----------------------------------------------

	TEST( StringBuilderTests, Test_StringBuilder_Performance )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		// Performance test: many small appends
		for ( int i = 0; i < 1000; ++i )
		{
			builder.append( "test" );
		}

		ASSERT_EQ( builder.length(), 4000 );

		// Verify content pattern
		std::string result = lease.toString();
		ASSERT_EQ( result.substr( 0, 8 ), "testtest" );
		ASSERT_EQ( result.substr( 392, 8 ), "testtest" );
	}

	//----------------------------------------------
	// Iterator interface tests
	//----------------------------------------------

	TEST( StringBuilderIteratorTests, Test_STL_Iterator_Interface_Basic )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();
		builder.append( "hello" );

		// Test iterator types
		static_assert( std::is_same_v<internal::StringBuilder::iterator, char*> );
		static_assert( std::is_same_v<internal::StringBuilder::const_iterator, const char*> );
		static_assert( std::is_same_v<internal::StringBuilder::value_type, char> );

		// Test begin/end iterators
		auto it_begin = builder.begin();
		auto it_end = builder.end();
		auto const_it_begin = builder.begin();
		auto const_it_end = builder.end();

		ASSERT_NE( it_begin, it_end );
		ASSERT_EQ( it_end - it_begin, 5 );
		ASSERT_EQ( const_it_end - const_it_begin, 5 );
	}

	TEST( StringBuilderIteratorTests, Test_STL_Iterator_Range_Based_For )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();
		builder.append( "abc" );

		std::string result;
		for ( char c : builder )
		{
			result += c;
		}

		ASSERT_EQ( "abc", result );
	}

	TEST( StringBuilderIteratorTests, Test_STL_Iterator_STL_Algorithms )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();
		builder.append( "hello" );

		// Test std::find
		auto it = std::find( builder.begin(), builder.end(), 'l' );
		ASSERT_NE( it, builder.end() );
		ASSERT_EQ( *it, 'l' );

		// Test std::count
		auto count = std::count( builder.begin(), builder.end(), 'l' );
		ASSERT_EQ( count, 2 );

		// Test std::distance
		auto distance = std::distance( builder.begin(), builder.end() );
		ASSERT_EQ( distance, 5 );
	}

	//----------------------------------------------
	// StringBuilder::Enumerator class tests
	//----------------------------------------------

	TEST( StringBuilderEnumeratorTests, Test_Enumerator_Basic_Operations )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();
		builder.append( "test" );

		internal::StringBuilder::Enumerator enumerator( builder );

		// Test initial state - next() should work before first character
		ASSERT_TRUE( enumerator.next() );
		ASSERT_EQ( enumerator.current(), 't' );

		ASSERT_TRUE( enumerator.next() );
		ASSERT_EQ( enumerator.current(), 'e' );

		ASSERT_TRUE( enumerator.next() );
		ASSERT_EQ( enumerator.current(), 's' );

		ASSERT_TRUE( enumerator.next() );
		ASSERT_EQ( enumerator.current(), 't' );

		// Should return false when past end
		ASSERT_FALSE( enumerator.next() );
	}

	TEST( StringBuilderEnumeratorTests, Test_Enumerator_Reset_Functionality )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();
		builder.append( "abc" );

		internal::StringBuilder::Enumerator enumerator( builder );

		// Move to second character
		ASSERT_TRUE( enumerator.next() );
		ASSERT_TRUE( enumerator.next() );
		ASSERT_EQ( enumerator.current(), 'b' );

		// Reset and start over
		enumerator.reset();

		ASSERT_TRUE( enumerator.next() );
		ASSERT_EQ( enumerator.current(), 'a' );

		ASSERT_TRUE( enumerator.next() );
		ASSERT_EQ( enumerator.current(), 'b' );

		ASSERT_TRUE( enumerator.next() );
		ASSERT_EQ( enumerator.current(), 'c' );

		ASSERT_FALSE( enumerator.next() );
	}

	TEST( StringBuilderEnumeratorTests, Test_Enumerator_Empty_StringBuilder )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		internal::StringBuilder::Enumerator enumerator( builder );

		// Should immediately return false for empty buffer
		ASSERT_FALSE( enumerator.next() );
	}

	TEST( StringBuilderEnumeratorTests, Test_Enumerator_CS_Style_Usage )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();
		builder.append( "hello" );

		internal::StringBuilder::Enumerator enumerator( builder );

		std::string result;
		while ( enumerator.next() )
		{
			result += enumerator.current();
		}

		ASSERT_EQ( "hello", result );
	}

	TEST( StringBuilderEnumeratorTests, Test_All_Iterator_Types_Consistency )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();
		builder.append( "consistency" );

		// STL iterator result
		std::string stl_result;
		for ( auto it = builder.begin(); it != builder.end(); ++it )
		{
			stl_result += *it;
		}

		// Range-based for result
		std::string range_result;
		for ( char c : builder )
		{
			range_result += c;
		}

		// Enumerator result
		std::string enum_result;
		internal::StringBuilder::Enumerator enumerator( builder );
		while ( enumerator.next() )
		{
			enum_result += enumerator.current();
		}

		// All should produce the same result
		ASSERT_EQ( "consistency", stl_result );
		ASSERT_EQ( "consistency", range_result );
		ASSERT_EQ( "consistency", enum_result );
	}

	//=====================================================================
	// StringBuilderLease tests
	//=====================================================================

	TEST( StringBuilderLeaseTests, Test_StringBuilderLease_BasicConstruction )
	{
		auto lease = internal::StringBuilderPool::lease();

		// Lease should be valid after construction
		ASSERT_NO_THROW( auto builder = lease.builder() );
		ASSERT_NO_THROW( [[maybe_unused]] auto& buffer = lease.buffer() );
		ASSERT_NO_THROW( auto str = lease.toString() );
	}

	TEST( StringBuilderLeaseTests, Test_StringBuilderLease_MoveConstruction )
	{
		auto lease1 = internal::StringBuilderPool::lease();
		lease1.builder().append( "test data" );

		// Move construct
		auto lease2 = std::move( lease1 );

		// lease2 should be valid
		ASSERT_NO_THROW( auto builder = lease2.builder() );
		ASSERT_EQ( lease2.toString(), "test data" );

		// lease1 should be invalid after move
		ASSERT_THROW( auto builder = lease1.builder(), std::runtime_error );
		ASSERT_THROW( [[maybe_unused]] auto& buffer = lease1.buffer(), std::runtime_error );
		ASSERT_THROW( auto str = lease1.toString(), std::runtime_error );
	}

	TEST( StringBuilderLeaseTests, Test_StringBuilderLease_MoveAssignment )
	{
		auto lease1 = internal::StringBuilderPool::lease();
		auto lease2 = internal::StringBuilderPool::lease();

		lease1.builder().append( "original data" );
		lease2.builder().append( "new data" );

		// Move assign
		lease1 = std::move( lease2 );

		// lease1 should now have lease2's data
		ASSERT_EQ( lease1.toString(), "new data" );

		// lease2 should be invalid after move
		ASSERT_THROW( auto builder = lease2.builder(), std::runtime_error );
	}

	TEST( StringBuilderLeaseTests, Test_StringBuilderLease_SelfMoveAssignment )
	{
		auto lease = internal::StringBuilderPool::lease();
		lease.builder().append( "self move test" );

		// Self move assignment should be safe
		lease = std::move( lease );

		// Lease should still be valid
		ASSERT_NO_THROW( auto builder = lease.builder() );
		ASSERT_EQ( lease.toString(), "self move test" );
	}

	TEST( StringBuilderLeaseTests, Test_StringBuilderLease_BuilderAccess )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		builder.append( "builder access test" );

		ASSERT_EQ( builder.length(), 19 );
		ASSERT_EQ( lease.toString(), "builder access test" );
	}

	TEST( StringBuilderLeaseTests, Test_StringBuilderLease_BufferAccess )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		// Direct buffer manipulation
		buffer.push_back( 'A' );
		buffer.push_back( 'B' );
		buffer.push_back( 'C' );

		ASSERT_EQ( buffer.size(), 3 );
		ASSERT_EQ( lease.toString(), "ABC" );
	}

	TEST( StringBuilderLeaseTests, Test_StringBuilderLease_ToString )
	{
		auto lease = internal::StringBuilderPool::lease();

		// Empty lease
		ASSERT_EQ( lease.toString(), "" );

		// With content
		lease.builder().append( "toString test" );
		ASSERT_EQ( lease.toString(), "toString test" );

		// Multiple calls should return same result
		ASSERT_EQ( lease.toString(), "toString test" );
		ASSERT_EQ( lease.toString(), "toString test" );
	}

	TEST( StringBuilderLeaseTests, Test_StringBuilderLease_MultipleBuilderCalls )
	{
		auto lease = internal::StringBuilderPool::lease();

		// Multiple builder() calls should return wrappers to same buffer
		auto builder1 = lease.builder();
		auto builder2 = lease.builder();

		builder1.append( "first" );
		ASSERT_EQ( builder2.length(), 5 ); // Should see changes from builder1

		builder2.append( " second" );
		ASSERT_EQ( builder1.length(), 12 ); // Should see changes from builder2

		ASSERT_EQ( lease.toString(), "first second" );
	}

	TEST( StringBuilderLeaseTests, Test_StringBuilderLease_BufferAndBuilderInteraction )
	{
		auto lease = internal::StringBuilderPool::lease();

		// Mix buffer and builder operations
		auto& buffer = lease.buffer();
		auto builder = lease.builder();

		buffer.push_back( 'X' );
		builder.append( "YZ" );
		buffer.push_back( '!' );

		ASSERT_EQ( lease.toString(), "XYZ!" );
		ASSERT_EQ( buffer.size(), 4 );
		ASSERT_EQ( builder.length(), 4 );
	}

	TEST( StringBuilderLeaseTests, Test_StringBuilderLease_RAII_Cleanup )
	{
		// Reset statistics for clean measurement
		internal::StringBuilderPool::resetStats();

		{
			auto lease = internal::StringBuilderPool::lease();
			lease.builder().append( "RAII test" );
			// Lease goes out of scope here - should return buffer to pool
		}

		// Next lease should get recycled buffer from thread-local cache
		auto newLease = internal::StringBuilderPool::lease();
		ASSERT_EQ( newLease.toString(), "" ); // Buffer should be cleared

		const auto& stats = internal::StringBuilderPool::stats();
		ASSERT_GT( stats.threadLocalHits, 0 ); // Should have hit cache
	}

	TEST( StringBuilderLeaseTests, Test_StringBuilderLease_ExceptionSafety )
	{
		auto lease = internal::StringBuilderPool::lease();
		lease.builder().append( "exception test" );

		// Move to invalidate
		auto movedLease = std::move( lease );

		// Access to invalidated lease should throw specific exception
		try
		{
			auto builder = lease.builder();
			FAIL() << "Expected std::runtime_error";
		}
		catch ( const std::runtime_error& e )
		{
			ASSERT_STREQ( e.what(), "Tried to access StringBuilder after it was returned to pool" );
		}

		try
		{
			[[maybe_unused]] auto& buffer = lease.buffer();
			FAIL() << "Expected std::runtime_error";
		}
		catch ( const std::runtime_error& e )
		{
			ASSERT_STREQ( e.what(), "Tried to access StringBuilder after it was returned to pool" );
		}

		try
		{
			auto str = lease.toString();
			FAIL() << "Expected std::runtime_error";
		}
		catch ( const std::runtime_error& e )
		{
			ASSERT_STREQ( e.what(), "Tried to access StringBuilder after it was returned to pool" );
		}
	}

	TEST( StringBuilderLeaseTests, Test_StringBuilderLease_LargeContent )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto builder = lease.builder();

		// Build large content
		std::string largeContent( 5000, 'L' );
		builder.append( largeContent );

		ASSERT_EQ( builder.length(), 5000 );
		ASSERT_EQ( lease.toString(), largeContent );
		ASSERT_EQ( lease.buffer().size(), 5000 );
	}

	TEST( StringBuilderLeaseTests, Test_StringBuilderLease_MemoryAlignment )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		// Test that buffer pointer is properly aligned
		void* bufferPtr = static_cast<void*>( buffer.data() );
		uintptr_t address = reinterpret_cast<uintptr_t>( bufferPtr );

		// Buffer should be properly aligned (at least to sizeof(void*))
		ASSERT_EQ( address % sizeof( void* ), 0 );
	}

	TEST( StringBuilderLeaseTests, Test_StringBuilderLease_StateValidation )
	{
		auto lease = internal::StringBuilderPool::lease();

		// Valid lease operations
		ASSERT_NO_THROW( lease.builder().append( "valid" ) );
		ASSERT_NO_THROW( lease.buffer().push_back( '!' ) );
		ASSERT_NO_THROW( auto result = lease.toString() );

		// After move, original should be invalid
		auto movedLease = std::move( lease );

		ASSERT_THROW( lease.builder(), std::runtime_error );
		ASSERT_THROW( lease.buffer(), std::runtime_error );
		ASSERT_THROW( lease.toString(), std::runtime_error );

		// Moved-to lease should be valid
		ASSERT_NO_THROW( movedLease.builder() );
		ASSERT_NO_THROW( movedLease.buffer() );
		ASSERT_NO_THROW( movedLease.toString() );
		ASSERT_EQ( movedLease.toString(), "valid!" );
	}

	//=====================================================================
	// StringBuilderPool tests
	//=====================================================================

	TEST( StringBuilderPoolTests, Test_StringBuilderPool_MultipleInstances )
	{
		// Reset statistics for clean measurement
		internal::StringBuilderPool::resetStats();

		// Create multiple instances
		auto lease1 = internal::StringBuilderPool::lease();
		auto lease2 = internal::StringBuilderPool::lease();
		auto lease3 = internal::StringBuilderPool::lease();

		// Each should be independent
		lease1.builder().append( "first" );
		lease2.builder().append( "second" );
		lease3.builder().append( "third" );

		ASSERT_EQ( lease1.toString(), "first" );
		ASSERT_EQ( lease2.toString(), "second" );
		ASSERT_EQ( lease3.toString(), "third" );

		const auto& stats = internal::StringBuilderPool::stats();
		ASSERT_EQ( stats.totalRequests, 3 );
	}

	TEST( StringBuilderPoolTests, Test_StringBuilderPool_SequentialReuse )
	{
		// Reset statistics for clean measurement
		internal::StringBuilderPool::resetStats();

		// Use and release sequentially to test reuse
		{
			auto lease = internal::StringBuilderPool::lease();
			lease.builder().append( "test1" );
		} // Returns to thread-local cache

		{
			auto lease = internal::StringBuilderPool::lease();
			ASSERT_EQ( lease.toString(), "" ); // Should be cleared
			lease.builder().append( "test2" );
		} // Returns to thread-local cache

		{
			auto lease = internal::StringBuilderPool::lease();
			ASSERT_EQ( lease.toString(), "" ); // Should be cleared
			lease.builder().append( "test3" );
		}

		const auto& stats = internal::StringBuilderPool::stats();
		ASSERT_EQ( stats.totalRequests, 3 );
		ASSERT_GE( stats.threadLocalHits, 2 ); // At least 2 hits from cache
	}

	TEST( StringBuilderPoolTests, Test_StringBuilderPool_EmptyUsage )
	{
		// Reset statistics for clean measurement
		internal::StringBuilderPool::resetStats();

		auto lease = internal::StringBuilderPool::lease();

		// Use lease without adding content
		ASSERT_EQ( lease.toString(), "" );
		ASSERT_EQ( lease.buffer().size(), 0 );
		ASSERT_EQ( lease.builder().length(), 0 );

		const auto& stats = internal::StringBuilderPool::stats();
		ASSERT_EQ( stats.totalRequests, 1 );
	}

	TEST( StringBuilderPoolTests, Test_StringBuilderPool_LargeContentHandling )
	{
		// Reset statistics for clean measurement
		internal::StringBuilderPool::resetStats();

		{
			auto lease = internal::StringBuilderPool::lease();
			auto builder = lease.builder();

			// Create large content that exceeds initial capacity
			std::string largeData( 5000, 'X' );
			builder.append( largeData );

			ASSERT_EQ( builder.length(), 5000 );
			ASSERT_EQ( lease.toString(), largeData );
		} // Buffer might be discarded due to size

		// Next allocation should work normally
		auto newLease = internal::StringBuilderPool::lease();
		newLease.builder().append( "small" );
		ASSERT_EQ( newLease.toString(), "small" );

		const auto& stats = internal::StringBuilderPool::stats();
		ASSERT_EQ( stats.totalRequests, 2 );
	}

	TEST( StringBuilderPoolTests, Test_StringBuilderPool_RapidAllocationDeallocation )
	{
		// Reset statistics for clean measurement
		internal::StringBuilderPool::resetStats();

		const int iterations = 100;

		// Rapid allocation/deallocation cycles
		for ( int i = 0; i < iterations; ++i )
		{
			auto lease = internal::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "iteration " );
			builder.append( std::to_string( i ) );

			std::string expected = "iteration " + std::to_string( i );
			ASSERT_EQ( lease.toString(), expected );
		} // Each lease destroyed at end of loop

		const auto& stats = internal::StringBuilderPool::stats();
		ASSERT_EQ( stats.totalRequests, iterations );

		// Should have high hit rate due to thread-local caching
		double hitRate = stats.hitRate;
		ASSERT_GT( hitRate, 0.90 ); // At least 90% hit rate
	}

	TEST( StringBuilderPoolTests, Test_StringBuilderPool_MixedUsagePatterns )
	{
		// Reset statistics for clean measurement
		internal::StringBuilderPool::resetStats();

		// Pattern 1: Short-lived leases
		for ( int i = 0; i < 5; ++i )
		{
			auto lease = internal::StringBuilderPool::lease();
			lease.builder().append( "short" );
		}

		// Pattern 2: Long-lived leases
		std::vector<internal::StringBuilderLease> longLived;
		for ( int i = 0; i < 3; ++i )
		{
			auto lease = internal::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "long " );
			builder.append( std::to_string( i ) );
			longLived.push_back( std::move( lease ) );
		}

		// Pattern 3: More short-lived after long-lived are created
		for ( int i = 0; i < 5; ++i )
		{
			auto lease = internal::StringBuilderPool::lease();
			lease.builder().append( "after" );
		}

		// Verify long-lived leases still work
		for ( size_t i = 0; i < longLived.size(); ++i )
		{
			std::string expected = "long " + std::to_string( i );
			ASSERT_EQ( longLived[i].toString(), expected );
		}

		const auto& stats = internal::StringBuilderPool::stats();
		ASSERT_EQ( stats.totalRequests, 13 ); // 5 + 3 + 5
	}

	TEST( StringBuilderPoolTests, Test_StringBuilderPool_ThreadLocalBehavior )
	{
		// Reset statistics for clean measurement
		internal::StringBuilderPool::resetStats();

		// Sequential allocations - each should potentially hit thread-local cache
		{
			auto lease = internal::StringBuilderPool::lease();
			lease.builder().append( "first allocation" );
		} // Buffer returned to thread-local cache

		{
			auto lease = internal::StringBuilderPool::lease();
			ASSERT_EQ( lease.toString(), "" ); // Should be cleared
			lease.builder().append( "second allocation" );
		} // Buffer returned to thread-local cache

		{
			auto lease = internal::StringBuilderPool::lease();
			ASSERT_EQ( lease.toString(), "" );
			lease.builder().append( "third allocation" );
		}

		const auto& stats = internal::StringBuilderPool::stats();

		// Verify total requests
		ASSERT_EQ( stats.totalRequests, 3 );

		// Verify accounting: all requests should be accounted for
		uint64_t totalHits = stats.threadLocalHits + stats.dynamicStringBufferPoolHits;
		uint64_t newAllocs = stats.newAllocations;
		ASSERT_EQ( totalHits + newAllocs, 3 );

		// In an efficient pool, we expect high thread-local hit rate
		// The exact numbers depend on prior test state, but should show good reuse
		ASSERT_GE( stats.threadLocalHits, 2 );			   // At least 2 thread-local hits
		ASSERT_EQ( stats.dynamicStringBufferPoolHits, 0 ); // No shared pool hits in sequential single-thread usage

		// Calculate hit rate
		double hitRate = stats.hitRate;
		ASSERT_GE( hitRate, 0.66 ); // At least 66% hit rate (2/3)
		ASSERT_LE( hitRate, 1.0 );	// Maximum 100% hit rate
	}

	TEST( StringBuilderPoolTests, Test_StringBuilderPool_StatisticsAccuracy )
	{
		// Reset statistics for clean measurement
		internal::StringBuilderPool::resetStats();

		const auto& initialStats = internal::StringBuilderPool::stats();
		ASSERT_EQ( initialStats.totalRequests, 0 );
		ASSERT_EQ( initialStats.threadLocalHits, 0 );
		ASSERT_EQ( initialStats.dynamicStringBufferPoolHits, 0 );
		ASSERT_EQ( initialStats.newAllocations, 0 );
		ASSERT_EQ( initialStats.hitRate, 0.0 );

		// Create and destroy several leases
		const int count = 10;
		for ( int i = 0; i < count; ++i )
		{
			auto lease = internal::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "stats test " );
			builder.append( std::to_string( i ) );
		}

		const auto& finalStats = internal::StringBuilderPool::stats();
		ASSERT_EQ( finalStats.totalRequests, count );
		ASSERT_GT( finalStats.hitRate, 0.0 );

		// Total should equal sum of all hit types
		uint64_t totalHits = finalStats.threadLocalHits +
							 finalStats.dynamicStringBufferPoolHits +
							 finalStats.newAllocations;
		ASSERT_EQ( totalHits, finalStats.totalRequests );
	}

	TEST( StringBuilderPoolTests, Test_StringBuilderPool_CapacityGrowth )
	{
		auto lease = internal::StringBuilderPool::lease();
		auto& buffer = lease.buffer();

		// Start with small capacity
		size_t initialCapacity = buffer.capacity();
		ASSERT_GE( initialCapacity, 256 ); // Should have at least initial capacity

		// Grow beyond initial capacity
		std::string largeString( initialCapacity * 2, 'G' );
		lease.builder().append( largeString );

		// Capacity should have grown
		ASSERT_GT( buffer.capacity(), initialCapacity );
		ASSERT_EQ( buffer.size(), largeString.size() );
		ASSERT_EQ( lease.toString(), largeString );
	}

	TEST( StringBuilderPoolTests, Test_StringBuilderPool_EdgeCases )
	{
		// Test null/empty string handling
		{
			auto lease = internal::StringBuilderPool::lease();
			lease.builder().append( static_cast<const char*>( nullptr ) );
			ASSERT_EQ( lease.toString(), "" );
		}

		// Test empty string_view
		{
			auto lease = internal::StringBuilderPool::lease();
			lease.builder().append( std::string_view{} );
			ASSERT_EQ( lease.toString(), "" );
		}

		// Test empty std::string
		{
			auto lease = internal::StringBuilderPool::lease();
			lease.builder().append( std::string{} );
			ASSERT_EQ( lease.toString(), "" );
		}

		// Test single character
		{
			auto lease = internal::StringBuilderPool::lease();
			lease.builder().push_back( 'X' );
			ASSERT_EQ( lease.toString(), "X" );
		}
	}

	TEST( StringBuilderPoolTests, Test_StringBuilderPool_FactoryMethod )
	{
		// Test that factory method returns valid lease
		auto lease = internal::StringBuilderPool::lease();

		// Should be immediately usable
		ASSERT_NO_THROW( lease.builder() );
		ASSERT_NO_THROW( lease.buffer() );
		ASSERT_NO_THROW( lease.toString() );

		// Should start empty
		ASSERT_EQ( lease.toString(), "" );
		ASSERT_EQ( lease.buffer().size(), 0 );
		ASSERT_EQ( lease.builder().length(), 0 );
	}

	TEST( StringBuilderPoolTests, Test_StringBuilderPool_ResourceManagement )
	{
		// Reset statistics for clean measurement
		internal::StringBuilderPool::resetStats();

		// Create many leases simultaneously to test resource limits
		std::vector<internal::StringBuilderLease> leases;
		const size_t maxLeases = 30; // More than max pool size (24)

		for ( size_t i = 0; i < maxLeases; ++i )
		{
			auto lease = internal::StringBuilderPool::lease();
			auto builder = lease.builder();
			builder.append( "lease " );
			builder.append( std::to_string( i ) );
			leases.push_back( std::move( lease ) );
		}

		// All should be valid and independent
		for ( size_t i = 0; i < maxLeases; ++i )
		{
			std::string expected = "lease " + std::to_string( i );
			ASSERT_EQ( leases[i].toString(), expected );
		}

		const auto& stats = internal::StringBuilderPool::stats();
		ASSERT_EQ( stats.totalRequests, maxLeases );

		// Should have created new allocations since we exceeded pool capacity
		ASSERT_GT( stats.newAllocations, 0 );
	}
}
