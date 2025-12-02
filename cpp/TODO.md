# vista-sdk-cpp TODO

Project roadmap and task tracking for the vista-sdk-cpp library.

### Todo

-   [ ] Add `ResourceGenerator` tool for compile-time resource embedding
-   [ ] Add `EmbeddedResource` static library (embed `.gz` files as byte arrays)
-   [ ] Add `EmbeddedTestData` static library (embed test data files)
-   [ ] Add DTOs with nfx-serialization for JSON deserialization
-   [ ] Add `VisVersionsGenerator` tool to link against `EmbeddedResource` library and create `VisVersions.h` and `VisVersionsExtensions.h`
-   [ ] Add `VIS` singleton
-   [ ] Add`CodebookName` enumeration with string conversion utilities
-   [ ] Add `StringBuilder` class with object pooling
-   [ ] Add`MetadataTag` class (metadata tag representation)
-   [ ] Add`Codebook` class (codebook access and lookup)
-   [ ] Add`Codebooks` class (container for version-specific codebooks)
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
-   [ ] Add unit tests
-   [ ] Add benchmark suite
-   [ ] Add sample programs demonstrating SDK usage

### In Progress

-   NIL

### Done ✓

-   [x] Create Basic CMake structure
