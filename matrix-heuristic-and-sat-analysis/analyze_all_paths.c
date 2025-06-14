// Analyze ALL winning paths for a given final state
// This will give us comprehensive statistics on choice preferences

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>
#include <math.h>

typedef uint8_t  u8;
typedef uint32_t u32;

// Copy essential constants and functions from hireme2.c
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

// Global data structures
static u8  inv_low[256][256];
static u8  inv_low_count[256];
static u32 invM[32];

// Statistics tracking
typedef struct {
    int total_occurrences;
    int choice_0_count;  // How many times choice index 0 was used
    int choice_1_count;  // How many times choice index 1 was used
} ChoiceStats;

// Track statistics for each multi-choice output across all paths
static ChoiceStats choice_stats[256];
static int total_paths_analyzed = 0;

// Path recording structure
typedef struct {
    u8 choices[256][32];  // choices[round][position] = choice made
} Path;

static Path* all_paths = NULL;
static int paths_capacity = 10000;
static int paths_found = 0;

// Helper functions
static double get_time_ms(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
}

static inline u8 dot_row(u32 row, const u8 v[32]) {
    u8 acc = 0;
    while (row) {
        int k = __builtin_ctz(row);
        acc ^= v[k];
        row &= row - 1;
    }
    return acc;
}

// Matrix inversion
static int invert32(const u32 A[32], u32 Ainv[32]) {
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

static void precompute(void) {
    // Build S_low^-1
    for (int b = 0; b < 256; ++b) inv_low_count[b] = 0;
    for (int x = 0; x < 256; ++x) {
        u8 y = confusion[x];
        inv_low[y][inv_low_count[y]++] = (u8)x;
    }
    
    // Build M^-1
    if (invert32(diffusion, invM)) {
        fprintf(stderr, "[FATAL] diffusion matrix not invertible!\n");
        exit(EXIT_FAILURE);
    }
}

// Modified BFS to record ALL paths
static int find_all_paths_bfs(const u8 initial_state[32]) {
    typedef struct {
        u8 state[32];
        int parent_idx;
        int round;
        u8 choice_made[32];  // What choice was made to get here
        u8 v_vector[32];     // The v-vector that generated the choices
    } StateNode;
    
    StateNode* states = malloc(10000000 * sizeof(StateNode));
    if (!states) {
        fprintf(stderr, "Memory allocation failed\n");
        return 0;
    }
    
    int state_count = 1;
    memcpy(states[0].state, initial_state, 32);
    states[0].parent_idx = -1;
    states[0].round = 0;
    
    // BFS through all 256 rounds
    int current_start = 0;
    int current_end = 1;
    
    for (int round = 0; round < 256; ++round) {
        int next_end = current_end;
        
        printf("Round %d: Processing %d states...\n", round, current_end - current_start);
        
        for (int idx = current_start; idx < current_end; ++idx) {
            u8* curr_state = states[idx].state;
            u8 v[32];
            
            // Compute M^-1 * c
            int valid_state = 1;
            for (int j = 0; j < 32; ++j) {
                v[j] = dot_row(invM[j], curr_state);
                if (inv_low_count[v[j]] == 0) {
                    valid_state = 0;
                    break;
                }
            }
            
            if (!valid_state) continue;
            
            // Calculate total combinations
            long long total_combinations = 1;
            int choices_per_pos[32];
            for (int j = 0; j < 32; ++j) {
                choices_per_pos[j] = inv_low_count[v[j]];
                total_combinations *= choices_per_pos[j];
                if (total_combinations > 100000) {
                    total_combinations = 100000;  // Cap for memory
                    break;
                }
            }
            
            // Generate all combinations
            for (int combo = 0; combo < total_combinations && state_count < 10000000; ++combo) {
                int temp_combo = combo;
                u8 new_state[32];
                u8 choices[32];
                
                // Convert combo to choices
                for (int j = 0; j < 32; ++j) {
                    int choice_idx = temp_combo % choices_per_pos[j];
                    temp_combo /= choices_per_pos[j];
                    new_state[j] = inv_low[v[j]][choice_idx];
                    choices[j] = choice_idx;
                }
                
                // Add new state
                memcpy(states[next_end].state, new_state, 32);
                states[next_end].parent_idx = idx;
                states[next_end].round = round + 1;
                memcpy(states[next_end].choice_made, choices, 32);
                memcpy(states[next_end].v_vector, v, 32);
                next_end++;
                
                if (next_end >= 10000000) {
                    printf("State limit reached\n");
                    break;
                }
            }
        }
        
        current_start = current_end;
        current_end = next_end;
        
        if (current_start == current_end) {
            printf("No valid states at round %d\n", round);
            break;
        }
    }
    
    // Extract all complete paths
    printf("\nExtracting complete paths...\n");
    paths_found = 0;
    
    for (int idx = current_start; idx < current_end; ++idx) {
        if (states[idx].round == 256) {
            // Found a complete path - reconstruct it
            if (paths_found >= paths_capacity) {
                printf("Path capacity reached\n");
                break;
            }
            
            // Trace back and record the path
            int curr_idx = idx;
            Path* path = &all_paths[paths_found];
            
            // Store v-vectors and choices for proper analysis
            typedef struct {
                u8 v_vector[32];
                u8 choices[32];
            } RoundData;
            
            RoundData round_data[256];
            int rounds_recorded = 0;
            
            while (curr_idx >= 0 && states[curr_idx].round > 0) {
                int round = states[curr_idx].round - 1;
                memcpy(path->choices[round], states[curr_idx].choice_made, 32);
                
                // Store v-vector and choices for this round
                if (rounds_recorded < 256) {
                    memcpy(round_data[round].v_vector, states[curr_idx].v_vector, 32);
                    memcpy(round_data[round].choices, states[curr_idx].choice_made, 32);
                    rounds_recorded++;
                }
                
                curr_idx = states[curr_idx].parent_idx;
            }
            
            // Now update choice statistics properly
            int multi_outputs[] = {0x02, 0x1c, 0x2d, 0x33, 0x49, 0x57, 0x66, 0x78, 
                                  0x8d, 0x93, 0xa2, 0xbc, 0xc6, 0xd8, 0xe9, 0xf7};
            
            for (int round = 0; round < 256; round++) {
                for (int j = 0; j < 32; j++) {
                    u8 v_val = round_data[round].v_vector[j];
                    u8 choice = round_data[round].choices[j];
                    
                    // Check if this v_val is a multi-choice output
                    for (int i = 0; i < 16; i++) {
                        if (v_val == multi_outputs[i] && inv_low_count[v_val] > 1) {
                            choice_stats[v_val].total_occurrences++;
                            if (choice == 0) {
                                choice_stats[v_val].choice_0_count++;
                            } else {
                                choice_stats[v_val].choice_1_count++;
                            }
                            break;
                        }
                    }
                }
            }
            
            paths_found++;
        }
    }
    
    free(states);
    return paths_found;
}

// Analyze all paths for choice statistics
static void analyze_all_paths() {
    printf("\n=== Analyzing %d paths ===\n", paths_found);
    
    // Clear statistics (they were already updated during path extraction)
    // The statistics are now properly computed in find_all_paths_bfs()
    
    total_paths_analyzed = paths_found;
}

// Print comprehensive statistics
static void print_statistics() {
    printf("\n=== COMPREHENSIVE CHOICE STATISTICS ===\n");
    printf("Based on %d complete winning paths\n\n", total_paths_analyzed);
    
    // Multi-choice outputs
    int multi_outputs[] = {0x02, 0x1c, 0x2d, 0x33, 0x49, 0x57, 0x66, 0x78, 
                          0x8d, 0x93, 0xa2, 0xbc, 0xc6, 0xd8, 0xe9, 0xf7};
    u8 choices[][2] = {
        {0x18, 0x5e}, {0x53, 0x71}, {0x84, 0xdc}, {0x06, 0x9a},
        {0x4d, 0xd1}, {0xb8, 0xcf}, {0x29, 0xb5}, {0x15, 0x97},
        {0x89, 0xab}, {0xa6, 0xe0}, {0x3a, 0xf3}, {0x0b, 0xc2},
        {0x37, 0x40}, {0x6f, 0xed}, {0x24, 0x7c}, {0x62, 0xfe}
    };
    
    for (int i = 0; i < 16; i++) {
        int output = multi_outputs[i];
        ChoiceStats* stats = &choice_stats[output];
        
        if (stats->total_occurrences > 0) {
            double choice0_pct = (double)stats->choice_0_count / stats->total_occurrences * 100;
            double choice1_pct = (double)stats->choice_1_count / stats->total_occurrences * 100;
            
            printf("Output 0x%02x:\n", output);
            printf("  Total occurrences across all paths: %d\n", stats->total_occurrences);
            printf("  Choice 0x%02x (idx 0): %d times (%.1f%%)\n", 
                   choices[i][0], stats->choice_0_count, choice0_pct);
            printf("  Choice 0x%02x (idx 1): %d times (%.1f%%)\n", 
                   choices[i][1], stats->choice_1_count, choice1_pct);
            
            if (fabs(choice0_pct - choice1_pct) > 5.0) {
                printf("  → STRONG PREFERENCE for 0x%02x\n", 
                       choice0_pct > choice1_pct ? choices[i][0] : choices[i][1]);
            } else {
                printf("  → No significant preference\n");
            }
            printf("\n");
        }
    }
    
    // Overall statistics
    int total_multi_choices = 0;
    int total_choice0 = 0;
    int total_choice1 = 0;
    
    for (int i = 0; i < 256; i++) {
        total_multi_choices += choice_stats[i].total_occurrences;
        total_choice0 += choice_stats[i].choice_0_count;
        total_choice1 += choice_stats[i].choice_1_count;
    }
    
    if (total_multi_choices > 0) {
        printf("=== OVERALL STATISTICS ===\n");
        printf("Total multi-choice decisions: %d\n", total_multi_choices);
        printf("Choice index 0: %d times (%.1f%%)\n", 
               total_choice0, (double)total_choice0 / total_multi_choices * 100);
        printf("Choice index 1: %d times (%.1f%%)\n", 
               total_choice1, (double)total_choice1 / total_multi_choices * 100);
    }
}

int main() {
    printf("=== Analyzing ALL Winning Paths ===\n");
    
    srand(42);  // Fixed seed for reproducibility
    precompute();
    
    // Allocate memory for paths
    all_paths = malloc(paths_capacity * sizeof(Path));
    if (!all_paths) {
        fprintf(stderr, "Failed to allocate memory for paths\n");
        return 1;
    }
    
    // Create a specific final state
    u8 final_state[32];
    // Simple final state for testing
    for (int i = 0; i < 32; i++) {
        final_state[i] = i;
    }
    
    printf("Finding all paths for the given final state...\n");
    double start = get_time_ms();
    
    int num_paths = find_all_paths_bfs(final_state);
    
    double elapsed = get_time_ms() - start;
    printf("Found %d complete paths in %.2f ms\n", num_paths, elapsed);
    
    if (num_paths > 0) {
        analyze_all_paths();
        print_statistics();
    } else {
        printf("No complete paths found!\n");
    }
    
    free(all_paths);
    return 0;
}