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

# Default fast build (no profiling)
$(TARGET): $(SOURCE)
	$(CC) $(CFLAGS_FAST) -o $@ $< -lm

# Fast build with profiling enabled
profile: $(SOURCE)
	$(CC) $(CFLAGS_PROFILE) -o $(TARGET)_profile $< -lm

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

# Clean up
clean:
	rm -f $(TARGET) $(TARGET)_profile $(TARGET)_pgo_gen $(TARGET)_pgo
	rm -f *.gcda *.gcno *.profraw *.profdata

# Test performance comparison
benchmark: $(TARGET) profile pgo
	@echo "=== Performance Benchmark ==="
	@echo "Fast build (no profiling):"
	@time ./$(TARGET) > /dev/null
	@echo ""
	@echo "PGO-optimized build:"
	@time ./$(TARGET)_pgo > /dev/null

.PHONY: profile pgo-gen pgo-use pgo clean benchmark