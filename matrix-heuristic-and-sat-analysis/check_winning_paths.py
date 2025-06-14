#!/usr/bin/env python3
import csv

# Read the v_analysis.csv file
with open('v_analysis.csv', 'r') as f:
    reader = csv.reader(f)
    header = next(reader)
    
    # Track successful runs
    successful_runs = set()
    run_success = {}
    
    for row in reader:
        if row[0] != 'CHOICE':  # Regular records
            run_id = int(row[0])
            round_num = int(row[1])
            success = int(row[3])
            
            if success == 1:
                successful_runs.add(run_id)
                if run_id not in run_success:
                    run_success[run_id] = []
                run_success[run_id].append(round_num)

print('=== Analysis Scope Check ===')
print()
print(f'Total successful runs captured: {len(successful_runs)}')
print(f'Successful run IDs: {sorted(successful_runs)[:10]}...' if len(successful_runs) > 10 else sorted(successful_runs))

# Count successful paths per run
successful_path_lengths = []
for run_id in sorted(successful_runs)[:5]:  # Show first 5
    if run_id in run_success:
        rounds = run_success[run_id]
        successful_path_lengths.append(len(rounds))
        print(f'  Run {run_id}: {len(rounds)} successful steps (rounds {min(rounds)}-{max(rounds)})')

# Check total runs attempted
all_runs = set()
with open('v_analysis.csv', 'r') as f:
    reader = csv.reader(f)
    next(reader)  # Skip header
    for row in reader:
        if row[0] != 'CHOICE':
            all_runs.add(int(row[0]))

print()
print(f'Total runs attempted: {len(all_runs)}')
print(f'Run IDs range: {min(all_runs)} to {max(all_runs)}')

# Count how many full paths (256 rounds) we captured
full_paths = 0
for run_id in successful_runs:
    if run_id in run_success and len(run_success[run_id]) == 256:
        full_paths += 1

print()
print(f'=== Key Finding ===')
print(f'Full winning paths captured (256 rounds): {full_paths}')
print(f'Total successful run IDs: {len(successful_runs)}')

# Analyze pattern - did we capture multiple independent solutions?
print()
print('=== Solution Diversity Check ===')

# Check if the runs are sequential or spread out
successful_list = sorted(successful_runs)
if len(successful_list) > 1:
    gaps = []
    for i in range(1, len(successful_list)):
        gap = successful_list[i] - successful_list[i-1]
        if gap > 1:
            gaps.append((successful_list[i-1], successful_list[i], gap))
    
    if gaps:
        print(f'Found {len(gaps)} gaps in successful run IDs, suggesting multiple independent attempts:')
        for start, end, gap in gaps[:5]:  # Show first 5 gaps
            print(f'  Gap: Run {start} to Run {end} (gap of {gap-1} failed attempts)')
    else:
        print('Successful runs are mostly consecutive, suggesting a single solution path explored multiple times')

# Final summary
print()
print('=== SUMMARY ===')
if full_paths == 1:
    print('The analysis appears to be based on a SINGLE winning path.')
elif full_paths > 1:
    print(f'The analysis is based on {full_paths} DIFFERENT winning paths!')
else:
    print('No complete winning paths found - only partial successes recorded.')