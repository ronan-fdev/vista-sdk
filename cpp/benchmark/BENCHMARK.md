# Vista SDK - C++ Benchmarking

**Windows:**

```bash
# Build benchmarks
cmake --build build --target BM_CodebooksLookup

# Execute with detailed output
.\build\bin\Release\BM_CodebooksLookup.exe --benchmark_min_time=10
```

**Linux:**

```bash
# Build benchmarks
cmake --build build --target BM_CodebooksLookup

# Execute with detailed output
./build/bin/Release/BM_CodebooksLookup --benchmark_min_time=10
```

## Test Environment

**Hardware Configuration:**

-   **CPU:** 12th Gen Intel Core i7-12800H (20 logical cores, 14 physical cores) @ 2.80 GHz
-   **RAM:** DDR4-3200 (32GB) - _will update with Linux system specs_
-   **CPU Cache:**
    -   L1 Data: 48 KiB (x10 cores)
    -   L1 Instruction: 32 KiB (x10 cores)
    -   L2 Unified: 1280 KiB (x10 cores)
    -   L3 Unified: 24576 KiB (x1 shared)

**Software Configuration:**

| Platform    | C++ Toolchain                              | C# Runtime               | OS Version |
| :---------- | :----------------------------------------- | :----------------------- | :--------- |
| **Windows** | Google Benchmark, MSVC 2022, Release build | .NET 8.0.16, RyuJIT AVX2 | Windows 10 |
| **Linux**   | TBC                                        | TBC                      | TBC        |

## Summary

| Operation Category          | C++ vs C# Performance |  Status  | Key Findings                     |
| :-------------------------- | :-------------------- | :------: | :------------------------------- |
| **Hash Table Operations**   | **2.3-2.6x faster**   |    ✅    | Superior low-level performance   |
| **String Hashing**          | **1.05-5.4x slower**  |    ❌    | Requires optimization            |
| **GMOD Loading**            | **1.15x faster**      |    ✅    | Competitive with 68x less memory |
| **GMOD Traversal**          | **1.7x slower**       |    ❌    | Requires optimization            |
| **High-Level APIs**         | **7-11x slower**      |    ❌    | Critical optimization needed     |
| **Path Parsing**            | **2-35x slower**      |    ❌    | Severe performance regression    |
| **Version Path Conversion** | **293x slower**       | ❌❌❌❌ | **CATASTROPHIC**                 |

---

## Codebooks Lookup

### Cross-Platform Performance Comparison

| Operation                   | Windows C++ | Linux C++ | Status | Notes |
| :-------------------------- | :---------- | :-------- | :----: | :---- |
| **Hash Table Lookup**       | 2.05 ns     | _TBD_     |        |       |
| **Frozen Dictionary**       | 2.06 ns     | _TBD_     |        |       |
| **Codebooks API (3 calls)** | 5.10 ns     | _TBD_     |        |       |
| **Single Codebook Lookup**  | 1.64 ns     | _TBD_     |        |       |

### Codebooks Lookup Performance (Windows)

Performance comparison between C++ and C# implementations for codebook lookup operations:

| Operation                   | C++ (Google Benchmark) | C# (BenchmarkDotNet) | Performance Ratio | Status | Notes                                     |
| :-------------------------- | :--------------------- | :------------------- | :---------------- | :----: | :---------------------------------------- |
| **Hash Table Lookup**       | 2.05 ns                | 4.64 ns              | **2.3x faster**   |   ✅   | C++ std::unordered_map vs C# Dictionary   |
| **Frozen Dictionary**       | 2.06 ns                | 4.83 ns              | **2.3x faster**   |   ✅   | Similar performance to regular hash table |
| **Codebooks API (3 calls)** | 5.10 ns                | 0.66 ns              | **7.7x slower**   |   ❌   | C++ needs major optimization              |
| **Single Codebook Lookup**  | 1.64 ns                | 0.22 ns (0.66÷3)     | **7.5x slower**   |   ❌   | Per-call overhead analysis                |

#### Detailed C++ Results (Windows)

| Benchmark                  | Time    | CPU     | Iterations | Memory Usage |
| :------------------------- | :------ | :------ | :--------- | :----------- |
| **Dict**                   | 2.05 ns | 2.05 ns | 7,055M     | 0 KB         |
| **FrozenDict**             | 2.06 ns | 2.06 ns | 6,892M     | 0 KB         |
| **Codebooks**              | 5.10 ns | 5.11 ns | 2,748M     | 0 KB         |
| **CodebooksArray**         | 4.89 ns | 4.89 ns | 2,863M     | 0 KB         |
| **SingleLookup_Dict**      | 1.41 ns | 1.41 ns | 9,956M     | 0 KB         |
| **SingleLookup_Codebooks** | 1.64 ns | 1.64 ns | 8,533M     | 0 KB         |
| **SingleLookup_Array**     | 1.63 ns | 1.63 ns | 8,374M     | 0 KB         |

#### Detailed C# Results (Windows)

| Method         | Mean      | Error     | StdDev    | Ratio        | RatioSD | Rank | Allocated | Alloc Ratio |
| :------------- | :-------- | :-------- | :-------- | :----------- | :------ | :--- | :-------- | :---------- |
| **FrozenDict** | 4.8284 ns | 0.0486 ns | 0.0431 ns | 1.04x slower | 0.01x   | 3    | -         | NA          |
| **Dict**       | 4.6447 ns | 0.0155 ns | 0.0145 ns | baseline     |         | 2    | -         | NA          |
| **Codebooks**  | 0.6573 ns | 0.0098 ns | 0.0092 ns | 7.07x faster | 0.10x   | 1    | -         | NA          |

---

## GMOD Load

### Cross-Platform Performance Comparison

| Operation     | Windows C++ | Linux C++ | Status | Notes |
| :------------ | :---------- | :-------- | :----: | :---- |
| **GMOD Load** | 26.5 ms     | _TBD_     |        |       |

### GMOD Load Performance (Windows)

Performance comparison between C++ and C# implementations for GMOD loading operations:

| Operation     | C++ (Google Benchmark) | C# (BenchmarkDotNet) | Performance Ratio | Status | Notes                                    |
| :------------ | :--------------------- | :------------------- | :---------------- | :----: | :--------------------------------------- |
| **GMOD Load** | 26.5 ms                | 30.4 ms              | **1.15x faster**  |   ✅   | C++ VIS::loadGmodDto + Gmod construction |

#### Detailed C++ Results (Windows)

| Benchmark    | Time    | CPU     | Iterations | Memory Usage |
| :----------- | :------ | :------ | :--------- | :----------- |
| **GmodLoad** | 26.5 ms | 26.4 ms | 484        | 224 KB       |

#### Detailed C# Results (Windows)

| Method   | Mean     | Error    | StdDev   | Gen0   | Gen1   | Gen2  | Allocated |
| :------- | :------- | :------- | :------- | :----- | :----- | :---- | :-------- |
| **Load** | 30.40 ms | 0.607 ms | 1.078 ms | 1562.5 | 1500.0 | 562.5 | 15.41 MB  |

---

## GMOD Lookup

### Cross-Platform Performance Comparison

| Operation             | Windows C++ | Linux C++ | Status | Notes |
| :-------------------- | :---------- | :-------- | :----: | :---- |
| **Hash Table Lookup** | 14.7 ns     | _TBD_     |        |       |
| **Frozen Dictionary** | 13.3 ns     | _TBD_     |        |       |
| **GMOD API Lookup**   | 173 ns      | _TBD_     |        |       |

### GMOD Lookup Performance (Windows)

Performance comparison between C++ and C# implementations for GMOD node lookup operations:

| Operation             | C++ (Google Benchmark) | C# (BenchmarkDotNet) | Performance Ratio | Status | Notes                                     |
| :-------------------- | :--------------------- | :------------------- | :---------------- | :----: | :---------------------------------------- |
| **Hash Table Lookup** | 14.7 ns                | 38.34 ns             | **2.6x faster**   |   ✅   | C++ std::unordered_map vs C# Dictionary   |
| **Frozen Dictionary** | 13.3 ns                | 15.21 ns             | **1.1x faster**   |   ✅   | Marginal C++ advantage                    |
| **GMOD API Lookup**   | 173 ns                 | 15.62 ns             | **11.1x slower**  |   ❌   | C++ CHD implementation needs optimization |

#### Detailed C++ Results (Windows)

| Benchmark      | Time    | CPU     | Iterations | Memory Usage |
| :------------- | :------ | :------ | :--------- | :----------- |
| **Dict**       | 14.7 ns | 14.6 ns | 963M       | 0 KB         |
| **FrozenDict** | 13.3 ns | 13.3 ns | 1,018M     | 0 KB         |
| **Gmod**       | 173 ns  | 173 ns  | 81M        | 0 KB         |

#### Detailed C# Results (Windows)

| Method         | Mean     | Error    | StdDev   | Ratio        | RatioSD | Rank | Allocated | Alloc Ratio |
| :------------- | :------- | :------- | :------- | :----------- | :------ | :--- | :-------- | :---------- |
| **Dict**       | 38.34 ns | 0.227 ns | 0.201 ns | baseline     |         | 3    | -         | NA          |
| **FrozenDict** | 15.21 ns | 0.039 ns | 0.035 ns | 2.52x faster | 0.02x   | 1    | -         | NA          |
| **Gmod**       | 15.62 ns | 0.255 ns | 0.226 ns | 2.46x faster | 0.04x   | 2    | -         | NA          |

---

## GMOD Path Parsing

### Cross-Platform Performance Comparison

| Operation                          | Windows C++ | Linux C++ | Status | Notes |
| :--------------------------------- | :---------- | :-------- | :----: | :---- |
| **TryParse**                       | 6.75 μs     | _TBD_     |        |       |
| **TryParseFullPath**               | 19.8 μs     | _TBD_     |        |       |
| **TryParseIndividualized**         | 4.21 μs     | _TBD_     |        |       |
| **TryParseFullPathIndividualized** | 20.2 μs     | _TBD_     |        |       |

### GMOD Path Parsing Performance (Windows)

Performance comparison between C++ and C# implementations for GMOD path parsing operations:

| Operation                          | C++ (Google Benchmark) | C# (BenchmarkDotNet) | Performance Ratio | Status | Notes                                       |
| :--------------------------------- | :--------------------- | :------------------- | :---------------- | :----: | :------------------------------------------ |
| **TryParse**                       | 6.75 μs                | 3.36 μs              | **2.0x slower**   |   ❌   | C++ path parsing overhead                   |
| **TryParseFullPath**               | 19.8 μs                | 571 ns               | **34.7x slower**  | ❌❌❌ | Critical performance issue - full paths     |
| **TryParseIndividualized**         | 4.21 μs                | 1.49 μs              | **2.8x slower**   |   ❌   | C++ location parsing overhead               |
| **TryParseFullPathIndividualized** | 20.2 μs                | 694 ns               | **29.1x slower**  | ❌❌❌ | Critical performance issue - individualized |

#### Detailed C++ Results (Windows)

| Benchmark                          | Time    | CPU     | Iterations | Memory Usage |
| :--------------------------------- | :------ | :------ | :--------- | :----------- |
| **TryParse**                       | 6.75 μs | 6.75 μs | 2,103K     | 0 KB         |
| **TryParseFullPath**               | 19.8 μs | 19.8 μs | 711K       | 0 KB         |
| **TryParseIndividualized**         | 4.21 μs | 4.21 μs | 3,343K     | 0 KB         |
| **TryParseFullPathIndividualized** | 20.2 μs | 20.2 μs | 695K       | 0 KB         |

#### Detailed C# Results (Windows)

| Method                             | Mean       | Error    | StdDev   | Categories    | Gen0   | Allocated |
| :--------------------------------- | :--------- | :------- | :------- | :------------ | :----- | :-------- |
| **TryParse**                       | 3,360.8 ns | 41.66 ns | 38.97 ns | No location   | 0.2213 | 2,792 B   |
| **TryParseFullPath**               | 571.2 ns   | 7.45 ns  | 5.82 ns  | No location   | 0.0601 | 760 B     |
| **TryParseIndividualized**         | 1,492.2 ns | 28.77 ns | 51.14 ns | With location | 0.2251 | 2,832 B   |
| **TryParseFullPathIndividualized** | 694.1 ns   | 4.33 ns  | 3.84 ns  | With location | 0.0935 | 1,176 B   |

---

## GMOD Traversal

### Cross-Platform Performance Comparison

| Operation          | Windows C++ | Linux C++ | Status | Notes |
| :----------------- | :---------- | :-------- | :----: | :---- |
| **Full Traversal** | 277 ms      | _TBD_     |        |       |

### GMOD Traversal Performance (Windows)

Performance comparison between C++ and C# implementations for GMOD node lookup operations:

| Operation          | C++ (Google Benchmark) | C# (BenchmarkDotNet) | Performance Ratio | Status | Notes                                |
| :----------------- | :--------------------- | :------------------- | :---------------- | :----: | :----------------------------------- |
| **Full Traversal** | 277 ms                 | 162.9 ms             | **1.7x slower**   |   ❌   | Significant optimization opportunity |

#### Detailed C++ Results (Windows)

| Benchmark         | Time   | CPU    | Iterations | Memory Usage |
| :---------------- | :----- | :----- | :--------- | :----------- |
| **FullTraversal** | 277 ms | 277 ms | 50-51      | 0 KB         |

#### Detailed C# Results (Windows)

| Method            | Mean     | Error   | StdDev  | Allocated |
| :---------------- | :------- | :------ | :------ | :-------- |
| **FullTraversal** | 162.9 ms | 0.97 ms | 0.91 ms | 5.3 KB    |

---

## GMOD Versioning Path Conversion

### Cross-Platform Performance Comparison

| Operation        | Windows C++ | Linux C++ | Status | Notes |
| :--------------- | :---------- | :-------- | :----: | :---- |
| **Convert Path** | 436 μs      | _TBD_     |        |       |

### GMOD Versioning Path Conversion Performance (Windows)

Performance comparison between C++ and C# implementations for GMOD version path conversion operations:

| Operation        | C++ (Google Benchmark) | C# (BenchmarkDotNet) | Performance Ratio |  Status  | Notes            |
| :--------------- | :--------------------- | :------------------- | :---------------- | :------: | :--------------- |
| **Convert Path** | 436 μs                 | 1.489 μs             | **293x slower**   | ❌❌❌❌ | **CATASTROPHIC** |

#### Detailed C++ Results (Windows)

| Benchmark       | Time   | CPU    | Iterations | Memory Usage |
| :-------------- | :----- | :----- | :--------- | :----------- |
| **ConvertPath** | 436 μs | 435 μs | 32,464     | 0 KB         |

#### Detailed C# Results (Windows)

| Method          | Mean     | Error     | StdDev    | Gen0   | Allocated |
| :-------------- | :------- | :-------- | :-------- | :----- | :-------- |
| **ConvertPath** | 1.489 μs | 0.0108 μs | 0.0090 μs | 0.2575 | 3.17 KB   |

---

## Short String Hashing

### Cross-Platform Performance Comparison

| Operation          | Windows C++ | Linux C++ | Status | Notes                     |
| :----------------- | :---------- | :-------- | :----: | :------------------------ |
| **Bcl (400)**      | 1.21 ns     | _TBD_     |   ❌   | Similar to C# performance |
| **Bcl (H346)**     | 3.33 ns     | _TBD_     |   ✅   | Competitive with C#       |
| **BclOrd (400)**   | 1.76 ns     | _TBD_     |   ❌   | Slightly slower than C#   |
| **BclOrd (H346)**  | 3.07 ns     | _TBD_     |   ✅   | Competitive with C#       |
| **Larsson (400)**  | 2.37 ns     | _TBD_     |   ❌   | 2x slower than C#         |
| **Larsson (H346)** | 17.6 ns     | _TBD_     |   ❌   | 5x slower than C#         |
| **CRC32 (400)**    | 2.13 ns     | _TBD_     |   ❌   | 2x slower than C#         |
| **CRC32 (H346)**   | 19.8 ns     | _TBD_     |   ❌   | 6x slower than C#         |
| **FNV (400)**      | 2.37 ns     | _TBD_     |   ❌   | 2x slower than C#         |
| **FNV (H346)**     | 17.7 ns     | _TBD_     |   ❌   | 5x slower than C#         |

### Short String Hashing Performance (Windows)

Performance comparison between C++ and C# implementations for hash function operations:

#### String "400" (3 characters)

| Algorithm          | C++ (Google Benchmark) | C# (BenchmarkDotNet) | Performance Ratio | Status | Notes                                      |
| :----------------- | :--------------------- | :------------------- | :---------------- | :----: | :----------------------------------------- |
| **Bcl**            | 1.21 ns                | 1.156 ns             | **1.05x slower**  |   ✅   | Competitive performance                    |
| **BclOrd**         | 1.76 ns                | 1.536 ns             | **1.15x slower**  |   ✅   | Slightly slower but acceptable             |
| **Larsson**        | 2.37 ns                | 1.225 ns             | **1.93x slower**  |   ❌   | Character-by-character processing overhead |
| **Crc32Intrinsic** | 2.13 ns                | 1.209 ns             | **1.76x slower**  |   ❌   | Hardware intrinsic overhead                |
| **Fnv**            | 2.37 ns                | 1.220 ns             | **1.94x slower**  |   ❌   | Manual implementation inefficiency         |

#### String "H346.11112" (10 characters)

| Algorithm          | C++ (Google Benchmark) | C# (BenchmarkDotNet) | Performance Ratio | Status | Notes                              |
| :----------------- | :--------------------- | :------------------- | :---------------- | :----: | :--------------------------------- |
| **Bcl**            | 3.33 ns                | 4.521 ns             | **1.36x faster**  |   ✅   | C++ advantage for longer strings   |
| **BclOrd**         | 3.07 ns                | 3.072 ns             | **1.00x equal**   |   ✅   | Nearly identical performance       |
| **Larsson**        | 17.6 ns                | 3.405 ns             | **5.17x slower**  |   ❌   | Severe performance degradation     |
| **Crc32Intrinsic** | 19.8 ns                | 3.274 ns             | **6.05x slower**  |   ❌   | Critical performance issue         |
| **Fnv**            | 17.7 ns                | 3.276 ns             | **5.40x slower**  |   ❌   | Manual hash algorithm inefficiency |

#### Detailed C++ Results (Windows)

| Benchmark                       | Time    | CPU     | Iterations | Memory Usage |
| :------------------------------ | :------ | :------ | :--------- | :----------- |
| **Short Strings (400)**         |         |         |            |              |
| **Bcl_400**                     | 1.21 ns | 1.21 ns | 11.6B      | 0 KB         |
| **BclOrd_400**                  | 1.76 ns | 1.76 ns | 8.0B       | 0 KB         |
| **Larsson_400**                 | 2.37 ns | 2.36 ns | 5.9B       | 0 KB         |
| **Crc32Intrinsic_400**          | 2.13 ns | 2.13 ns | 6.5B       | 0 KB         |
| **Fnv_400**                     | 2.37 ns | 2.37 ns | 5.8B       | 0 KB         |
| **Longer Strings (H346.11112)** |         |         |            |              |
| **Bcl_H346_11112**              | 3.33 ns | 3.33 ns | 4.2B       | 18.0KB       |
| **BclOrd_H346_11112**           | 3.07 ns | 3.07 ns | 4.5B       | 0 KB         |
| **Larsson_H346_11112**          | 17.6 ns | 17.6 ns | 800M       | 0 KB         |
| **Crc32Intrinsic_H346_11112**   | 19.8 ns | 19.8 ns | 716M       | 0 KB         |
| **Fnv_H346_11112**              | 17.7 ns | 17.7 ns | 792M       | 0 KB         |

#### Detailed C# Results (Windows)

| Method                          | Input      | Mean     | Error     | StdDev    | Ratio        | Allocated |
| :------------------------------ | :--------- | :------- | :-------- | :-------- | :----------- | :-------- |
| **Short Strings (400)**         |            |          |           |           |              |           |
| **Bcl**                         | 400        | 1.156 ns | 0.0178 ns | 0.0149 ns | baseline     | -         |
| **BclOrd**                      | 400        | 1.536 ns | 0.0147 ns | 0.0137 ns | 1.33x slower | -         |
| **Larsson**                     | 400        | 1.225 ns | 0.0098 ns | 0.0087 ns | 1.06x slower | -         |
| **Crc32Intrinsic**              | 400        | 1.209 ns | 0.0068 ns | 0.0060 ns | 1.05x slower | -         |
| **Fnv**                         | 400        | 1.220 ns | 0.0300 ns | 0.0266 ns | 1.06x slower | -         |
| **Longer Strings (H346.11112)** |            |          |           |           |              |           |
| **Bcl**                         | H346.11112 | 4.521 ns | 0.0194 ns | 0.0172 ns | baseline     | -         |
| **BclOrd**                      | H346.11112 | 3.072 ns | 0.0299 ns | 0.0250 ns | 1.47x faster | -         |
| **Larsson**                     | H346.11112 | 3.405 ns | 0.0126 ns | 0.0118 ns | 1.33x faster | -         |
| **Crc32Intrinsic**              | H346.11112 | 3.274 ns | 0.0083 ns | 0.0073 ns | 1.38x faster | -         |
| **Fnv**                         | H346.11112 | 3.276 ns | 0.0165 ns | 0.0146 ns | 1.38x faster | -         |
