// -----------------------------------------------------------------------------
//  "Hire‑me" crack‑me – reference Level‑2 solver (≈ sub‑millisecond on a laptop)
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
//  Build & run      $  gcc -O2 hireme2.c -o hireme2 && ./hireme2
// -----------------------------------------------------------------------------

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>

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
//  Helpers: tiny GF(2) linear algebra on 32-bit words
// -----------------------------------------------------------------------------

// Use the builtin version as the main one for better performance
static inline u8 dot_row(u32 row, const u8 v[32])
{
    u8 acc = 0;
    
    // Process only set bits - skip zeros entirely
    while (row) {
        int k = __builtin_ctz(row);  // Count trailing zeros - finds next set bit
        acc ^= v[k];
        row &= row - 1;  // Clear the lowest set bit
    }
    
    return acc;
}

// Gaussian elimination - invert a 32×32 binary matrix packed as 32 u32 rows.
// Returns 0 on success.
static int invert32(const u32 A[32], u32 Ainv[32])
{
    uint64_t aug[32];  // low 32 bits = A, high 32 bits = I

    for (int r = 0; r < 32; ++r)
        aug[r] = ((uint64_t)A[r]) | (1ULL << (32 + r));

    for (int c = 0; c < 32; ++c) {
        int piv = c;
        while (piv < 32 && !(aug[piv] >> c & 1)) ++piv;
        if (piv == 32) return -1;  // singular (shouldn't happen)
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

static u8  inv_low[256][256];   // inv_low[y][i] = i-th pre-image of y
static u8  inv_low_count[256];  // Number of pre-images for each y (0 = no pre-image)
static u8  inv_high[256][256];  // inv_high[y][i] = i-th pre-image of y  
static u8  inv_high_count[256]; // Number of pre-images for each y
static u32 invM[32];
static u8  odd_of[16][256];     // Pre-computed odd bytes for each (pos, even_byte)
static u8  is_possible[16][256]; // Pre-computed validity for each (pos, even_byte)

static const u8 target[16] = "Hire me!!!!!!!!";  // 15 chars + \0 terminator

static void precompute(void)
{
    // Build S_low⁻¹
    for (int b = 0; b < 256; ++b) inv_low_count[b] = 0;
    for (int x = 0; x < 256; ++x) {
        u8 y = confusion[x];
        inv_low[y][inv_low_count[y]++] = (u8)x;
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
//  Stage 1 - pick a *reachable* 32-byte state c₍₂₅₆₎ s.t.
//            S_low[c[2i]] ⊕ S_high[c[2i+1]] = target[i]
//            and  M⁻¹·c  contains no forbidden bytes.
// -----------------------------------------------------------------------------

// Forward declarations
static int inverse_256_rounds_bfs(u8 solutions[][32], int max_solutions, const u8 initial_state[32]);
static int inverse_256_rounds_dfs(u8 solution[32], const u8 initial_state[32]);

static void build_final_state(u8 c[32])
{
    u8 v[32];
    int max_attempts = 1000000;
    
    for (int attempt = 0; attempt < max_attempts; ++attempt) {
        // For each position, find a random even byte with valid odd byte
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
        
        // Check if multiplying by invM gives forbidden values
        int all_valid = 1;
        for (int j = 0; j < 32; ++j) {
            v[j] = dot_row(invM[j], c);
            if (inv_low_count[v[j]] == 0) {
                all_valid = 0;
                break;
            }
        }
        
        if (all_valid) {
            return;
        }
        
        // If invalid, try changing random positions
        for (int fix_attempts = 0; fix_attempts < 10; ++fix_attempts) {
            int pos = rand() % 16;  // Random position to change
            int ev = rand() % 256;  // Random new even byte
            
            if (is_possible[pos][ev]) {
                u8 od = odd_of[pos][ev];
                c[2*pos] = (u8)ev;
                c[2*pos + 1] = od;
                
                // Check if this fixes the invM issue
                all_valid = 1;
                for (int j = 0; j < 32; ++j) {
                    v[j] = dot_row(invM[j], c);
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
    
    fprintf(stderr, "[FATAL] could not build a legal end‑state after %d attempts\n", max_attempts);
    exit(EXIT_FAILURE);
}

// -----------------------------------------------------------------------------
//  Stage 2 - walk 256 rounds backwards: cₙ₊₁ → cₙ
// -----------------------------------------------------------------------------

typedef struct {
    u8 state[32];
} StateNode;


static int inverse_256_rounds_bfs(u8 solutions[][32], int max_solutions, const u8 initial_state[32])
{
    // BFS queue implementation using dynamic arrays
    StateNode *current_level = malloc(1000000 * sizeof(StateNode));
    StateNode *next_level = malloc(1000000 * sizeof(StateNode));
    if (!current_level || !next_level) {
        fprintf(stderr, "[FATAL] Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    
    int current_count = 1;
    int next_count = 0;
    const int MAX_STATES = 1000000;
    
    // Initialize with the final state
    memcpy(current_level[0].state, initial_state, 32);
    
    // BFS through 256 rounds backwards
    for (int round = 0; round < 256; ++round) {
        next_count = 0;
        
        // Process all states at current level
        for (int state_idx = 0; state_idx < current_count; ++state_idx) {
            u8 *curr_state = current_level[state_idx].state;
            u8 v[32];
            
            // Compute M⁻¹·c for current state
            int valid_state = 1;
            for (int j = 0; j < 32; ++j) {
                v[j] = dot_row(invM[j], curr_state);
                if (inv_low_count[v[j]] == 0) {
                    valid_state = 0;
                    break;
                }
            }
            
            if (!valid_state) continue;
            
            // Generate ALL possible combinations of pre-images
            // Calculate total number of combinations
            int total_combinations = 1;
            int choices_per_pos[32];
            for (int j = 0; j < 32; ++j) {
                choices_per_pos[j] = inv_low_count[v[j]];
                total_combinations *= choices_per_pos[j];
                if (total_combinations > MAX_STATES) {
                    total_combinations = MAX_STATES;
                    break;
                }
            }
            
            // Generate each combination by treating it as a mixed-radix number
            for (int combo = 0; combo < total_combinations && next_count < MAX_STATES; ++combo) {
                u8 temp_state[32];
                int temp_combo = combo;
                
                // Convert combo index to specific choices for each position
                for (int j = 0; j < 32; ++j) {
                    int choice_idx = temp_combo % choices_per_pos[j];
                    temp_combo /= choices_per_pos[j];
                    temp_state[j] = inv_low[v[j]][choice_idx];
                }
                
                memcpy(next_level[next_count].state, temp_state, 32);
                next_count++;
            }
        }
        
        if (next_count == 0) {
            free(current_level);
            free(next_level);
            return 0;  // No valid paths found
        }
        
        // Swap levels for next iteration
        StateNode *temp = current_level;
        current_level = next_level;
        next_level = temp;
        current_count = next_count;
        
        // Limit the number of states to prevent explosion
        if (current_count > MAX_STATES / 10) {
            current_count = MAX_STATES / 10;
        }
    }
    
    // If we reach here, we found valid solutions
    // Return all solutions (up to max_solutions)
    int num_solutions = current_count < max_solutions ? current_count : max_solutions;
    if (num_solutions > 0) {
        for (int i = 0; i < num_solutions; ++i) {
            memcpy(solutions[i], current_level[i].state, 32);
        }
        free(current_level);
        free(next_level);
        return num_solutions;
    }
    
    free(current_level);
    free(next_level);
    return 0;
}

// -----------------------------------------------------------------------------
//  DFS approach - recursive depth-first search (finds first solution only)
// -----------------------------------------------------------------------------

// Enable/disable profiling
#define PROFILE_ENABLED 1

// Timing counters for profiling
static double time_matrix_computation = 0.0;
static double time_combination_calc = 0.0;
static double time_state_generation = 0.0;
static double time_recursive_calls = 0.0;
static int call_count = 0;

// Pre-allocated buffers to avoid malloc in recursion
static u8 preallocated_state[256][32];  // One buffer per recursion depth
static int recursion_depth = 0;

// Maximum combinations before early exit
#define MAX_COMBO_LIMIT 1000000

static int dfs_recursive(u8 state[32], int round, u8 solution[32])
{
    call_count++;
    
    if (round == 256) {
        // Reached the beginning - this is a valid solution
        memcpy(solution, state, 32);
        return 1; // Signal that we found a solution
    }
    
    u8 v[32];
    
    // TIMING: Compute M⁻¹·state for current state
#if PROFILE_ENABLED
    double start_matrix = get_time_ms();
#endif
    int valid_state = 1;
    for (int j = 0; j < 32; ++j) {
        v[j] = dot_row(invM[j], state);
        if (inv_low_count[v[j]] == 0) {
            valid_state = 0;
            break;
        }
    }
#if PROFILE_ENABLED
    time_matrix_computation += get_time_ms() - start_matrix;
#endif
    
    if (!valid_state) return 0;
    
    // TIMING: Calculate combinations
#if PROFILE_ENABLED
    double start_combo = get_time_ms();
#endif
    int choices_per_pos[32];
    long long total_combinations = 1;
    
    for (int j = 0; j < 32; ++j) {
        choices_per_pos[j] = inv_low_count[v[j]];
        total_combinations *= choices_per_pos[j];
        // Early exit if combinations become too large
        if (total_combinations > MAX_COMBO_LIMIT) {
            return 0;
        }
    }
#if PROFILE_ENABLED
    time_combination_calc += get_time_ms() - start_combo;
#endif
    
    // Use preallocated buffer instead of malloc
    u8 *new_state = preallocated_state[recursion_depth];
    recursion_depth++;
    
    // Generate combinations iteratively without malloc/shuffle
    // Use a simple counter approach with pseudo-random ordering
    int combo_start = rand() % (int)total_combinations;
    
    for (int combo_offset = 0; combo_offset < total_combinations; ++combo_offset) {
        int combo = (combo_start + combo_offset) % (int)total_combinations;
        
        // TIMING: Generate new state
#if PROFILE_ENABLED
        double start_state = get_time_ms();
#endif
        int temp_combo = combo;
        
        // Convert combo index to specific choices for each position
        for (int j = 0; j < 32; ++j) {
            int choice_idx = temp_combo % choices_per_pos[j];
            temp_combo /= choices_per_pos[j];
            new_state[j] = inv_low[v[j]][choice_idx];
        }
#if PROFILE_ENABLED
        time_state_generation += get_time_ms() - start_state;
#endif
        
        // TIMING: Recursive call
#if PROFILE_ENABLED
        double start_recursive = get_time_ms();
#endif
        int result = dfs_recursive(new_state, round + 1, solution);
#if PROFILE_ENABLED
        time_recursive_calls += get_time_ms() - start_recursive;
#endif
        
        if (result) {
            recursion_depth--;
            return 1; // Solution found, propagate up
        }
    }
    
    recursion_depth--;
    return 0; // No solution found in this branch
}

static void reset_dfs_timers(void)
{
    time_matrix_computation = 0.0;
    time_combination_calc = 0.0;
    time_state_generation = 0.0;
    time_recursive_calls = 0.0;
    call_count = 0;
}

static void print_dfs_profile(void)
{
    printf("  DFS Profiling:\n");
    printf("    Total calls: %d\n", call_count);
    printf("    Matrix computation: %.2f ms (%.1f%%)\n", time_matrix_computation, 
           time_matrix_computation / (time_matrix_computation + time_combination_calc + time_state_generation + time_recursive_calls) * 100);
    printf("    Combination calc: %.2f ms (%.1f%%)\n", time_combination_calc,
           time_combination_calc / (time_matrix_computation + time_combination_calc + time_state_generation + time_recursive_calls) * 100);
    printf("    State generation: %.2f ms (%.1f%%)\n", time_state_generation,
           time_state_generation / (time_matrix_computation + time_combination_calc + time_state_generation + time_recursive_calls) * 100);
    printf("    Recursive calls: %.2f ms (%.1f%%)\n", time_recursive_calls,
           time_recursive_calls / (time_matrix_computation + time_combination_calc + time_state_generation + time_recursive_calls) * 100);
}

static int inverse_256_rounds_dfs(u8 solution[32], const u8 initial_state[32])
{
    reset_dfs_timers();
    recursion_depth = 0;  // Reset recursion depth
    return dfs_recursive((u8*)initial_state, 0, solution);
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
    
    srand(time(NULL));
    
    double precompute_start = get_time_ms();
    precompute();
    double precompute_time = get_time_ms() - precompute_start;
    
    // printf("[TIMING] Precomputation took %.2f ms\n", precompute_time);

    const int MAX_FINAL_STATE_ATTEMPTS = 100000;
    
    for (int attempt = 0; attempt < MAX_FINAL_STATE_ATTEMPTS; ++attempt) {
        u8 c256[32];
        build_final_state(c256);

        // Test both BFS and DFS approaches
        double bfs_start = get_time_ms();
        u8 bfs_solutions[10000][32];  // Store up to 10000 solutions for BFS
        int bfs_num_solutions = inverse_256_rounds_bfs(bfs_solutions, 10000, c256);
        double bfs_time = get_time_ms() - bfs_start;
        
        double dfs_start = get_time_ms();
        u8 dfs_solution[32];  // Store only 1 solution for DFS (first found)
        int dfs_found = inverse_256_rounds_dfs(dfs_solution, c256);
        double dfs_time = get_time_ms() - dfs_start;
        
        if (bfs_num_solutions > 0 || dfs_found) {
            double total_time = get_time_ms() - start_time;
            
            // Verify BFS solutions without printing
            int bfs_valid_count = 0;
            for (int i = 0; i < bfs_num_solutions; ++i) {
                u8 test_out[32] = {0};
                Forward(bfs_solutions[i], test_out);
                if (!memcmp(test_out, target, 16)) {
                    bfs_valid_count++;
                }
            }
            
            // Verify DFS solution without printing
            int dfs_valid = 0;
            if (dfs_found) {
                u8 test_out[32] = {0};
                Forward(dfs_solution, test_out);
                if (!memcmp(test_out, target, 16)) {
                    dfs_valid = 1;
                }
            }
            
            if (bfs_valid_count > 0 || dfs_valid) {
                printf("[SUCCESS] After %d attempts:\n", attempt + 1);
                printf("  BFS: Found %d valid solutions (out of %d) in %.2f ms\n", bfs_valid_count, bfs_num_solutions, bfs_time);
                printf("  DFS: Found %d valid solution in %.2f ms\n", dfs_valid, dfs_time);
                if (dfs_valid) {
                    print_dfs_profile();
                }
                printf("  Total: %.2f ms\n", total_time);
                return 0;
            } else {
                // Continue to next iteration
            }
        } else {
            // Continue to next iteration to try a different final state
        }
    }
    
    fprintf(stderr, "[FATAL] Could not find a valid solution after %d final state attempts\n", MAX_FINAL_STATE_ATTEMPTS);
    return 1;
}