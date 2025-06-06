#!/usr/bin/env python3

# Generate 32-byte LUT for Apple Silicon optimization
# Each LUT entry: bit i set -> bytes [i, i+8, i+16, i+24] = 0xFF, others = 0x00

print("// Auto-generated 32-byte LUT")
print("__attribute__((aligned(16), section(\"__TEXT_CONST,constant_mask\")))")
print("static const uint8_t mask32_lut[256][32] = {")

for mask in range(256):
    entry = [0x00] * 32
    for bit in range(8):
        if mask & (1 << bit):
            # Set bytes at positions: bit, bit+8, bit+16, bit+24
            entry[bit] = 0xFF
            entry[bit + 8] = 0xFF
            entry[bit + 16] = 0xFF
            entry[bit + 24] = 0xFF
    
    line = "    {" + ",".join(f"0x{b:02X}" for b in entry) + "}"
    if mask < 255:
        line += ","
    print(line)

print("};")