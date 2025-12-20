# Changelog

## [Unreleased]

### Added

-   `ResourceGenerator` tool for compile-time resource embedding
-   `EmbeddedResource` library with zlib-ng decompression
-   `EmbeddedTestData` library for embedding test data files
-   DTOs for Gmod, Codebooks, Locations, GmodVersioning, ISO19848
-   `VisVersionsGenerator` tool for `VisVersions.h` and `VisVersionsExtensions.h` code generation
-   `VIS` singleton class for accessing VIS versions and embedded resources
-   `CodebookName` enumeration and conversion utilities for VIS codebook types
-   `StringBuilder` class
-   `MetadataTag` class for VIS Local ID metadata with `StringBuilder` integration
-   `Codebook` class with position validation and `MetadataTag` creation
-   `Codebooks` class for VIS version-specific codebook access
-   `VIS::codebooks()` method with lazy loading and caching
-   `ParsingErrors` class for error accumulation with STL-compatible iteration
-   `LocationParsingErrorBuilder` for typed validation error accumulation
-   `LocationGroup` enumeration for classifying VIS location codes
-   `Location` value class for VIS location representation
-   `RelativeLocation` class with code, name, definition, and Location
-   `Locations` class for version-specific location collections with parsing and validation
-   `VIS::locations()` method with lazy loading and caching
-   `LocationBuilder` class with immutable builder pattern for constructing Location instances

### Changed

-   NIL

### Deprecated

-   NIL

### Removed

-   NIL

### Fixed

-   NIL

### Security

-   NIL
