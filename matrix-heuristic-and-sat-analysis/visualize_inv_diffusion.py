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
    # Create identity matrix for inverse
    inverse = [1 << i for i in range(n)]
    
    # Copy original to work matrix
    work = original.copy()
    
    # Gaussian elimination in GF(2)
    for col in range(n):
        # Find pivot
        pivot = -1
        for row in range(col, n):
            if work[row] & (1 << col):
                pivot = row
                break
        
        if pivot == -1:
            continue  # Singular matrix
        
        # Swap rows if needed
        if pivot != col:
            work[col], work[pivot] = work[pivot], work[col]
            inverse[col], inverse[pivot] = inverse[pivot], inverse[col]
        
        # Eliminate
        for row in range(n):
            if row != col and (work[row] & (1 << col)):
                work[row] ^= work[col]
                inverse[row] ^= inverse[col]
    
    return inverse

def visualize_inverse_diffusion():
    """Create multiple visualizations of the inverse diffusion matrix"""
    # Get the inverse matrix
    diffusion_inv = invert_diffusion_matrix_gf2(diffusion)
    
    print("="*80)
    print("INVERSE DIFFUSION MATRIX VISUALIZATION")
    print("="*80)
    
    # 1. Binary visualization with grid
    print("\n1. BINARY MATRIX (32x32):")
    print("   - Each row represents output bit position")
    print("   - Each column represents input bit position")
    print("   - '#' = 1 (connection exists), '.' = 0 (no connection)")
    print("\n     " + "".join(f"{i%10}" for i in range(32)))
    print("     " + "-"*32)
    for i in range(32):
        row_str = f"{i:2d} | "
        for j in range(32):
            bit = (diffusion_inv[i] >> j) & 1
            row_str += "#" if bit else "."
        # Count 1s in this row
        ones = bin(diffusion_inv[i]).count('1')
        row_str += f" | {ones:2d} ones"
        print(row_str)
    print("     " + "-"*32)
    
    # 2. Hexadecimal representation
    print("\n2. HEXADECIMAL VALUES:")
    print("   Row | Hex Value      | Binary (MSB...LSB)")
    print("   " + "-"*60)
    for i in range(32):
        binary_str = bin(diffusion_inv[i])[2:].zfill(32)
        # Group binary into 4-bit chunks for readability
        binary_grouped = ' '.join(binary_str[j:j+4] for j in range(0, 32, 4))
        print(f"   {i:2d}  | 0x{diffusion_inv[i]:08x}     | {binary_grouped}")
    
    # 3. Statistical analysis
    print("\n3. STATISTICAL ANALYSIS:")
    total_ones = sum(bin(row).count('1') for row in diffusion_inv)
    density = total_ones / (32 * 32)
    
    # Count ones per row and column
    row_counts = [bin(row).count('1') for row in diffusion_inv]
    col_counts = [0] * 32
    for row in diffusion_inv:
        for col in range(32):
            if (row >> col) & 1:
                col_counts[col] += 1
    
    print(f"   - Total 1s: {total_ones} out of 1024")
    print(f"   - Density: {density:.2%}")
    print(f"   - Average 1s per row: {sum(row_counts)/32:.2f}")
    print(f"   - Average 1s per column: {sum(col_counts)/32:.2f}")
    print(f"   - Min 1s in a row: {min(row_counts)}")
    print(f"   - Max 1s in a row: {max(row_counts)}")
    print(f"   - Min 1s in a column: {min(col_counts)}")
    print(f"   - Max 1s in a column: {max(col_counts)}")
    
    # 4. Pattern analysis
    print("\n4. PATTERN ANALYSIS:")
    # Check for diagonal patterns
    diagonal_ones = sum(1 for i in range(32) if (diffusion_inv[i] >> i) & 1)
    print(f"   - Diagonal elements (i,i): {diagonal_ones} ones")
    
    # Check for symmetry
    is_symmetric = True
    for i in range(32):
        for j in range(i+1, 32):
            bit_ij = (diffusion_inv[i] >> j) & 1
            bit_ji = (diffusion_inv[j] >> i) & 1
            if bit_ij != bit_ji:
                is_symmetric = False
                break
        if not is_symmetric:
            break
    print(f"   - Matrix is symmetric: {'Yes' if is_symmetric else 'No'}")
    
    # 5. Verification
    print("\n5. VERIFICATION:")
    # Multiply diffusion * diffusion_inv in GF(2)
    is_identity = True
    non_identity_count = 0
    for i in range(32):
        result = 0
        for k in range(32):
            if (diffusion[i] >> k) & 1:
                result ^= diffusion_inv[k]
        expected = 1 << i
        if result != expected:
            is_identity = False
            non_identity_count += 1
    
    print(f"   - Diffusion × Diffusion_inv = Identity: {'✓ YES' if is_identity else f'✗ NO ({non_identity_count} errors)'}")
    
    # 6. Block structure visualization (8x8 blocks)
    print("\n6. BLOCK STRUCTURE (4 blocks of 8x8):")
    print("   Visualizing density in 8x8 blocks:")
    for block_row in range(4):
        for block_col in range(4):
            # Count 1s in this 8x8 block
            block_ones = 0
            for i in range(8):
                for j in range(8):
                    row_idx = block_row * 8 + i
                    col_idx = block_col * 8 + j
                    if (diffusion_inv[row_idx] >> col_idx) & 1:
                        block_ones += 1
            density = block_ones / 64
            print(f"   Block[{block_row},{block_col}]: {block_ones:2d}/64 ({density:5.1%})", end="  ")
        print()
    
    # 7. Connection graph
    print("\n7. CONNECTION ANALYSIS:")
    print("   Shows which output bits depend on which input bits")
    print("   (Showing first 8 rows for brevity)")
    for i in range(8):
        connections = []
        for j in range(32):
            if (diffusion_inv[i] >> j) & 1:
                connections.append(str(j))
        print(f"   Out[{i:2d}] = XOR of inputs: {{{', '.join(connections)}}}")

if __name__ == "__main__":
    visualize_inverse_diffusion()