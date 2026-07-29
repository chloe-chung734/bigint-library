# BigInt Arithmetic Library

An arbitrary-precision integer library implemented in C++, supporting operations on numbers far beyond the range of native integer types.

## Features

- Dynamic digit storage — no fixed-size limit on operand magnitude
- Operator overloading for natural syntax (`+`, `-`, `*`, `/`)
- Supported operations: addition, subtraction, multiplication, division, factorial, Fibonacci
- Validated correctness on operands exceeding 1,000 digits

## Files

- `bigint.cpp` — core library implementation (BigInt class, operator overloads, arithmetic logic)
- `results.csv` — benchmark output from a completed performance test (see Performance section below)

## Build & Run

```bash
g++ -std=c++17 -o bigint bigint.cpp
./bigint
```

## Performance

Benchmarked using an instructor-provided test harness (CECS 325) against a naive base-10 reference implementation, across operand sizes from 32 to 5,000+ digits. Both implementations use the same schoolbook (O(n²)) algorithm for multiplication; correctness was verified to match exactly at every tested size.

| Operation | Digits | Median Time (ns) | Ops/sec   | Relative Speed |
|-----------|--------|-------------------|-----------|-----------------|
| Add       | 32     | 613                | 2,014,208 | 0.26x           |
| Add       | 64     | 394                | 2,551,429 | 0.65x           |
| Add       | 128    | 628                | 1,597,708 | 0.79x           |
| Add       | 256    | 1,181              | 861,600   | 0.86x           |
| Multiply  | 32     | 6,597              | 188,364   | 0.75x           |
| Multiply  | 64     | 20,991             | 49,223    | 0.87x           |
| Multiply  | 128    | 81,681             | 12,263    | 0.89x           |

*Full data, including up to 5,000-digit operands, available in `results.csv`.*

The performance gap narrows as operand size increases, driven by two implementation-level factors: this library stores digits as ASCII characters (`char '0'`–`'9'`), requiring a conversion on every digit access, whereas the reference stores raw digit values; and this library's addition grows its result vector via repeated `push_back` calls rather than pre-allocating exact size upfront. Both are constant-factor overheads rather than algorithmic differences — correctness and asymptotic behavior are identical between implementations.

## Background

Built as a project for CECS 325 (CSU Long Beach) to explore low-level memory management, dynamic storage, and operator overloading in C++ without relying on built-in arbitrary-precision types. Performance was benchmarked using an instructor-provided test harness against a reference implementation.