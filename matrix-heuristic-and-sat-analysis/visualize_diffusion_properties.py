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

def visualize_properties():
    """Create specific visualizations for cryptographic properties"""
    diffusion_inv = invert_diffusion_matrix_gf2(diffusion)
    
    # Convert to 2D array
    matrix = [[0 for _ in range(32)] for _ in range(32)]
    for i in range(32):
        for j in range(32):
            matrix[i][j] = (diffusion_inv[i] >> j) & 1
    
    print("="*80)
    print("CRYPTOGRAPHIC PROPERTIES VISUALIZATION")
    print("="*80)
    
    # 1. DIAGONAL vs OFF-DIAGONAL VISUALIZATION
    print("\n1. DIAGONAL vs OFF-DIAGONAL PATTERN:")
    print("   Showing 8x8 block structure with emphasis on diagonal blocks")
    print("   [D] = Diagonal block (low density)  [X] = Off-diagonal (high density)")
    print()
    
    # Create a visual representation
    for br in range(4):
        print("   ", end="")
        for bc in range(4):
            # Count 1s in this block
            total = sum(matrix[br*8 + i][bc*8 + j] 
                       for i in range(8) for j in range(8))
            
            if br == bc:  # Diagonal block
                print(f"[D:{total:2d}]", end="  ")
            else:
                print(f"[X:{total:2d}]", end="  ")
        print()
    
    # 2. DIFFUSION PATH VISUALIZATION
    print("\n2. DIFFUSION PATHS - How bits spread:")
    print("   Tracking how input bit 0 affects output bits")
    print()
    
    # Show which output bits are affected by input bit 0
    affected_by_bit0 = []
    for i in range(32):
        if matrix[i][0]:
            affected_by_bit0.append(i)
    
    print(f"   Input bit 0 affects {len(affected_by_bit0)} output bits:")
    print(f"   Output bits: {affected_by_bit0}")
    
    # Visualize as byte groups
    print("\n   Affected bytes (● = affected bit):")
    for byte in range(4):
        print(f"   Byte {byte}: ", end="")
        for bit in range(8):
            idx = byte * 8 + bit
            if idx in affected_by_bit0:
                print("●", end=" ")
            else:
                print("○", end=" ")
        print()
    
    # 3. AVALANCHE EFFECT VISUALIZATION
    print("\n3. AVALANCHE EFFECT - Single bit change impact:")
    print("   Shows how many output bits change when each input bit flips")
    print()
    
    # Create avalanche visualization
    print("   Input →")
    print("   Byte#  01234567")
    print("   " + "-"*20)
    
    for byte in range(4):
        print(f"   B{byte}:    ", end="")
        for bit in range(8):
            input_bit = byte * 8 + bit
            # Count how many output bits this input affects
            affected = sum(matrix[i][input_bit] for i in range(32))
            # Use different symbols based on impact
            if affected < 10:
                symbol = str(affected)
            elif affected < 15:
                symbol = "+"
            else:
                symbol = "#"
            print(symbol, end="")
        print()
    
    # 4. BYTE-LEVEL MIXING VISUALIZATION
    print("\n4. BYTE-LEVEL MIXING MATRIX:")
    print("   Shows how input bytes affect output bytes")
    print("   (number = bits from input byte affecting output byte)")
    print()
    
    print("        Input Bytes")
    print("        B0  B1  B2  B3")
    print("       " + "-"*16)
    
    for out_byte in range(4):
        print(f"   B{out_byte} |", end="")
        for in_byte in range(4):
            # Count connections between these bytes
            connections = 0
            for out_bit in range(8):
                for in_bit in range(8):
                    if matrix[out_byte*8 + out_bit][in_byte*8 + in_bit]:
                        connections += 1
            print(f" {connections:2d}", end="")
        print(" |")
    
    # 5. CIRCULAR SHIFT PATTERN DETECTION
    print("\n5. CIRCULAR PATTERN ANALYSIS:")
    print("   Checking for rotational relationships between rows")
    
    # Check if rows are related by rotation
    def get_rotation_distance(row1, row2):
        """Check if row2 is a rotation of row1"""
        for shift in range(32):
            if row1 == ((row2 << shift) | (row2 >> (32 - shift))) & 0xFFFFFFFF:
                return shift
        return -1
    
    rotation_found = False
    for i in range(min(8, 31)):  # Check first few rows
        for j in range(i+1, min(8, 32)):
            dist = get_rotation_distance(diffusion_inv[i], diffusion_inv[j])
            if dist != -1:
                print(f"   Row {j} is row {i} rotated by {dist} positions")
                rotation_found = True
    
    if not rotation_found:
        print("   No simple rotational patterns found in first 8 rows")
    
    # 6. VISUAL HEATMAP OF CONNECTIONS
    print("\n6. CONNECTION DENSITY HEATMAP:")
    print("   Darker = more connections in that region")
    print("   (Using 4x4 blocks for visualization)")
    print()
    
    # Calculate density for 4x4 blocks
    print("     ", end="")
    for i in range(8):
        print(f"{i:2d}", end=" ")
    print()
    
    for br in range(8):
        print(f"  {br}: ", end="")
        for bc in range(8):
            total = sum(matrix[br*4 + i][bc*4 + j] 
                       for i in range(4) for j in range(4))
            density = total / 16
            
            # Use Unicode block characters for density
            if density == 0:
                char = "  "
            elif density < 0.2:
                char = "░░"
            elif density < 0.4:
                char = "▒▒"
            elif density < 0.6:
                char = "▓▓"
            elif density < 0.8:
                char = "██"
            else:
                char = "██"
            print(char, end=" ")
        print()
    
    # 7. SYMMETRY VISUALIZATION
    print("\n7. SYMMETRY PROPERTIES:")
    
    # Check various symmetries
    is_symmetric = True
    is_antisymmetric = True
    
    for i in range(32):
        for j in range(i+1, 32):
            if matrix[i][j] != matrix[j][i]:
                is_symmetric = False
            if matrix[i][j] == matrix[j][i] and matrix[i][j] == 1:
                is_antisymmetric = False
    
    print(f"   Matrix is symmetric: {'Yes' if is_symmetric else 'No'}")
    print(f"   Matrix is antisymmetric: {'Yes' if is_antisymmetric else 'No'}")
    
    # Check block-level symmetries
    print("\n   Block-level density symmetries:")
    for i in range(4):
        for j in range(i+1, 4):
            block_ij = sum(matrix[i*8 + r][j*8 + c] 
                          for r in range(8) for c in range(8))
            block_ji = sum(matrix[j*8 + r][i*8 + c] 
                          for r in range(8) for c in range(8))
            if block_ij == block_ji:
                print(f"   Blocks [{i},{j}] and [{j},{i}] have same density: {block_ij}")

if __name__ == "__main__":
    visualize_properties()