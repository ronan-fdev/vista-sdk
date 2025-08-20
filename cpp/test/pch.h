#pragma once

#include <immintrin.h>

/* STL */
#include <charconv>
#include <stdexcept>
#include <fstream>
#include <future>
#include <queue>
#include <shared_mutex>
#include <unordered_set>
#include <variant>

/* Libs */
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#if defined( _MSC_VER )
#	include <libcpuid.h>
#elif defined( __GNUC__ )
#	include <cpuid.h>
#endif
