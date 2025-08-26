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
#include <algorithm>
#include <array>
#include <atomic>
#include <barrier>
#include <charconv>
#include <chrono>
#include <mutex>
#include <numeric>
#include <random>
#include <shared_mutex>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <variant>
#include <vector>

/* Libs */
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <benchmark/benchmark.h>
#if defined( __GNUC__ )
#	include <cpuid.h>
#endif
