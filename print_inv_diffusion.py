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

def create_printable_matrix():
    """Create a clean, printable version of the inverse diffusion matrix"""
    diffusion_inv = invert_diffusion_matrix_gf2(diffusion)
    
    print("INVERSE DIFFUSION MATRIX (32x32)")
    print("=" * 70)
    print()
    
    # Clean matrix with better spacing for printing
    print("      ", end="")
    for i in range(32):
        print(f"{i:2d}", end=" " if i < 31 else "\n")
    
    print("    +" + "-" * 95 + "+")
    
    for i in range(32):
        print(f" {i:2d} |", end="")
        for j in range(32):
            bit = (diffusion_inv[i] >> j) & 1
            print(f"  {bit}", end="")
        print(" |")
    
    print("    +" + "-" * 95 + "+")
    
    print("\nHexadecimal representation:")
    print("-" * 40)
    for i in range(32):
        print(f"Row {i:2d}: 0x{diffusion_inv[i]:08X}")
    
    # Also save to file
    with open('/Users/adnanehamid/nerf/inverse_diffusion_matrix.txt', 'w') as f:
        f.write("INVERSE DIFFUSION MATRIX (32x32)\n")
        f.write("=" * 70 + "\n\n")
        
        # Header
        f.write("      ")
        for i in range(32):
            f.write(f"{i:2d} ")
        f.write("\n")
        
        f.write("    +" + "-" * 95 + "+\n")
        
        # Matrix
        for i in range(32):
            f.write(f" {i:2d} |")
            for j in range(32):
                bit = (diffusion_inv[i] >> j) & 1
                f.write(f"  {bit}")
            f.write(" |\n")
        
        f.write("    +" + "-" * 95 + "+\n")
        
        f.write("\nHexadecimal representation:\n")
        f.write("-" * 40 + "\n")
        for i in range(32):
            f.write(f"Row {i:2d}: 0x{diffusion_inv[i]:08X}\n")
    
    print("\nMatrix saved to: inverse_diffusion_matrix.txt")

if __name__ == "__main__":
    create_printable_matrix()