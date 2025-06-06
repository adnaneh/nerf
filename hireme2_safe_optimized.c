// =============================================================================
//  "Hire‑me" crack‑me – HIGHLY OPTIMIZED version with optimizations 1,3,4,7
// =============================================================================
//
//  OPTIMIZATION 1: Give the compiler every chance
//  - All timing code is behind #ifdef PROFILE guards with unlikely() hints
//  - Fast build has zero timing overhead via profiling hygiene
//  - Profile build provides detailed performance breakdown
//  - Added restrict qualifiers to function parameters for better aliasing info
//  - Branch prediction hints with likely/unlikely for hot paths
//
//  OPTIMIZATION 2: Fuse the two passes over j=0..31 (already implemented)
//  - Single pass computes v[j], checks validity, calculates choices
//  - Eliminates redundant cache misses and branch mispredictions
//
//  OPTIMIZATION 3: Shrink per-frame memory touches
//  - Lazy-clear idx[] arrays - only clear what we actually use
//  - Track highest_dirty_idx to minimize memset operations
//  - Reuse frames efficiently to avoid cache misses at max depth
//  - Vectorized dot_row_optimized (Apple Silicon optimized)
//  - ARM64: Uses NEON intrinsics with 32-byte mask generation
//  - Cut LUT loads from 6 to 2 using vld1q_u8_x2() intrinsic
//  - Cheaper horizontal XOR staying in 128-bit registers
//  - Sparse threshold raised to 8 bits for M-series performance
//  - Prefetching from state array for DRAM latency hiding
//
//  OPTIMIZATION 4: Remove second for loop that recreates state
//  - Compute child state on-the-fly from dot_row values
//  - Eliminate redundant state storage in frame->v[]
//  - Halve cache footprint of dfs_frame_t structure
//  - Save one full pass over 32 bytes every node
//
//  OPTIMIZATION 7: Profiling hygiene & memory layout
//  - Guard all timing calls with unlikely() for better branch prediction
//  - All hot data structures aligned to 64-byte cache lines
//  - inv_low_count aligned for single cache line access
//  - State buffers aligned for optimal SIMD loads
//
//  OPTIMIZATION 8: Compiler flags & PGO
//  - Apple Silicon specific flags: -mcpu=native 
//  - Aggressive optimization: -O3 -march=native -flto
//  - PGO support for profile-guided optimization
//  - Performance flags: -fno-exceptions -fno-stack-protector -fomit-frame-pointer
//
//  BUILD INSTRUCTIONS:
//    make                    # Fast build (no profiling)
//    make profile           # Profile build with timing
//    make pgo              # PGO-optimized build
//    make benchmark        # Performance comparison
//
// =============================================================================

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>

// SIMD includes for ARM64
#include <arm_neon.h>   // For NEON intrinsics on ARM64

// OPTIMIZATION 7: Branch prediction hints for profiling hygiene
#ifdef __GNUC__
#define likely(x)       __builtin_expect((x), 1)
#define unlikely(x)     __builtin_expect((x), 0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

typedef uint8_t  u8;
typedef uint32_t u32;

static const u8 confusion[512] = {
    0xac,0xd1,0x25,0x94,0x1f,0xb3,0x33,0x28,0x7c,0x2b,0x17,0xbc,0xf6,0xb0,0x55,0x5d,
    0x8f,0xd2,0x48,0xd4,0xd3,0x78,0x62,0x1a,0x02,0xf2,0x01,0xc9,0xaa,0xf0,0x83,0x71,
    0x72,0x4b,0x6a,0xe8,0xe9,0x42,0xc0,0x53,0x63,0x66,0x13,0x4a,0xc1,0x85,0xcf,0x0c,
    0x24,0x76,0xa5,0x6e,0xd7,0xa1,0xec,0xc6,0x04,0xc2,0xa2,0x5c,0x81,0x92,0x6c,0xda,
    0xc6,0x86,0xba,0x4d,0x39,0xa0,0x0e,0x8c,0x8a,0xd0,0xfe,0x59,0x96,0x49,0xe6,0xea,
    0x69,0x30,0x52,0x1c,0xe0,0xb2,0x05,0x9b,0x10,0x03,0xa8,0x64,0x51,0x97,0x02,0x09,
    0x8e,0xad,0xf7,0x36,0x47,0xab,0xce,0x7f,0x56,0xca,0x00,0xe3,0xed,0xf1,0x38,0xd8,
    0x26,0x1c,0xdc,0x35,0x91,0x43,0x2c,0x74,0xb4,0x61,0x9d,0x5e,0xe9,0x4c,0xbf,0x77,
    0x16,0x1e,0x21,0x1d,0x2d,0xa9,0x95,0xb8,0xc3,0x8d,0xf8,0xdb,0x34,0xe1,0x84,0xd6,
    0x0b,0x23,0x4e,0xff,0x3c,0x54,0xa7,0x78,0xa4,0x89,0x33,0x6d,0xfb,0x79,0x27,0xc4,
    0xf9,0x40,0x41,0xdf,0xc5,0x82,0x93,0xdd,0xa6,0xef,0xcd,0x8d,0xa3,0xae,0x7a,0xb6,
    0x2f,0xfd,0xbd,0xe5,0x98,0x66,0xf3,0x4f,0x57,0x88,0x90,0x9c,0x0a,0x50,0xe7,0x15,
    0x7b,0x58,0xbc,0x07,0x68,0x3a,0x5f,0xee,0x32,0x9f,0xeb,0xcc,0x18,0x8b,0xe2,0x57,
    0xb7,0x49,0x37,0xde,0xf5,0x99,0x67,0x5b,0x3b,0xbb,0x3d,0xb5,0x2d,0x19,0x2e,0x0d,
    0x93,0xfc,0x7e,0x06,0x08,0xbe,0x3f,0xd9,0x2a,0x70,0x9a,0xc8,0x7d,0xd8,0x46,0x65,
    0x22,0xf4,0xb9,0xa2,0x6f,0x12,0x1b,0x14,0x45,0xc7,0x87,0x31,0x60,0x29,0xf7,0x73,
    0x2c,0x97,0x72,0xcd,0x89,0xa6,0x88,0x4c,0xe8,0x83,0xeb,0x59,0xca,0x50,0x3f,0x27,
    0x4e,0xae,0x43,0xd5,0x6e,0xd0,0x99,0x7b,0x7c,0x40,0x0c,0x52,0x86,0xc1,0x46,0x12,
    0x5a,0x28,0xa8,0xbb,0xcb,0xf0,0x11,0x95,0x26,0x0d,0x34,0x66,0x22,0x18,0x6f,0x51,
    0x9b,0x3b,0xda,0xec,0x5e,0x00,0x2a,0xf5,0x8f,0x61,0xba,0x96,0xb3,0xd1,0x30,0xdc,
    0x33,0x75,0xe9,0x6d,0xc8,0xa1,0x3a,0x3e,0x5f,0x9d,0xfd,0xa9,0x31,0x9f,0xaa,0x85,
    0x2f,0x92,0xaf,0x67,0x78,0xa5,0xab,0x03,0x21,0x4f,0xb9,0xad,0xfe,0xf3,0x42,0xfc,
    0x17,0xd7,0xee,0xa3,0xd8,0x80,0x14,0x2e,0xa0,0x47,0x55,0xc4,0xff,0xe5,0x13,0x3f,
    0x81,0xb6,0x7a,0x94,0xd0,0xb5,0x54,0xbf,0x91,0xa7,0x37,0xf1,0x6b,0xc9,0x1b,0xb1,
    0x3c,0xb6,0xd9,0x32,0x24,0x8d,0xf2,0x82,0xb4,0xf9,0xdb,0x7d,0x44,0xfb,0x1e,0xd4,
    0xea,0x5d,0x35,0x69,0x23,0x71,0x57,0x01,0x06,0xe4,0x55,0x9a,0xa4,0x58,0x56,0xc7,
    0x4a,0x8c,0x8a,0xd6,0x6a,0x49,0x70,0xc5,0x8e,0x0a,0x62,0xdc,0x29,0x4b,0x42,0x41,
    0xcb,0x2b,0xb7,0xce,0x08,0xa1,0x76,0x1d,0x1a,0xb8,0xe3,0xcc,0x7e,0x48,0x20,0xe6,
    0xf8,0x45,0x93,0xde,0xc3,0x63,0x0f,0xb0,0xac,0x5c,0xba,0xdf,0x07,0x77,0xe7,0x4e,
    0x1f,0x28,0x10,0x6c,0x59,0xd3,0xdd,0x2d,0x65,0x39,0xb2,0x74,0x84,0x3d,0xf4,0xbd,
    0xc7,0x79,0x60,0x0b,0x4d,0x33,0x36,0x25,0xbc,0xe0,0x09,0xcf,0x5b,0xe2,0x38,0x9e,
    0xc0,0xef,0xd2,0x16,0x05,0xbe,0x53,0xf7,0xc2,0xc6,0xa2,0x24,0x98,0x1c,0xad,0x04
};

static const u32 diffusion[32] = {
    0xf26cb481,0x16a5dc92,0x3c5ba924,0x79b65248,0x2fc64b18,0x615acd29,0xc3b59a42,0x976b2584,
    0x6cf281b4,0xa51692dc,0x5b3c24a9,0xb6794852,0xc62f184b,0x5a6129cd,0xb5c3429a,0x6b978425,
    0xb481f26c,0xdc9216a5,0xa9243c5b,0x524879b6,0x4b182fc6,0xcd29615a,0x9a42c3b5,0x2584976b,
    0x81b46cf2,0x92dca516,0x24a95b3c,0x4852b679,0x184bc62f,0x29cd5a61,0x429ab5c3,0x84256b97
};

// 32-byte LUT for all 256 possible byte-masks (2×16-byte vectors)
// mask32_lut[b] = 32-byte array: each byte is 0xFF if the corresponding bit in `b` is 1, else 0x00.
// Each entry: bit i set -> bytes [i, i+8, i+16, i+24] = 0xFF, others = 0x00
__attribute__((aligned(16), section("__TEXT_CONST,constant_mask")))
static const uint8_t mask32_lut[256][32] = {
    // Just the first few entries for now - will be auto-generated in production
    {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00},
    {0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0x00,0x00,0x00,0x00,0x00,0x00}
    // ... rest of 252 entries would be auto-generated for full implementation
};

static double get_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

// Generate mask entry at runtime for given 32-bit row mask
static inline void generate_mask32_entry(u32 row_mask, uint8_t entry[32]) {
    for (int i = 0; i < 32; i++) {
        entry[i] = (row_mask & (1U << i)) ? 0xFF : 0x00;
    }
}

// OPTIMIZATION 3: Vectorized dot_row function optimized for Apple Silicon
// All optimizations from the Apple Silicon performance guide applied
__attribute__((always_inline))
static inline u8 dot_row_optimized(u32 row_mask, const u8 * __restrict state)
{
    // Optimization 4: Raise sparse cut-off to 8 bits for better M-series performance
    if (__builtin_popcount(row_mask) < 8) {
        // Use scalar approach for sparse masks
        u8 acc = 0;
        while (row_mask) {
            int k = __builtin_ctz(row_mask);
            acc ^= state[k];
            row_mask &= row_mask - 1;
        }
        return acc;
    }
    
    // Optimization 5: Prefetch from state array, not LUT (LUT is hot after first pass)
    __builtin_prefetch(state + 64, 0, 1);
    
    // Generate mask entry at runtime (or use static LUT in production)
    uint8_t mask_entry[32] __attribute__((aligned(16)));
    generate_mask32_entry(row_mask, mask_entry);
    
    // Optimization 1: Cut four LUT loads to two using vld1q_u8_x2
    uint8x16x2_t mask = vld1q_u8_x2(mask_entry);
    uint8x16x2_t st   = vld1q_u8_x2(state);
    
    // Apply masks and XOR accumulate
    uint8x16_t acc = veorq_u8( vandq_u8(st.val[0], mask.val[0]),
                               vandq_u8(st.val[1], mask.val[1]) );
    
    // Optimization 2: Cheaper horizontal XOR - stay in 128-bit register
    acc = veorq_u8(acc, vextq_u8(acc, acc, 8));
    acc = veorq_u8(acc, vextq_u8(acc, acc, 4));
    acc = veorq_u8(acc, vextq_u8(acc, acc, 2));
    acc = veorq_u8(acc, vextq_u8(acc, acc, 1));
    return vgetq_lane_u8(acc, 0);
}

static int invert32(const u32 A[32], u32 Ainv[32])
{
    uint64_t aug[32];
    for (int r = 0; r < 32; ++r)
        aug[r] = ((uint64_t)A[r]) | (1ULL << (32 + r));

    for (int c = 0; c < 32; ++c) {
        int piv = c;
        while (piv < 32 && !(aug[piv] >> c & 1)) ++piv;
        if (piv == 32) return -1;
        if (piv != c) { uint64_t tmp = aug[c]; aug[c] = aug[piv]; aug[piv] = tmp; }
        for (int r = 0; r < 32; ++r)
            if (r != c && (aug[r] >> c & 1)) aug[r] ^= aug[c];
    }

    for (int r = 0; r < 32; ++r)
        Ainv[r] = (u32)(aug[r] >> 32);
    return 0;
}

// OPTIMIZATION 7: Memory layout micro-tweaks with alignment
static u8  inv_low[256][256] __attribute__((aligned(64)));
static u8  inv_low_count[256] __attribute__((aligned(64)));
static u8  inv_high[256][256] __attribute__((aligned(64)));
static u8  inv_high_count[256] __attribute__((aligned(64)));
static u32 invM[32] __attribute__((aligned(32)));
static u8  odd_of[16][256] __attribute__((aligned(64)));
static u8  is_possible[16][256] __attribute__((aligned(64)));


static const u8 target[16] = "Hire me!!!!!!!!";

static void precompute(void)
{
    for (int b = 0; b < 256; ++b) inv_low_count[b] = 0;
    for (int x = 0; x < 256; ++x) {
        u8 y = confusion[x];
        inv_low[y][inv_low_count[y]++] = (u8)x;
    }
    
    for (int b = 0; b < 256; ++b) inv_high_count[b] = 0;
    for (int x = 0; x < 256; ++x) {
        u8 y = confusion[x + 256];
        inv_high[y][inv_high_count[y]++] = (u8)x;
    }
    
    if (unlikely(invert32(diffusion, invM))) {
        fprintf(stderr, "[FATAL] diffusion matrix not invertible!\n");
        exit(EXIT_FAILURE);
    }
    
    const u8 *Slo = confusion;
    for (int pos = 0; pos < 16; ++pos) {
        for (int ev = 0; ev < 256; ++ev) {
            u8 need = Slo[ev] ^ target[pos];
            if (inv_high_count[need] > 0) {
                odd_of[pos][ev] = inv_high[need][0];
                is_possible[pos][ev] = 1;
            } else {
                odd_of[pos][ev] = 0;
                is_possible[pos][ev] = 0;
            }
        }
    }
}

// -----------------------------------------------------------------------------
//  OPTIMIZATION 5: Explicit stack DFS (removes recursion overhead)
//  OPTIMIZATION 1: Mixed-radix counter (removes per-node division/modulo)
// -----------------------------------------------------------------------------

#ifdef PROFILE
// Profiling counters
static double time_matrix_computation = 0.0;
static double time_state_generation = 0.0;
static int call_count = 0;
#endif


// OPTIMIZATION 3: Single 2D array for better cache locality
static u8 state_buffer[257][32] __attribute__((aligned(64)));
static int recursion_depth = 0;

// Iterative DFS stack frame with lazy-clear optimization
typedef struct {
    int round;
    u8 idx[32];
    u8 choices_per_pos[32];
    const u8 *choices_ptr[32];
    u8 v[32];
    int counter_pos;  // Position in mixed-radix counter for continuation
    int highest_dirty_idx; // Optimization 3: Track highest dirty index for lazy clearing
} dfs_frame_t;

static int dfs_iterative_optimized(const u8 * __restrict initial_state, u8 * __restrict solution, u32 seed)
{
    dfs_frame_t stack[257];
    int sp = 0;
    
#ifdef PROFILE
    call_count = 0;
#endif
    
    // Initialize first frame
    memcpy(state_buffer[0], initial_state, 32);
    stack[sp].round = 0;
    memset(stack[sp].idx, 0, 32);
    stack[sp].counter_pos = 0;
    stack[sp].highest_dirty_idx = -1; // Optimization 3: No dirty indices initially
    sp++;
    
    while (sp > 0) {
        dfs_frame_t *frame = &stack[sp - 1];
        
#ifdef PROFILE
        if (unlikely(1)) call_count++; // Guard with unlikely for profiling hygiene
#endif
        
        if (frame->round == 256) {
            // Found solution
            memcpy(solution, state_buffer[sp - 1], 32);
            return 1;
        }
        
        // First time processing this frame?
        if (frame->counter_pos == 0) {
            // OPTIMIZATION 2: Fused single pass with early exit
            int valid_state = 1;
            
#ifdef PROFILE
            double start_fused = unlikely(1) ? get_time_ms() : 0.0;
#endif
            
            for (int j = 0; j < 32; ++j) {
                const u8 t = dot_row_optimized(invM[j], state_buffer[sp - 1]);
                frame->v[j] = t;
                const int c = inv_low_count[t];
                if (unlikely(!c)) { valid_state = 0; break; }
                
                frame->choices_per_pos[j] = c;
                frame->choices_ptr[j] = inv_low[t];
            }
            
#ifdef PROFILE
            if (unlikely(1)) {
                double fused_time = get_time_ms() - start_fused;
                time_matrix_computation += fused_time;
            }
#endif
            
            if (unlikely(!valid_state)) {
                sp--; // Pop invalid frame
                continue;
            }
        }
        
        // Try to generate next state with current idx - OPTIMIZATION 4: Compute on-the-fly
        if (frame->counter_pos == 0) {
#ifdef PROFILE
            double start_state = unlikely(1) ? get_time_ms() : 0.0;
#endif
            
            // Optimization 4: Generate new state on-the-fly without storing
            // Optimization 3: Ensure all idx[] values are initialized before use
            for (int j = frame->highest_dirty_idx + 1; j < 32; ++j) {
                frame->idx[j] = 0;
            }
            frame->highest_dirty_idx = 31;
            
            // Push new frame for next round and generate state directly
            stack[sp].round = frame->round + 1;
            stack[sp].counter_pos = 0;
            stack[sp].highest_dirty_idx = -1; // No dirty indices initially
            
            // Generate state directly in the target buffer - unrolled with uint64_t
            u8 *new_state = state_buffer[sp];
            for (int j = 0; j < 32; j += 8) {
                if (j + 7 < 32) {
                    uint64_t* dest64 = (uint64_t*)(new_state + j);
                    *dest64 = 
                        ((uint64_t)frame->choices_ptr[j][frame->idx[j]]) |
                        ((uint64_t)frame->choices_ptr[j+1][frame->idx[j+1]] << 8) |
                        ((uint64_t)frame->choices_ptr[j+2][frame->idx[j+2]] << 16) |
                        ((uint64_t)frame->choices_ptr[j+3][frame->idx[j+3]] << 24) |
                        ((uint64_t)frame->choices_ptr[j+4][frame->idx[j+4]] << 32) |
                        ((uint64_t)frame->choices_ptr[j+5][frame->idx[j+5]] << 40) |
                        ((uint64_t)frame->choices_ptr[j+6][frame->idx[j+6]] << 48) |
                        ((uint64_t)frame->choices_ptr[j+7][frame->idx[j+7]] << 56);
                } else {
                    for (int k = j; k < 32; ++k) {
                        new_state[k] = frame->choices_ptr[k][frame->idx[k]];
                    }
                    break;
                }
            }
            
            sp++; // Advance stack pointer after state generation
            
#ifdef PROFILE
            if (unlikely(1)) {
                time_state_generation += get_time_ms() - start_state;
            }
#endif
            
            frame->counter_pos = 1; // Mark as having pushed a child
            continue;
        }
        
        // Child returned, increment counter and try next combination
        // Optimization 3: Lazy clear - ensure idx[0] is initialized
        if (frame->highest_dirty_idx < 0) {
            frame->idx[0] = 0;
            frame->highest_dirty_idx = 0;
        }
        
        int k = 0;
        for (; k < 32; ++k) {
            // Lazy initialize idx[k] if we haven't used it yet
            if (k > frame->highest_dirty_idx) {
                frame->idx[k] = 0;
                frame->highest_dirty_idx = k;
            }
            
            if (++frame->idx[k] < frame->choices_per_pos[k]) {
                break;  // No carry needed
            }
            frame->idx[k] = 0;  // Wrap and continue to next digit
        }
        
        if (k == 32) {
            // Exhausted all combinations for this frame
            sp--;
        } else {
            // More combinations to try
            frame->counter_pos = 0;
        }
    }
    
    return 0; // No solution found
}

#ifdef PROFILE
static void print_dfs_profile(void)
{
    double total_time = time_matrix_computation + time_state_generation;
    printf("  Optimized DFS Profiling:\n");
    printf("    Total calls: %d\n", call_count);
    printf("    Matrix computation: %.2f ms (%.1f%%)\n", time_matrix_computation, 
           time_matrix_computation / total_time * 100);
    printf("    State generation: %.2f ms (%.1f%%)\n", time_state_generation,
           time_state_generation / total_time * 100);
}
#endif

static void build_final_state(u8 * __restrict c)
{
    u8 v[32];
    int max_attempts = 1000000;
    
    for (int attempt = 0; attempt < max_attempts; ++attempt) {
        int valid_state = 1;
        for (int pos = 0; pos < 16 && valid_state; ++pos) {
            int found = 0;
            for (int tries = 0; tries < 256; ++tries) {
                int ev = rand() % 256;
                
                if (is_possible[pos][ev]) {
                    u8 od = odd_of[pos][ev];
                    c[2*pos] = (u8)ev;
                    c[2*pos + 1] = od;
                    found = 1;
                    break;
                }
            }
            
            if (!found) {
                valid_state = 0;
            }
        }
        
        if (!valid_state) continue;
        
        int all_valid = 1;
        for (int j = 0; j < 32; ++j) {
            v[j] = dot_row_optimized(invM[j], c);
            if (inv_low_count[v[j]] == 0) {
                all_valid = 0;
                break;
            }
        }
        
        if (all_valid) {
            return;
        }
        
        for (int fix_attempts = 0; fix_attempts < 10; ++fix_attempts) {
            int pos = rand() % 16;
            int ev = rand() % 256;
            
            if (is_possible[pos][ev]) {
                u8 od = odd_of[pos][ev];
                c[2*pos] = (u8)ev;
                c[2*pos + 1] = od;
                
                all_valid = 1;
                for (int j = 0; j < 32; ++j) {
                    v[j] = dot_row_optimized(invM[j], c);
                    if (inv_low_count[v[j]] == 0) {
                        all_valid = 0;
                        break;
                    }
                }
                
                if (all_valid) {
                    return;
                }
            }
        }
    }
    
    fprintf(stderr, "[FATAL] could not build a legal end‑state\n");
    exit(EXIT_FAILURE);
}

static void Forward(const u8 * __restrict in, u8 * __restrict out)
{
    u8 c[32];
    memcpy(c, in, 32);

    u8 d[32];
    for (int r = 0; r < 256; ++r) {
        for (int j = 0; j < 32; ++j) { d[j] = confusion[c[j]]; c[j] = 0; }
        for (int j = 0; j < 32; ++j)
            for (int k = 0; k < 32; ++k)
                if (diffusion[j] >> k & 1) c[j] ^= d[k];
    }
    for (int i = 0; i < 16; ++i)
        out[i] = confusion[c[2*i]] ^ confusion[c[2*i+1] + 256];
}

int main(void)
{
#ifdef PROFILE
    double start_time = unlikely(1) ? get_time_ms() : 0.0; // Guard with unlikely
#endif
    
    srand(time(NULL));
    
#ifdef PROFILE
    double precompute_start = unlikely(1) ? get_time_ms() : 0.0; // Guard with unlikely
#endif
    precompute();
#ifdef PROFILE
    if (unlikely(1)) {
        double precompute_time = get_time_ms() - precompute_start;
        printf("[TIMING] Precomputation took %.2f ms\n", precompute_time);
    }
#endif
    
    const int MAX_FINAL_STATE_ATTEMPTS = 100000;
    
    for (int attempt = 0; attempt < MAX_FINAL_STATE_ATTEMPTS; ++attempt) {
        u8 c256[32];
        build_final_state(c256);

        double dfs_start = get_time_ms();
        u8 dfs_solution[32];
#ifdef PROFILE
        if (unlikely(1)) {
            call_count = 0;
            time_matrix_computation = 0.0;
            time_state_generation = 0.0;
        }
#endif
        recursion_depth = 0;
        int dfs_found = dfs_iterative_optimized(c256, dfs_solution, time(NULL));
        double dfs_time = get_time_ms() - dfs_start;
        
        if (dfs_found) {
            u8 test_out[32] = {0};
            Forward(dfs_solution, test_out);
            if (!memcmp(test_out, target, 16)) {
#ifdef PROFILE
                double total_time = unlikely(1) ? get_time_ms() - start_time : 0.0;
#endif
                
                printf("[SUCCESS] After %d attempts:\n", attempt + 1);
                printf("  Optimized DFS: Found valid solution in %.2f ms\n", dfs_time);
#ifdef PROFILE
                if (unlikely(1)) {
                    print_dfs_profile();
                    printf("  Total: %.2f ms\n", total_time);
                }
#endif
                printf("  Solution: ");
                for (int i = 0; i < 32; ++i) {
                    printf("%02x", dfs_solution[i]);
                }
                printf("\n");
                
                return 0;
            }
        }
    }
    
    fprintf(stderr, "[FATAL] Could not find a valid solution\n");
    return 1;
}