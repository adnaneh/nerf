#!/usr/bin/env python3
import subprocess
import re
import sys

n = int(sys.argv[1]) if len(sys.argv) > 1 else 10
binary = sys.argv[2] if len(sys.argv) > 2 else './hireme2_safe_optimized_profile'

times = []
nodes = []

print(f'Running {n} iterations...')

for i in range(n):
    result = subprocess.run([binary], capture_output=True, text=True)
    
    time_match = re.search(r'Optimized DFS: Found valid solution in ([\d.]+) ms', result.stdout)
    node_match = re.search(r'DFS nodes visited: (\d+)', result.stdout)
    
    if time_match and node_match:
        times.append(float(time_match.group(1)))
        nodes.append(int(node_match.group(1)))
    
    if (i+1) % max(1, n//10) == 0:
        print(f'  Progress: {i+1}/{n}')

if times and nodes:
    avg_time = sum(times) / len(times)
    avg_nodes = sum(nodes) / len(nodes)
    
    print(f'\nResults:')
    print(f'  Average time: {avg_time:.2f} ms')
    print(f'  Average DFS nodes visited: {avg_nodes:,.0f}')
    print(f'  Min/Max time: {min(times):.2f} / {max(times):.2f} ms')
    print(f'  Min/Max nodes: {min(nodes):,} / {max(nodes):,}')
else:
    print('Failed to extract metrics')