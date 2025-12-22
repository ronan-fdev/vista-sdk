# vista-sdk-cpp

> C++ implementation of the Vista maritime standards ecosystem, providing tools for vessel data standardization, sensor naming, and maritime digitalization according to **ISO 19848**, **ISO 19847**, and **DNV VIS**

> **⚠️ WARNING: API NOT STABLE - LIBRARY UNDER ACTIVE DEVELOPMENT**
> This library is in active development and the API may change without notice. Use with caution in production environments.

## API Overview

The vista-sdk-cpp provides a comprehensive C++ API for working with the DNV Vessel Information Structure (VIS) standard:

### Core Components

-   **`VIS`** - Singleton entry point providing access to versioned VIS data
    -   Access to Gmod, Codebooks, and Locations for all VIS versions
    -   Version management and latest version queries
    -   See [Sample_Codebooks.cpp](samples/simple/Sample_Codebooks.cpp) for usage examples
-   **`Gmod`** (Generic Product Model) - Equipment hierarchy and product tree
    -   Fast node lookup via operator[]
    -   Tree traversal with customizable handlers
    -   Product type and selection navigation
    -   See [Sample_Gmod.cpp](samples/simple/Sample_Gmod.cpp) for usage examples
-   **`GmodPath`** - Validated paths through the Gmod tree
    -   Parse short paths (e.g., "411.1-1P") with tree search
    -   Parse full paths (e.g., "VE/400a/410/411/411i/411.1-1P") for direct resolution
    -   Location-based individualization for equipment mapping
    -   Path comparison and format conversion
    -   See [Sample_GmodPath.cpp](samples/simple/Sample_GmodPath.cpp) for usage examples
-   **`Codebooks`** - Metadata tag validation and creation
    -   11 codebook types: Quantity, Content, Calculation, State, Command, Type, Position, etc.
    -   Value validation with standard and custom tag support
    -   Position codebook with special grouping validation
    -   See [Sample_Codebooks.cpp](samples/simple/Sample_Codebooks.cpp) for usage examples
-   **`Locations`** - Physical location representation and validation
    -   Parse and validate location strings (e.g., "1PS", "14C")
    -   Location components: Number, Side, Vertical, Transverse, Longitudinal
    -   Fluent builder API via `LocationBuilder`
    -   See [Sample_Locations.cpp](samples/simple/Sample_Locations.cpp) for usage examples
-   **`ImoNumber`** - IMO ship identification numbers
    -   Parse from string or integer formats
    -   See [Sample_ImoNumber.cpp](samples/simple/Sample_ImoNumber.cpp) for usage examples

### Quick Start

```cpp
#include <dnv/vista/sdk/VIS.h>

using namespace dnv::vista::sdk;

// Get VIS singleton and access versioned data
const auto& vis = VIS::instance();
const auto& gmod = vis.gmod(vis.latest());
const auto& locations = vis.locations(vis.latest());

// Parse a GMOD path
auto path = GmodPath::fromString("411.1-1P", gmod, locations);
if (path.has_value()) {
    std::cout << "Full path: " << path->toFullPathString() << "\n";
}

// Validate a location
auto location = locations.parse("1PS");
if (location.has_value()) {
    std::cout << "Valid location: " << location->value() << "\n";
}
```

## CMake Integration

```cmake
# --- Library build types ---
option(VISTA_SDK_CPP_BUILD_STATIC        "Build static library"               ON )
option(VISTA_SDK_CPP_BUILD_SHARED        "Build shared library"               OFF)

# --- Build components ---
option(VISTA_SDK_CPP_BUILD_TOOLS         "Build tools"                        OFF)
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

## Build Workflow

The SDK uses a multi-stage code generation pipeline:

```
┌───────────────────────────────────────────────────────────────────────────┐
│ Stage 1: Resource Embedding                                               │
├───────────────────────────────────────────────────────────────────────────┤
│                                                                           │
│  resources/*.json.gz ──► ResourceGenerator ──► embedded_*.cpp ──► .lib    │
│                                                                           │
│  Converts .gz resources to C++ byte arrays at build time                  │
└───────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌───────────────────────────────────────────────────────────────────────────┐
│ Stage 2: Type-Safe API Generation                                         │
├───────────────────────────────────────────────────────────────────────────┤
│                                                                           │
│  EmbeddedResource.lib ──► VisVersionsGenerator ──► VisVersions.h          │
│                                                                           │
│  Generates VisVersion enum and utilities from embedded VIS data           │
└───────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌───────────────────────────────────────────────────────────────────────────┐
│ Stage 3: SDK Library & Tests                                              │
├───────────────────────────────────────────────────────────────────────────┤
│                                                                           │
│  VisVersions.h + SDK sources ──► vista-sdk-cpp.lib                        │
│                                                                           │
└───────────────────────────────────────────────────────────────────────────┘
```

All stages are automated via CMake custom commands and run during the build.

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
├── tools/                  # Command-line utilities for VIS exploration
├── CHANGELOG.md            # Version history and changes
├── CMakeLists.txt          # Main CMake build configuration
├── README.md               # This documentation file
└── TODO.md                 # Project roadmap and task tracking
```

## Samples

The SDK includes comprehensive usage examples in `samples/simple/`:

-   **[Sample_Codebooks.cpp](samples/simple/Sample_Codebooks.cpp)** - VIS version management, codebook access, value validation, metadata tag creation
-   **[Sample_Gmod.cpp](samples/simple/Sample_Gmod.cpp)** - Generic Product Model navigation, node metadata, product types/selections
-   **[Sample_GmodPath.cpp](samples/simple/Sample_GmodPath.cpp)** - Path parsing (short/full), error handling, individualization, comparison
-   **[Sample_Locations.cpp](samples/simple/Sample_Locations.cpp)** - Location parsing, validation, LocationBuilder fluent API
-   **[Sample_ImoNumber.cpp](samples/simple/Sample_ImoNumber.cpp)** - IMO number creation, validation, checksum verification

Build samples with:

```bash
cmake -B build -DVISTA_SDK_CPP_BUILD_SAMPLES=ON
cmake --build build
./build/bin/samples/Sample_GmodPath  # Run an example
```

## Command-Line Tools

The SDK includes CLI utilities for exploring VIS data interactively:

### codebooks-cli - VIS Codebooks Explorer

Browse and search VIS codebook standard values:

```bash
codebooks-cli                       # List all codebooks (latest version)
codebooks-cli --version 3-4a        # Use specific VIS version
codebooks-cli --codebook qty        # Show Quantity codebook values
codebooks-cli --search temperature  # Search across all codebooks
codebooks-cli --help                # Show all options
```

### locations-cli - VIS Location Codes Explorer

Explore VIS location codes and definitions:

```bash
locations-cli                       # List all location codes (latest version)
locations-cli --version 3-4a        # Use specific VIS version
locations-cli --code P              # Show details for location 'P' (Port)
locations-cli --search starboard    # Search location names/definitions
locations-cli --help                # Show all options
```

### gmodexplorer-cli - VIS GMOD Tree Explorer

Navigate and explore the Generic Product Model hierarchy:

```bash
gmodexplorer-cli                    # Show tree overview (latest version)
gmodexplorer-cli --version 3-4a     # Use specific VIS version
gmodexplorer-cli --code 411.1       # Show node details
gmodexplorer-cli --search engine    # Search nodes by name/definition
gmodexplorer-cli --tree 400a 2      # Show subtree (depth 2)
gmodexplorer-cli --path "411.1-1P"  # Parse and validate GMOD path
gmodexplorer-cli --help             # Show all options
```

Build tools with:

```bash
cmake -B build -DVISTA_SDK_CPP_BUILD_TOOLS=ON
cmake --build build
./build/bin/tools/codebooks-cli --search pressure
./build/bin/tools/locations-cli --code S
./build/bin/tools/gmodexplorer-cli --search "engine"
```

## Performance

For detailed performance metrics and benchmarks, see the [benchmark documentation](benchmark/README.md).

## Roadmap

See [TODO.md](TODO.md) for upcoming features and project roadmap.

## Changelog

See the [CHANGELOG.md](CHANGELOG.md) for a detailed history of changes, new features, and bug fixes.

## License

This project is licensed under the MIT License.

## Core dependencies

-   **[zlib-ng](https://github.com/zlib-ng/zlib-ng)**: zlib replacement for .gz decompression (zlib License)
-   **[nfx-serialization](https://github.com/nfx-libs/nfx-serialization)**: JSON serialization library for C++ (MIT License)
    -   **[nlohmann/json](https://github.com/nlohmann/json)**: JSON for Modern C++ (MIT License)
-   **[nfx-stringbuilder](https://github.com/nfx-libs/nfx-stringbuilder)**: String builder library for C++ (MIT License)
-   **[nfx-stringutils](https://github.com/nfx-libs/nfx-stringutils)**: String utilities library (MIT License)
-   **[nfx-containers](https://github.com/nfx-libs/nfx-containers)**: Perfect hash map and container library (MIT License)
    -   **[nfx-hashing](https://github.com/nfx-libs/nfx-hashing)**: Hash function library (MIT License)

### Development Dependencies

-   **[GoogleTest](https://github.com/google/googletest)**: Testing framework (BSD 3-Clause License) - Development only
-   **[Google Benchmark](https://github.com/google/benchmark)**: Performance benchmarking framework (Apache 2.0 License) - Development only

All dependencies are automatically fetched via CMake FetchContent when building the library, tests, or benchmarks.

---

_Updated on December 21, 2025_
