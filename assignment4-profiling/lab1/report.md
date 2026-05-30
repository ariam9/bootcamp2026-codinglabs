# Intro Profiling Lab Report

## 1. Optimizations Made

- row-major access - in compute_congestion_pressure, changed column-major access to more cache-friendly row-major access to reduce L1 cache misses
- fixed memory leak in shortest_path_bfs - replaced distances, visited and frontier from manually heap allocated(via new[]) and delete-forgotten arrays to RAII compliant objects(std::vector first, then std::array in a later optimisation)
- reduced allocations in shortest_path_bfs - distances, visited and frontier were pulled out of the function body to be global variables instead, to reduce heap allocations happening every time the function is called(1200 times)
- switched if/else in next_pressure_value to ternary - enables compiler to use a cmov(conditional move instruction) instead of branching, allowing it to inline the function since it becomes less complex
- replaced /8 with >>3 in two places and /2 with >> 1 in one place - the variables' values were unsigned ints(>= 0) anyway, so this is more efficient and might promote the compiler to auto vectorise
- used __restrict__ across a call boundary in compute_congestion_pressure - in order to convince the compiler that the arrays don't overlap, so that it autovectorises(success!)

Stuck to -O2 for measuring the effects of my optimisations, instead of the obviously superior optimisations made by the compiler on -O3.

## 2. Methodology Walkthrough

Include before/after evidence from:

- `time`
- `perf stat`
- FlameGraph
- Callgrind/KCachegrind
- Valgrind leak summary

## 3. Correctness Evidence

Include:

- `make test`
- Final normal run output
- Checksum comparison before and after optimization

## 4. Conceptual Questions

Answer Q1.1 through Q6.1 from the README.
