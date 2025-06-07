#!/usr/bin/env python3
import subprocess
import re
import sys

n = int(sys.argv[1]) if len(sys.argv) > 1 else 10

print('Calculating improvement...')

# Run without heuristic
times_no = []
nodes_no = []
for i in range(n):
    result = subprocess.run(['./hireme2_safe_optimized_no_heuristic_profile'], capture_output=True, text=True)
    time_match = re.search(r'Optimized DFS: Found valid solution in ([\d.]+) ms', result.stdout)
    node_match = re.search(r'DFS nodes visited: (\d+)', result.stdout)
    if time_match and node_match:
        times_no.append(float(time_match.group(1)))
        nodes_no.append(int(node_match.group(1)))

# Run with heuristic
times_with = []
nodes_with = []
for i in range(n):
    result = subprocess.run(['./hireme2_safe_optimized_profile'], capture_output=True, text=True)
    time_match = re.search(r'Optimized DFS: Found valid solution in ([\d.]+) ms', result.stdout)
    node_match = re.search(r'DFS nodes visited: (\d+)', result.stdout)
    if time_match and node_match:
        times_with.append(float(time_match.group(1)))
        nodes_with.append(int(node_match.group(1)))

if times_no and nodes_no and times_with and nodes_with:
    avg_time_no = sum(times_no) / len(times_no)
    avg_nodes_no = sum(nodes_no) / len(nodes_no)
    avg_time_with = sum(times_with) / len(times_with)
    avg_nodes_with = sum(nodes_with) / len(nodes_with)
    
    if avg_time_no > 0:
        time_improvement = ((avg_time_no - avg_time_with) / avg_time_no) * 100
    else:
        time_improvement = 0
        
    if avg_nodes_no > 0:
        node_improvement = ((avg_nodes_no - avg_nodes_with) / avg_nodes_no) * 100
    else:
        node_improvement = 0
    
    print(f'Average nodes visited:')
    print(f'  Without heuristic: {avg_nodes_no:,.0f}')
    print(f'  With heuristic: {avg_nodes_with:,.0f}')
    print(f'  Improvement: {node_improvement:.1f}% {"fewer" if node_improvement > 0 else "more"} nodes')
    print(f'')
    print(f'Average time:')
    print(f'  Without heuristic: {avg_time_no:.2f} ms')
    print(f'  With heuristic: {avg_time_with:.2f} ms')
    print(f'  Improvement: {time_improvement:.1f}% {"faster" if time_improvement > 0 else "slower"}')
else:
    print('Failed to extract metrics')