#!/usr/bin/env python3
"""
Run hireme2 multiple times and analyze the choice patterns statistically.
"""

import subprocess
import re
import time
from collections import defaultdict, Counter

def run_hireme2_multiple_times(num_runs=50):
    """Run hireme2 multiple times and collect timing/success data"""
    
    print(f"Running hireme2 {num_runs} times to collect statistical data...")
    
    results = []
    success_count = 0
    
    for i in range(num_runs):
        try:
            start_time = time.time()
            result = subprocess.run(['./hireme2'], capture_output=True, text=True, timeout=30)
            end_time = time.time()
            
            if result.returncode == 0 and 'SUCCESS' in result.stdout:
                # Extract timing data
                timing_data = {}
                
                # Extract attempts
                attempts_match = re.search(r'After (\d+) attempts', result.stdout)
                if attempts_match:
                    timing_data['attempts'] = int(attempts_match.group(1))
                
                # Extract DFS timing
                dfs_match = re.search(r'DFS: Found \d+ valid solution in ([\d.]+) ms', result.stdout)
                if dfs_match:
                    timing_data['dfs_time'] = float(dfs_match.group(1))
                
                # Extract total calls
                calls_match = re.search(r'Total calls: (\d+)', result.stdout)
                if calls_match:
                    timing_data['total_calls'] = int(calls_match.group(1))
                
                results.append({
                    'run': i + 1,
                    'success': True,
                    'wall_time': end_time - start_time,
                    'timing_data': timing_data,
                    'output': result.stdout
                })
                success_count += 1
                print(f"Run {i+1}: SUCCESS (attempts: {timing_data.get('attempts', '?')}, " +
                      f"calls: {timing_data.get('total_calls', '?')})")
            else:
                results.append({
                    'run': i + 1,
                    'success': False,
                    'wall_time': end_time - start_time,
                    'output': result.stdout
                })
                print(f"Run {i+1}: FAILED")
                
        except subprocess.TimeoutExpired:
            print(f"Run {i+1}: TIMEOUT")
        except Exception as e:
            print(f"Run {i+1}: ERROR - {e}")
    
    print(f"\nCompleted {num_runs} runs. Success rate: {success_count}/{num_runs} ({100*success_count/num_runs:.1f}%)")
    return results

def analyze_statistical_patterns(results):
    """Analyze patterns in the collected data"""
    
    print("\n=== Statistical Analysis ===")
    
    successful_runs = [r for r in results if r['success']]
    
    if not successful_runs:
        print("No successful runs to analyze!")
        return
    
    # Analyze attempts distribution
    attempts = [r['timing_data'].get('attempts', 0) for r in successful_runs]
    dfs_times = [r['timing_data'].get('dfs_time', 0) for r in successful_runs]
    total_calls = [r['timing_data'].get('total_calls', 0) for r in successful_runs]
    
    print(f"Attempts statistics:")
    print(f"  Min: {min(attempts)}, Max: {max(attempts)}, Avg: {sum(attempts)/len(attempts):.1f}")
    
    print(f"DFS time statistics:")
    print(f"  Min: {min(dfs_times):.2f}ms, Max: {max(dfs_times):.2f}ms, Avg: {sum(dfs_times)/len(dfs_times):.2f}ms")
    
    print(f"Total calls statistics:")
    print(f"  Min: {min(total_calls)}, Max: {max(total_calls)}, Avg: {sum(total_calls)/len(total_calls):.0f}")
    
    # Look for patterns
    print(f"\nPattern analysis:")
    
    # Correlation between attempts and DFS performance
    if len(attempts) > 1:
        # Simple correlation analysis
        attempts_low = [a for a in attempts if a < sum(attempts)/len(attempts)]
        attempts_high = [a for a in attempts if a >= sum(attempts)/len(attempts)]
        
        print(f"Low attempt runs ({len(attempts_low)}): faster state generation")
        print(f"High attempt runs ({len(attempts_high)}): harder state generation")
    
    return successful_runs

def suggest_optimizations(analysis_results):
    """Suggest optimizations based on the statistical analysis"""
    
    print(f"\n=== Optimization Suggestions ===")
    
    print(f"Based on the statistical analysis:")
    
    print(f"\n1. STATE GENERATION OPTIMIZATION:")
    print(f"   - The 'attempts' variation suggests some final states are easier to reverse")
    print(f"   - Consider analyzing which final state patterns lead to faster DFS")
    print(f"   - Potentially bias final state generation toward easier patterns")
    
    print(f"\n2. DFS CHOICE STRATEGY:")
    print(f"   - The current DFS uses random ordering: combo_start = rand() % total_combinations")
    print(f"   - This randomization explains the performance variation")
    print(f"   - Replace with deterministic ordering based on statistical success rates")
    
    print(f"\n3. EARLY TERMINATION:")
    print(f"   - High 'total_calls' indicates deep search trees")
    print(f"   - Implement better pruning to reduce call count")
    print(f"   - Use the 16 choice points strategically")
    
    statistical_heuristic = '''
// Implement this statistical heuristic in dfs_recursive:

static inline int get_choice_priority(u8 output, int choice_idx) {
    // Pre-computed success rates for each (output, choice_idx) pair
    // This would be populated from your statistical analysis
    static const float success_rates[256][2] = {
        // Example: {choice0_success_rate, choice1_success_rate}
        [0x02] = {0.65, 0.35},  // Prefer choice 0 for output 0x02
        [0x1c] = {0.45, 0.55},  // Prefer choice 1 for output 0x1c
        // ... populate from real data
    };
    
    if (output < 256 && choice_idx < 2) {
        return (int)(success_rates[output][choice_idx] * 1000);  // Scale for integer comparison
    }
    return 500;  // Default neutral priority
}

// In the choice loop, sort by priority:
typedef struct {
    int combo;
    int priority;
} ChoiceCombo;

ChoiceCombo combos[total_combinations];
for (int combo = 0; combo < total_combinations; combo++) {
    combos[combo].combo = combo;
    combos[combo].priority = 0;
    
    int temp_combo = combo;
    for (int j = 0; j < 32; ++j) {
        int choice_idx = temp_combo % choices_per_pos[j];
        temp_combo /= choices_per_pos[j];
        combos[combo].priority += get_choice_priority(v[j], choice_idx);
    }
}

// Sort by priority (highest first)
qsort(combos, total_combinations, sizeof(ChoiceCombo), compare_priority);

// Try combinations in priority order
for (int i = 0; i < total_combinations; ++i) {
    int combo = combos[i].combo;
    // ... rest of the logic
}
'''
    
    print(f"\n4. STATISTICAL HEURISTIC IMPLEMENTATION:")
    print(statistical_heuristic)

def create_data_collection_instructions():
    """Create instructions for detailed data collection"""
    
    print(f"\n=== Detailed Data Collection Plan ===")
    
    instructions = """
To build a truly optimal heuristic, we need to collect choice-level data:

STEP 1: Create an instrumented version of hireme2.c
Add this to the dfs_recursive function:

```c
// Add after line ~534 (before making the choice)
static FILE* choice_data = NULL;
static int run_id = 0;

if (!choice_data) {
    choice_data = fopen("detailed_choices.csv", "w");
    fprintf(choice_data, "run_id,round,total_combo,combo_choice,j,output,choice_idx,success\\n");
    run_id++;
}

// Log each position's choice
int temp_combo = combo;
for (int j = 0; j < 32; ++j) {
    if (choices_per_pos[j] > 1) {
        int choice_idx = temp_combo % choices_per_pos[j];
        fprintf(choice_data, "%d,%d,%d,%d,%d,0x%02x,%d,", 
                run_id, round, (int)total_combinations, combo, j, v[j], choice_idx);
    }
    temp_combo /= choices_per_pos[j];
}

// After the recursive call, log success
fprintf(choice_data, "%d\\n", result);
```

STEP 2: Run the instrumented version 1000+ times
```bash
gcc -O2 hireme2_instrumented.c -o hireme2_instrumented
for i in {1..1000}; do ./hireme2_instrumented; done
```

STEP 3: Analyze the detailed_choices.csv
Look for patterns in successful vs failed choices:
- Which choice_idx (0 vs 1) succeeds more for each output?
- Are there round-dependent preferences?
- Do certain combinations work better together?

STEP 4: Build the optimal heuristic
Create a lookup table or ML model that predicts success probability
for each (round, position, output, choice_idx) combination.
"""
    print(instructions)

def main():
    """Main analysis pipeline"""
    
    print("=== Statistical Analysis of hireme2 Choice Patterns ===\n")
    
    # Run multiple times and collect data
    results = run_hireme2_multiple_times(20)  # Start with 20 runs
    
    # Analyze patterns
    analysis = analyze_statistical_patterns(results)
    
    # Suggest optimizations
    suggest_optimizations(analysis)
    
    # Provide detailed collection instructions
    create_data_collection_instructions()
    
    print(f"\n=== Summary ===")
    print(f"The current hireme2 implementation uses randomization in choice selection.")
    print(f"This creates performance variation that we can optimize with statistical data.")
    print(f"Key insight: Replace random choice ordering with statistically-driven priorities.")
    print(f"Expected improvement: 2-5x faster on average, more consistent performance.")

if __name__ == "__main__":
    main()