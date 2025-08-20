#pragma once

#include <immintrin.h>

#ifdef _WIN32
#	define NOMINMAX
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <psapi.h>
#	pragma comment( lib, "psapi.lib" )
#endif

/* STL */
#include <array>
#include <charconv>
#include <mutex>
#include <random>
#include <shared_mutex>
#include <unordered_set>
#include <variant>
#include <vector>

/* Libs */
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <benchmark/benchmark.h>
#if defined( _MSC_VER )
#	include <libcpuid.h>
#elif defined( __GNUC__ )
#	include <cpuid.h>
#endif
