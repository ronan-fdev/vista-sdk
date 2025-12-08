# vista-sdk-cpp TODO

Project roadmap and task tracking for the vista-sdk-cpp library.

### Todo

-   [ ] Add `Location` class and `LocationBuilder`
-   [ ] Add `Locations` class (locations collection/parser per VIS version)
-   [ ] Add `ImoNumber` class (IMO number validation for UniversalId)
-   [ ] Add`GmodNode` class (Gmod tree node with metadata)
-   [ ] Add`Gmod` class (wrapper around GmodDto with query/navigation API)
-   [ ] Add`GmodPath` class (path representation and parsing)
-   [ ] Add `LocalIdItems` (primary/secondary item representation)
-   [ ] Add `LocalId` class and `LocalIdBuilder`
-   [ ] Add `UniversalId` class and `UniversalIdBuilder`
-   [ ] Add Transport layer (ISO-19848 `DataChannel` and `TimeSeriesData` classes)
-   [ ] Add `GmodVersioning` (cross-version conversion system)
-   [ ] Add `LocalIdQuery` / `LocalIdQueryBuilder` (fluent query API for LocalIds)
-   [ ] Add `GmodPathQuery` (query API for GmodPaths)
-   [ ] Add `MetadataTagsQuery` (query API for metadata tags)
-   [ ] Add `ParsingErrors` class (error collection and reporting)
-   [ ] Add sample programs demonstrating SDK usage

### In Progress

-   [ ] Add unit tests
    -   [x] Add `TESTS_EmbeddedResource`
    -   [x] Add `TESTS_EmbeddedTestData`
    -   [x] Add `TESTS_VisVersions`
    -   [x] Add `TESTS_VIS`
    -   [x] Add `TESTS_CodebookNames`
    -   [x] Add `TESTS_StringBuilder`
    -   [x] Add `TESTS_Codebook`
    -   [x] Add `TESTS_Codebooks`
-   [ ] Add benchmark suite
    -   [x] Add `BM_StringBuilder`
    -   [x] Add `BM_VISCacheEfficiency`

### Done ✓

-   [x] Create Basic CMake structure
-   [x] Add `ResourceGenerator` tool for compile-time resource embedding
-   [x] Add `EmbeddedResource` static library (embed `.gz` files as byte arrays)
-   [x] Add `EmbeddedTestData` static library (embed test data files)
-   [x] Add DTOs with nfx-serialization for JSON deserialization
-   [x] Add `VisVersionsGenerator` tool to link against `EmbeddedResource` library and create `VisVersions.h` and `VisVersionsExtensions.h`
-   [x] Add `VIS` singleton
-   [x] Add`CodebookName` enumeration with string conversion utilities
-   [x] Add `StringBuilder` class
-   [x] Add`MetadataTag` class (metadata tag representation)
-   [x] Add`Codebook` class (codebook access and lookup)
-   [x] Add`Codebooks` class (container for version-specific codebooks)
