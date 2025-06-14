#include <bits/stdc++.h>
using namespace std;

const int MAX_BITS = 512;
using Poly = bitset<MAX_BITS>;

inline int deg(const Poly& p) {
    for (int i = MAX_BITS - 1; i >= 0; --i)
        if (p[i]) return i;
    return -1;
}
inline bool isConst(const Poly& p, bool one = false) {
    return one ? (p.count() == 1 && p[0]) : (p.none());
}

Poly mul(const Poly& a, const Poly& b) {
    Poly r;
    int da = deg(a);
    if (da < 0) return r;
    for (int i = 0; i <= da; ++i)
        if (a[i]) r ^= (b << i);
    return r;
}

Poly divOrRem(Poly a, const Poly& b, bool wantRem) {
    Poly q;
    int db = deg(b);
    for (int da = deg(a); da >= db; da = deg(a)) {
        int shift = da - db;
        q.set(shift);
        a ^= (b << shift);
    }
    return wantRem ? a : q;
}
inline Poly rem (Poly a, const Poly& b)        { return divOrRem(a, b, true ); }
inline Poly quo (Poly a, const Poly& b)        { return divOrRem(a, b, false); }

Poly gcd(Poly a, Poly b) {
    while (b.any()) { Poly r = rem(a, b); a = b; b = r; }
    return a;
}

vector<Poly> berlekamp(const Poly& P) {
    vector<Poly> toFactor{P}, irreducible;
    while (!toFactor.empty()) {
        Poly f = toFactor.back(); toFactor.pop_back();
        int n = deg(f);
        if (n <= 1) { irreducible.push_back(f); continue; }

        vector<Poly> xp(n);
        xp[0].set(0);
        Poly x2;  x2.set(2);
        x2 = rem(x2, f);
        for (int j = 1; j < n; ++j) xp[j] = rem(mul(xp[j-1], x2), f);

        vector<Poly> M(n, Poly());
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) if (xp[j][i]) M[i].set(j);
            M[i].flip(i);
        }

        vector<int> piv(n, -1);
        int row = 0;
        for (int col = 0; col < n; ++col) {
            int sel = -1;
            for (int r = row; r < n; ++r) if (M[r][col]) { sel = r; break; }
            if (sel == -1) continue;
            swap(M[row], M[sel]);
            piv[col] = row;
            for (int r = 0; r < n; ++r)
                if (r != row && M[r][col]) M[r] ^= M[row];
            ++row;
        }
        vector<Poly> ker;
        for (int col = 0; col < n; ++col) if (piv[col] == -1) {
            Poly v; v.set(col);
            for (int c = 0; c < n; ++c)
                if (piv[c] != -1 && M[piv[c]][col]) v.flip(c);
            ker.push_back(v);
        }

        if (ker.size() <= 1) {
            irreducible.push_back(f); continue;
        }

        bool split = false;
        int k = ker.size();
        for (unsigned mask = 1; mask < (1u << k) && !split; ++mask) {
            Poly g;
            for (int i = 0; i < k; ++i) if (mask & (1u << i)) g ^= ker[i];
            if (isConst(g, true) || g.none()) continue;
            Poly d = gcd(f, g);
            if (!d.none() && d != f) {
                toFactor.push_back(d);
                toFactor.push_back(quo(f, d));
                split = true;
            }
        }
    }
    return irreducible;
}

void dfs(int idx, const vector<Poly>& f, const Poly& P,
         int size, Poly cur, set<string>& sol) {
    if (deg(cur) >= size) return;
    if (idx == (int)f.size()) {
        Poly other = quo(P, cur);
        if (deg(other) < size) {
            auto toHex = [size](const Poly& p) {
                int words = size / 32;
                string s;
                for (int w = 0; w < words; ++w) {
                    uint32_t v = 0;
                    for (int b = 0; b < 32; ++b)
                        if (p[w*32 + b]) v |= 1u << b;
                    if (w) s.push_back(' ');
                    stringstream ss; ss << hex << setw(8) << setfill('0') << nouppercase << v;
                    s += ss.str();
                }
                return s;
            };
            sol.insert(toHex(cur) + ' ' + toHex(other));
        }
        return;
    }
    dfs(idx + 1, f, P, size, cur, sol);
    dfs(idx + 1, f, P, size, mul(cur, f[idx]), sol);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int S;
    if (!(cin >> S)) return 0;
    int W = S / 16;
    Poly B;
    for (int i = 0; i < W; ++i) {
        string token;  cin >> token;
        uint32_t word = stoul(token, nullptr, 16);
        for (int b = 0; b < 32; ++b)
            if (word & (1u << b)) B.set(i * 32 + b);
    }

    vector<Poly> irreducible = berlekamp(B);
    set<string> solutions;
    Poly one; one.set(0);
    dfs(0, irreducible, B, S, one, solutions);

    for (const string& line : solutions) cout << line << '\n';
    return 0;
}
