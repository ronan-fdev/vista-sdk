# vista-sdk-cpp TODO

Project roadmap and task tracking for the vista-sdk-cpp library.

### Todo

-   [ ] Implement `LocalId` parsing and building
-   [ ] Implement `UniversalId` parsing and building
-   [ ] Add sample programs demonstrating SDK usage
-   [ ] Implement Transport layer (ISO-19848 `DataChannel` and `TimeSeriesData`)

### In Progress

-   [ ] Implement core SDK classes (`VIS`, `Codebook`, `Location`, `Gmod `, `GmodPath`, etc.)
    -   [x] `VIS` singleton class (access to versions and embedded resources)
    -   [x] `CodebookName` enumeration with string conversion utilities
    -   [x] `MetadataTag` class (metadata tag representation)
    -   [ ] `Codebook` class (codebook access and lookup)
    -   [ ] `Codebooks` class (container for version-specific codebooks)
    -   [ ] `Location` class (location representation)
    -   [ ] `Gmod` class (wrapper around GmodDto with query/navigation API)
    -   [ ] `GmodPath` class (path representation and parsing)

### Done ✓

-   [x] Create Basic CMake structure
-   [x] Add `ResourceGenerator` tool for compile-time resource embedding
-   [x] Create `EmbeddedResource` static library (embed `.gz` files as byte arrays)
-   [x] Create `EmbeddedTestData` static library (embed test data files)
-   [x] Create DTOs with nfx-serialization for JSON deserialization
-   [x] Create `VisVersionsGenerator` tool to link against `EmbeddedResource` library and create `VisVersions.h` and `VisVersionsExtensions.h`
