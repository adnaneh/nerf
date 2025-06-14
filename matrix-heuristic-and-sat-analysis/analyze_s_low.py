#!/usr/bin/env python3
from collections import Counter

# Extract s_low values from confusion array (first 256 bytes)
s_low_hex = """
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
0x22,0xf4,0xb9,0xa2,0x6f,0x12,0x1b,0x14,0x45,0xc7,0x87,0x31,0x60,0x29,0xf7,0x73
"""

# Parse hex values
s_low = []
for line in s_low_hex.strip().split('\n'):
    values = line.strip().rstrip(',').split(',')
    for val in values:
        s_low.append(int(val, 16))

print("=== S_low (S-box) Analysis ===")
print(f"Length: {len(s_low)} bytes")
print(f"Min value: {min(s_low)}")
print(f"Max value: {max(s_low)}")
print(f"Number of unique values: {len(set(s_low))}")

# Check if it's a permutation
is_permutation = len(set(s_low)) == 256 and min(s_low) == 0 and max(s_low) == 255
print(f"\nIs s_low a permutation? {is_permutation}")

# Analyze mapping properties
if is_permutation:
    # Find fixed points
    fixed_points = [i for i in range(256) if s_low[i] == i]
    print(f"Number of fixed points: {len(fixed_points)}")
    if fixed_points:
        print(f"Fixed points: {fixed_points}")
    
    # Analyze cycles
    visited = [False] * 256
    cycles = []
    for start in range(256):
        if not visited[start]:
            cycle = []
            current = start
            while not visited[current]:
                visited[current] = True
                cycle.append(current)
                current = s_low[current]
            cycles.append(len(cycle))
    
    cycle_counts = Counter(cycles)
    print(f"\nCycle structure:")
    for length, count in sorted(cycle_counts.items()):
        print(f"  Cycles of length {length}: {count}")
else:
    # If not a permutation, analyze the mapping
    print("\nNot a permutation! Analyzing mapping properties...")
    
    # Count how many times each value appears
    value_counts = Counter(s_low)
    print(f"Number of distinct output values: {len(value_counts)}")
    
    # Find values that appear multiple times
    multi_mapped = [(val, count) for val, count in value_counts.items() if count > 1]
    if multi_mapped:
        print(f"\nValues that appear multiple times:")
        for val, count in sorted(multi_mapped):
            print(f"  Value 0x{val:02x} appears {count} times")
            # Find which inputs map to this value
            inputs = [i for i in range(256) if s_low[i] == val]
            print(f"    Inputs: {[f'0x{i:02x}' for i in inputs]}")
    
    # Find missing values (no pre-image)
    all_values = set(range(256))
    present_values = set(s_low)
    missing_values = all_values - present_values
    if missing_values:
        print(f"\nMissing values (no pre-image): {len(missing_values)}")
        print(f"Missing: {[f'0x{v:02x}' for v in sorted(missing_values)]}")

# Check for any linear structure
print("\n=== Linear Structure Analysis ===")

# Check if s_low(x) = ax + b for some a, b
def check_affine(s_box):
    """Check if S-box has affine structure s(x) = ax + b (mod 256)"""
    # Try to find a, b from first two mappings
    if s_box[0] == s_box[1]:
        return False, None, None
    
    # For x=0: s(0) = b
    b = s_box[0]
    
    # For x=1: s(1) = a + b, so a = s(1) - b
    a = (s_box[1] - b) % 256
    
    # Check if this holds for all x
    for x in range(256):
        expected = (a * x + b) % 256
        if s_box[x] != expected:
            return False, None, None
    
    return True, a, b

is_affine, a, b = check_affine(s_low)
print(f"Is affine (s(x) = ax + b mod 256)? {is_affine}")
if is_affine:
    print(f"  a = {a}, b = {b}")

# Check for XOR structure
def check_xor_structure(s_box):
    """Check if S-box has structure s(x) = x XOR k for some constant k"""
    k = s_box[0] ^ 0  # s(0) = 0 XOR k = k
    
    for x in range(256):
        if s_box[x] != (x ^ k):
            return False, None
    
    return True, k

is_xor, k = check_xor_structure(s_low)
print(f"Is XOR structure (s(x) = x XOR k)? {is_xor}")
if is_xor:
    print(f"  k = 0x{k:02x}")

# Check for polynomial structure
print("\nChecking for s(x) = x^n patterns...")
for n in [3, 5, 7, 9, 11, 13, 15, 17, 31, 63, 127]:
    matches = 0
    for x in range(256):
        if s_low[x] == (pow(x, n, 256)):
            matches += 1
    if matches > 10:
        print(f"  x^{n} mod 256: {matches}/256 matches")

# Check differential properties
print("\n=== Differential Properties ===")
# For each input difference, see what output differences occur
diff_table = [[0 for _ in range(256)] for _ in range(256)]
for x1 in range(256):
    for x2 in range(256):
        input_diff = x1 ^ x2
        output_diff = s_low[x1] ^ s_low[x2]
        diff_table[input_diff][output_diff] += 1

# Find max differential probability
max_diff = max(max(row) for row in diff_table)
print(f"Maximum differential probability: {max_diff}/256 = {max_diff/256:.3f}")

# Check for interesting differential patterns
print("\nChecking for differential patterns...")
# Check if input diff 1 always gives a specific output diff
diff_1_outputs = Counter()
for x in range(256):
    y = (x + 1) % 256
    diff_1_outputs[s_low[y] ^ s_low[x]] += 1
if len(diff_1_outputs) < 10:
    print(f"  Input diff 1 gives output diffs: {dict(diff_1_outputs)}")

# Matrix representation analysis
print("\n=== Matrix Representation Analysis ===")
print("Checking if S_low can be represented as a matrix multiplication in GF(2)...")

def bit_vector(x):
    """Convert byte to 8-bit vector"""
    return [(x >> i) & 1 for i in range(8)]

def vector_to_byte(v):
    """Convert 8-bit vector to byte"""
    return sum(v[i] << i for i in range(8))

# Try to find a matrix M such that S_low(x) = M * x (in GF(2))
# Build the transformation matrix
M = [[0 for _ in range(8)] for _ in range(8)]
c = [0 for _ in range(8)]  # constant vector

# Use x=0 to find the constant term
s0_bits = bit_vector(s_low[0])
c = s0_bits

# Use powers of 2 to find each column of M
for i in range(8):
    x = 1 << i  # x has only bit i set
    sx_bits = bit_vector(s_low[x])
    # Column i of M is (S(2^i) - S(0)) in GF(2)
    for j in range(8):
        M[j][i] = (sx_bits[j] ^ c[j])

print("\nTrying to find affine transformation S_low(x) = Mx + c in GF(2)^8...")
print(f"Constant vector c = {c}")
print("Matrix M:")
for row in M:
    print(f"  {row}")

# Verify if this works for all inputs
is_affine_gf2 = True
errors = 0
for x in range(256):
    x_bits = bit_vector(x)
    # Compute Mx + c in GF(2)
    result_bits = [0] * 8
    for i in range(8):
        bit_sum = c[i]
        for j in range(8):
            bit_sum ^= (M[i][j] & x_bits[j])
        result_bits[i] = bit_sum
    result = vector_to_byte(result_bits)
    
    if result != s_low[x]:
        is_affine_gf2 = False
        errors += 1
        if errors <= 3:
            print(f"  Mismatch at x={x}: expected {s_low[x]}, got {result}")

print(f"\nIs S_low affine in GF(2)^8? {is_affine_gf2}")
if not is_affine_gf2:
    print(f"Number of mismatches: {errors}/256")

# Check for multiplicative inverse structure
print("\n=== Multiplicative Inverse Analysis ===")
print("Checking if S_low could be related to multiplicative inverse...")

# Check pattern at specific values
print(f"s_low[0] = 0x{s_low[0]:02x}")
print(f"s_low[1] = 0x{s_low[1]:02x}")
print(f"s_low[2] = 0x{s_low[2]:02x}")
print(f"s_low[255] = 0x{s_low[255]:02x}")

# AES S-box for comparison (based on multiplicative inverse)
aes_sbox = [
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
]

# Compare with AES S-box
matches = sum(1 for i in range(256) if s_low[i] == aes_sbox[i])
print(f"\nMatches with AES S-box: {matches}/256")

# Look for XOR relationships
print("\n=== XOR Relationship Analysis ===")
# Check if s_low relates to position by XOR
xor_counts = Counter()
for i in range(256):
    xor_val = s_low[i] ^ i
    xor_counts[xor_val] += 1

print("Most common XOR values (s_low[i] XOR i):")
for val, count in xor_counts.most_common(10):
    print(f"  0x{val:02x}: appears {count} times")

# Summary
print("\n=== Summary ===")
results = {
    'is_permutation': is_permutation,
    'is_affine': is_affine,
    'is_xor': is_xor,
    'is_affine_gf2': is_affine_gf2,
    'max_differential': max_diff,
    'matches_aes': matches
}

for key, value in results.items():
    print(f"{key}: {value}")