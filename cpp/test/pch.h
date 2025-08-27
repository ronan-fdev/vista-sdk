#pragma once

#include <immintrin.h>

/* STL */
#include <charconv>
#include <exception>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <stdexcept>
#include <string>
#include <stdexcept>
#include <unordered_set>
#include <variant>
#include <vector>

/* Libs */
#include <fmt/format.h>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#if defined( __GNUC__ )
#	include <cpuid.h>
#endif
