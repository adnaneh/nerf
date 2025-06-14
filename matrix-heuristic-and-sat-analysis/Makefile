# OPTIMIZATION 8: Compiler flags & PGO
CC = clang

# Architecture-specific SIMD flags
UNAME_M := $(shell uname -m)
ifeq ($(UNAME_M),x86_64)
    SIMD_FLAGS = -mavx2
else ifeq ($(UNAME_M),arm64)
    SIMD_FLAGS = -mcpu=native
else
    SIMD_FLAGS =
endif

CFLAGS_BASE = -O3 -march=native -mtune=native -fno-exceptions -fno-stack-protector -fomit-frame-pointer $(SIMD_FLAGS)
CFLAGS_LTO = -flto
CFLAGS_FAST = $(CFLAGS_BASE) $(CFLAGS_LTO) -DNDEBUG -ffast-math -funroll-loops
CFLAGS_PROFILE = $(CFLAGS_BASE) $(CFLAGS_LTO) -DPROFILE -g
CFLAGS_PGO_GEN = $(CFLAGS_FAST) -fprofile-instr-generate
CFLAGS_PGO_USE = $(CFLAGS_FAST) -fprofile-instr-use=default.profdata

TARGET = hireme2_safe_optimized
SOURCE = hireme2_safe_optimized.c
TARGET_NO_HEURISTIC = hireme2_safe_optimized_no_heuristic
SOURCE_NO_HEURISTIC = hireme2_safe_optimized_no_heuristic.c

# Default fast build (no profiling)
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS_FAST) -o $@ $< -lm

# Fast build with profiling enabled
profile: $(SOURCE)
	$(CC) $(CFLAGS_PROFILE) -o $(TARGET)_profile $< -lm

# Build without heuristic with profiling
profile_no_heuristic: $(SOURCE_NO_HEURISTIC)
	$(CC) $(CFLAGS_PROFILE) -o $(TARGET_NO_HEURISTIC)_profile $< -lm

# PGO build: generate profile data
pgo-gen: $(SOURCE)
	$(CC) $(CFLAGS_PGO_GEN) -o $(TARGET)_pgo_gen $< -lm

# PGO build: use profile data
pgo-use: $(SOURCE)
	$(CC) $(CFLAGS_PGO_USE) -o $(TARGET)_pgo $< -lm

# Complete PGO workflow
pgo: pgo-gen
	@echo "Running PGO training..."
	LLVM_PROFILE_FILE="default.profraw" ./$(TARGET)_pgo_gen > /dev/null 2>&1 || true
	@echo "Converting profile data..."
	xcrun llvm-profdata merge -output=default.profdata default.profraw
	@echo "Building optimized binary with PGO..."
	$(MAKE) pgo-use
	@echo "PGO-optimized binary: $(TARGET)_pgo"

# SAT solver build
sat_solver: hireme_sat_solver.cpp
	g++ -std=c++17 -I/opt/homebrew/include -I/opt/homebrew/Cellar/z3/4.15.1/include -L/opt/homebrew/lib -lz3 hireme_sat_solver.cpp -o hireme_sat_solver

# Minimal test solver
minimal_test: minimal_test.cpp
	g++ -std=c++17 -I/opt/homebrew/include -I/opt/homebrew/Cellar/z3/4.15.1/include -L/opt/homebrew/lib -lz3 minimal_test.cpp -o minimal_test

# Minimal correct solver using CryptoMiniSat
minimal_correct: minimal_correct.cpp
	g++ -std=c++17 -O3 -march=native -flto -I/opt/homebrew/include -L/opt/homebrew/lib -lcryptominisat5 minimal_correct.cpp -o minimal_correct

# Minimal optimized solver using CryptoMiniSat
minimal_optimized: minimal_optimized.cpp
	g++ -std=c++17 -O3 -march=native -flto -I/opt/homebrew/include -L/opt/homebrew/lib -lcryptominisat5 minimal_optimized.cpp -o minimal_optimized

# Minimal smart solver using CryptoMiniSat
minimal_smart: minimal_smart.cpp
	g++ -std=c++17 -O3 -march=native -flto -I/opt/homebrew/include -L/opt/homebrew/lib -lcryptominisat5 minimal_smart.cpp -o minimal_smart

# Test variable count
test_var_count: test_var_count.cpp
	g++ -std=c++17 -O3 -march=native -flto -I/opt/homebrew/include -L/opt/homebrew/lib -lcryptominisat5 test_var_count.cpp -o test_var_count

# 1-round test
minimal_1round: ../minimal_1round.cpp
	g++ -std=c++17 -O3 -march=native -flto -I/opt/homebrew/include -L/opt/homebrew/lib -lcryptominisat5 ../minimal_1round.cpp -o minimal_1round

# 1-round test with pairwise encoding
minimal_1round_pairwise: ../minimal_1round_pairwise.cpp
	g++ -std=c++17 -O3 -march=native -flto -I/opt/homebrew/include -L/opt/homebrew/lib -lcryptominisat5 ../minimal_1round_pairwise.cpp -o minimal_1round_pairwise

# Clean up
clean:
	rm -f $(TARGET) $(TARGET)_profile $(TARGET)_pgo_gen $(TARGET)_pgo
	rm -f *.gcda *.gcno *.profraw *.profdata hireme_sat_solver

# Test performance comparison
BENCH_ITERATIONS ?= 1000
benchmark: $(TARGET) profile
	@echo "=== Performance Benchmark ==="
	@echo "Running $(BENCH_ITERATIONS) iterations..."
	@echo ""
	@printf "Fast build (no profiling): "
	@python3 -c "import subprocess, time; n=$(BENCH_ITERATIONS); start = time.time(); [subprocess.run(['./$(TARGET)'], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL) for _ in range(n)]; elapsed = time.time() - start; total_ms = elapsed * 1000; avg_ms = total_ms / n; print(f'{total_ms:.2f} ms total, {avg_ms:.2f} ms per iteration')"
	@echo ""
	@echo "Profile build ($(BENCH_ITERATIONS) iterations):"
	@python3 benchmark_profile.py $(BENCH_ITERATIONS) ./$(TARGET)_profile

# Compare with and without heuristic
compare: profile profile_no_heuristic
	@echo "=== Heuristic Comparison Benchmark ==="
	@echo "Running $(BENCH_ITERATIONS) iterations for each version..."
	@echo ""
	@echo "WITHOUT heuristic optimization:"
	@python3 benchmark_profile.py $(BENCH_ITERATIONS) ./$(TARGET_NO_HEURISTIC)_profile
	@echo ""
	@echo "WITH heuristic optimization:"
	@python3 benchmark_profile.py $(BENCH_ITERATIONS) ./$(TARGET)_profile
	@echo ""
	@echo "=== Summary ==="
	@python3 compare_heuristic.py $(BENCH_ITERATIONS)

.PHONY: profile profile_no_heuristic pgo-gen pgo-use pgo clean benchmark compare sat_solver