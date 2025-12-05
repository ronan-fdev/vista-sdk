# vista-sdk-cpp

> C++ implementation of the Vista maritime standards ecosystem, providing tools for vessel data standardization, sensor naming, and maritime digitalization according to **ISO 19848**, **ISO 19847**, and **DNV VIS**

> **⚠️ WARNING: API NOT STABLE - LIBRARY UNDER ACTIVE DEVELOPMENT**
> This library is in active development and the API may change without notice. Use with caution in production environments.

## CMake Integration

```cmake
# --- Library build types ---
option(VISTA_SDK_CPP_BUILD_STATIC        "Build static library"               ON )
option(VISTA_SDK_CPP_BUILD_SHARED        "Build shared library"               OFF)

# --- Build components ---
option(VISTA_SDK_CPP_BUILD_TESTS         "Build tests"                        OFF)
option(VISTA_SDK_CPP_BUILD_SAMPLES       "Build samples"                      OFF)
option(VISTA_SDK_CPP_BUILD_BENCHMARKS    "Build benchmarks"                   OFF)
option(VISTA_SDK_CPP_BUILD_DOCUMENTATION "Build Doxygen documentation"        OFF)

# --- Installation ---
option(VISTA_SDK_CPP_INSTALL_PROJECT     "Install project"                    OFF)

# --- Packaging ---
option(VISTA_SDK_CPP_PACKAGE_SOURCE      "Enable source package generation"   OFF)
option(VISTA_SDK_CPP_PACKAGE_ARCHIVE     "Enable TGZ/ZIP package generation"  OFF)
option(VISTA_SDK_CPP_PACKAGE_DEB         "Enable DEB package generation"      OFF)
option(VISTA_SDK_CPP_PACKAGE_RPM         "Enable RPM package generation"      OFF)
option(VISTA_SDK_CPP_PACKAGE_WIX         "Enable WiX Windows installer (MSI)" OFF)
```

## Project Structure

```
vista-sdk/cpp/
├── benchmark/              # Performance benchmarks with Google Benchmark
├── cmake/                  # CMake modules and build configuration
├── doc/                    # Doxygen documentation generation
├── include/dnv/vista/sdk/  # Public API headers
├── licenses/               # Third-party license files
├── samples/                # Usage examples
├── src/                    # Implementation files
├── test/                   # Unit tests with GoogleTest
├── CHANGELOG.md            # Version history and changes
├── CMakeLists.txt          # Main CMake build configuration
├── README.md               # This documentation file
└── TODO.md                 # Project roadmap and task tracking
```

## Roadmap

See [TODO.md](TODO.md) for upcoming features and project roadmap.

## Changelog

See the [CHANGELOG.md](CHANGELOG.md) for a detailed history of changes, new features, and bug fixes.

## License

This project is licensed under the MIT License.

## Core dependencies

-   **[zlib-ng](https://github.com/zlib-ng/zlib-ng)**: zlib replacement for .gz decompression (zlib License)
-   **[nfx-serialization](https://github.com/nfx-libs/nfx-serialization)**: JSON serialization library for C++ (MIT License)
    -   **[nfx-stringutils](https://github.com/nfx-libs/nfx-stringutils)**: String utilities library (MIT License)
    -   **[nlohmann/json](https://github.com/nlohmann/json)**: JSON for Modern C++ (MIT License)

### Development Dependencies

-   **[GoogleTest](https://github.com/google/googletest)**: Testing framework (BSD 3-Clause License) - Development only
-   **[Google Benchmark](https://github.com/google/benchmark)**: Performance benchmarking framework (Apache 2.0 License) - Development only

All dependencies are automatically fetched via CMake FetchContent when building the library, tests, or benchmarks.

---

_Updated on December 05, 2025_
