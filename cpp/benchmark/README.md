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

**C# Benchmarks:**

```bash
# Windows & Linux
dotnet run -c Release --project benchmark/Vista.SDK.Benchmarks/Vista.SDK.Benchmarks.csproj
```

---

## Test Environment

**Hardware Configuration:**

-   **CPU:** 12th Gen Intel Core i7-12800H (20 logical cores, 14 physical cores) @ 2.80 GHz (base) / 4.80 GHz (turbo)
-   **CPU Cache:**
    -   L1 Data: 48 KiB (x10 cores)
    -   L1 Instruction: 32 KiB (x10 cores)
    -   L2 Unified: 1280 KiB (x10 cores)
    -   L3 Unified: 24576 KiB (x1 shared)
-   **RAM:** DDR4-3200 (64GB)
-   **GPU:** NVIDIA RTX A2000 8GB GDDR6

**Software Configuration:**

| Platform    | Benchmark Framework     | C++ Compiler | C# Runtime               | OS Version                              |
| :---------- | :---------------------- | :----------- | :----------------------- | :-------------------------------------- |
| **Windows** | Google Benchmark v1.9.4 | MSVC 2022    | .NET 8.0.16, RyuJIT AVX2 | Windows 10                              |
| **Linux**   | Google Benchmark v1.9.4 | Clang 16.0.6 | .NET 8.0.17, RyuJIT AVX2 | LMDE 6 (faye) / Debian 6.1.140-1 x86_64 |

---

## Executive Summary

### Windows Platform Performance

| Operation Category           | C++ vs C# Performance            | Status | Key Findings                        |
| :--------------------------- | :------------------------------- | :----: | :---------------------------------- |
| **Hash Table Operations**    | **0.97-2.15x faster**            |   ✅   | C++ shows performance advantage     |
| **Codebook Access (Direct)** | **7.07x faster** vs C# baseline  |   ✅   | Strong C++ performance advantage    |
| **Codebook Access (API)**    | **1,219x slower** vs C# baseline |   ⚠️   | Expected method call overhead       |
| **String Hashing (Short)**   | **1.02-1.74x mixed**             |   ✅   | Mixed results, algorithm dependent  |
| **String Hashing (Long)**    | **1.02x slower to 1.74x faster** |   ✅   | C++ advantage on longer strings     |
| **GMOD Loading**             | **1.29x faster**                 |   ✅   | Good performance with less memory   |
| **GMOD Lookup**              | **1.21-2.15x faster**            |   ✅   | C++ shows performance advantage     |
| **GMOD Traversal**           | **1.73x slower**                 |   ❌   | **Optimization opportunity**        |
| **Path Parsing**             | **9.38-20.5x slower**            |  ❌❌  | **Significant optimization needed** |
| **Version Path Conversion**  | **162x slower**                  | ❌❌❌ | **Critical performance gap**        |

### Linux Platform Performance

| Operation Category           | C++ vs C# Performance            | Status | Key Findings                                      |
| :--------------------------- | :------------------------------- | :----: | :------------------------------------------------ |
| **Hash Table Operations**    | **1.20-3.37x faster**            |   ✅   | Strong C++ advantage on Linux                     |
| **Codebook Access (Direct)** | **7.73x faster** vs C# baseline  |   ✅   | Excellent Linux performance boost                 |
| **Codebook Access (API)**    | **330x slower** vs C# baseline   |   ⚠️   | Much better than Windows, still expected overhead |
| **String Hashing (Short)**   | **1.03-2.40x mixed**             |   ✅   | Mixed results, some algorithms competitive        |
| **String Hashing (Long)**    | **1.02x slower to 1.81x faster** |   ✅   | Generally better performance than Windows         |
| **GMOD Loading**             | **2.43x faster**                 |   ✅   | Excellent performance advantage                   |
| **GMOD Lookup**              | **1.20-1.33x slower**            |   ❌   | Regression vs Windows, mixed vs C#                |
| **GMOD Traversal**           | **1.97x slower**                 |   ❌   | **Worse than Windows, needs optimization**        |
| **Path Parsing**             | **4.35-9.26x slower**            |  ❌❌  | **Improved but still significant gap**            |
| **Version Path Conversion**  | **117x slower**                  | ❌❌❌ | **Better than Windows but still critical**        |

### Cross-Platform Summary

| Operation Category           | Linux vs Windows (C++) | Status | Platform Recommendation                  |
| :--------------------------- | :--------------------- | :----: | :--------------------------------------- |
| **Hash Table Operations**    | **1.13-11.9x faster**  |   ✅   | **Linux strongly preferred**             |
| **Codebook Access (Direct)** | **1.47-3.99x faster**  |   ✅   | **Linux significantly better**           |
| **Codebook Access (API)**    | **11.9x faster**       |   ✅   | **Linux dramatically better**            |
| **String Hashing (Short)**   | **1.04-2.31x mixed**   |   ⚠️   | **Algorithm-dependent performance**      |
| **String Hashing (Long)**    | **1.12-1.32x faster**  |   ✅   | **Linux slightly better**                |
| **GMOD Loading**             | **1.88x faster**       |   ✅   | **Linux preferred**                      |
| **GMOD Lookup**              | **1.71-2.87x mixed**   |   ⚠️   | **Mixed results, platform-specific**     |
| **GMOD Traversal**           | **1.13x slower**       |   ❌   | **Windows slightly better**              |
| **Path Parsing**             | **2.01-2.21x faster**  |   ✅   | **Linux improvement but both need work** |
| **Version Path Conversion**  | **1.40x faster**       |   ✅   | **Linux better but both critical**       |

---

## Codebooks Lookup

### Cross-Platform Performance Comparison

Performance comparison between different C++ access methods for Vista SDK codebook operations:

| Operation             | Windows C++ | vs Baseline  | Linux C++ | vs Baseline  | Cross-Platform   | Notes                                   |
| :-------------------- | :---------- | :----------: | :-------- | :----------: | :--------------- | :-------------------------------------- |
| **SDK Instance**      | 0.688 ns    |  **1.00x**   | 0.210 ns  |  **1.00x**   | **3.28x faster** | 🚀 Ultra fast: fastest access           |
| **SDK Reference**     | 0.838 ns    |  **1.22x**   | 0.210 ns  |  **1.00x**   | **3.99x faster** | 🚀 Ultra fast                           |
| **Array Lookup**      | 1.62 ns     |  **2.35x**   | 0.645 ns  |  **3.07x**   | **2.51x faster** | ⚡ Fast baseline operation              |
| **Vector Lookup**     | 1.80 ns     |  **2.62x**   | 1.60 ns   |  **7.62x**   | **1.13x faster** | 🔥 Very fast linear search              |
| **Hash Table Lookup** | 2.11 ns     |  **3.07x**   | 1.40 ns   |  **6.67x**   | **1.51x faster** | 🔥 Excellent hash performance           |
| **Tree Lookup**       | 7.13 ns     |  **10.36x**  | 2.82 ns   |  **13.43x**  | **2.53x faster** | ⚠️ Red-black tree overhead              |
| **SDK API Method**    | 826 ns      |  **1,200x**  | 69.4 ns   |   **330x**   | **11.9x faster** | ⚠️ **Expected method call overhead**    |
| **VIS Singleton**     | 125,614 ns  | **182,574x** | 85,536 ns | **407,314x** | **1.47x faster** | ⚠️ **Expected initialization overhead** |

#### STL Container Performance

| C++ Method       | Windows Time | Linux Time | Performance Ratio | Implementation                      | Performance vs Array | Status | Notes                           |
| :--------------- | :----------- | :--------- | :---------------- | :---------------------------------- | :------------------- | :----- | :------------------------------ |
| **Array**        | 1.62 ns      | 0.645 ns   | **2.51x faster**  | `std::array` linear search          | **Baseline**         | ✅     | ⚡ Fastest for 3 elements       |
| **Vector**       | 1.80 ns      | 1.60 ns    | **1.13x faster**  | `std::vector` linear search         | **2.48x slower**     | ✅     | 🔥 Minimal overhead             |
| **UnorderedMap** | 2.11 ns      | 1.40 ns    | **1.51x faster**  | `std::unordered_map::find()`        | **2.17x slower**     | ✅     | 🔥 Excellent hash performance   |
| **Map**          | 7.13 ns      | 2.82 ns    | **2.53x faster**  | `std::map::find()` (red-black tree) | **4.37x slower**     | ❌     | Tree overhead for small dataset |

#### Vista SDK Access Performance

| C++ Method             | Windows Time   | Linux Time    | Performance Ratio | Implementation                     | Performance vs Array | Status | Notes                                    |
| :--------------------- | :------------- | :------------ | :---------------- | :--------------------------------- | :------------------- | :----- | :--------------------------------------- |
| **CodebooksInstance**  | **0.688 ns**   | **0.210 ns**  | **3.28x faster**  | Owned instance + direct access     | **3.07x faster**     | 🚀     | Ultra-fast: fastest method               |
| **CodebooksReference** | **0.838 ns**   | **0.210 ns**  | **3.99x faster**  | Reference wrapper + direct access  | **3.07x faster**     | 🚀     | Ultra-fast with minimal wrapper overhead |
| **CodebooksAPI**       | **826 ns**     | **69.4 ns**   | **11.9x faster**  | `codebook()` method call           | **108x slower**      | ⚠️     | Expected: bounds checking + validation   |
| **CodebooksVISCall**   | **125,614 ns** | **85,536 ns** | **1.47x faster**  | `VIS::instance().codebooks()` call | **132,651x slower**  | ⚠️     | Expected: singleton + initialization     |

#### Detailed C++ Results

##### Windows Platform

| Benchmark                 | Time           | CPU            | Iterations | Performance Category       |
| :------------------------ | :------------- | :------------- | :--------- | :------------------------- |
| **BM_CodebooksInstance**  | **0.688 ns**   | **0.688 ns**   | 20.7B      | 🚀 **Ultra-fast**          |
| **BM_CodebooksReference** | **0.838 ns**   | **0.833 ns**   | 16.7B      | 🚀 **Ultra-fast**          |
| **BM_Array**              | **1.62 ns**    | **1.62 ns**    | 8.4B       | ⚡ **Fast**                |
| **BM_Vector**             | **1.80 ns**    | **1.80 ns**    | 7.6B       | ⚡ **Fast**                |
| **BM_UnorderedMap**       | **2.11 ns**    | **2.11 ns**    | 6.7B       | ⚡ **Fast**                |
| **BM_Map**                | **7.13 ns**    | **7.11 ns**    | 1.9B       | ⚠️ **Slow**                |
| **BM_CodebooksAPI**       | **826 ns**     | **826 ns**     | 16.9M      | ❌ **Expected: slow**      |
| **BM_CodebooksVISCall**   | **125,614 ns** | **125,558 ns** | 112K       | ❌ **Expected: very slow** |

##### Linux Platform

| Benchmark                 | Time          | CPU           | Iterations | Performance Category       |
| :------------------------ | :------------ | :------------ | :--------- | :------------------------- |
| **BM_CodebooksInstance**  | **0.210 ns**  | **0.210 ns**  | 66.8B      | 🚀 **Ultra-fast**          |
| **BM_CodebooksReference** | **0.210 ns**  | **0.210 ns**  | 66.6B      | 🚀 **Ultra-fast**          |
| **BM_Array**              | **0.645 ns**  | **0.644 ns**  | 21.8B      | ⚡ **Fast**                |
| **BM_Vector**             | **1.60 ns**   | **1.60 ns**   | 8.9B       | ⚡ **Fast**                |
| **BM_UnorderedMap**       | **1.40 ns**   | **1.40 ns**   | 10.0B      | ⚡ **Fast**                |
| **BM_Map**                | **2.82 ns**   | **2.82 ns**   | 5.0B       | ⚠️ **Slow**                |
| **BM_CodebooksAPI**       | **69.4 ns**   | **69.4 ns**   | 202M       | ❌ **Expected: slow**      |
| **BM_CodebooksVISCall**   | **85,536 ns** | **85,529 ns** | 168K       | ❌ **Expected: very slow** |

#### Detailed C# Results (Windows)

| Method     |      Mean |     Error |    StdDev |        Ratio | RatioSD | Rank | Allocated | Alloc Ratio |
| ---------- | --------: | --------: | --------: | -----------: | ------: | ---: | --------: | ----------: |
| FrozenDict | 4.9802 ns | 0.0699 ns | 0.0584 ns | 1.02x slower |   0.01x |    3 |         - |          NA |
| Dict       | 4.8993 ns | 0.0239 ns | 0.0212 ns |     baseline |         |    2 |         - |          NA |
| Codebooks  | 0.6937 ns | 0.0219 ns | 0.0195 ns | 7.07x faster |   0.21x |    1 |         - |          NA |

#### Detailed C# Results (Linux)

| Method         | Mean    | Error    | StdDev   | Ratio        | RatioSD | Rank | Allocated | Alloc Ratio |
| :------------- | :------ | :------- | :------- | :----------- | :------ | :--- | :-------- | :---------- |
| **FrozenDict** | 5.04 ns | 0.011 ns | 0.009 ns | 1.12x slower | 0.00x   | 3    | -         | NA          |
| **Dict**       | 4.49 ns | 0.013 ns | 0.011 ns | baseline     |         | 2    | -         | NA          |
| **Codebooks**  | 0.58 ns | 0.021 ns | 0.018 ns | 7.73x faster | 0.22x   | 1    | -         | NA          |

---

## GMOD Load

### Cross-Platform Performance Comparison

| Operation     | Windows C++ | vs Baseline | Linux C++ | vs Baseline | Cross-Platform   | Notes                     |
| :------------ | :---------- | :---------: | :-------- | :---------: | :--------------- | :------------------------ |
| **GMOD Load** | 23.5 ms     |  **1.00x**  | 12.5 ms   |  **1.00x**  | **1.88x faster** | ⚡ Full GMOD construction |

### GMOD Load Performance Comparison

| C++ Method   | Windows Time | Linux Time | Performance Ratio | C# Method | C# Time (Win) | C# Time (Linux) | C++ vs C# (Windows) | C++ vs C# (Linux)   |
| :----------- | :----------- | :--------- | :---------------- | :-------- | :------------ | :-------------- | :------------------ | :------------------ |
| **gmodLoad** | 23.5 ms      | 12.5 ms    | **1.88x faster**  | **Load**  | 30.40 ms      | 29.68 ms        | ✅ **1.29x faster** | ✅ **2.37x faster** |

#### Detailed C++ Results

##### Windows Platform

| Benchmark    | Time    | CPU     | Iterations |
| :----------- | :------ | :------ | :--------- |
| **gmodLoad** | 23.5 ms | 23.5 ms | 490        |

##### Linux Platform

| Benchmark    | Time    | CPU     | Iterations |
| :----------- | :------ | :------ | :--------- |
| **gmodLoad** | 12.5 ms | 12.5 ms | 1,029      |

#### Detailed C# Results (Windows)

| Method   | Mean     | Error    | StdDev   | Gen0   | Gen1   | Gen2  | Allocated |
| :------- | :------- | :------- | :------- | :----- | :----- | :---- | :-------- |
| **Load** | 30.40 ms | 0.607 ms | 1.078 ms | 1562.5 | 1500.0 | 562.5 | 15.41 MB  |

#### Detailed C# Results (Linux)

| Method   |     Mean |    Error |   StdDev | Rank |      Gen0 |      Gen1 |     Gen2 | Allocated |
| -------- | -------: | -------: | -------: | ---: | --------: | --------: | -------: | --------: |
| **Load** | 29.68 ms | 0.200 ms | 0.187 ms |    1 | 1593.7500 | 1562.5000 | 625.0000 |  15.41 MB |

---

## GMOD Lookup

### Cross-Platform Performance Comparison

| Operation             | Windows C++ | vs Baseline | Linux C++ | vs Baseline | Cross-Platform   | Notes                            |
| :-------------------- | :---------- | :---------: | :-------- | :---------: | :--------------- | :------------------------------- |
| **Frozen Dictionary** | 18.4 ns     |  **1.00x**  | 51.2 ns   |  **1.00x**  | **2.78x slower** | ⚡ Baseline lookup method        |
| **Hash Table Lookup** | 17.8 ns     |  **0.97x**  | 51.1 ns   |  **1.00x**  | **2.87x slower** | 🚀 Slightly faster               |
| **GMOD API Lookup**   | 22.2 ns     |  **1.21x**  | 13.0 ns   |  **0.25x**  | **1.71x faster** | ✅ Expected method call overhead |

### GMOD Lookup Performance Comparison

#### Cross-Platform C++ Results

| C++ Method     | Windows Time | Linux Time | Performance Ratio | C# Method (Win) | C# Time (Win) | C# Method (Linux) | C# Time (Linux) | C++ vs C# (Windows) | C++ vs C# (Linux)   |
| :------------- | :----------- | :--------- | :---------------- | :-------------- | :------------ | :---------------- | :-------------- | :------------------ | :------------------ |
| **dict**       | 17.8 ns      | 51.1 ns    | **2.87x slower**  | **Dict**        | 38.34 ns      | **Dict**          | 37.85 ns        | ✅ **2.15x faster** | ❌ **1.35x slower** |
| **frozenDict** | 18.4 ns      | 51.2 ns    | **2.78x slower**  | **FrozenDict**  | 15.21 ns      | **FrozenDict**    | 15.81 ns        | ❌ **1.21x slower** | ❌ **3.24x slower** |
| **gmod**       | 22.2 ns      | 13.0 ns    | **1.71x faster**  | **Gmod**        | 15.62 ns      | **Gmod**          | 17.07 ns        | ❌ **1.42x slower** | ✅ **1.31x faster** |

#### Detailed C++ Results

##### Windows Platform

| Benchmark      | Time    | CPU     | Iterations |
| :------------- | :------ | :------ | :--------- |
| **Dict**       | 17.8 ns | 17.9 ns | 786M       |
| **FrozenDict** | 18.4 ns | 18.4 ns | 766M       |
| **Gmod**       | 22.2 ns | 22.1 ns | 627M       |

##### Linux Platform

| Benchmark      | Time    | CPU     | Iterations |
| :------------- | :------ | :------ | :--------- |
| **Dict**       | 51.1 ns | 51.1 ns | 275M       |
| **FrozenDict** | 51.2 ns | 51.2 ns | 274M       |
| **Gmod**       | 13.0 ns | 13.0 ns | 1.08B      |

#### Detailed C# Results (Windows)

| Method         | Mean     | Error    | StdDev   | Ratio        | RatioSD | Rank | Allocated | Alloc Ratio |
| :------------- | :------- | :------- | :------- | :----------- | :------ | :--- | :-------- | :---------- |
| **Dict**       | 38.34 ns | 0.227 ns | 0.201 ns | baseline     |         | 3    | -         | NA          |
| **FrozenDict** | 15.21 ns | 0.039 ns | 0.035 ns | 2.52x faster | 0.02x   | 1    | -         | NA          |
| **Gmod**       | 15.62 ns | 0.255 ns | 0.226 ns | 2.46x faster | 0.04x   | 2    | -         | NA          |

#### Detailed C# Results (Linux)

| Method         | Mean     | Error    | StdDev   | Ratio        | RatioSD | Rank | Allocated | Alloc Ratio |
| :------------- | :------- | :------- | :------- | :----------- | :------ | :--- | :-------- | :---------- |
| **Dict**       | 37.85 ns | 0.366 ns | 0.343 ns | baseline     |         | 3    | -         | NA          |
| **Gmod**       | 17.07 ns | 0.210 ns | 0.186 ns | 2.22x faster | 0.03x   | 2    | -         | NA          |
| **FrozenDict** | 15.81 ns | 0.348 ns | 0.401 ns | 2.40x faster | 0.06x   | 1    | -         | NA          |

---

## GMOD Path Parsing

### Cross-Platform Performance Comparison

| Operation                          | Windows C++ | Linux C++ | Performance Ratio | Status | Notes                       |
| :--------------------------------- | :---------- | :-------- | :---------------- | :----: | :-------------------------- |
| **TryParse**                       | 31.5 μs     | 14.6 μs   | **2.16x faster**  |   ✅   | Significant Linux advantage |
| **TryParseFullPath**               | 8.67 μs     | 4.31 μs   | **2.01x faster**  |   ✅   | Consistent improvement      |
| **TryParseIndividualized**         | 30.5 μs     | 13.8 μs   | **2.21x faster**  |   ✅   | Strong Linux performance    |
| **TryParseFullPathIndividualized** | 8.72 μs     | 3.99 μs   | **2.18x faster**  |   ✅   | Excellent optimization      |

### GMOD Path Parsing Performance Comparison

#### Cross-Platform C++ Results

| C++ Method                         | Windows Time | Linux Time | Performance Ratio | C# Method (Win)                    | C# Time (Win) | C# Method (Linux)                  | C# Time (Linux) | C++ vs C# (Windows) | C++ vs C# (Linux)   |
| :--------------------------------- | :----------- | :--------- | :---------------- | :--------------------------------- | :------------ | :--------------------------------- | :-------------- | :------------------ | :------------------ |
| **tryParse**                       | 31.5 μs      | 14.6 μs    | **2.16x faster**  | **TryParse**                       | 3.36 μs       | **TryParse**                       | 3.55 μs         | ❌ **9.38x slower** | ❌ **4.11x slower** |
| **tryParseFullPath**               | 8.67 μs      | 4.31 μs    | **2.01x faster**  | **TryParseFullPath**               | 571 ns        | **TryParseFullPath**               | 630 ns          | ❌ **15.2x slower** | ❌ **6.84x slower** |
| **tryParseIndividualized**         | 30.5 μs      | 13.8 μs    | **2.21x faster**  | **TryParseIndividualized**         | 1.49 μs       | **TryParseIndividualized**         | 1.60 μs         | ❌ **20.5x slower** | ❌ **8.63x slower** |
| **tryParseFullPathIndividualized** | 8.72 μs      | 3.99 μs    | **2.18x faster**  | **TryParseFullPathIndividualized** | 694 ns        | **TryParseFullPathIndividualized** | 788 ns          | ❌ **12.6x slower** | ❌ **5.06x slower** |

#### Detailed C++ Results

##### Windows Platform

| Benchmark                          | Time    | CPU     | Iterations |
| :--------------------------------- | :------ | :------ | :--------- |
| **tryParse**                       | 31.5 μs | 31.6 μs | 448K       |
| **tryParseFullPath**               | 8.67 μs | 8.67 μs | 1.61M      |
| **tryParseIndividualized**         | 30.5 μs | 30.5 μs | 459K       |
| **tryParseFullPathIndividualized** | 8.72 μs | 8.71 μs | 1.61M      |

##### Linux Platform

| Benchmark                          | Time    | CPU     | Iterations |
| :--------------------------------- | :------ | :------ | :--------- |
| **tryParse**                       | 14.6 μs | 14.6 μs | 942K       |
| **tryParseFullPath**               | 4.31 μs | 4.31 μs | 3.26M      |
| **tryParseIndividualized**         | 13.8 μs | 13.8 μs | 1.02M      |
| **tryParseFullPathIndividualized** | 3.99 μs | 3.99 μs | 3.53M      |

#### Detailed C# Results (Windows)

| Method                             | Mean       | Error    | StdDev   | Categories    | Gen0   | Allocated |
| :--------------------------------- | :--------- | :------- | :------- | :------------ | :----- | :-------- |
| **TryParse**                       | 3,360.8 ns | 41.66 ns | 38.97 ns | No location   | 0.2213 | 2,792 B   |
| **TryParseFullPath**               | 571.2 ns   | 7.45 ns  | 5.82 ns  | No location   | 0.0601 | 760 B     |
| **TryParseIndividualized**         | 1,492.2 ns | 28.77 ns | 51.14 ns | With location | 0.2251 | 2,832 B   |
| **TryParseFullPathIndividualized** | 694.1 ns   | 4.33 ns  | 3.84 ns  | With location | 0.0935 | 1,176 B   |

#### Detailed C# Results (Linux)

| Method                             | Categories    |       Mean |    Error |   StdDev |   Gen0 | Allocated |
| ---------------------------------- | ------------- | ---------: | -------: | -------: | -----: | --------: |
| **TryParseFullPath**               | No location   |   630.4 ns |  2.53 ns |  2.11 ns | 0.0601 |     760 B |
| **TryParse**                       | No location   | 3,548.7 ns | 14.72 ns | 11.49 ns | 0.2213 |    2792 B |
| **TryParseFullPathIndividualized** | With location |   787.9 ns |  2.30 ns |  1.92 ns | 0.0935 |    1176 B |
| **TryParseIndividualized**         | With location | 1,598.6 ns | 13.64 ns | 12.76 ns | 0.2251 |    2832 B |

---

## GMOD Traversal

### Cross-Platform Performance Comparison

| Operation          | Windows C++ | Linux C++ | Performance Ratio | Status | Notes                 |
| :----------------- | :---------- | :-------- | :---------------- | :----: | :-------------------- |
| **Full Traversal** | 282 ms      | 318 ms    | **1.13x slower**  |   ⚠️   | Linux slightly slower |

### GMOD Traversal Performance Comparison

#### Cross-Platform C++ Results

| C++ Method        | Windows Time | Linux Time | Performance Ratio | C# Method (Win)   | C# Time (Win) | C# Method (Linux) | C# Time (Linux) | C++ vs C# (Windows) | C++ vs C# (Linux)   |
| :---------------- | :----------- | :--------- | :---------------- | :---------------- | :------------ | :---------------- | :-------------- | :------------------ | :------------------ |
| **fullTraversal** | 282 ms       | 318 ms     | **1.13x slower**  | **FullTraversal** | 162.9 ms      | **FullTraversal** | 161.3 ms        | ❌ **1.73x slower** | ❌ **1.97x slower** |

#### Detailed C++ Results

##### Windows Platform

| Benchmark         | Time   | CPU    | Iterations |
| :---------------- | :----- | :----- | :--------- |
| **FullTraversal** | 282 ms | 266 ms | 50-51      |

##### Linux Platform

| Benchmark         | Time   | CPU    | Iterations |
| :---------------- | :----- | :----- | :--------- |
| **FullTraversal** | 318 ms | 318 ms | 44         |

#### Detailed C# Results (Windows)

| Method            | Mean     | Error   | StdDev  | Allocated |
| :---------------- | :------- | :------ | :------ | :-------- |
| **FullTraversal** | 162.9 ms | 0.97 ms | 0.91 ms | 5.3 KB    |

#### Detailed C# Results (Linux)

| Method            |     Mean |   Error |  StdDev | Allocated |
| ----------------- | -------: | ------: | ------: | --------: |
| **FullTraversal** | 161.3 ms | 1.29 ms | 1.20 ms |    5.3 KB |

---

## GMOD Versioning Path Conversion

### Cross-Platform Performance Comparison

| Operation        | Windows C++ | Linux C++ | Performance Ratio | Status | Notes                                |
| :--------------- | :---------- | :-------- | :---------------- | :----: | :----------------------------------- |
| **Convert Path** | 241 μs      | 172 μs    | **1.40x faster**  |   ⚠️   | Linux improvement but still critical |

### GMOD Versioning Path Conversion Performance Comparison

#### Cross-Platform C++ Results

| C++ Method      | Windows Time | Linux Time | Performance Ratio | C# Method (Win) | C# Time (Win) | C# Method (Linux) | C# Time (Linux) | C++ vs C# (Windows) | C++ vs C# (Linux)  |
| :-------------- | :----------- | :--------- | :---------------- | :-------------- | :------------ | :---------------- | :-------------- | :------------------ | :----------------- |
| **convertPath** | 241 μs       | 172 μs     | **1.40x faster**  | **ConvertPath** | 1.489 μs      | **ConvertPath**   | 1.464 μs        | ❌ **162x slower**  | ❌ **117x slower** |

#### Detailed C++ Results

##### Windows Platform

| Benchmark       | Time   | CPU    | Iterations |
| :-------------- | :----- | :----- | :--------- |
| **convertPath** | 241 μs | 219 μs | 64,000     |

##### Linux Platform

| Benchmark       | Time   | CPU    | Iterations |
| :-------------- | :----- | :----- | :--------- |
| **convertPath** | 172 μs | 172 μs | 80,599     |

#### Detailed C# Results (Windows)

| Method          | Mean     | Error     | StdDev    | Gen0   | Allocated |
| :-------------- | :------- | :-------- | :-------- | :----- | :-------- |
| **ConvertPath** | 1.489 μs | 0.0108 μs | 0.0090 μs | 0.2575 | 3.17 KB   |

#### Detailed C# Results (Linux)

| Method          | Mean     | Error     | StdDev    | Gen0   | Allocated |
| :-------------- | :------- | :-------- | :-------- | :----- | :-------- |
| **ConvertPath** | 1.464 μs | 0.0115 μs | 0.0107 μs | 0.2575 | 3.17 KB   |

---

## Short String Hashing

### Cross-Platform Performance Comparison

| Operation         | Windows C++ | Linux C++ | Performance Ratio | Status | Notes                      |
| :---------------- | :---------- | :-------- | :---------------- | :----: | :------------------------- |
| **bcl (400)**     | 1.18 ns     | 2.72 ns   | **2.31x slower**  |   ❌   | Linux significantly slower |
| **bcl (H346)**    | 2.61 ns     | 2.52 ns   | **1.04x faster**  |   ✅   | Similar performance        |
| **bclOrd (400)**  | 1.77 ns     | 0.931 ns  | **1.90x faster**  |   ✅   | Linux advantage            |
| **bclOrd (H346)** | 2.92 ns     | 2.02 ns   | **1.45x faster**  |   ✅   | Strong Linux performance   |
| **Larson (400)**  | 1.54 ns     | 1.60 ns   | **1.04x slower**  |   ✅   | Similar performance        |
| **Larson (H346)** | 4.85 ns     | 3.43 ns   | **1.41x faster**  |   ✅   | Linux improvement          |
| **crc32 (400)**   | 1.57 ns     | 1.38 ns   | **1.14x faster**  |   ✅   | Slight Linux advantage     |
| **crc32 (H346)**  | 4.51 ns     | 4.22 ns   | **1.07x faster**  |   ✅   | Marginal improvement       |
| **fnv (400)**     | 1.48 ns     | 1.17 ns   | **1.26x faster**  |   ✅   | Linux faster               |
| **fnv (H346)**    | 3.39 ns     | 2.61 ns   | **1.30x faster**  |   ✅   | Good Linux performance     |

### Short String Hashing Performance Comparison

#### Cross-Platform C++ Results

| Algorithm         | Windows Time | Linux Time | Performance Ratio | C# Baseline (Win) | C# Baseline (Linux) | C++ vs C# (Windows) | C++ vs C# (Linux)   |
| :---------------- | :----------- | :--------- | :---------------- | :---------------- | :------------------ | :------------------ | :------------------ |
| **bcl (400)**     | 1.18 ns      | 2.72 ns    | **2.31x slower**  | 1.135 ns          | 1.214 ns            | ❌ **1.04x slower** | ❌ **2.24x slower** |
| **bcl (H346)**    | 2.61 ns      | 2.52 ns    | **1.04x faster**  | 4.551 ns          | 4.560 ns            | ✅ **1.74x faster** | ✅ **1.81x faster** |
| **bclOrd (400)**  | 1.77 ns      | 0.931 ns   | **1.90x faster**  | 1.514 ns          | 1.368 ns            | ❌ **1.17x slower** | ✅ **1.47x faster** |
| **bclOrd (H346)** | 2.92 ns      | 2.02 ns    | **1.45x faster**  | 3.127 ns          | 2.340 ns            | ✅ **1.07x faster** | ✅ **1.16x faster** |
| **Larson (400)**  | 1.54 ns      | 1.60 ns    | **1.04x slower**  | 1.219 ns          | 1.244 ns            | ❌ **1.26x slower** | ❌ **1.29x slower** |
| **Larson (H346)** | 4.85 ns      | 3.43 ns    | **1.41x faster**  | 3.349 ns          | 3.494 ns            | ❌ **1.45x slower** | ✅ **1.02x faster** |
| **crc32 (400)**   | 1.57 ns      | 1.38 ns    | **1.14x faster**  | 1.215 ns          | 1.235 ns            | ❌ **1.29x slower** | ❌ **1.12x slower** |
| **crc32 (H346)**  | 4.51 ns      | 4.22 ns    | **1.07x faster**  | 3.259 ns          | 3.441 ns            | ❌ **1.38x slower** | ❌ **1.23x slower** |
| **fnv (400)**     | 1.48 ns      | 1.17 ns    | **1.26x faster**  | 1.205 ns          | 1.291 ns            | ❌ **1.23x slower** | ✅ **1.10x faster** |
| **fnv (H346)**    | 3.39 ns      | 2.61 ns    | **1.30x faster**  | 3.337 ns          | 3.741 ns            | ❌ **1.02x slower** | ✅ **1.43x faster** |

#### Detailed C++ Results

##### Windows Platform

| Benchmark               | Input      | Time    | CPU     | Iterations |
| :---------------------- | :--------- | :------ | :------ | :--------- |
| **bcl_400**             | 400        | 1.18 ns | 1.18 ns | 12.1B      |
| **bcl_H346_11112**      | H346.11112 | 2.61 ns | 2.61 ns | 5.4B       |
| **bclOrd_400**          | 400        | 1.77 ns | 1.77 ns | 8.0B       |
| **bclOrd_H346_11112**   | H346.11112 | 2.92 ns | 2.92 ns | 4.7B       |
| **Larson_400**          | 400        | 1.54 ns | 1.54 ns | 9.1B       |
| **Larson_H346_11112**   | H346.11112 | 4.85 ns | 4.83 ns | 2.9B       |
| **crc32Intrinsic_400**  | 400        | 1.57 ns | 1.56 ns | 8.9B       |
| **crc32Intrinsic_H346** | H346.11112 | 4.51 ns | 4.51 ns | 3.1B       |
| **fnv_400**             | 400        | 1.48 ns | 1.48 ns | 9.3B       |
| **fnv_H346_11112**      | H346.11112 | 3.39 ns | 3.39 ns | 4.1B       |

##### Linux Platform

| Benchmark               | Input      | Time     | CPU      | Iterations |
| :---------------------- | :--------- | :------- | :------- | :--------- |
| **bcl_400**             | 400        | 2.72 ns  | 2.72 ns  | 5.42B      |
| **bcl_H346_11112**      | H346.11112 | 2.52 ns  | 2.52 ns  | 5.63B      |
| **bclOrd_400**          | 400        | 0.931 ns | 0.931 ns | 15.09B     |
| **bclOrd_H346_11112**   | H346.11112 | 2.02 ns  | 2.02 ns  | 6.96B      |
| **Larson_400**          | 400        | 1.60 ns  | 1.60 ns  | 8.73B      |
| **Larson_H346_11112**   | H346.11112 | 3.43 ns  | 3.43 ns  | 4.08B      |
| **crc32Intrinsic_400**  | 400        | 1.38 ns  | 1.38 ns  | 10.17B     |
| **crc32Intrinsic_H346** | H346.11112 | 4.22 ns  | 4.22 ns  | 3.34B      |
| **fnv_400**             | 400        | 1.17 ns  | 1.17 ns  | 11.84B     |
| **fnv_H346_11112**      | H346.11112 | 2.61 ns  | 2.61 ns  | 5.39B      |

#### Detailed C# Results (Windows)

| Method                  | Input      | Mean     | Error     | StdDev    | Ratio        | RatioSD | Rank | Allocated | Alloc Ratio |
| :---------------------- | :--------- | :------- | :-------- | :-------- | :----------- | :------ | :--- | :-------- | :---------- |
| **Bcl_400**             | 400        | 1.135 ns | 0.0108 ns | 0.0101 ns | baseline     |         | 1    | -         | NA          |
| **BclOrd_400**          | 400        | 1.514 ns | 0.0064 ns | 0.0060 ns | 1.33x slower | 0.01x   | 2    | -         | NA          |
| **Larson_400**          | 400        | 1.219 ns | 0.0036 ns | 0.0034 ns | 1.07x slower | 0.01x   | 3    | -         | NA          |
| **Crc32Intrinsic_400**  | 400        | 1.215 ns | 0.0040 ns | 0.0037 ns | 1.07x slower | 0.01x   | 4    | -         | NA          |
| **Fnv_400**             | 400        | 1.205 ns | 0.0033 ns | 0.0031 ns | 1.06x slower | 0.01x   | 5    | -         | NA          |
| **Bcl_H346**            | H346.11112 | 4.551 ns | 0.0143 ns | 0.0134 ns | baseline     |         | 1    | -         | NA          |
| **BclOrd_H346**         | H346.11112 | 3.127 ns | 0.0074 ns | 0.0070 ns | 1.46x faster | 0.01x   | 2    | -         | NA          |
| **Larson_H346**         | H346.11112 | 3.349 ns | 0.0098 ns | 0.0092 ns | 1.36x faster | 0.01x   | 3    | -         | NA          |
| **Crc32Intrinsic_H346** | H346.11112 | 3.259 ns | 0.0093 ns | 0.0087 ns | 1.40x faster | 0.01x   | 4    | -         | NA          |
| **Fnv_H346**            | H346.11112 | 3.337 ns | 0.0116 ns | 0.0108 ns | 1.36x faster | 0.01x   | 5    | -         | NA          |

#### Detailed C# Results (Linux)

| Method             | Input      |     Mean |     Error |    StdDev |        Ratio | RatioSD | Rank | Allocated | Alloc Ratio |
| :----------------- | :--------- | -------: | --------: | --------: | -----------: | ------: | ---: | --------: | ----------: |
| **BclOrd**         | 400        | 1.368 ns | 0.0264 ns | 0.0234 ns | 1.13x slower |   0.03x |    4 |         - |          NA |
| **Fnv**            | 400        | 1.291 ns | 0.0092 ns | 0.0077 ns | 1.06x slower |   0.01x |    3 |         - |          NA |
| **Larsson**        | 400        | 1.244 ns | 0.0097 ns | 0.0091 ns | 1.03x slower |   0.01x |    2 |         - |          NA |
| **Crc32Intrinsic** | 400        | 1.235 ns | 0.0053 ns | 0.0044 ns | 1.02x slower |   0.01x |    2 |         - |          NA |
| **Bcl**            | 400        | 1.214 ns | 0.0146 ns | 0.0137 ns |     baseline |         |    1 |         - |          NA |
|                    |            |          |           |           |              |         |      |           |             |
| **Bcl**            | H346.11112 | 4.560 ns | 0.0101 ns | 0.0079 ns |     baseline |         |    5 |         - |          NA |
| **Fnv**            | H346.11112 | 3.741 ns | 0.1091 ns | 0.1633 ns | 1.26x faster |   0.03x |    4 |         - |          NA |
| **Larsson**        | H346.11112 | 3.494 ns | 0.0172 ns | 0.0161 ns | 1.31x faster |   0.01x |    3 |         - |          NA |
| **Crc32Intrinsic** | H346.11112 | 3.441 ns | 0.0109 ns | 0.0091 ns | 1.32x faster |   0.00x |    2 |         - |          NA |
| **BclOrd**         | H346.11112 | 2.340 ns | 0.0407 ns | 0.0340 ns | 1.95x faster |   0.03x |    1 |         - |          NA |

---

_Last updated: July 19, 2025_
