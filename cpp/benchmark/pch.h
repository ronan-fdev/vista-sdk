#pragma once

#ifdef _WIN32
#	define NOMINMAX
#	define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#	include <psapi.h>
#	pragma comment( lib, "psapi.lib" )
#endif

/* STL */
#include <array>
#include <unordered_set>

/* Libs */
#include <spdlog/spdlog.h>
#include <benchmark/benchmark.h>
#include <nlohmann/json.hpp>
