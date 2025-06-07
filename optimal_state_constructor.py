#!/usr/bin/env python3
"""
Optimal state constructor that directly builds working states with high probability.
Based on structural analysis of the s_low confusion matrix and target constraints.
"""

import random

# s_low and s_high from hireme2.c
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

# Target string
target = [0x48, 0x69, 0x72, 0x65, 0x20, 0x6d, 0x65, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x21, 0x00]

# Missing values in s_low (no pre-images)
missing_values = {15, 17, 32, 41, 62, 68, 90, 107, 117, 128, 158, 175, 177, 203, 213, 228, 250}

# Diffusion matrix from hireme2.c
diffusion = [
    0xf26cb481,0x16a5dc92,0x3c5ba924,0x79b65248,0x2fc64b18,0x615acd29,0xc3b59a42,0x976b2584,
    0x6cf281b4,0xa51692dc,0x5b3c24a9,0xb6794852,0xc62f184b,0x5a6129cd,0xb5c3429a,0x6b978425,
    0xb481f26c,0xdc9216a5,0xa9243c5b,0x524879b6,0x4b182fc6,0xcd29615a,0x9a42c3b5,0x2584976b,
    0x81b46cf2,0x92dca516,0x24a95b3c,0x4852b679,0x184bc62f,0x29cd5a61,0x429ab5c3,0x84256b97
]

def precompute_valid_pairs():
    """Pre-compute all valid (even, odd) pairs for each target position"""
    # Build inverse mapping for s_low
    inv_s_low = {}
    for x in range(256):
        y = s_low[x]
        if y not in inv_s_low:
            inv_s_low[y] = []
        inv_s_low[y].append(x)
    
    valid_pairs = []
    for pos in range(16):
        target_byte = target[pos]
        pairs = []
        
        # For each possible odd byte value
        for odd_val in range(256):
            s_high_output = s_high[odd_val]
            needed_s_low = target_byte ^ s_high_output
            
            # Check if this s_low value has pre-images
            if needed_s_low in inv_s_low:
                for even_val in inv_s_low[needed_s_low]:
                    pairs.append((even_val, odd_val))
        
        valid_pairs.append(pairs)
    
    return valid_pairs

def dot_row(row, v):
    """Compute dot product of a row with vector v in GF(2)"""
    acc = 0
    for k in range(32):
        if (row >> k) & 1:
            acc ^= v[k]
    return acc

def invert_diffusion_matrix():
    """Invert the 32x32 diffusion matrix"""
    # Gaussian elimination in GF(2)
    aug = []
    for r in range(32):
        # Augmented matrix: [A | I] as 64-bit integers
        aug.append(diffusion[r] | (1 << (32 + r)))
    
    # Forward elimination
    for c in range(32):
        # Find pivot
        piv = c
        while piv < 32 and not (aug[piv] >> c & 1):
            piv += 1
        if piv == 32:
            raise ValueError("Matrix is singular")
        
        # Swap rows
        if piv != c:
            aug[c], aug[piv] = aug[piv], aug[c]
        
        # Eliminate
        for r in range(32):
            if r != c and (aug[r] >> c & 1):
                aug[r] ^= aug[c]
    
    # Extract inverse (high 32 bits)
    return [row >> 32 for row in aug]

def construct_optimal_final_state(strategy="balanced"):
    """
    Construct a final state that satisfies target constraints with high probability.
    
    Strategies:
    - "balanced": Choose pairs randomly from all valid options
    - "minimal": Prefer smaller input values
    - "diverse": Maximize diversity in chosen values
    - "structured": Use patterns that might help with inverse diffusion
    """
    
    valid_pairs = precompute_valid_pairs()
    state = [0] * 32
    
    if strategy == "balanced":
        # Random choice from valid pairs
        for pos in range(16):
            if valid_pairs[pos]:
                even_val, odd_val = random.choice(valid_pairs[pos])
                state[2*pos] = even_val
                state[2*pos + 1] = odd_val
                
    elif strategy == "minimal":
        # Choose smallest valid values
        for pos in range(16):
            if valid_pairs[pos]:
                # Sort by sum of values and pick the smallest
                best_pair = min(valid_pairs[pos], key=lambda p: p[0] + p[1])
                state[2*pos] = best_pair[0]
                state[2*pos + 1] = best_pair[1]
                
    elif strategy == "diverse":
        # Maximize diversity - avoid repeating values
        used_values = set()
        for pos in range(16):
            if valid_pairs[pos]:
                # Find pair with most unused values
                best_pair = None
                best_score = -1
                for pair in valid_pairs[pos]:
                    score = sum(1 for v in pair if v not in used_values)
                    if score > best_score:
                        best_score = score
                        best_pair = pair
                
                if best_pair:
                    state[2*pos] = best_pair[0]
                    state[2*pos + 1] = best_pair[1]
                    used_values.add(best_pair[0])
                    used_values.add(best_pair[1])
                    
    elif strategy == "structured":
        # Use structured patterns that might help
        # Strategy: Try to create patterns in pairs of positions
        for pos in range(16):
            if valid_pairs[pos]:
                # For positions with target 0x21 (many of them), try to use similar patterns
                if target[pos] == 0x21:
                    # Prefer pairs where even and odd values are close
                    best_pair = min(valid_pairs[pos], key=lambda p: abs(p[0] - p[1]))
                else:
                    # For other positions, use balanced approach
                    best_pair = random.choice(valid_pairs[pos])
                
                state[2*pos] = best_pair[0]
                state[2*pos + 1] = best_pair[1]
    
    return state

def verify_final_state(state):
    """Verify that a final state produces the correct target output"""
    output = []
    for i in range(16):
        even_out = s_low[state[2*i]]
        odd_out = s_high[state[2*i + 1]]
        output.append(even_out ^ odd_out)
    
    return output == target

def check_inverse_validity(state, inv_diffusion):
    """Check if a state would be valid under inverse diffusion"""
    # Compute what the previous state would need to be
    prev_state = [0] * 32
    for j in range(32):
        prev_state[j] = dot_row(inv_diffusion[j], state)
        if prev_state[j] in missing_values:
            return False, j
    return True, -1

def generate_working_states(num_states=10, strategy="balanced"):
    """Generate multiple working final states using different strategies"""
    
    print(f"=== GENERATING {num_states} WORKING STATES (strategy: {strategy}) ===\n")
    
    # Pre-compute inverse diffusion matrix
    try:
        inv_diffusion = invert_diffusion_matrix()
        print("✓ Diffusion matrix successfully inverted")
    except:
        print("✗ Failed to invert diffusion matrix")
        inv_diffusion = None
    
    working_states = []
    attempts = 0
    max_attempts = num_states * 100  # Allow up to 100x attempts
    
    while len(working_states) < num_states and attempts < max_attempts:
        attempts += 1
        
        # Construct a candidate final state
        state = construct_optimal_final_state(strategy)
        
        # Verify it produces the correct output
        if verify_final_state(state):
            # Check if it would be valid for inverse diffusion
            if inv_diffusion:
                is_valid, fail_pos = check_inverse_validity(state, inv_diffusion)
                if is_valid:
                    working_states.append(state)
                    print(f"✓ Working state {len(working_states):2d}: {[hex(b) for b in state[:8]]}... (attempt {attempts})")
                else:
                    print(f"✗ Valid output but invalid for inverse diffusion at pos {fail_pos} (attempt {attempts})")
            else:
                working_states.append(state)
                print(f"✓ Working state {len(working_states):2d}: {[hex(b) for b in state[:8]]}... (attempt {attempts}) [no inverse check]")
        else:
            print(f"✗ Invalid output (attempt {attempts})")
    
    print(f"\nGeneration complete: {len(working_states)}/{num_states} states found in {attempts} attempts")
    print(f"Success rate: {len(working_states)/attempts:.1%}")
    
    return working_states

def output_c_code(states):
    """Output C code for the working states"""
    print("\n=== C CODE FOR WORKING STATES ===\n")
    
    print("// Pre-computed working final states")
    print(f"static const u8 working_states[{len(states)}][32] = {{")
    
    for i, state in enumerate(states):
        print(f"    {{ // State {i}")
        for row in range(4):
            values = ", ".join(f"0x{state[row*8 + j]:02x}" for j in range(8))
            print(f"        {values},")
        print("    },")
    
    print("};")
    print()
    print("// Usage:")
    print("u8 final_state[32];")
    print("memcpy(final_state, working_states[rand() % " + str(len(states)) + "], 32);")

if __name__ == "__main__":
    # Generate working states with different strategies
    for strategy in ["balanced", "minimal", "diverse", "structured"]:
        print(f"\n{'='*60}")
        states = generate_working_states(3, strategy)
        
        if states:
            print(f"\nFirst state details for {strategy} strategy:")
            state = states[0]
            print(f"Full state: {[hex(b) for b in state]}")
            
            # Verify output
            output = []
            for i in range(16):
                even_out = s_low[state[2*i]]
                odd_out = s_high[state[2*i + 1]]
                result = even_out ^ odd_out
                output.append(result)
                print(f"Position {i:2d}: s_low[0x{state[2*i]:02x}] ⊕ s_high[0x{state[2*i+1]:02x}] = 0x{even_out:02x} ⊕ 0x{odd_out:02x} = 0x{result:02x} {'✓' if result == target[i] else '✗'}")
    
    # Output best results
    print(f"\n{'='*60}")
    final_states = generate_working_states(5, "balanced")
    if final_states:
        output_c_code(final_states)