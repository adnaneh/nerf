#!/usr/bin/env python3

# s_low values from confusion[0:256]
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
    0x22,0xf4,0xb9,0xa2,0x6f,0x12,0x1b,0x14,0x45,0xc7,0x87,0x31,0x60,0x29,0xf7,0x73
]

print("=== S_LOW ANALYSIS ===\n")

# 1. Fixed points: s_low(x) = x
print("1. FIXED POINTS (where s_low(x) = x):")
fixed_points = []
for x in range(256):
    if s_low[x] == x:
        fixed_points.append(x)
        print(f"   s_low(0x{x:02X}) = 0x{x:02X}")

if not fixed_points:
    print("   None found")
print()

# 2. Involutions: s_low(s_low(x)) = x (2-cycles)
print("2. INVOLUTIONS (where s_low(s_low(x)) = x, excluding fixed points):")
involutions = []
for x in range(256):
    if x not in fixed_points and s_low[x] < 256:
        if s_low[s_low[x]] == x and x < s_low[x]:  # avoid duplicates
            involutions.append((x, s_low[x]))
            print(f"   s_low(0x{x:02X}) = 0x{s_low[x]:02X} and s_low(0x{s_low[x]:02X}) = 0x{x:02X}")

if not involutions:
    print("   None found")
print()

# 3. Check if s_low maps any value to its complement
print("3. COMPLEMENT MAPPINGS (where s_low(x) = ~x = 255-x):")
complement_mappings = []
for x in range(256):
    if s_low[x] == (255 - x):
        complement_mappings.append(x)
        print(f"   s_low(0x{x:02X}) = 0x{s_low[x]:02X} = ~0x{x:02X}")

if not complement_mappings:
    print("   None found")
print()

# 4. Check for XOR relationships: s_low(x) = x XOR k for some constant k
print("4. XOR RELATIONSHIPS (checking if s_low(x) = x ⊕ k for constant k):")
xor_patterns = {}
for x in range(256):
    k = s_low[x] ^ x
    if k not in xor_patterns:
        xor_patterns[k] = []
    xor_patterns[k].append(x)

# Find the most common XOR patterns
sorted_patterns = sorted(xor_patterns.items(), key=lambda item: len(item[1]), reverse=True)
print(f"   Most common XOR differences (showing top 5):")
for k, values in sorted_patterns[:5]:
    print(f"   k=0x{k:02X}: {len(values)} values")
print()

# 5. Cycles of length > 2
print("5. LONGER CYCLES (length > 2):")
visited = [False] * 256
cycles = []
for start in range(256):
    if not visited[start]:
        cycle = []
        current = start
        while current < 256 and not visited[current]:
            visited[current] = True
            cycle.append(current)
            current = s_low[current]
        
        if len(cycle) > 2 and current == start:
            cycles.append(cycle)
            print(f"   Cycle of length {len(cycle)}: ", end="")
            print(" → ".join(f"0x{x:02X}" for x in cycle[:5]), end="")
            if len(cycle) > 5:
                print(f" → ... → 0x{cycle[-1]:02X}")
            else:
                print(f" → 0x{start:02X}")

if not cycles:
    print("   None found")
print()

# 6. Values that map to themselves + constant
print("6. ARITHMETIC RELATIONSHIPS (where s_low(x) = x + k mod 256):")
arithmetic_patterns = {}
for x in range(256):
    k = (s_low[x] - x) % 256
    if k not in arithmetic_patterns:
        arithmetic_patterns[k] = []
    arithmetic_patterns[k].append(x)

# Find patterns with multiple values
significant_patterns = [(k, values) for k, values in arithmetic_patterns.items() if len(values) > 10]
if significant_patterns:
    sorted_arith = sorted(significant_patterns, key=lambda item: len(item[1]), reverse=True)
    print(f"   Patterns with >10 values:")
    for k, values in sorted_arith[:3]:
        print(f"   s_low(x) = x + {k} (mod 256): {len(values)} values")
else:
    print("   No significant arithmetic patterns found")
print()

# 7. Special relationships with powers of 2
print("7. POWER OF 2 RELATIONSHIPS:")
for i in range(8):
    x = 1 << i  # 2^i
    print(f"   s_low(0x{x:02X}) = 0x{s_low[x]:02X}")
    if s_low[x] & (s_low[x] - 1) == 0 and s_low[x] != 0:
        print(f"      → maps to power of 2!")
print()

# 8. Check distance preservation
print("8. DISTANCE RELATIONSHIPS:")
# Check if s_low preserves any distances
distance_preserved = 0
for x in range(255):
    if abs(s_low[x+1] - s_low[x]) == 1:
        distance_preserved += 1

print(f"   Adjacent inputs mapping to adjacent outputs: {distance_preserved}/255")

# Check Hamming distance preservation
hamming_preserved = 0
for x in range(256):
    for bit in range(8):
        y = x ^ (1 << bit)  # flip one bit
        if bin(s_low[x] ^ s_low[y]).count('1') == 1:
            hamming_preserved += 1

print(f"   Single-bit changes preserved in output: {hamming_preserved}/{256*8}")