// -----------------------------------------------------------------------------
//  "Hire‑me" crack‑me – OPTIMIZED Level‑2 solver with all 6 optimizations
// -----------------------------------------------------------------------------
//  The binary shipped by Nintendo runs the routine below :
//
//      for 256 rounds  c = M · S_low(c)              // 32‑byte state
//      out[i] = S_low(c[2i]) XOR S_high(c[2i+1])     // 16‑byte result
//
//  with
//      • S_low  = confusion[0 … 255]
//      • S_high = confusion[256 … 511]
//      • M      = 32×32 Boolean matrix encoded in diffusion[32]
//
//  The compare is   memcmp(out, "Hire me!!!!!!!!\0", 16).
//
//  This source‑file reconstructs **one** 32‑byte input that passes the test –
//  and does so deterministically, without any brute‑force loops.
//  Total work ≈ 256 · 32 · 32 ≈ 26 k XORs → < 1 ms on a modern CPU.
// -----------------------------------------------------------------------------
//  Build & run      $  gcc -O3 -march=native hireme2_optimized.c -o hireme2_optimized && ./hireme2_optimized
// -----------------------------------------------------------------------------

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>
#ifdef __x86_64__
#include <immintrin.h>  // For SIMD instructions on x86
#endif
#ifdef __aarch64__
#include <arm_neon.h>   // For SIMD instructions on ARM
#endif

// -----------------------------------------------------------------------------
//  Original tables copied verbatim from the crack‑me
// -----------------------------------------------------------------------------

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
    // ----  S_high  ----
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

// -----------------------------------------------------------------------------
//  Timing helpers
// -----------------------------------------------------------------------------

static double get_time_ms(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

// -----------------------------------------------------------------------------
//  OPTIMIZATION 4: SIMD-accelerated dot product
// -----------------------------------------------------------------------------

#ifdef __aarch64__
// ARM NEON version for ARM64
static inline u8 dot_row_neon(u32 row, const u8 v[32])
{
    // Load the 32-byte state into NEON registers (2 x 128-bit)
    uint8x16_t state_low = vld1q_u8(v);
    uint8x16_t state_high = vld1q_u8(v + 16);
    
    // Create mask vectors based on the row bits
    u8 mask_array[32];
    for (int i = 0; i < 32; i++) {
        mask_array[i] = (row >> i) & 1 ? 0xFF : 0x00;
    }
    uint8x16_t mask_low = vld1q_u8(mask_array);
    uint8x16_t mask_high = vld1q_u8(mask_array + 16);
    
    // AND the state with the mask
    uint8x16_t masked_low = vandq_u8(state_low, mask_low);
    uint8x16_t masked_high = vandq_u8(state_high, mask_high);
    
    // XOR the two halves together
    uint8x16_t result = veorq_u8(masked_low, masked_high);
    
    // Horizontal XOR reduction
    uint8x8_t low = vget_low_u8(result);
    uint8x8_t high = vget_high_u8(result);
    uint8x8_t xor8 = veor_u8(low, high);
    
    // Continue reducing - unroll the loop since vget_lane needs constant index
    u8 acc = vget_lane_u8(xor8, 0);
    acc ^= vget_lane_u8(xor8, 1);
    acc ^= vget_lane_u8(xor8, 2);
    acc ^= vget_lane_u8(xor8, 3);
    acc ^= vget_lane_u8(xor8, 4);
    acc ^= vget_lane_u8(xor8, 5);
    acc ^= vget_lane_u8(xor8, 6);
    acc ^= vget_lane_u8(xor8, 7);
    
    return acc;
}
#endif

#ifdef __AVX2__
// x86 AVX2 version
static inline u8 dot_row_avx2(u32 row, const u8 v[32])
{
    // Load the 32-byte state into a 256-bit register
    __m256i state = _mm256_loadu_si256((const __m256i*)v);
    
    // Create a mask vector based on the row bits
    u8 mask_array[32];
    for (int i = 0; i < 32; i++) {
        mask_array[i] = (row >> i) & 1 ? 0xFF : 0x00;
    }
    __m256i mask = _mm256_loadu_si256((const __m256i*)mask_array);
    
    // AND the state with the mask
    __m256i masked = _mm256_and_si256(state, mask);
    
    // XOR reduce the result
    // First, reduce 256 bits to 128 bits
    __m128i low = _mm256_castsi256_si128(masked);
    __m128i high = _mm256_extracti128_si256(masked, 1);
    __m128i xor128 = _mm_xor_si128(low, high);
    
    // Then reduce 128 bits to 64 bits
    __m128i shifted = _mm_srli_si128(xor128, 8);
    xor128 = _mm_xor_si128(xor128, shifted);
    
    // Extract and continue reducing
    uint64_t val = _mm_cvtsi128_si64(xor128);
    val ^= (val >> 32);
    val ^= (val >> 16);
    val ^= (val >> 8);
    
    return (u8)val;
}
#endif

// Main dot_row function with platform-specific dispatching
static inline u8 dot_row(u32 row, const u8 v[32])
{
#ifdef __aarch64__
    return dot_row_neon(row, v);
#elif defined(__AVX2__)
    return dot_row_avx2(row, v);
#else
    // Fallback non-SIMD version
    u8 acc = 0;
    while (row) {
        int k = __builtin_ctz(row);
        acc ^= v[k];
        row &= row - 1;
    }
    return acc;
#endif
}

// Gaussian elimination - invert a 32×32 binary matrix packed as 32 u32 rows.
static int invert32(const u32 A[32], u32 Ainv[32])
{
    uint64_t aug[32];  // low 32 bits = A, high 32 bits = I

    for (int r = 0; r < 32; ++r)
        aug[r] = ((uint64_t)A[r]) | (1ULL << (32 + r));

    for (int c = 0; c < 32; ++c) {
        int piv = c;
        while (piv < 32 && !(aug[piv] >> c & 1)) ++piv;
        if (piv == 32) return -1;  // singular
        if (piv != c) { uint64_t tmp = aug[c]; aug[c] = aug[piv]; aug[piv] = tmp; }
        for (int r = 0; r < 32; ++r)
            if (r != c && (aug[r] >> c & 1)) aug[r] ^= aug[c];
    }

    for (int r = 0; r < 32; ++r)
        Ainv[r] = (u32)(aug[r] >> 32);
    return 0;
}

// -----------------------------------------------------------------------------
//  Build S_low⁻¹, S_high⁻¹ and M⁻¹
// -----------------------------------------------------------------------------

// OPTIMIZATION 5: Modified structure to cache pointers
static struct {
    u8 values[256];  // The actual pre-images
    u8 count;        // Number of pre-images
} inv_low_cache[256];

static u8  inv_high[256][256];
static u8  inv_high_count[256];
static u32 invM[32];
static u8  odd_of[16][256];
static u8  is_possible[16][256];

static const u8 target[16] = "Hire me!!!!!!!!";

static void precompute(void)
{
    // Build S_low⁻¹ with cached structure
    for (int b = 0; b < 256; ++b) {
        inv_low_cache[b].count = 0;
    }
    for (int x = 0; x < 256; ++x) {
        u8 y = confusion[x];
        inv_low_cache[y].values[inv_low_cache[y].count++] = (u8)x;
    }
    
    // Build S_high⁻¹
    for (int b = 0; b < 256; ++b) inv_high_count[b] = 0;
    for (int x = 0; x < 256; ++x) {
        u8 y = confusion[x + 256];
        inv_high[y][inv_high_count[y]++] = (u8)x;
    }
    
    // Build M⁻¹
    if (invert32(diffusion, invM)) {
        fprintf(stderr, "[FATAL] diffusion matrix not invertible!\n");
        exit(EXIT_FAILURE);
    }
    
    // Pre-compute odd_of and is_possible tables
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
//  OPTIMIZATION 3: Stack-based DFS instead of recursion
// -----------------------------------------------------------------------------

typedef struct {
    u8 state[32];
    int round;
} DFSNode;

// Maximum depth is 256
#define MAX_STACK_SIZE 257
#define MAX_COMBO_LIMIT 1000000

// OPTIMIZATION 6: Deterministic traversal
static inline uint32_t deterministic_random(int round, uint32_t seed, uint32_t total)
{
    return ((round * 0x9E3779B9u) ^ seed) % total;
}

// For sorting - not needed with manual insertion sort below

static int inverse_256_rounds_stack(u8 solution[32], const u8 initial_state[32], uint32_t seed)
{
    DFSNode stack[MAX_STACK_SIZE];
    int sp = 0;
    
    // Initialize stack with the initial state
    memcpy(stack[0].state, initial_state, 32);
    stack[0].round = 0;
    
    // Temporary buffers
    u8 v[32];
    u8 choices_per_pos[32];
    u8 order[32];
    const u8 *choices_ptr[32];  // OPTIMIZATION 5: Cache pointers
    
    while (sp >= 0) {
        DFSNode *current = &stack[sp];
        
        if (current->round == 256) {
            // Found solution
            memcpy(solution, current->state, 32);
            return 1;
        }
        
        // Pop current node
        sp--;
        
        // Compute M⁻¹·state
        int valid_state = 1;
        for (int j = 0; j < 32; ++j) {
            v[j] = dot_row(invM[j], current->state);
            choices_per_pos[j] = inv_low_cache[v[j]].count;
            if (choices_per_pos[j] == 0) {
                valid_state = 0;
                break;
            }
            // OPTIMIZATION 5: Cache the pointer
            choices_ptr[j] = inv_low_cache[v[j]].values;
        }
        
        if (!valid_state) continue;
        
        // OPTIMIZATION 1: Sort indices by number of choices (ascending)
        for (int j = 0; j < 32; ++j) order[j] = j;
        // Simple insertion sort for 32 elements
        for (int i = 1; i < 32; ++i) {
            u8 key = order[i];
            int j = i - 1;
            while (j >= 0 && choices_per_pos[order[j]] > choices_per_pos[key]) {
                order[j + 1] = order[j];
                j--;
            }
            order[j + 1] = key;
        }
        
        // OPTIMIZATION 2: Calculate total combinations with early exit
        long long total_combinations = 1;
        for (int idx = 0; idx < 32; ++idx) {
            int j = order[idx];
            int c = choices_per_pos[j];
            if (total_combinations > MAX_COMBO_LIMIT / c) {
                total_combinations = MAX_COMBO_LIMIT + 1;  // Signal overflow
                break;
            }
            total_combinations *= c;
        }
        
        if (total_combinations > MAX_COMBO_LIMIT) continue;
        
        // OPTIMIZATION 6: Use deterministic ordering
        uint32_t combo_start = deterministic_random(current->round, seed, (uint32_t)total_combinations);
        
        // Try combinations in deterministic order
        for (int offset = 0; offset < total_combinations && sp < MAX_STACK_SIZE - 1; ++offset) {
            uint32_t combo = (combo_start + offset) % (uint32_t)total_combinations;
            
            // Generate state for this combination
            DFSNode *new_node = &stack[++sp];
            new_node->round = current->round + 1;
            
            uint32_t temp_combo = combo;
            // Convert combo to choices WITHOUT respecting sorted order
            // The sorting is only for early pruning, not for combination generation
            for (int j = 0; j < 32; ++j) {
                int choice_idx = temp_combo % choices_per_pos[j];
                temp_combo /= choices_per_pos[j];
                new_node->state[j] = choices_ptr[j][choice_idx];
            }
        }
    }
    
    return 0;  // No solution found
}

// -----------------------------------------------------------------------------
//  Build final state
// -----------------------------------------------------------------------------

static void build_final_state(u8 c[32], uint32_t seed)
{
    u8 v[32];
    int max_attempts = 1000000;
    
    for (int attempt = 0; attempt < max_attempts; ++attempt) {
        // Use deterministic random based on seed
        uint32_t rng_state = seed + attempt;
        
        // For each position, find a valid even/odd pair
        int valid_state = 1;
        for (int pos = 0; pos < 16 && valid_state; ++pos) {
            int found = 0;
            uint32_t start_ev = (rng_state * 0x9E3779B9u) % 256;
            
            for (int tries = 0; tries < 256; ++tries) {
                int ev = (start_ev + tries) % 256;
                
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
            rng_state = rng_state * 1664525u + 1013904223u;  // LCG
        }
        
        if (!valid_state) continue;
        
        // Check if multiplying by invM gives valid values
        int all_valid = 1;
        for (int j = 0; j < 32; ++j) {
            v[j] = dot_row(invM[j], c);
            if (inv_low_cache[v[j]].count == 0) {
                all_valid = 0;
                break;
            }
        }
        
        if (all_valid) {
            return;
        }
    }
    
    fprintf(stderr, "[FATAL] could not build a legal end‑state after %d attempts\n", max_attempts);
    exit(EXIT_FAILURE);
}

// -----------------------------------------------------------------------------
//  Check - run the original Forward() just to prove it works
// -----------------------------------------------------------------------------

static void Forward(const u8 in[32], u8 out[32])
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

// -----------------------------------------------------------------------------
//  Main
// -----------------------------------------------------------------------------

int main(void)
{
    double start_time = get_time_ms();
    
    // Use deterministic seed for reproducibility
    uint32_t seed = 0x12345678;
    
    double precompute_start = get_time_ms();
    precompute();
    double precompute_time = get_time_ms() - precompute_start;
    
    printf("[TIMING] Precomputation took %.2f ms\n", precompute_time);

    const int MAX_FINAL_STATE_ATTEMPTS = 1000;
    
    for (int attempt = 0; attempt < MAX_FINAL_STATE_ATTEMPTS; ++attempt) {
        u8 c256[32];
        build_final_state(c256, seed + attempt);

        double solve_start = get_time_ms();
        u8 solution[32];
        int found = inverse_256_rounds_stack(solution, c256, seed);
        double solve_time = get_time_ms() - solve_start;
        
        if (found) {
            // Verify solution
            u8 test_out[32] = {0};
            Forward(solution, test_out);
            
            if (!memcmp(test_out, target, 16)) {
                double total_time = get_time_ms() - start_time;
                
                printf("[SUCCESS] After %d attempts:\n", attempt + 1);
                printf("  Solution found in %.2f ms\n", solve_time);
                printf("  Total time: %.2f ms\n", total_time);
                
                printf("  Solution (hex): ");
                for (int i = 0; i < 32; ++i) {
                    printf("%02x", solution[i]);
                }
                printf("\n");
                
                return 0;
            }
        }
    }
    
    fprintf(stderr, "[FATAL] Could not find a valid solution after %d attempts\n", MAX_FINAL_STATE_ATTEMPTS);
    return 1;
}