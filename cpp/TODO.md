# vista-sdk-cpp TODO

Project roadmap and task tracking for the vista-sdk-cpp library.

### Todo

-   [ ] Implement `LocalId` parsing and building
-   [ ] Implement `UniversalId` parsing and building
-   [ ] Implement `Codebook` classes
-   [ ] Create unit tests for core SDK functionality
-   [ ] Create benchmark suite
-   [ ] Add sample programs demonstrating SDK usage
-   [ ] Implement Transport layer (ISO-19848 `DataChannel` and `TimeSeriesData`)

### In Progress

-   [ ] Implement core SDK classes (`Gmod`, `VIS`, `GmodPath`, `Location`, etc.)

### Done ✓

-   [x] Create Basic CMake structure
-   [x] Create `EmbeddedResource` static library (embed `.gz` files as byte arrays)
-   [x] Create `VISVersionsGenerator` tool to link against `EmbeddedResource` library and create `VISVersions.h`
