# Benchmarks

---

## Test Environment

### Hardware Configuration

| Component                | Specification                                                 |
| ------------------------ | ------------------------------------------------------------- |
| **Computer Model**       | Lenovo ThinkPad P15v Gen 3                                    |
| **CPU**                  | 12th Gen Intel Core i7-12800H (20 logical, 14 physical cores) |
| **Base Clock**           | 2.80 GHz                                                      |
| **Turbo Clock**          | 4.80 GHz                                                      |
| **L1 Data Cache**        | 48 KiB (×6 P-cores) + 32 KiB (×8 E-cores)                     |
| **L1 Instruction Cache** | 32 KiB (×6 P-cores) + 64 KiB (×2 E-core clusters)             |
| **L2 Unified Cache**     | 1.25 MiB (×6 P-cores) + 2 MiB (×2 E-core clusters)            |
| **L3 Unified Cache**     | 24 MiB (×1 shared)                                            |
| **RAM**                  | DDR4-3200 (32GB)                                              |
| **GPU**                  | NVIDIA RTX A2000 4GB GDDR6                                    |

### Software Configuration

| Platform    | OS         | Benchmark Framework     | C++ Compiler              | vista-sdk-cpp Version |
| ----------- | ---------- | ----------------------- | ------------------------- | --------------------- |
| **Linux**   | LMDE 7     | Google Benchmark v1.9.4 | GCC 14.2.0-x64            | v0.0.0                |
| **Linux**   | LMDE 7     | Google Benchmark v1.9.4 | Clang 19.1.7-x64          | v0.0.0                |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | MinGW GCC 14.2.0-x64      | v0.0.0                |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | Clang-GNU-CLI 19.1.5-x64  | v0.0.0                |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | Clang-MSVC-CLI 19.1.5-x64 | v0.0.0                |
| **Windows** | Windows 10 | Google Benchmark v1.9.4 | MSVC 19.44.35217.0-x64    | v0.0.0                |

---

# Performance Results

## Codebooks Benchmarks

### Codebooks lookup

| Operation           | Linux GCC   | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ------------------- | ----------- | ----------- | ----------------: | --------------------: | ---------------------: | -----------: |
| **BM_UnorderedMap** | 6.40ns      | 1.78ns      |            7.17ns |                2.02ns |             **1.35ns** |       2.01ns |
| **BM_Map**          | **3.66ns**  | 5.62ns      |            4.33ns |                11.6ns |                 8.99ns |       9.51ns |
| **BM_Codebooks**    | **0.826ns** | 0.890ns     |           0.948ns |                1.17ns |                0.855ns |       1.02ns |

## VIS Benchmarks

### Caching

| Operation                         | Linux GCC  | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| --------------------------------- | ---------- | ----------- | ----------------: | --------------------: | ---------------------: | -----------: |
| **BM_Codebooks_ColdStart**        | 34.6ns     | **32.9ns**  |            87.9ns |                76.7ns |                 73.2ns |       69.8ns |
| **BM_Codebooks_CachedAccess**     | 34.3ns     | **33.2ns**  |            83.7ns |                87.9ns |                 73.9ns |       69.7ns |
| **BM_Codebooks_MultipleVersions** | 37.3ns     | **35.4ns**  |            88.9ns |                94.9ns |                 75.0ns |       69.8ns |
| **BM_Codebooks_RealisticUsage**   | 104ns      | **98.6ns**  |             262ns |                 226ns |                  215ns |        218ns |
| **BM_Codebooks_LookupAfterCache** | **18.5ns** | 22.4ns      |            19.5ns |                21.3ns |                 19.7ns |       21.5ns |

## StringBuilder Benchmarks

### Short Strings

| Operation                             | Linux GCC  | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ------------------------------------- | ---------- | ----------- | ----------------: | --------------------: | ---------------------: | -----------: |
| **BM_StdString_ShortAppends**         | **10.8ns** | 17.5ns      |            35.3ns |                44.4ns |                 35.7ns |       44.8ns |
| **BM_NFX_StringBuilder_ShortAppends** | 47.3ns     | **41.8ns**  |             108ns |                86.7ns |                 72.3ns |       68.3ns |
| **BM_DNV_StringBuilder_ShortAppends** | 58.2ns     | **54.6ns**  |             179ns |                98.5ns |                 84.3ns |       88.9ns |

### Stream operator

| Operation                          | Linux GCC | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ---------------------------------- | --------- | ----------- | ----------------: | --------------------: | ---------------------: | -----------: |
| **BM_StdStringStream_StreamOps**   | **204ns** | 217ns       |             281ns |                 675ns |                  584ns |        670ns |
| **BM_NFX_StringBuilder_StreamOps** | 103ns     | **91.2ns**  |             181ns |                 156ns |                  140ns |        122ns |
| **BM_DNV_StringBuilder_StreamOps** | 120ns     | **108ns**   |             254ns |                 187ns |                  160ns |        149ns |

### MetadataTag building

| Operation                             | Linux GCC | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ------------------------------------- | --------- | ----------- | ----------------: | --------------------: | ---------------------: | -----------: |
| **BM_StdString_MetadataPath**         | 27.9ns    | **13.8ns**  |            51.1ns |                54.4ns |                 45.1ns |       78.6ns |
| **BM_NFX_StringBuilder_MetadataPath** | 191ns     | **166ns**   |             316ns |                 267ns |                  250ns |        210ns |
| **BM_DNV_StringBuilder_MetadataPath** | 216ns     | **192ns**   |             388ns |                 296ns |                  290ns |        253ns |

### Large Strings

| Operation                                 | Linux GCC  | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ----------------------------------------- | ---------- | ----------- | ----------------: | --------------------: | ---------------------: | -----------: |
| **BM_StdString_LargeString/10**           | **48.3ns** | 50.3ns      |            90.1ns |                 133ns |                  133ns |          124 |
| **BM_StdString_LargeString/100**          | 886ns      | **714ns**   |             925ns |                1202ns |                 1130ns |         1229 |
| **BM_StdString_LargeString/1000**         | 8612ns     | **7070ns**  |           10032ns |               13041ns |                11621ns |        14885 |
| **BM_NFX_StringBuilder_LargeString/10**   | 232ns      | **197ns**   |             340ns |                 336ns |                  305ns |          340 |
| **BM_NFX_StringBuilder_LargeString/100**  | 2170ns     | **1711ns**  |            2845ns |                3357ns |                 2571ns |         2775 |
| **BM_NFX_StringBuilder_LargeString/1000** | 21666ns    | **17329ns** |           31254ns |               29793ns |                25447ns |        27697 |
| **BM_DNV_StringBuilder_LargeString/10**   | 268ns      | **246ns**   |             443ns |                 403ns |                  379ns |          373 |
| **BM_DNV_StringBuilder_LargeString/100**  | 2392ns     | **2111ns**  |            3445ns |                3317ns |                 2746ns |         3028 |
| **BM_DNV_StringBuilder_LargeString/1000** | 24216ns    | **20315ns** |           33755ns |               33633ns |                28458ns |        30383 |

### Multiple toString()

| Operation                                 | Linux GCC | Linux Clang | Windows MinGW GCC | Windows Clang-GNU-CLI | Windows Clang-MSVC-CLI | Windows MSVC |
| ----------------------------------------- | --------- | ----------- | ----------------: | --------------------: | ---------------------: | -----------: |
| **BM_NFX_StringBuilder_MultipleToString** | 118ns     | **107ns**   |             205ns |                 184ns |                  173ns |        157ns |
| **BM_DNV_StringBuilder_MultipleToString** | 137ns     | **124ns**   |             290ns |                 213ns |                  193ns |        184ns |

---

_Updated on December 12, 2025_
