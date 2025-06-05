#!/usr/bin/env python3
import numpy as np

# The missing values from the confusion matrix
missing_values = [0x0f, 0x11, 0x20, 0x3e, 0x44, 0x5a, 0x6b, 0x75, 0x80, 0x9e, 0xaf, 0xb1, 0xcb, 0xd5, 0xe4, 0xfa]

print("Missing values (hex):", [hex(v) for v in missing_values])
print("Missing values (decimal):", missing_values)
print()

# Convert each value to its 8-bit binary representation
def to_binary_vector(val):
    """Convert an integer to an 8-bit binary vector"""
    return [(val >> i) & 1 for i in range(8)]

# Create matrix where each row is a missing value in binary
binary_matrix = []
for val in missing_values:
    binary_vec = to_binary_vector(val)
    binary_matrix.append(binary_vec)
    print(f"0x{val:02x} = {val:3d} = ", end="")
    print(''.join(str(b) for b in reversed(binary_vec)), end="")  # Show MSB first
    print(f" = {binary_vec}")

# Convert to numpy array
A = np.array(binary_matrix, dtype=int)
print(f"\nMatrix shape: {A.shape} (16 missing values × 8 bits)")

# Compute rank using Gaussian elimination in GF(2)
def gf2_rank(matrix):
    """Compute rank of a matrix over GF(2) using Gaussian elimination"""
    m = matrix.copy()
    rows, cols = m.shape
    rank = 0
    
    for col in range(cols):
        # Find pivot
        pivot_row = None
        for row in range(rank, rows):
            if m[row, col] == 1:
                pivot_row = row
                break
        
        if pivot_row is None:
            continue
            
        # Swap rows if needed
        if pivot_row != rank:
            m[[pivot_row, rank]] = m[[rank, pivot_row]]
        
        # Eliminate column
        for row in range(rows):
            if row != rank and m[row, col] == 1:
                m[row] = (m[row] + m[rank]) % 2
        
        rank += 1
    
    return rank

# Calculate rank
rank = gf2_rank(A)
print(f"\nRank of the matrix over GF(2): {rank}")

# Verify using row echelon form
print("\nRow echelon form:")
m = A.copy()
rows, cols = m.shape
current_row = 0

for col in range(cols):
    # Find pivot
    pivot_row = None
    for row in range(current_row, rows):
        if m[row, col] == 1:
            pivot_row = row
            break
    
    if pivot_row is None:
        continue
        
    # Swap rows if needed
    if pivot_row != current_row:
        m[[pivot_row, current_row]] = m[[current_row, pivot_row]]
    
    # Eliminate column
    for row in range(rows):
        if row != current_row and m[row, col] == 1:
            m[row] = (m[row] + m[current_row]) % 2
    
    current_row += 1

# Print row echelon form
print("Matrix in row echelon form:")
for i, row in enumerate(m):
    print(f"Row {i:2d}: ", ''.join(str(b) for b in row))

# Count non-zero rows
non_zero_rows = sum(1 for row in m if any(row))
print(f"\nNon-zero rows: {non_zero_rows}")

# Find a basis for the space
print("\nBasis vectors (in terms of original missing values):")
basis_indices = []
current_row = 0
for col in range(cols):
    for row in range(current_row, rows):
        if A[row, col] == 1:
            basis_indices.append(row)
            current_row += 1
            break

# Remove duplicates and limit to rank
basis_indices = list(dict.fromkeys(basis_indices))[:rank]

print(f"Basis consists of {len(basis_indices)} vectors:")
for idx in basis_indices:
    val = missing_values[idx]
    print(f"  0x{val:02x} = {to_binary_vector(val)}")

# Additional analysis: check linear independence
print("\n=== Linear Independence Analysis ===")
print(f"The 16 missing values span a {rank}-dimensional subspace of GF(2)^8")
print(f"This means only {rank} of the 16 values are linearly independent")
print(f"The remaining {16 - rank} values can be expressed as linear combinations of the basis")

# Check which bit positions contribute to the rank
print("\nBit position analysis:")
bit_significance = []
for bit in range(8):
    bit_column = A[:, bit]
    if np.any(bit_column):
        count = np.sum(bit_column)
        bit_significance.append((bit, count))
        print(f"  Bit {bit}: appears in {count}/16 values")

# XOR closure analysis
print("\n=== XOR Closure Analysis ===")
# Generate all possible XOR combinations of missing values
closure = set(missing_values)
print(f"Starting with {len(closure)} values")

# Add all pairwise XORs
for v1 in missing_values:
    for v2 in missing_values:
        closure.add(v1 ^ v2)

print(f"After pairwise XORs: {len(closure)} values")

# Continue until closure is stable
prev_size = 0
iteration = 0
while len(closure) != prev_size:
    prev_size = len(closure)
    new_values = set()
    for v1 in list(closure):
        for v2 in missing_values:
            new_values.add(v1 ^ v2)
    closure.update(new_values)
    iteration += 1
    print(f"After iteration {iteration}: {len(closure)} values")

print(f"\nThe XOR closure contains {len(closure)} values")
print(f"This confirms the space has dimension {rank} (2^{rank} = {2**rank})")