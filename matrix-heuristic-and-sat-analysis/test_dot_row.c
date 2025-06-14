#include <stdint.h>
#include <stdio.h>
#include <string.h>
#ifdef __aarch64__
#include <arm_neon.h>
#endif

typedef uint8_t u8;
typedef uint32_t u32;

// Original dot_row
static inline u8 dot_row_original(u32 row, const u8 v[32])
{
    u8 acc = 0;
    for (int k = 0; k < 32; ++k)
        if (row >> k & 1) acc ^= v[k];
    return acc;
}

// Optimized dot_row
static inline u8 dot_row_optimized(u32 row, const u8 v[32])
{
    u8 acc = 0;
    while (row) {
        int k = __builtin_ctz(row);
        acc ^= v[k];
        row &= row - 1;
    }
    return acc;
}

#ifdef __aarch64__
// ARM NEON version
static inline u8 dot_row_neon(u32 row, const u8 v[32])
{
    // Load the 32-byte state into NEON registers (2 x 128-bit)
    uint8x16_t state_low = vld1q_u8(v);
    uint8x16_t state_high = vld1q_u8(v + 16);
    
    // Create mask vectors based on the row bits
    u8 mask_array[32];
    for (int i = 0; i < 32; i++) {
        mask_array[i] = (row >> i) & 1 ? 0xFF : 0x00;
    }
    uint8x16_t mask_low = vld1q_u8(mask_array);
    uint8x16_t mask_high = vld1q_u8(mask_array + 16);
    
    // AND the state with the mask
    uint8x16_t masked_low = vandq_u8(state_low, mask_low);
    uint8x16_t masked_high = vandq_u8(state_high, mask_high);
    
    // XOR the two halves together
    uint8x16_t result = veorq_u8(masked_low, masked_high);
    
    // Horizontal XOR reduction
    uint8x8_t low = vget_low_u8(result);
    uint8x8_t high = vget_high_u8(result);
    uint8x8_t xor8 = veor_u8(low, high);
    
    // Continue reducing - unroll the loop since vget_lane needs constant index
    u8 acc = vget_lane_u8(xor8, 0);
    acc ^= vget_lane_u8(xor8, 1);
    acc ^= vget_lane_u8(xor8, 2);
    acc ^= vget_lane_u8(xor8, 3);
    acc ^= vget_lane_u8(xor8, 4);
    acc ^= vget_lane_u8(xor8, 5);
    acc ^= vget_lane_u8(xor8, 6);
    acc ^= vget_lane_u8(xor8, 7);
    
    return acc;
}
#endif

int main(void)
{
    // Test vectors
    u8 test_v[32];
    for (int i = 0; i < 32; i++) {
        test_v[i] = (u8)(i * 7 + 3);  // Some test pattern
    }
    
    u32 test_rows[] = {0x12345678, 0xFFFFFFFF, 0x00000000, 0x80000001, 0xAAAAAAAA};
    
    printf("Testing dot_row implementations:\n");
    
    for (int i = 0; i < 5; i++) {
        u32 row = test_rows[i];
        u8 result_original = dot_row_original(row, test_v);
        u8 result_optimized = dot_row_optimized(row, test_v);
#ifdef __aarch64__
        u8 result_neon = dot_row_neon(row, test_v);
        printf("Row 0x%08X: original=%02X, optimized=%02X, neon=%02X\n", 
               row, result_original, result_optimized, result_neon);
        if (result_original != result_optimized || result_original != result_neon) {
            printf("ERROR: Results don't match!\n");
            return 1;
        }
#else
        printf("Row 0x%08X: original=%02X, optimized=%02X\n", 
               row, result_original, result_optimized);
        if (result_original != result_optimized) {
            printf("ERROR: Results don't match!\n");
            return 1;
        }
#endif
    }
    
    printf("All tests passed!\n");
    return 0;
}