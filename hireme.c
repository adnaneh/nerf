#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned char u8;
typedef unsigned int u32;

u8 confusion[512]={
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
0x22,0xf4,0xb9,0xa2,0x6f,0x12,0x1b,0x14,0x45,0xc7,0x87,0x31,0x60,0x29,0xf7,0x73,
0x2c,0x97,0x72,0xcd,0x89,0xa6,0x88,0x4c,0xe8,0x83,0xeb,0x59,0xca,0x50,0x3f,0x27,
0x4e,0xae,0x43,0xd5,0x6e,0xd0,0x99,0x7b,0x7c,0x40,0x0c,0x52,0x86,0xc1,0x46,0x12,
0x5a,0x28,0xa8,0xbb,0xcb,0xf0,0x11,0x95,0x26,0x0d,0x34,0x66,0x22,0x18,0x6f,0x51,
0x9b,0x3b,0xda,0xec,0x5e,0x00,0x2a,0xf5,0x8f,0x61,0xba,0x96,0xb3,0xd1,0x30,0xdc,
0x33,0x75,0xe9,0x6d,0xc8,0xa1,0x3a,0x3e,0x5f,0x9d,0xfd,0xa9,0x31,0x9f,0xaa,0x85,
0x2f,0x92,0xaf,0x67,0x78,0xa5,0xab,0x03,0x21,0x4f,0xb9,0xad,0xfe,0xf3,0x42,0xfc,
0x17,0xd7,0xee,0xa3,0xd8,0x80,0x14,0x2e,0xa0,0x47,0x55,0xc4,0xff,0xe5,0x13,0x3f,
0x81,0xb6,0x7a,0x94,0xd0,0xb5,0x54,0xbf,0x91,0xa7,0x37,0xf1,0x6b,0xc9,0x1b,0xb1,
0x3c,0xb6,0xd9,0x32,0x24,0x8d,0xf2,0x82,0xb4,0xf9,0xdb,0x7d,0x44,0xfb,0x1e,0xd4,
0xea,0x5d,0x35,0x69,0x23,0x71,0x57,0x01,0x06,0xe4,0x55,0x9a,0xa4,0x58,0x56,0xc7,
0x4a,0x8c,0x8a,0xd6,0x6a,0x49,0x70,0xc5,0x8e,0x0a,0x62,0xdc,0x29,0x4b,0x42,0x41,
0xcb,0x2b,0xb7,0xce,0x08,0xa1,0x76,0x1d,0x1a,0xb8,0xe3,0xcc,0x7e,0x48,0x20,0xe6,
0xf8,0x45,0x93,0xde,0xc3,0x63,0x0f,0xb0,0xac,0x5c,0xba,0xdf,0x07,0x77,0xe7,0x4e,
0x1f,0x28,0x10,0x6c,0x59,0xd3,0xdd,0x2d,0x65,0x39,0xb2,0x74,0x84,0x3d,0xf4,0xbd,
0xc7,0x79,0x60,0x0b,0x4d,0x33,0x36,0x25,0xbc,0xe0,0x09,0xcf,0x5b,0xe2,0x38,0x9e,
0xc0,0xef,0xd2,0x16,0x05,0xbe,0x53,0xf7,0xc2,0xc6,0xa2,0x24,0x98,0x1c,0xad,0x04};

u32 diffusion[32]={
0xf26cb481,0x16a5dc92,0x3c5ba924,0x79b65248,0x2fc64b18,0x615acd29,0xc3b59a42,0x976b2584,
0x6cf281b4,0xa51692dc,0x5b3c24a9,0xb6794852,0xc62f184b,0x5a6129cd,0xb5c3429a,0x6b978425,
0xb481f26c,0xdc9216a5,0xa9243c5b,0x524879b6,0x4b182fc6,0xcd29615a,0x9a42c3b5,0x2584976b,
0x81b46cf2,0x92dca516,0x24a95b3c,0x4852b679,0x184bc62f,0x29cd5a61,0x429ab5c3,0x84256b97};

// Create inverse lookup tables for both halves of confusion matrix
u8 inv_confusion1[256];  // Inverse of confusion[0..255]
u8 inv_confusion2[256];  // Inverse of confusion[256..511]

// Global duplicate tracking
u8 duplicates[256];      // Array of duplicated values
int num_duplicates = 0;  // Count of duplicated values

// Matrix operations for GF(2) arithmetic
void invert_diffusion_matrix(u32 original[32], u32 inverse[32]) {
    // Create identity matrix for inverse
    for (int i = 0; i < 32; i++) {
        inverse[i] = 1U << i;
    }
    
    // Copy original to work matrix
    u32 work[32];
    for (int i = 0; i < 32; i++) {
        work[i] = original[i];
    }
    
    // Gaussian elimination
    for (int col = 0; col < 32; col++) {
        // Find pivot
        int pivot = -1;
        for (int row = col; row < 32; row++) {
            if (work[row] & (1U << col)) {
                pivot = row;
                break;
            }
        }
        
        if (pivot == -1) continue; // Singular matrix
        
        // Swap rows if needed
        if (pivot != col) {
            u32 temp = work[col];
            work[col] = work[pivot];
            work[pivot] = temp;
            
            temp = inverse[col];
            inverse[col] = inverse[pivot];
            inverse[pivot] = temp;
        }
        
        // Eliminate
        for (int row = 0; row < 32; row++) {
            if (row != col && (work[row] & (1U << col))) {
                work[row] ^= work[col];
                inverse[row] ^= inverse[col];
            }
        }
    }
}

// Test matrix multiplication in GF(2)
void test_matrix_inversion(u32 original[32], u32 inverse[32]) {
    printf("\nTesting diffusion matrix inversion:\n");
    
    // Multiply original * inverse to check if we get identity
    u32 result[32];
    for (int i = 0; i < 32; i++) {
        result[i] = 0;
        for (int j = 0; j < 32; j++) {
            if (original[i] & (1U << j)) {
                result[i] ^= inverse[j];
            }
        }
    }
    
    // Check if result is identity matrix
    int is_identity = 1;
    for (int i = 0; i < 32; i++) {
        u32 expected = 1U << i;
        if (result[i] != expected) {
            is_identity = 0;
            printf("Row %d: expected 0x%08x, got 0x%08x\n", i, expected, result[i]);
        }
    }
    
    if (is_identity) {
        printf("✓ Matrix inversion test PASSED - original * inverse = identity\n");
    } else {
        printf("✗ Matrix inversion test FAILED - original * inverse ≠ identity\n");
    }
}

void find_and_print_duplicates() {
    printf("Analyzing confusion matrix for duplicates and missing values...\n\n");
    
    // Analyze first half
    printf("=== First half analysis (confusion[0..255]) ===\n");
    
    // Count occurrences of each value in first half
    int count1[256] = {0};
    for (int i = 0; i < 256; i++) {
        count1[confusion[i]]++;
    }
    
    // Find and print duplicates for first half
    printf("Duplicated values:\n");
    num_duplicates = 0;
    for (int value = 0; value < 256; value++) {
        if (count1[value] > 1) {
            // Store in global duplicates array
            duplicates[num_duplicates++] = value;
            
            printf("  Value 0x%02x (%d) appears %d times at positions: ", 
                   value, value, count1[value]);
            
            // Print all positions where this value appears
            int first = 1;
            for (int i = 0; i < 256; i++) {
                if (confusion[i] == value) {
                    if (!first) printf(", ");
                    printf("%d", i);
                    first = 0;
                }
            }
            printf("\n");
        }
    }
    printf("Total duplicated values: %d\n\n", num_duplicates);
    
    // Find and print missing values for first half
    printf("Missing values:\n");
    int missing_count1 = 0;
    for (int value = 0; value < 256; value++) {
        if (count1[value] == 0) {
            if (missing_count1 > 0 && missing_count1 % 8 == 0) printf("\n  ");
            else if (missing_count1 > 0) printf(", ");
            else printf("  ");
            printf("0x%02x", value);
            missing_count1++;
        }
    }
    printf("\nTotal missing values: %d\n\n", missing_count1);
    
    // Analyze second half
    printf("=== Second half analysis (confusion[256..511]) ===\n");
    
    // Count occurrences of each value in second half
    int count2[256] = {0};
    for (int i = 256; i < 512; i++) {
        count2[confusion[i]]++;
    }
    
    // Find and print duplicates for second half
    printf("Duplicated values:\n");
    int duplicates2_count = 0;
    for (int value = 0; value < 256; value++) {
        if (count2[value] > 1) {
            duplicates2_count++;
            printf("  Value 0x%02x (%d) appears %d times at positions: ", 
                   value, value, count2[value]);
            
            // Print all positions where this value appears
            int first = 1;
            for (int i = 256; i < 512; i++) {
                if (confusion[i] == value) {
                    if (!first) printf(", ");
                    printf("%d", i - 256);  // Show relative position within second half
                    first = 0;
                }
            }
            printf("\n");
        }
    }
    printf("Total duplicated values: %d\n\n", duplicates2_count);
    
    // Find and print missing values for second half
    printf("Missing values:\n");
    int missing_count2 = 0;
    for (int value = 0; value < 256; value++) {
        if (count2[value] == 0) {
            if (missing_count2 > 0 && missing_count2 % 8 == 0) printf("\n  ");
            else if (missing_count2 > 0) printf(", ");
            else printf("  ");
            printf("0x%02x", value);
            missing_count2++;
        }
    }
    printf("\nTotal missing values: %d\n\n", missing_count2);
}

void create_inverse_confusion() {
    // Find and print duplicates first
    find_and_print_duplicates();
    
    // Initialize inverse arrays
    memset(inv_confusion1, 0, sizeof(inv_confusion1));
    memset(inv_confusion2, 0, sizeof(inv_confusion2));
    
    // Create inverse for first half (confusion[0..255])
    for (int i = 0; i < 256; i++) {
        inv_confusion1[confusion[i]] = i;
    }
    
    // Create inverse for second half (confusion[256..511])
    for (int i = 0; i < 256; i++) {
        inv_confusion2[confusion[256 + i]] = i;
    }
}

void Forward(u8 c[32], u8 d[32], u8 s[512], u32 p[32]) {
    for(u32 i = 0; i < 256; i++) {
        for(u8 j = 0; j < 32; j++) {
            d[j] = s[c[j]];
            c[j] = 0;
        }

        for(u8 j = 0; j < 32; j++)
            for(u8 k = 0; k < 32; k++)
                c[j] ^= d[k] * ((p[j] >> k) & 1);
    }
    for(u8 i = 0; i < 16; i++)
        d[i] = s[c[i*2]] ^ s[c[i*2+1] + 256];
}

// Check if any element of c[] contains a duplicated confusion value from first half
int contains_duplicate_confusion_value(u8 c[32], u32 iteration) {
    // Check if any element in c[] matches a pre-computed duplicate
    int found_any = 0;
    for (int i = 0; i < 32; i++) {
        for (int j = 0; j < num_duplicates; j++) {
            if (c[i] == duplicates[j]) {
                printf("Warning (iteration %u): Vector c[%d] = 0x%02x is a duplicated value in confusion matrix first half\n", iteration + 1, i, c[i]);
                found_any = 1;
                // Don't return here - continue checking all elements
            }
        }
    }
    return found_any;
}

void Backward(u8 c[32], u8 d[32], u8 s[512], u32 p_inv[32]) {
    // Print the candidate decompressed output we're working backwards from
    printf("Working backwards from candidate 32-byte state:\n");
    printf("  Hex: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x ", c[i]);
        if ((i + 1) % 16 == 0) printf("\n       ");
    }
    printf("\n");
    printf("  ASCII: \"");
    for (int i = 0; i < 32; i++) {
        if (c[i] >= 32 && c[i] <= 126) {
            printf("%c", c[i]);
        } else {
            printf(".");
        }
    }
    printf("\"\n\n");
    
    // Reverse the 256 rounds
    for(u32 i = 0; i < 256; i++) {
        u8 temp[32];
        
        // Apply inverse diffusion
        for(u8 j = 0; j < 32; j++) {
            temp[j] = 0;
            for(u8 k = 0; k < 32; k++)
                temp[j] ^= c[k] * ((p_inv[j] >> k) & 1);
        }
        
        // Print state after inverse diffusion
        printf("Iteration %u - After inverse diffusion:\n", i + 1);
        printf("  Hex: ");
        for (int j = 0; j < 32; j++) {
            printf("%02x ", temp[j]);
            if ((j + 1) % 16 == 0 && j < 31) printf("\n       ");
        }
        printf("\n");
        
        // Check if temp vector has duplicated confusion values before applying inverse
        contains_duplicate_confusion_value(temp, i);
        
        // Apply inverse confusion (first half)
        for(u8 j = 0; j < 32; j++) {
            c[j] = inv_confusion1[temp[j]];
        }
    }
}

int solve_challenge() {
    u8 target[] = "Hire me!!!!!!!!";
    
    // Print target in byte format
    printf("Target output:\n");
    printf("  Hex: ");
    for (int i = 0; i < 16; i++) {
        printf("%02x ", target[i]);
    }
    printf("\n");
    printf("  ASCII: \"%.15s\" (15 chars + null byte)\n", target);
    printf("  Length check: strlen=\"%zu\", array has 16 bytes\n\n", strlen((char*)target));
    
    // Create inverse confusion lookup tables
    create_inverse_confusion();
    
    // Create inverse diffusion matrix
    u32 diffusion_inv[32];
    invert_diffusion_matrix(diffusion, diffusion_inv);
    
    // Test that the inversion is correct
    test_matrix_inversion(diffusion, diffusion_inv);
    
    // Step 1: Work backwards from target to find the state before final confusion
    u8 pre_final[32];
    memset(pre_final, 0, sizeof(pre_final));  // Initialize to zeros
    
    printf("Attempting to find pre_final values with fixed a=82...\n");
    
    // For each output byte, we need to find the pair that produces it
    for (int i = 0; i < 16; i++) {
        u8 target_byte = target[i];
        
        // We need to find values a, b such that:
        // confusion[a] XOR confusion[b + 256] = target_byte
        
        // Try all possible values for the first element (even indices)

        int a = 82;
        int alt_b = 82;  // Alternative value for b
        u8 needed_b = target_byte ^ confusion[a];
        
        // Find if this value exists in the second half
        u8 b = inv_confusion2[needed_b];
        if (confusion[b + 256] == needed_b) {
            pre_final[i * 2] = a;
            pre_final[i * 2 + 1] = b;
            printf("Success: Position %d, target=0x%02x, a=%d (0x%02x), b=%d (0x%02x), confusion[%d]=0x%02x, confusion[%d]=0x%02x\n", 
                   i, target_byte, a, a, b, b, a, confusion[a], b + 256, confusion[b + 256]);
        } else {
            printf("Warning: For position %d, target byte 0x%02x with a=%d (confusion[%d]=0x%02x), needed_b=0x%02x is not found in confusion[256..511]\n", 
                   i, target_byte, a, a, confusion[a], needed_b);
            
            // Try with alternative b value
            printf("Trying alternative b=%d (confusion[%d]=0x%02x)...\n", alt_b, alt_b + 256, confusion[alt_b + 256]);
            
            // Calculate what 'a' we need for this alternative b
            // We need: confusion[needed_a] XOR confusion[alt_b + 256] = target_byte
            // Therefore: confusion[needed_a] = target_byte XOR confusion[alt_b + 256]
            u8 b_value = confusion[alt_b + 256];
            u8 needed_a_value = target_byte ^ b_value;
            
            printf("  Need confusion[a] = 0x%02x ^ 0x%02x = 0x%02x\n", target_byte, b_value, needed_a_value);
            
            // Find 'a' where confusion[a] = needed_a_value
            int found = 0;
            for (int search_a = 0; search_a < 256; search_a++) {
                if (confusion[search_a] == needed_a_value) {
                    pre_final[i * 2] = search_a;
                    pre_final[i * 2 + 1] = alt_b;
                    printf("Success with alternative: Position %d, target=0x%02x, a=%d (0x%02x), b=%d (0x%02x), confusion[%d]=0x%02x, confusion[%d]=0x%02x\n", 
                           i, target_byte, search_a, search_a, alt_b, alt_b, search_a, confusion[search_a], alt_b + 256, confusion[alt_b + 256]);
                    found = 1;
                    break;
                }
            }
            
            if (!found) {
                printf("Alternative also failed: needed confusion[a]=0x%02x is not found in confusion[0..255]\n", needed_a_value);
                // Set default values
                pre_final[i * 2] = 0;
                pre_final[i * 2 + 1] = 0;
            }
        }
    }
    
    // Print the resulting pre_final array
    printf("\nResulting pre_final array:\n");
    printf("  Hex: ");
    for (int i = 0; i < 32; i++) {
        printf("%02x ", pre_final[i]);
        if ((i + 1) % 16 == 0) printf("\n       ");
    }
    printf("\n");
    
    // Test that compression using the formula produces the target
    printf("\nTesting compression formula:\n");
    u8 test_compression[16];
    u8* s = confusion;  // s is the confusion array
    u8* c = pre_final;  // c is our pre_final array
    for(u8 i=0;i<16;i++)
        test_compression[i]=s[c[i*2]]^s[c[i*2+1]+256];
    
    printf("Target:      %.16s\n", target);
    printf("Compressed:  %.16s\n", test_compression);
    
    if (memcmp(test_compression, target, 16) == 0) {
        printf("✓ Compression test PASSED - pre_final values correctly produce target\n");
    } else {
        printf("✗ Compression test FAILED - pre_final values do not produce target\n");
        for (int i = 0; i < 16; i++) {
            if (test_compression[i] != target[i]) {
                printf("  Mismatch at position %d: expected 0x%02x, got 0x%02x\n", 
                       i, target[i], test_compression[i]);
            }
        }
    }
    
    // Step 2: Apply backward algorithm to get original input
    u8 original_input[32];
    memcpy(original_input, pre_final, 32);
    
    u8 dummy[32];
    Backward(original_input, dummy, confusion, diffusion_inv);
    
    // Step 3: Verify the solution
    u8 test_output[32];
    u8 test_input[32];
    memcpy(test_input, original_input, 32);
    
    Forward(test_input, test_output, confusion, diffusion);
    
    printf("Original input found:\n");
    for (int i = 0; i < 32; i++) {
        printf("0x%02x", original_input[i]);
        if (i < 31) printf(",");
        if ((i + 1) % 8 == 0) printf("\n");
    }
    
    printf("\nVerification - Target: %.16s\n", target);
    printf("Verification - Output: %.16s\n", test_output);
    
    return memcmp(test_output, target, 16);
}

void test_specific_input() {
    printf("\n=== Testing specific input ===\n");
    
    u8 input[32] = {
        0x66,0xd5,0x4e,0x28,0x5f,0xff,0x6b,0x53,0xac,0x3b,0x34,0x14,0xb5,0x3c,0xb2,0xc6,
        0xa4,0x85,0x1e,0x0d,0x86,0xc7,0x4f,0xba,0x75,0x5e,0xcb,0xc3,0x6e,0x48,0x79,0x8f
    };
    
    printf("Original input (hex):\n");
    for (int i = 0; i < 32; i++) {
        printf("%02x ", input[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    
    // Apply Forward algorithm to original
    u8 output[32];
    u8 temp_input[32];
    memcpy(temp_input, input, 32);
    Forward(temp_input, output, confusion, diffusion);
    
    printf("\nOutput after Forward (hex):\n");
    for (int i = 0; i < 16; i++) {
        printf("%02x ", output[i]);
    }
    printf("\n");
    
    printf("Output as string: \"");
    for (int i = 0; i < 16; i++) {
        if (output[i] >= 32 && output[i] <= 126) {
            printf("%c", output[i]);
        } else {
            printf(".");
        }
    }
    printf("\"\n");
    
    // Now test with reversed input
    printf("\n=== Testing reversed input ===\n");
    
    u8 reversed_input[32];
    for (int i = 0; i < 32; i++) {
        reversed_input[i] = input[31 - i];
    }
    
    printf("Reversed input (hex):\n");
    for (int i = 0; i < 32; i++) {
        printf("%02x ", reversed_input[i]);
        if ((i + 1) % 16 == 0) printf("\n");
    }
    
    // Apply Forward algorithm to reversed
    memcpy(temp_input, reversed_input, 32);
    Forward(temp_input, output, confusion, diffusion);
    
    printf("\nOutput after Forward (hex):\n");
    for (int i = 0; i < 16; i++) {
        printf("%02x ", output[i]);
    }
    printf("\n");
    
    printf("Output as string: \"");
    for (int i = 0; i < 16; i++) {
        if (output[i] >= 32 && output[i] <= 126) {
            printf("%c", output[i]);
        } else {
            printf(".");
        }
    }
    printf("\"\n");
}

int main(int argc, char* argv[]) {
    // Test specific input first
    test_specific_input();
    
    // Then run the challenge solver
    int result = solve_challenge();
    
    if (result == 0) {
        printf("\n*** SUCCESS! Solution found! ***\n");
        printf("Contact: apply@nerd.nintendo.com\n");
    } else {
        printf("\n*** FAILED! Solution incorrect ***\n");
    }
    
    return result;
}