#!/usr/bin/env python3
import numpy as np

# The confusion matrix first half from hireme.c
confusion_first_half = [
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

# The missing values
missing_values = [0x0f, 0x11, 0x20, 0x3e, 0x44, 0x5a, 0x6b, 0x75, 0x80, 0x9e, 0xaf, 0xb1, 0xcb, 0xd5, 0xe4, 0xfa]

# Find all unique values in the first half
present_values = list(set(confusion_first_half))
present_values.sort()

print("=== Building the 5-dimensional space ===")

# First, generate the full XOR closure of the missing values
def generate_xor_closure(values):
    """Generate the XOR closure of a set of values"""
    closure = set(values)
    prev_size = 0
    
    while len(closure) != prev_size:
        prev_size = len(closure)
        new_values = set()
        for v1 in list(closure):
            for v2 in list(closure):
                new_values.add(v1 ^ v2)
        closure.update(new_values)
    
    return sorted(list(closure))

# Generate the 5-dimensional space (XOR closure of missing values)
space_5d = generate_xor_closure(missing_values)
space_5d_set = set(space_5d)

print(f"The 5-dimensional space contains {len(space_5d)} values")
print(f"Space includes: {[hex(v) for v in space_5d[:10]]}...")

# Now find all values in [0, 255] that are NOT in this space
all_values = set(range(256))
values_outside_space = sorted(list(all_values - space_5d_set))

print(f"\nValues outside the 5-dimensional space: {len(values_outside_space)} values")
print(f"First 10 values outside: {[hex(v) for v in values_outside_space[:10]]}...")

# Find which present values are outside the space
present_outside = [v for v in present_values if v not in space_5d_set]
print(f"\nPresent values outside the space: {len(present_outside)} values")

# Find pairs where BOTH elements are outside the 5-dimensional space
print("\n=== Finding pairs where both elements are outside the 5-dimensional space ===")

pairs_outside = []
for i, val1 in enumerate(values_outside_space):
    for val2 in values_outside_space[i+1:]:
        # Check if both values are present in the confusion matrix
        if val1 in present_values and val2 in present_values:
            xor_result = val1 ^ val2
            pairs_outside.append((val1, val2, xor_result))

print(f"\nFound {len(pairs_outside)} pairs where:")
print("- Both elements are present in the confusion matrix")
print("- Both elements are outside the 5-dimensional space")

# Show first 20 pairs
print("\nFirst 20 such pairs:")
for i, (v1, v2, xor) in enumerate(pairs_outside[:20]):
    in_space = "in space" if xor in space_5d_set else "NOT in space"
    in_present = "present" if xor in present_values else "missing"
    print(f"  0x{v1:02x} ⊕ 0x{v2:02x} = 0x{xor:02x} ({in_space}, {in_present})")

# Analyze the XOR results
xor_in_space = 0
xor_outside_space = 0
xor_present = 0
xor_missing = 0

for v1, v2, xor in pairs_outside:
    if xor in space_5d_set:
        xor_in_space += 1
    else:
        xor_outside_space += 1
    
    if xor in present_values:
        xor_present += 1
    else:
        xor_missing += 1

print(f"\n=== Analysis of XOR results ===")
print(f"Total pairs where both elements are outside space: {len(pairs_outside)}")
print(f"Their XOR results:")
print(f"  - In the 5-dimensional space: {xor_in_space}")
print(f"  - Outside the 5-dimensional space: {xor_outside_space}")
print(f"  - Present in confusion matrix: {xor_present}")
print(f"  - Missing from confusion matrix: {xor_missing}")

# Special case: pairs that stay outside the space when XORed
pairs_stay_outside = [(v1, v2, xor) for v1, v2, xor in pairs_outside if xor not in space_5d_set]
print(f"\nPairs that stay outside the space when XORed: {len(pairs_stay_outside)}")
if pairs_stay_outside:
    print("First 10 examples:")
    for i, (v1, v2, xor) in enumerate(pairs_stay_outside[:10]):
        in_present = "present" if xor in present_values else "missing"
        print(f"  0x{v1:02x} ⊕ 0x{v2:02x} = 0x{xor:02x} ({in_present})")

# Check closure property
print("\n=== Checking closure outside the space ===")
outside_closure = set(values_outside_space)
for v1 in values_outside_space:
    for v2 in values_outside_space:
        outside_closure.add(v1 ^ v2)

print(f"Starting with {len(values_outside_space)} values outside the space")
print(f"After XOR closure: {len(outside_closure)} values")
print(f"This means XORing values outside the space {'does NOT' if len(outside_closure) > len(values_outside_space) else 'DOES'} keep you outside")