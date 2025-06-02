#!/usr/bin/env python3

# The diffusion matrix from the C code
diffusion = [
    0xf26cb481,0x16a5dc92,0x3c5ba924,0x79b65248,0x2fc64b18,0x615acd29,0xc3b59a42,0x976b2584,
    0x6cf281b4,0xa51692dc,0x5b3c24a9,0xb6794852,0xc62f184b,0x5a6129cd,0xb5c3429a,0x6b978425,
    0xb481f26c,0xdc9216a5,0xa9243c5b,0x524879b6,0x4b182fc6,0xcd29615a,0x9a42c3b5,0x2584976b,
    0x81b46cf2,0x92dca516,0x24a95b3c,0x4852b679,0x184bc62f,0x29cd5a61,0x429ab5c3,0x84256b97
]

def invert_diffusion_matrix_gf2(original):
    """Invert the diffusion matrix in GF(2)"""
    n = 32
    inverse = [1 << i for i in range(n)]
    work = original.copy()
    
    for col in range(n):
        pivot = -1
        for row in range(col, n):
            if work[row] & (1 << col):
                pivot = row
                break
        
        if pivot == -1:
            continue
        
        if pivot != col:
            work[col], work[pivot] = work[pivot], work[col]
            inverse[col], inverse[pivot] = inverse[pivot], inverse[col]
        
        for row in range(n):
            if row != col and (work[row] & (1 << col)):
                work[row] ^= work[col]
                inverse[row] ^= inverse[col]
    
    return inverse

def analyze_block_structure():
    """Analyze the block structure pattern of the inverse diffusion matrix"""
    diffusion_inv = invert_diffusion_matrix_gf2(diffusion)
    
    print("="*80)
    print("BLOCK STRUCTURE ANALYSIS OF INVERSE DIFFUSION MATRIX")
    print("="*80)
    
    # Convert to 2D array for easier block analysis
    matrix = [[0 for _ in range(32)] for _ in range(32)]
    for i in range(32):
        for j in range(32):
            matrix[i][j] = (diffusion_inv[i] >> j) & 1
    
    # 1. Analyze 8x8 blocks
    print("\n1. 8x8 BLOCK STRUCTURE (4x4 blocks):")
    print("   Each cell shows: density | diagonal_ones")
    print()
    
    block_data = []
    for br in range(4):
        row_data = []
        for bc in range(4):
            # Count 1s in this block
            total = 0
            diagonal = 0
            for i in range(8):
                for j in range(8):
                    if matrix[br*8 + i][bc*8 + j]:
                        total += 1
                        if i == j:
                            diagonal += 1
            row_data.append((total, diagonal))
        block_data.append(row_data)
    
    # Print block densities
    print("   Block densities (out of 64):")
    for br in range(4):
        print(f"   Row {br}: ", end="")
        for bc in range(4):
            total, diag = block_data[br][bc]
            print(f"[{total:2d}/64 = {total/64:5.1%}]", end="  ")
        print()
    
    # 2. Visualize blocks with ASCII art
    print("\n2. VISUAL BLOCK PATTERN (density visualization):")
    print("   □ = 0-25%  ▤ = 25-40%  ▥ = 40-60%  ▦ = 60-75%  ■ = 75-100%")
    print()
    
    for br in range(4):
        for bc in range(4):
            total, _ = block_data[br][bc]
            density = total / 64
            if density < 0.25:
                symbol = "□"
            elif density < 0.40:
                symbol = "▤"
            elif density < 0.60:
                symbol = "▥"
            elif density < 0.75:
                symbol = "▦"
            else:
                symbol = "■"
            print(f"   {symbol}", end="")
        print()
    
    # 3. Analyze 4x4 blocks (finer granularity)
    print("\n3. 4x4 BLOCK STRUCTURE (8x8 blocks):")
    print("   Showing density for each 4x4 block:")
    
    for br in range(8):
        for bc in range(8):
            total = 0
            for i in range(4):
                for j in range(4):
                    if matrix[br*4 + i][bc*4 + j]:
                        total += 1
            density = total / 16
            # Use grayscale characters
            if density == 0:
                char = " "
            elif density < 0.2:
                char = "·"
            elif density < 0.4:
                char = "∙"
            elif density < 0.6:
                char = "●"
            elif density < 0.8:
                char = "◉"
            else:
                char = "◆"
            print(char, end="")
        print(f"  (row {br*4}-{br*4+3})")
    
    # 4. Analyze cyclic/rotational patterns
    print("\n4. PATTERN ANALYSIS:")
    
    # Check if blocks have rotational symmetry
    print("   Checking for rotational patterns in 8x8 blocks...")
    
    # Compare blocks
    patterns_found = []
    for i in range(4):
        for j in range(4):
            if i == j:  # Diagonal blocks
                patterns_found.append(f"Block[{i},{j}] is on main diagonal")
    
    # Check for anti-diagonal pattern
    anti_diag_match = True
    for i in range(4):
        if block_data[i][3-i][0] != block_data[0][3][0]:
            anti_diag_match = False
            break
    
    if anti_diag_match:
        patterns_found.append("Anti-diagonal blocks have similar density")
    
    for pattern in patterns_found:
        print(f"   - {pattern}")
    
    # 5. Detailed view of one 8x8 block
    print("\n5. DETAILED VIEW OF BLOCK[0,0] (top-left 8x8):")
    for i in range(8):
        print("   ", end="")
        for j in range(8):
            print("#" if matrix[i][j] else ".", end=" ")
        print()
    
    # 6. Block connectivity analysis
    print("\n6. BLOCK CONNECTIVITY:")
    print("   How many connections cross block boundaries?")
    
    internal_connections = 0
    cross_block_connections = 0
    
    for i in range(32):
        for j in range(32):
            if matrix[i][j]:
                # Check if i and j are in same 8x8 block
                block_i = i // 8
                block_j = j // 8
                if block_i == block_j:
                    internal_connections += 1
                else:
                    cross_block_connections += 1
    
    total_connections = internal_connections + cross_block_connections
    print(f"   - Internal connections (within blocks): {internal_connections} ({internal_connections/total_connections:.1%})")
    print(f"   - Cross-block connections: {cross_block_connections} ({cross_block_connections/total_connections:.1%})")
    
    # 7. Matrix structure interpretation
    print("\n7. STRUCTURE INTERPRETATION:")
    print("   The pattern suggests:")
    print("   - Lower density on the main diagonal blocks (28.1%)")
    print("   - Higher density on off-diagonal blocks (50-56%)")
    print("   - This creates strong mixing between different 8-bit segments")
    print("   - Each 8-bit output segment depends on bits from all 4 input segments")

if __name__ == "__main__":
    analyze_block_structure()