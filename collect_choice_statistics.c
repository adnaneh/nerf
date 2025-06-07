// Simple data collection script to analyze DFS choice patterns
// Compile with: gcc -O2 collect_choice_statistics.c -o collect_stats

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Copy the relevant parts from hireme2.c for analysis
typedef unsigned char u8;
typedef unsigned int u32;

// Confusion matrix (first 256 bytes = S_low)
static const u8 confusion[256] = {
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
    0x22,0xf4,0xb9,0xa2,0x6f,0x12,0x1b,0x14,0x45,0xc7,0x87,0x31,0x60,0x29,0xf7,0x73
};

// Global data for analysis
static FILE* choice_log = NULL;
static int total_choices_made = 0;
static int successful_runs = 0;

// Multi-choice outputs and their pre-images
typedef struct {
    u8 output;
    u8 choice1, choice2;
} ChoicePoint;

static const ChoicePoint choice_points[] = {
    {0x02, 0x18, 0x5e}, {0x1c, 0x53, 0x71}, {0x2d, 0x84, 0xdc}, {0x33, 0x06, 0x9a},
    {0x49, 0x4d, 0xd1}, {0x57, 0xb8, 0xcf}, {0x66, 0x29, 0xb5}, {0x78, 0x15, 0x97},
    {0x8d, 0x89, 0xab}, {0x93, 0xa6, 0xe0}, {0xa2, 0x3a, 0xf3}, {0xbc, 0x0b, 0xc2},
    {0xc6, 0x37, 0x40}, {0xd8, 0x6f, 0xed}, {0xe9, 0x24, 0x7c}, {0xf7, 0x62, 0xfe}
};

#define NUM_CHOICE_POINTS (sizeof(choice_points) / sizeof(choice_points[0]))

void log_choice(int run_number, u8 output, u8 choice_made, int choice_index, int success) {
    if (!choice_log) {
        choice_log = fopen("choice_statistics.csv", "w");
        fprintf(choice_log, "run,output,choice_made,choice_index,success\n");
    }
    
    fprintf(choice_log, "%d,0x%02x,0x%02x,%d,%d\n", 
            run_number, output, choice_made, choice_index, success);
    total_choices_made++;
}

// Simulate different choice strategies and measure success rates
int test_strategy_deterministic_first(int run_number) {
    // Always choose first option
    for (int i = 0; i < NUM_CHOICE_POINTS; i++) {
        log_choice(run_number, choice_points[i].output, choice_points[i].choice1, 0, 1);
    }
    return 1; // Assume success for simulation
}

int test_strategy_deterministic_second(int run_number) {
    // Always choose second option
    for (int i = 0; i < NUM_CHOICE_POINTS; i++) {
        log_choice(run_number, choice_points[i].output, choice_points[i].choice2, 1, 1);
    }
    return 1; // Assume success for simulation
}

int test_strategy_random(int run_number) {
    // Random choice
    for (int i = 0; i < NUM_CHOICE_POINTS; i++) {
        int choice_idx = rand() % 2;
        u8 choice = (choice_idx == 0) ? choice_points[i].choice1 : choice_points[i].choice2;
        log_choice(run_number, choice_points[i].output, choice, choice_idx, 1);
    }
    return 1; // Assume success for simulation
}

int test_strategy_prefer_even(int run_number) {
    // Prefer even values
    for (int i = 0; i < NUM_CHOICE_POINTS; i++) {
        u8 choice1 = choice_points[i].choice1;
        u8 choice2 = choice_points[i].choice2;
        
        if ((choice1 % 2) == 0 && (choice2 % 2) != 0) {
            log_choice(run_number, choice_points[i].output, choice1, 0, 1);
        } else if ((choice1 % 2) != 0 && (choice2 % 2) == 0) {
            log_choice(run_number, choice_points[i].output, choice2, 1, 1);
        } else {
            // Both even or both odd, choose first
            log_choice(run_number, choice_points[i].output, choice1, 0, 1);
        }
    }
    return 1;
}

int test_strategy_prefer_smaller(int run_number) {
    // Prefer smaller values
    for (int i = 0; i < NUM_CHOICE_POINTS; i++) {
        u8 choice1 = choice_points[i].choice1;
        u8 choice2 = choice_points[i].choice2;
        
        if (choice1 < choice2) {
            log_choice(run_number, choice_points[i].output, choice1, 0, 1);
        } else {
            log_choice(run_number, choice_points[i].output, choice2, 1, 1);
        }
    }
    return 1;
}

void analyze_choice_patterns() {
    printf("=== Choice Pattern Analysis ===\n\n");
    
    printf("Multi-choice outputs and their characteristics:\n");
    for (int i = 0; i < NUM_CHOICE_POINTS; i++) {
        u8 output = choice_points[i].output;
        u8 c1 = choice_points[i].choice1;
        u8 c2 = choice_points[i].choice2;
        
        // Map forward to see what they become
        u8 future1 = confusion[c1];
        u8 future2 = confusion[c2];
        
        printf("Output 0x%02x: choices 0x%02x→0x%02x, 0x%02x→0x%02x\n",
               output, c1, future1, c2, future2);
        printf("  Difference: %d, XOR: 0x%02x, Parity: %d,%d\n",
               abs(c1 - c2), c1 ^ c2, c1 % 2, c2 % 2);
        printf("  Bit patterns: %08d, %08d\n", 
               (int)strtol("dummy", NULL, 2), (int)strtol("dummy", NULL, 2)); // Placeholder
    }
    
    printf("\nPattern observations:\n");
    printf("- All choice points lead to other choice points (symmetric)\n");
    printf("- No clear mathematical preference visible\n");
    printf("- Need empirical data from actual solution runs\n");
}

void run_strategy_comparison() {
    printf("\n=== Strategy Comparison Simulation ===\n");
    
    srand(time(NULL));
    
    const int NUM_RUNS = 100;
    
    printf("Simulating %d runs for each strategy...\n", NUM_RUNS);
    
    // Test different strategies
    printf("Testing deterministic-first strategy...\n");
    for (int i = 0; i < NUM_RUNS; i++) {
        if (test_strategy_deterministic_first(i)) successful_runs++;
    }
    
    printf("Testing deterministic-second strategy...\n");
    for (int i = NUM_RUNS; i < 2*NUM_RUNS; i++) {
        if (test_strategy_deterministic_second(i)) successful_runs++;
    }
    
    printf("Testing random strategy...\n");
    for (int i = 2*NUM_RUNS; i < 3*NUM_RUNS; i++) {
        if (test_strategy_random(i)) successful_runs++;
    }
    
    printf("Testing prefer-even strategy...\n");
    for (int i = 3*NUM_RUNS; i < 4*NUM_RUNS; i++) {
        if (test_strategy_prefer_even(i)) successful_runs++;
    }
    
    printf("Testing prefer-smaller strategy...\n");
    for (int i = 4*NUM_RUNS; i < 5*NUM_RUNS; i++) {
        if (test_strategy_prefer_smaller(i)) successful_runs++;
    }
    
    printf("Generated %d choice records for analysis\n", total_choices_made);
    printf("Strategy comparison data saved to choice_statistics.csv\n");
}

void suggest_real_data_collection() {
    printf("\n=== Real Data Collection Instructions ===\n\n");
    
    printf("To collect actual solution data from hireme2:\n\n");
    
    printf("1. MODIFY hireme2.c to add this logging in dfs_recursive:\n");
    printf("   ```c\n");
    printf("   // Add before the choice loop\n");
    printf("   static FILE* real_log = NULL;\n");
    printf("   if (!real_log) {\n");
    printf("       real_log = fopen(\"real_choices.csv\", \"w\");\n");
    printf("       fprintf(real_log, \"round,pos,output,choice,success\\n\");\n");
    printf("   }\n");
    printf("   \n");
    printf("   // In the combination loop, log each choice:\n");
    printf("   for (int j = 0; j < 32; ++j) {\n");
    printf("       if (choices_per_pos[j] > 1) {\n");
    printf("           int choice_idx = temp_combo %% choices_per_pos[j];\n");
    printf("           fprintf(real_log, \"%%d,%%d,0x%%02x,%%d,\", round, j, v[j], choice_idx);\n");
    printf("       }\n");
    printf("   }\n");
    printf("   \n");
    printf("   // After recursive call:\n");
    printf("   fprintf(real_log, \"%%d\\n\", result);\n");
    printf("   ```\n\n");
    
    printf("2. COMPILE and RUN multiple times:\n");
    printf("   ```bash\n");
    printf("   gcc -O2 hireme2_instrumented.c -o hireme2_instrumented\n");
    printf("   for i in {1..1000}; do ./hireme2_instrumented; done\n");
    printf("   ```\n\n");
    
    printf("3. ANALYZE the real_choices.csv file:\n");
    printf("   - Count success rates for choice 0 vs choice 1 for each output\n");
    printf("   - Look for round-dependent patterns\n");
    printf("   - Check position-dependent preferences\n");
    printf("   - Identify early vs late round differences\n\n");
    
    printf("4. BUILD the optimized heuristic:\n");
    printf("   Use the statistical data to create a lookup table or function\n");
    printf("   that picks the choice with highest empirical success rate.\n\n");
}

int main() {
    printf("=== DFS Choice Pattern Analysis Tool ===\n");
    
    analyze_choice_patterns();
    run_strategy_comparison();
    suggest_real_data_collection();
    
    if (choice_log) {
        fclose(choice_log);
    }
    
    printf("\n=== Next Steps ===\n");
    printf("1. Use the real data collection method described above\n");
    printf("2. Analyze choice_statistics.csv to find patterns\n");
    printf("3. Implement the best-performing strategy in hireme2.c\n");
    printf("4. Measure the performance improvement\n");
    
    return 0;
}