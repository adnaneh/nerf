#!/usr/bin/env python3
"""
Deep structural analysis to find patterns that lead directly to working states
or significantly increase the probability of finding valid solutions.
"""

import struct
from collections import defaultdict, Counter
import itertools

# s_low confusion table (first 256 values from hireme2.c)
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

# Diffusion matrix from hireme2.c
diffusion = [
    0xf26cb481,0x16a5dc92,0x3c5ba924,0x79b65248,0x2fc64b18,0x615acd29,0xc3b59a42,0x976b2584,
    0x6cf281b4,0xa51692dc,0x5b3c24a9,0xb6794852,0xc62f184b,0x5a6129cd,0xb5c3429a,0x6b978425,
    0xb481f26c,0xdc9216a5,0xa9243c5b,0x524879b6,0x4b182fc6,0xcd29615a,0x9a42c3b5,0x2584976b,
    0x81b46cf2,0x92dca516,0x24a95b3c,0x4852b679,0x184bc62f,0x29cd5a61,0x429ab5c3,0x84256b97
]

# Target from hireme2.c
target = b"Hire me!!!!!!!!\x00"

def analyze_structural_patterns():
    """Find structural patterns that could lead to high-probability working states"""
    
    print("=== STRUCTURAL PATTERN ANALYSIS FOR WORKING STATES ===\n")
    
    # Build inverse mappings
    inv_s_low = defaultdict(list)
    for x in range(256):
        inv_s_low[s_low[x]].append(x)
    
    missing_values = set()
    for y in range(256):
        if len(inv_s_low[y]) == 0:
            missing_values.add(y)
    
    print(f"Missing values: {sorted(missing_values)}")
    print(f"Missing count: {len(missing_values)}")
    
    # 1. Analyze the target structure
    print("\n1. TARGET STRUCTURE ANALYSIS")
    print("-" * 50)
    
    target_bytes = list(target[:16])  # Only first 16 bytes matter
    print(f"Target: {[hex(b) for b in target_bytes]}")
    print(f"Target ASCII: {target[:16]}")
    
    # Check what the target requires for the final XOR
    # out[i] = S_low(c[2i]) XOR S_high(c[2i+1])
    # So: S_low(c[2i]) = target[i] XOR S_high(c[2i+1])
    
    # Extract s_high from hireme2.c (confusion[256:512])
    s_high = [
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
    ]
    
    # Build inverse for s_high
    inv_s_high = defaultdict(list)
    for x in range(256):
        inv_s_high[s_high[x]].append(x)
    
    # 2. Find valid final state patterns
    print("\n2. VALID FINAL STATE CONSTRAINTS")
    print("-" * 50)
    
    valid_pairs_per_position = []
    for pos in range(16):
        target_byte = target_bytes[pos]
        valid_pairs = []
        
        # For each possible odd byte value
        for odd_val in range(256):
            s_high_output = s_high[odd_val]
            needed_s_low = target_byte ^ s_high_output
            
            # Check if this s_low value has pre-images
            if needed_s_low in inv_s_low and len(inv_s_low[needed_s_low]) > 0:
                for even_val in inv_s_low[needed_s_low]:
                    valid_pairs.append((even_val, odd_val))
        
        valid_pairs_per_position.append(valid_pairs)
        print(f"Position {pos:2d} (target=0x{target_byte:02x}): {len(valid_pairs)} valid (even,odd) pairs")
    
    # Find positions with fewest constraints (these might be easier to satisfy)
    min_pairs = min(len(pairs) for pairs in valid_pairs_per_position)
    max_pairs = max(len(pairs) for pairs in valid_pairs_per_position)
    print(f"\nConstraint range: {min_pairs} to {max_pairs} valid pairs per position")
    
    most_constrained = [i for i, pairs in enumerate(valid_pairs_per_position) if len(pairs) == min_pairs]
    print(f"Most constrained positions: {most_constrained}")
    
    # 3. Analyze diffusion matrix properties
    print("\n\n3. DIFFUSION MATRIX STRUCTURE ANALYSIS")
    print("-" * 50)
    
    def popcount(x):
        return bin(x).count('1')
    
    # Analyze the weight distribution of diffusion matrix rows
    weights = [popcount(row) for row in diffusion]
    print(f"Row weights: {weights}")
    print(f"Weight range: {min(weights)} to {max(weights)}")
    print(f"Average weight: {sum(weights)/len(weights):.1f}")
    
    # Look for sparse rows (these might give us more control)
    sparse_rows = [(i, w) for i, w in enumerate(weights) if w < 10]
    dense_rows = [(i, w) for i, w in enumerate(weights) if w > 20]
    print(f"Sparse rows (<10 bits): {sparse_rows}")
    print(f"Dense rows (>20 bits): {dense_rows}")
    
    # 4. Inverse diffusion analysis
    print("\n\n4. INVERSE DIFFUSION PATTERNS")
    print("-" * 50)
    
    # Simulate Gaussian elimination to understand inverse structure
    def invert_matrix_gf2(matrix):
        """Invert a 32x32 binary matrix using Gaussian elimination"""
        aug = []
        for r in range(32):
            # Augmented matrix: [A | I]
            row = matrix[r] | (1 << (32 + r))  # 64-bit: low 32 = A, high 32 = I
            aug.append(row)
        
        # Forward elimination
        for c in range(32):
            # Find pivot
            piv = c
            while piv < 32 and not (aug[piv] >> c & 1):
                piv += 1
            if piv == 32:
                return None  # Singular
            
            # Swap rows
            if piv != c:
                aug[c], aug[piv] = aug[piv], aug[c]
            
            # Eliminate
            for r in range(32):
                if r != c and (aug[r] >> c & 1):
                    aug[r] ^= aug[c]
        
        # Extract inverse (high 32 bits)
        return [row >> 32 for row in aug]
    
    inv_diffusion = invert_matrix_gf2(diffusion)
    if inv_diffusion:
        inv_weights = [popcount(row) for row in inv_diffusion]
        print(f"Inverse matrix row weights: {inv_weights[:10]}...") # Show first 10
        print(f"Inverse weight range: {min(inv_weights)} to {max(inv_weights)}")
        print(f"Inverse average weight: {sum(inv_weights)/len(inv_weights):.1f}")
        
        # Find sparse inverse rows
        sparse_inv_rows = [(i, w) for i, w in enumerate(inv_weights) if w < 8]
        print(f"Sparse inverse rows (<8 bits): {sparse_inv_rows}")
    
    # 5. Look for special state patterns
    print("\n\n5. SPECIAL STATE PATTERN SEARCH")
    print("-" * 50)
    
    # Pattern 1: States with many zeros
    def test_pattern_zeros():
        patterns = []
        for num_zeros in [16, 20, 24, 28]:
            # Try states with num_zeros zero bytes
            for trial in range(100):
                state = [0] * 32
                # Fill non-zero positions randomly with valid values
                valid_values = [x for x in range(256) if x not in missing_values]
                
                import random
                positions = list(range(32))
                random.shuffle(positions)
                for i in range(32 - num_zeros):
                    state[positions[i]] = random.choice(valid_values)
                
                # Check if this leads to valid inverse diffusion
                if inv_diffusion:
                    inverse_state = [0] * 32
                    all_valid = True
                    for j in range(32):
                        val = 0
                        for k in range(32):
                            if (inv_diffusion[j] >> k) & 1:
                                val ^= state[k]
                        inverse_state[j] = val
                        if inverse_state[j] in missing_values:
                            all_valid = False
                            break
                    
                    if all_valid:
                        patterns.append((num_zeros, state[:8]))  # Store first 8 bytes as example
                        if len(patterns) >= 3:
                            break
        
        print(f"Found {len(patterns)} sparse patterns")
        for num_zeros, example in patterns:
            print(f"  {num_zeros} zeros: {[hex(b) for b in example]}...")
    
    # Pattern 2: Structured patterns
    def test_pattern_structured():
        patterns = []
        # Try patterns like all even, all odd, arithmetic sequences, etc.
        
        # All even pattern
        state = [i*2 % 256 for i in range(32)]
        state = [x for x in state if x not in missing_values]
        if len(state) >= 16:
            patterns.append(("all_even", state[:8]))
        
        # Arithmetic sequence
        for start in range(0, 16):
            for step in range(1, 8):
                state = [(start + i*step) % 256 for i in range(32)]
                valid_state = [x for x in state if x not in missing_values]
                if len(valid_state) == len(state):  # All values valid
                    patterns.append((f"arith_{start}_{step}", state[:8]))
                    break
        
        print(f"Found {len(patterns)} structured patterns")
        for pattern_type, example in patterns:
            print(f"  {pattern_type}: {[hex(b) for b in example]}...")
    
    test_pattern_zeros()
    test_pattern_structured()
    
    # 6. Generate high-probability seeds
    print("\n\n6. HIGH-PROBABILITY SEED GENERATION")
    print("-" * 50)
    
    def generate_smart_seed():
        """Generate a seed that's more likely to work"""
        # Strategy: Start from target constraints and work backwards
        
        # Find the least constrained position
        min_constraint_pos = min(range(16), key=lambda i: len(valid_pairs_per_position[i]))
        min_pairs = len(valid_pairs_per_position[min_constraint_pos])
        
        print(f"Starting from least constrained position {min_constraint_pos} ({min_pairs} pairs)")
        
        # Pick a random valid pair for this position
        import random
        chosen_pair = random.choice(valid_pairs_per_position[min_constraint_pos])
        print(f"Chosen pair for position {min_constraint_pos}: even=0x{chosen_pair[0]:02x}, odd=0x{chosen_pair[1]:02x}")
        
        # Build a complete state using this constraint
        state = [0] * 32
        state[2*min_constraint_pos] = chosen_pair[0]
        state[2*min_constraint_pos + 1] = chosen_pair[1]
        
        # Fill other positions with bias toward less constrained choices
        for pos in range(16):
            if pos != min_constraint_pos:
                # Choose from available pairs, biased toward those with more options
                available_pairs = valid_pairs_per_position[pos]
                if available_pairs:
                    pair = random.choice(available_pairs)
                    state[2*pos] = pair[0]
                    state[2*pos + 1] = pair[1]
        
        return state
    
    # Generate several smart seeds
    smart_seeds = []
    for i in range(5):
        seed = generate_smart_seed()
        smart_seeds.append(seed)
        print(f"Smart seed {i}: {[hex(b) for b in seed[:8]]}...")
    
    # 7. Probability analysis
    print("\n\n7. PROBABILITY ENHANCEMENT ANALYSIS")
    print("-" * 50)
    
    # Calculate theoretical probability improvement
    total_combinations = 1
    for pairs in valid_pairs_per_position:
        total_combinations *= len(pairs)
    
    # Compare with random probability
    random_prob = ((256 - len(missing_values)) / 256) ** 32
    smart_prob = 1.0  # We're guaranteeing the final constraint is satisfied
    
    print(f"Random state probability: {random_prob:.2e}")
    print(f"Smart construction probability: {smart_prob:.2e}")
    print(f"Improvement factor: {smart_prob / random_prob:.2e}")
    
    print(f"\nTotal valid final state combinations: {total_combinations:.2e}")
    
    return valid_pairs_per_position, smart_seeds

if __name__ == "__main__":
    valid_pairs, seeds = analyze_structural_patterns()