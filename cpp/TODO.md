# vista-sdk-cpp TODO

Project roadmap and task tracking for the vista-sdk-cpp library.

### Todo

-   [ ] Create `VisVersionsGenerator` tool to link against `EmbeddedResource` library and create `VisVersions.h` and `VisVersionsExtensions.h`
-   [ ] Implement core SDK classes (`VIS`, `Codebook`, `Location`, `Gmod `, `GmodPath`, etc.)
-   [ ] Implement `LocalId` parsing and building
-   [ ] Implement `UniversalId` parsing and building
-   [ ] Add sample programs demonstrating SDK usage
-   [ ] Implement Transport layer (ISO-19848 `DataChannel` and `TimeSeriesData`)

### In Progress

-   NIL

### Done ✓

-   [x] Create Basic CMake structure
-   [x] Add `ResourceGenerator` tool for compile-time resource embedding
-   [x] Create `EmbeddedResource` static library (embed `.gz` files as byte arrays)
-   [x] Create `EmbeddedTestData` static library (embed test data files)
