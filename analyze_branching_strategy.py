#!/usr/bin/env python3
"""
Analysis of branching strategies for s_low inverse mapping.
Helps understand which choices minimize branches during reversal.
"""

from collections import defaultdict, Counter
import itertools

# s_low confusion table (first 256 values)
s_low = [
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
    0x22,0xf4,0xb9,0xa2,0x6f,0x12,0x1b,0x14,0x45,0xc7,0x87,0x31,0x60,0xfd,0xf7,0x73
]

def analyze_branching_strategies():
    """Analyze different strategies for minimizing branches during s_low inversion"""
    
    print("=== S_LOW BRANCHING STRATEGY ANALYSIS ===\n")
    
    # Build inverse mapping
    inv_s_low = defaultdict(list)
    for x in range(256):
        inv_s_low[s_low[x]].append(x)
    
    # 1. Identify branching points
    print("1. BRANCHING POINTS ANALYSIS")
    print("-" * 50)
    
    single_choice = []
    double_choice = []
    no_choice = []
    
    for y in range(256):
        preimages = inv_s_low[y]
        if len(preimages) == 0:
            no_choice.append(y)
        elif len(preimages) == 1:
            single_choice.append(y)
        elif len(preimages) == 2:
            double_choice.append((y, preimages))
    
    print(f"Values with no pre-image (dead ends): {len(no_choice)}")
    print(f"Values with 1 pre-image (no choice): {len(single_choice)}")
    print(f"Values with 2 pre-images (branching): {len(double_choice)}")
    print(f"Branching factor per position: {len(double_choice)/256:.3f}")
    
    print("\nBranching points (output -> [input1, input2]):")
    for y, preimages in double_choice:
        print(f"  {hex(y)} -> [{hex(preimages[0])}, {hex(preimages[1])}]")
    
    # 2. Analyze choice patterns
    print("\n\n2. CHOICE PATTERN ANALYSIS")
    print("-" * 50)
    
    # Look for patterns in the pre-image pairs
    print("Pre-image pair analysis:")
    for y, preimages in double_choice:
        x1, x2 = preimages
        diff = x1 ^ x2
        print(f"  {hex(y)}: {hex(x1)} vs {hex(x2)} (diff: {hex(diff)}, distance: {abs(x1-x2)})")
    
    # Check if there are patterns in the differences
    differences = [abs(p[0] - p[1]) for _, p in double_choice]
    xor_diffs = [p[0] ^ p[1] for _, p in double_choice]
    
    print(f"\nDifference statistics:")
    print(f"  Average distance: {sum(differences)/len(differences):.1f}")
    print(f"  Min distance: {min(differences)}")
    print(f"  Max distance: {max(differences)}")
    print(f"  Common XOR differences: {Counter(xor_diffs).most_common(5)}")
    
    # 3. Simulate branching explosion
    print("\n\n3. BRANCHING EXPLOSION SIMULATION")
    print("-" * 50)
    
    def count_total_branches(state_length):
        """Count total possible branches for a state of given length"""
        total = 1
        for pos in range(state_length):
            # For each position, we might encounter a branching value
            # Probability of branching = len(double_choice) / 256
            # When we branch, we get 2 choices
            branch_prob = len(double_choice) / 256
            expected_choices = 1 + branch_prob  # 1 if no branch, 2 if branch
            total *= expected_choices
        return total
    
    print("Expected branching explosion by state length:")
    for length in [4, 8, 16, 32]:
        branches = count_total_branches(length)
        print(f"  State length {length:2d}: {branches:.2e} branches")
    
    # 4. Optimal choice strategies
    print("\n\n4. OPTIMAL CHOICE STRATEGIES")
    print("-" * 50)
    
    # Strategy 1: Choose the smaller input value
    print("Strategy 1: Always choose smaller input value")
    smaller_choices = []
    for y, preimages in double_choice:
        smaller = min(preimages)
        smaller_choices.append(smaller)
    print(f"  Would choose: {[hex(x) for x in smaller_choices[:10]]}...")
    
    # Strategy 2: Choose based on bit patterns
    print("\nStrategy 2: Choose based on bit patterns")
    even_choices = []
    odd_choices = []
    for y, preimages in double_choice:
        for x in preimages:
            if x % 2 == 0:
                even_choices.append(x)
            else:
                odd_choices.append(x)
    
    print(f"  Even pre-images: {len(even_choices)} ({len(even_choices)/(len(even_choices)+len(odd_choices)):.2%})")
    print(f"  Odd pre-images: {len(odd_choices)} ({len(odd_choices)/(len(even_choices)+len(odd_choices)):.2%})")
    
    # Strategy 3: Choose based on position in alphabet
    print("\nStrategy 3: Frequency-based choice")
    # Count how often each input appears in the forward mapping
    input_freq = Counter(range(256))
    choice_freq = {}
    for y, preimages in double_choice:
        for x in preimages:
            choice_freq[x] = choice_freq.get(x, 0) + 1
    
    print("Most frequent branching inputs:")
    for x, freq in sorted(choice_freq.items(), key=lambda item: item[1], reverse=True)[:5]:
        print(f"  {hex(x)}: appears in {freq} branching decisions")
    
    # 5. Practical recommendations
    print("\n\n5. PRACTICAL RECOMMENDATIONS")
    print("-" * 50)
    
    print("For minimizing branches during reversal:")
    print()
    print("1. EARLY TERMINATION:")
    print("   - Stop immediately when encountering missing values")
    print(f"   - Missing values: {[hex(x) for x in sorted(no_choice)]}")
    print()
    print("2. DETERMINISTIC CHOICE STRATEGY:")
    print("   - Use consistent rule for 2-choice situations")
    print("   - Recommended: Always choose the smaller input value")
    print("   - This gives deterministic behavior and is simple to implement")
    print()
    print("3. PRUNING HEURISTICS:")
    print("   - Track which inputs lead to dead ends in deeper levels")
    print("   - Use memoization to avoid re-exploring failed branches")
    print()
    print("4. BRANCH ORDERING:")
    print("   - Try 'smaller' input values first (they may be more constrained)")
    print("   - Or use domain knowledge about which inputs are more likely")
    
    # 6. Code generation for optimal strategy
    print("\n\n6. GENERATED LOOKUP TABLES")
    print("-" * 50)
    
    print("C code for optimal choice strategy:")
    print()
    print("// Pre-computed optimal choices for branching values")
    print("static const u8 optimal_choice[256] = {")
    
    choices = [0] * 256
    for y, preimages in double_choice:
        # Choose the smaller input value as the optimal choice
        choices[y] = min(preimages)
    
    for i in range(0, 256, 16):
        row = ", ".join(f"0x{choices[i+j]:02x}" for j in range(16))
        print(f"    {row},")
    print("};")
    print()
    print("// Usage in inversion:")
    print("u8 choose_preimage(u8 output_value) {")
    print("    return optimal_choice[output_value];")
    print("}")
    
    # 7. Probability analysis
    print("\n\n7. PROBABILITY ANALYSIS")
    print("-" * 50)
    
    total_combinations_32 = 1
    for pos in range(32):
        # For a random 32-byte state, what's the probability of success?
        prob_no_missing = (256 - len(no_choice)) / 256
        total_combinations_32 *= prob_no_missing
    
    print(f"Probability that random 32-byte state has no missing values:")
    print(f"  P = ({256 - len(no_choice)}/256)^32 = {total_combinations_32:.2e}")
    print(f"  Expected attempts to find valid state: {1/total_combinations_32:.0f}")
    
    return inv_s_low, double_choice, no_choice

if __name__ == "__main__":
    inv_s_low, double_choice, no_choice = analyze_branching_strategies()