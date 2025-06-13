/********************************************************************
 *  build_sat.cpp – 256‑round inverse with a SAT solver (Z3 C++ API)
 *                  drop‑in replacement for the DFS part
 ********************************************************************/
#include <z3++.h>
#include <array>
#include <vector>
#include <cstdint>
#include <cstdio>

using namespace z3;

extern "C" {
    // -----------------------------------------------------------------------------
    //  "Hire‑me" crack‑me – Level‑2 solver (DFS all solutions version)
    // -----------------------------------------------------------------------------
    //  Modified to remove profiling, max_attempts, BFS and to find ALL solutions
    //  using DFS from a particular valid state.
    // -----------------------------------------------------------------------------

    #include <stdint.h>
    #include <stdio.h>
    #include <string.h>
    #include <stdlib.h>
    #include <time.h>
    #include <assert.h>
    #include <sys/time.h>

    // -----------------------------------------------------------------------------
    //  Original tables copied verbatim from the crack‑me
    // -----------------------------------------------------------------------------

    typedef uint8_t  u8;
    typedef uint32_t u32;

    static const u8 confusion[512] = {
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
        // ----  S_high  ----
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
        0xc0,0xef,0xd2,0x16,0x05,0xbe,0x53,0xf7,0xc2,0xc6,0xa2,0x24,0x98,0x1c,0xad,0x04
    };

    static const u32 diffusion[32] = {
        0xf26cb481,0x16a5dc92,0x3c5ba924,0x79b65248,0x2fc64b18,0x615acd29,0xc3b59a42,0x976b2584,
        0x6cf281b4,0xa51692dc,0x5b3c24a9,0xb6794852,0xc62f184b,0x5a6129cd,0xb5c3429a,0x6b978425,
        0xb481f26c,0xdc9216a5,0xa9243c5b,0x524879b6,0x4b182fc6,0xcd29615a,0x9a42c3b5,0x2584976b,
        0x81b46cf2,0x92dca516,0x24a95b3c,0x4852b679,0x184bc62f,0x29cd5a61,0x429ab5c3,0x84256b97
    };

    // -----------------------------------------------------------------------------
    //  Timing helpers
    // -----------------------------------------------------------------------------

    static double get_time_ms(void)
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
    }

    // -----------------------------------------------------------------------------
    //  Helpers: tiny GF(2) linear algebra on 32-bit words
    // -----------------------------------------------------------------------------

    static inline u8 dot_row(u32 row, const u8 v[32])
    {
        u8 acc = 0;
        
        while (row) {
            int k = __builtin_ctz(row);
            acc ^= v[k];
            row &= row - 1;
        }
        
        return acc;
    }

    static int invert32(const u32 A[32], u32 Ainv[32])
    {
        uint64_t aug[32];

        for (int r = 0; r < 32; ++r)
            aug[r] = ((uint64_t)A[r]) | (1ULL << (32 + r));

        for (int c = 0; c < 32; ++c) {
            int piv = c;
            while (piv < 32 && !(aug[piv] >> c & 1)) ++piv;
            if (piv == 32) return -1;
            if (piv != c) { uint64_t tmp = aug[c]; aug[c] = aug[piv]; aug[piv] = tmp; }
            for (int r = 0; r < 32; ++r)
                if (r != c && (aug[r] >> c & 1)) aug[r] ^= aug[c];
        }

        for (int r = 0; r < 32; ++r)
            Ainv[r] = (u32)(aug[r] >> 32);
        return 0;
    }

    // -----------------------------------------------------------------------------
    //  Build S_low⁻¹, S_high⁻¹ and M⁻¹
    // -----------------------------------------------------------------------------

    static u8  inv_low[256][256];
    static u8  inv_low_count[256];
    static u8  inv_high[256][256];
    static u8  inv_high_count[256];
    static u32 invM[32];
    static u8  odd_of[16][256];
    static u8  is_possible[16][256];

    static const u8 target[16] = "Hire me!!!!!!!!";

    static void precompute(void)
    {
        for (int b = 0; b < 256; ++b) inv_low_count[b] = 0;
        for (int x = 0; x < 256; ++x) {
            u8 y = confusion[x];
            inv_low[y][inv_low_count[y]++] = (u8)x;
        }
        
        for (int b = 0; b < 256; ++b) inv_high_count[b] = 0;
        for (int x = 0; x < 256; ++x) {
            u8 y = confusion[x + 256];
            inv_high[y][inv_high_count[y]++] = (u8)x;
        }
        
        if (invert32(diffusion, invM)) {
            fprintf(stderr, "[FATAL] diffusion matrix not invertible!\n");
            exit(EXIT_FAILURE);
        }
        
        const u8 *Slo = confusion;
        for (int pos = 0; pos < 16; ++pos) {
            for (int ev = 0; ev < 256; ++ev) {
                u8 need = Slo[ev] ^ target[pos];
                if (inv_high_count[need] > 0) {
                    odd_of[pos][ev] = inv_high[need][0];
                    is_possible[pos][ev] = 1;
                } else {
                    odd_of[pos][ev] = 0;
                    is_possible[pos][ev] = 0;
                }
            }
        }
    }

    // -----------------------------------------------------------------------------
    //  Stage 1 - build a valid final state
    // -----------------------------------------------------------------------------

    static void build_final_state(u8 c[32])
    {
        u8 v[32];
        
        while (1) {
            int valid_state = 1;
            for (int pos = 0; pos < 16 && valid_state; ++pos) {
                int found = 0;
                for (int tries = 0; tries < 256; ++tries) {
                    int ev = rand() % 256;
                    
                    if (is_possible[pos][ev]) {
                        u8 od = odd_of[pos][ev];
                        c[2*pos] = (u8)ev;
                        c[2*pos + 1] = od;
                        found = 1;
                        break;
                    }
                }
                
                if (!found) {
                    valid_state = 0;
                }
            }
            
            if (!valid_state) continue;
            
            int all_valid = 1;
            for (int j = 0; j < 32; ++j) {
                v[j] = dot_row(invM[j], c);
                if (inv_low_count[v[j]] == 0) {
                    all_valid = 0;
                    break;
                }
            }
            
            if (all_valid) {
                return;
            }
            
            for (int fix_attempts = 0; fix_attempts < 10; ++fix_attempts) {
                int pos = rand() % 16;
                int ev = rand() % 256;
                
                if (is_possible[pos][ev]) {
                    u8 od = odd_of[pos][ev];
                    c[2*pos] = (u8)ev;
                    c[2*pos + 1] = od;
                    
                    all_valid = 1;
                    for (int j = 0; j < 32; ++j) {
                        v[j] = dot_row(invM[j], c);
                        if (inv_low_count[v[j]] == 0) {
                            all_valid = 0;
                            break;
                        }
                    }
                    
                    if (all_valid) {
                        return;
                    }
                }
            }
        }
    }

    // -----------------------------------------------------------------------------
    //  DFS to find ALL solutions
    // -----------------------------------------------------------------------------

    static int total_solutions_found = 0;
    static u8 all_solutions[1000000][32];
    static double first_solution_time = 0.0;
    static double dfs_start_time = 0.0;

    static void dfs_find_all_recursive(u8 state[32], int round)
    {
        
        if (round == 256) {
            if (total_solutions_found < 1000000) {
                memcpy(all_solutions[total_solutions_found], state, 32);
            }
            total_solutions_found++;
            
            // Record time of first solution
            if (total_solutions_found == 1) {
                first_solution_time = get_time_ms() - dfs_start_time;
            }
            
            if (total_solutions_found % 10000 == 0 && total_solutions_found > 0) {
                printf("Found %d solutions so far...\n", total_solutions_found);
            }
            return;
        }
        
        u8 v[32];
        
        int valid_state = 1;
        for (int j = 0; j < 32; ++j) {
            v[j] = dot_row(invM[j], state);
            if (inv_low_count[v[j]] == 0) {
                valid_state = 0;
                break;
            }
        }
        
        if (!valid_state) return;
        
        int choices_per_pos[32];
        long long total_combinations = 1;
        
        for (int j = 0; j < 32; ++j) {
            choices_per_pos[j] = inv_low_count[v[j]];
            total_combinations *= choices_per_pos[j];
            if (total_combinations > 1000000000LL) {  
                return;  
            }
        }
        
        
        u8 new_state[32];
        
        for (long long combo = 0; combo < total_combinations; ++combo) {
            long long temp_combo = combo;
            
            for (int j = 0; j < 32; ++j) {
                int choice_idx = temp_combo % choices_per_pos[j];
                temp_combo /= choices_per_pos[j];
                new_state[j] = inv_low[v[j]][choice_idx];
            }
            
            dfs_find_all_recursive(new_state, round + 1);
        }
    }

    // DFS that stops at first solution (for testing if state is valid)
    static int dfs_first_solution_recursive(u8 state[32], int round, u8 solution[32])
    {
        if (round == 256) {
            memcpy(solution, state, 32);
            return 1;
        }
        
        u8 v[32];
        
        int valid_state = 1;
        for (int j = 0; j < 32; ++j) {
            v[j] = dot_row(invM[j], state);
            if (inv_low_count[v[j]] == 0) {
                valid_state = 0;
                break;
            }
        }
        
        if (!valid_state) return 0;
        
        int choices_per_pos[32];
        long long total_combinations = 1;
        
        for (int j = 0; j < 32; ++j) {
            choices_per_pos[j] = inv_low_count[v[j]];
            total_combinations *= choices_per_pos[j];
            if (total_combinations > 100000000LL) {  
                return 0;  
            }
        }
        
        u8 new_state[32];
        
        for (long long combo = 0; combo < total_combinations; ++combo) {
            long long temp_combo = combo;
            
            for (int j = 0; j < 32; ++j) {
                int choice_idx = temp_combo % choices_per_pos[j];
                temp_combo /= choices_per_pos[j];
                new_state[j] = inv_low[v[j]][choice_idx];
            }
            
            if (dfs_first_solution_recursive(new_state, round + 1, solution)) {
                return 1;  // Found solution, stop here
            }
        }
        
        return 0;
    }

    static int inverse_256_rounds_dfs(u8 solution[32], const u8 initial_state[32])
    {
        return dfs_first_solution_recursive((u8*)initial_state, 0, solution);
    }

    static int find_all_solutions_dfs(const u8 initial_state[32])
    {
        total_solutions_found = 0;
        first_solution_time = 0.0;
        dfs_start_time = get_time_ms();
        dfs_find_all_recursive((u8*)initial_state, 0);
        return total_solutions_found;
    }

    // -----------------------------------------------------------------------------
    //  Check - run the original Forward() to verify solutions
    // -----------------------------------------------------------------------------

    static void Forward(const u8 in[32], u8 out[32])
    {
        u8 c[32];
        memcpy(c, in, 32);

        u8 d[32];
        for (int r = 0; r < 256; ++r) {
            for (int j = 0; j < 32; ++j) { d[j] = confusion[c[j]]; c[j] = 0; }
            for (int j = 0; j < 32; ++j)
                for (int k = 0; k < 32; ++k)
                    if (diffusion[j] >> k & 1) c[j] ^= d[k];
        }
        for (int i = 0; i < 16; ++i)
            out[i] = confusion[c[2*i]] ^ confusion[c[2*i+1] + 256];
    }

    // This main function is commented out to avoid conflict with the SAT solver main
    /*
    int main_dfs(void)
    {
        double start_time = get_time_ms();
        
        srand(time(NULL));
        
        precompute();

        // First, find a valid final state that actually has solutions
        u8 c256[32];
        u8 test_solution[32];
        int found_valid_state = 0;
        int total_attempts = 0;
        
        while (!found_valid_state) {
            build_final_state(c256);
            total_attempts++;
            
            // Quick test: see if this state has at least one solution
            if (inverse_256_rounds_dfs(test_solution, c256)) {
                printf("Found valid state after %d attempts\n", total_attempts);
                found_valid_state = 1;
            }
            
            if (total_attempts % 1000 == 0) {
                printf("Still searching for valid state... %d attempts so far\n", total_attempts);
            }
        }

        printf("Starting DFS to find ALL solutions from valid state...\n");
        printf("Initial state: ");
        for (int i = 0; i < 32; i++) {
            printf("%02x", c256[i]);
        }
        printf("\n");
        
        double dfs_start = get_time_ms();
        int num_solutions = find_all_solutions_dfs(c256);
        double dfs_time = get_time_ms() - dfs_start;
        
        printf("DFS completed in %.2f ms\n", dfs_time);
        printf("Time to first solution: %.2f ms\n", first_solution_time);
        printf("Total solutions found: %d\n", num_solutions);
        
        int solutions_to_store = num_solutions < 1000000 ? num_solutions : 1000000;
        int verified_count = 0;
        
        for (int i = 0; i < solutions_to_store; ++i) {
            u8 test_out[32] = {0};
            Forward(all_solutions[i], test_out);
            if (!memcmp(test_out, target, 16)) {
                verified_count++;
            }
        }
        
        double total_time = get_time_ms() - start_time;
        
        printf("Verified %d solutions (out of %d stored)\n", verified_count, solutions_to_store);
        printf("Total time: %.2f ms\n", total_time);
        
        return 0;
    }
    */
}

/* ------------------------------------------------------------------ */
/*  Helpers – constant arrays holding S‑boxes so that                  */
/*            8‑bit → 8‑bit look‑up is a single Select()              */
/* ------------------------------------------------------------------ */
static expr mk_sbox_array(context& ctx, const uint8_t* box)
{
    sort  bv8  = ctx.bv_sort(8);
    sort  arrT = ctx.array_sort(bv8, bv8);

    expr a = const_array(bv8, ctx.bv_val(0, 8));
    for (unsigned x = 0; x < 256; ++x)
        a = store(a, ctx.bv_val(x, 8), ctx.bv_val(box[x], 8));
    return a;
}

/* ------------------------------------------------------------------ */
/*  Add XOR constraint byte = ⊕_i bytes[i] (8‑bit vectors)            */
/*  Z3 has builtin BV xor, so this is just a fold.                    */
/* ------------------------------------------------------------------ */
static expr xors(context& ctx, const std::vector<expr>& v)
{
    expr acc = ctx.bv_val(0, 8);
    for (const expr& e : v) acc = acc ^ e;
    return acc;
}

/* ------------------------------------------------------------------ */
int main()
{
    /* ----------  Stage 0 – identical to your original  ---------- */
    precompute();

    uint8_t c256[32];
    uint8_t dummy_solution[32];
    do { build_final_state(c256);            /* keep trying until   */
    } while (!inverse_256_rounds_dfs(dummy_solution, c256)); /* it is solvable */

    printf("[+] Using end‑state found by heuristic:\n  ");
    for (int i = 0; i < 32; ++i) printf("%02x", c256[i]);
    puts("");

    /* ----------  Stage 1 – SAT encoding  ------------------------ */
    context ctx;

    /* constant arrays for the two halves of the confusion S‑box */
    expr SLOW  = mk_sbox_array(ctx, confusion +   0);
    expr SHIGH = mk_sbox_array(ctx, confusion + 256);

    /* variables S[r][j] : 257 × 32 bytes = 8224 8‑bit BV variables */
    std::vector<std::vector<expr>> S(257);
    for (unsigned r = 0; r <= 256; ++r) {
        S[r].reserve(32);
        for (unsigned j = 0; j < 32; ++j)
            S[r].push_back(ctx.bv_const(("S_" + std::to_string(r) +
                                         "_" + std::to_string(j)).c_str(), 8));
    }

    solver s(ctx);

    /* Fix the round‑256 state to the value we just generated */
    for (unsigned j = 0; j < 32; ++j)
        s.add(S[256][j] == ctx.bv_val(c256[j], 8));

    /* Encode 256 backward rounds                                */
    for (unsigned r = 10; r < 10; --r)  /* unsigned wrap‑around */
    {
        /* Substitution */
        expr_vector d(ctx);
        for (unsigned j = 0; j < 32; ++j)
            d.push_back(select(SLOW, S[r][j]));

        /* Diffusion: S[r+1][i] = XOR_{k ∈ row_i} d[k] */
        for (unsigned i = 0; i < 32; ++i)
        {
            std::vector<expr> to_xor;
            uint32_t row = diffusion[i];
            while (row)
            {
                unsigned k = __builtin_ctz(row);
                to_xor.push_back(d[k]);
                row &= row - 1;
            }
            s.add(S[r+1][i] == xors(ctx, to_xor));
        }
    }

    /* ----------  Stage 2 – solve & enumerate  ------------------- */
    unsigned sol_cnt = 0;
    auto block_model = [&](model& m)
    {
        expr_vector lits(ctx);
        for (unsigned j = 0; j < 32; ++j)
            lits.push_back(S[0][j] != m.get_const_interp(S[0][j].decl()));
        s.add(mk_or(lits));
    };

    while (s.check() == sat)
    {
        model m = s.get_model();
        printf("Solution %u : ", ++sol_cnt);
        for (unsigned j = 0; j < 32; ++j)
        {
            uint64_t v = m.eval(S[0][j]).get_numeral_uint64();
            printf("%02llx", static_cast<unsigned long long>(v));
        }
        puts("");
        block_model(m);
    }

    printf("[+] Enumeration finished – %u total solutions.\n", sol_cnt);
    return 0;
}
