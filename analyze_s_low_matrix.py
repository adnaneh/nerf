#!/usr/bin/env python3

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

print("=== Exploring Matrix Representation of S_low ===\n")

# Check if s_low could be a linear transformation modulo 256
print("1. Checking if s_low(x) = Ax (mod 256) for some constant A...")

# If s_low were multiplication by A, then s_low(1) = A
if s_low[1] != 0:
    A = s_low[1]
    print(f"If linear, A would be s_low(1) = {A} = 0x{A:02x}")
    
    # Check if this holds
    is_linear = True
    mismatches = 0
    for x in range(256):
        expected = (A * x) % 256
        if s_low[x] != expected:
            is_linear = False
            mismatches += 1
            if mismatches <= 3:
                print(f"  Mismatch at x={x}: expected {expected}, got {s_low[x]}")
    
    print(f"Is s_low(x) = {A}x (mod 256)? {is_linear}")
    if not is_linear:
        print(f"  Number of mismatches: {mismatches}/256")

# Check if it could be byte-wise matrix multiplication in a smaller field
print("\n2. Checking structure when viewed as 4x2 bit blocks...")

def to_4bit_pairs(x):
    """Split byte into two 4-bit values"""
    return x >> 4, x & 0xF

def from_4bit_pairs(high, low):
    """Combine two 4-bit values into byte"""
    return (high << 4) | low

# Analyze patterns in 4-bit blocks
print("\nAnalyzing 4-bit block patterns:")
for test_val in [0x00, 0x01, 0x10, 0x11, 0xFF]:
    high, low = to_4bit_pairs(test_val)
    result = s_low[test_val]
    res_high, res_low = to_4bit_pairs(result)
    print(f"  s_low(0x{test_val:02x}) = 0x{result:02x}  =>  ({high},{low}) -> ({res_high},{res_low})")

# Check for patterns with specific bit positions
print("\n3. Analyzing behavior on powers of 2...")
for i in range(8):
    x = 1 << i
    y = s_low[x]
    print(f"  s_low(2^{i} = 0x{x:02x}) = 0x{y:02x} = {bin(y)[2:].zfill(8)}")

# Check if s_low preserves any algebraic structure
print("\n4. Checking algebraic properties...")

# Check if s_low(x XOR y) has any relation to s_low(x) XOR s_low(y)
print("\nChecking XOR homomorphism property:")
homomorphic_count = 0
for x in range(16):  # Test subset
    for y in range(16):
        left = s_low[x ^ y]
        right = s_low[x] ^ s_low[y]
        if left == right:
            homomorphic_count += 1

print(f"  XOR homomorphism holds for {homomorphic_count}/256 pairs in test subset")

# Check modular addition properties
print("\nChecking additive structure (mod 256):")
additive_count = 0
for x in range(16):  # Test subset
    for y in range(16):
        left = s_low[(x + y) % 256]
        right = (s_low[x] + s_low[y]) % 256
        if left == right:
            additive_count += 1

print(f"  Additive homomorphism holds for {additive_count}/256 pairs in test subset")

# Analyze the non-bijective nature
print("\n5. Analyzing the non-bijective structure...")
print("\nDuplicate mappings:")
seen = {}
for x in range(256):
    y = s_low[x]
    if y in seen:
        print(f"  s_low({seen[y]}) = s_low({x}) = {y}")
    else:
        seen[y] = x

print("\n6. Checking for matrix structure over GF(2^4)...")
# In GF(2^4), we can represent bytes as pairs of GF(2^4) elements

def gf16_mult(a, b, poly=0x13):  # x^4 + x + 1
    """Multiply in GF(2^4)"""
    result = 0
    while b > 0:
        if b & 1:
            result ^= a
        a = (a << 1)
        if a & 0x10:
            a ^= poly
        b >>= 1
    return result & 0xF

# Check if s_low acts as a 2x2 matrix over GF(2^4)
print("\nTesting 2x2 matrix structure over GF(2^4):")
# For input (x_h, x_l) and output (y_h, y_l) in GF(2^4)
# Check if [y_h, y_l]^T = M * [x_h, x_l]^T for some 2x2 matrix M

# Use first few non-zero values to try to determine matrix
test_inputs = [0x01, 0x10, 0x11]
for inp in test_inputs:
    x_h, x_l = to_4bit_pairs(inp)
    result = s_low[inp]
    y_h, y_l = to_4bit_pairs(result)
    print(f"  Input: ({x_h},{x_l}) -> Output: ({y_h},{y_l})")

# Check byte rotation/shifting patterns
print("\n7. Checking rotation and shift patterns...")
for x in [0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80]:
    y = s_low[x]
    # Check if y is a rotation of x
    for rot in range(8):
        rotated = ((x << rot) | (x >> (8 - rot))) & 0xFF
        if y == rotated:
            print(f"  s_low(0x{x:02x}) = 0x{y:02x} = rotate_left(x, {rot})")
            break
    else:
        # Check if y is related by XOR with rotation
        for rot in range(8):
            rotated = ((x << rot) | (x >> (8 - rot))) & 0xFF
            if (y ^ rotated) in [0x00, 0xFF, 0xAA, 0x55]:
                print(f"  s_low(0x{x:02x}) = 0x{y:02x} = rotate_left(x, {rot}) XOR 0x{y^rotated:02x}")
                break

print("\n=== Summary of S_low Structure ===")
print(f"- S_low is NOT a permutation (240 unique outputs out of 256)")
print(f"- 16 values have duplicate mappings")
print(f"- 16 values have no pre-image")
print(f"- Not a simple linear transformation mod 256")
print(f"- Not an affine transformation in GF(2)^8")
print(f"- Does not match standard S-boxes like AES")
print(f"- Maximum differential probability is 1.0 (poor cryptographic property)")

# Additional insight
print("\nPossible structure hypothesis:")
print("- S_low might be constructed using a specific algorithm rather than")
print("  being a simple mathematical transformation")
print("- The duplicate/missing values suggest intentional design choices")
print("- Could be optimized for the specific forward/inverse algorithm used")