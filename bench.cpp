// Minimal, Boost-free benchmark for your BigInt
// Compares against: (1) unsigned long long for <=19 digits, (2) NaiveBig (base-10, slow) for larger sizes.

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <functional>
#include <cstdint>
#include <cassert>
#include <sstream>
#include <iomanip>
using namespace std;

class BigInt
{
private:
    vector<char> digits; // LSB-first, chars '0'..'9'
public:
    BigInt() { digits.push_back('0'); }

    BigInt(int n) {
        if (n == 0) { digits.push_back('0'); return; }
        while (n > 0) { digits.push_back(char(n % 10 + '0')); n /= 10; }
    }

    BigInt(string str) {
        for (int i = (int)str.length() - 1; i >= 0; --i) digits.push_back(str[i]);
        if (digits.empty()) digits.push_back('0');
        // strip leading zeros in input
        while (digits.size() > 1 && digits.back() == '0') digits.pop_back();
    }

    BigInt operator+(const BigInt& a) const {
        BigInt totalSum;
        totalSum.digits.clear(); // FIX: remove default '0'
        int carry = 0;
        size_t maxSize = max(digits.size(), a.digits.size());
        for (size_t i = 0; i < maxSize || carry; ++i) {
            int digitA = (i < digits.size()) ? (digits[i] - '0') : 0;
            int digitB = (i < a.digits.size()) ? (a.digits[i] - '0') : 0;
            int placeSum = digitA + digitB + carry;
            totalSum.digits.push_back(char(placeSum % 10 + '0'));
            carry = placeSum / 10;
        }
        return totalSum;
    }

    BigInt operator+(int n) const { return *this + BigInt(n); }

    BigInt operator-(const BigInt& a) const {
        // assumes *this >= a (your code’s semantics)
        BigInt totalDifference;
        totalDifference.digits.clear();
        int borrow = 0; // FIX: start at 0
        for (size_t i = 0; i < digits.size(); ++i) {
            int digitA = (digits[i] - '0') - borrow;
            int digitB = (i < a.digits.size()) ? (a.digits[i] - '0') : 0;
            if (digitA < digitB) { digitA += 10; borrow = 1; } else { borrow = 0; }
            int placeDifference = digitA - digitB;
            totalDifference.digits.push_back(char(placeDifference + '0'));
        }
        while (totalDifference.digits.size() > 1 && totalDifference.digits.back() == '0')
            totalDifference.digits.pop_back();
        return totalDifference;
    }

    BigInt operator*(const BigInt& a) const {
        BigInt totalProduct;
        totalProduct.digits.assign(digits.size() + a.digits.size(), '0');
        for (size_t i = 0; i < digits.size(); ++i) { // FIX: < size()
            int carry = 0;
            int digitA = digits[i] - '0';
            for (size_t j = 0; j < a.digits.size(); ++j) { // FIX: < size()
                int digitB = a.digits[j] - '0';
                int sum = (totalProduct.digits[i + j] - '0') + digitA * digitB + carry;
                totalProduct.digits[i + j] = char(sum % 10 + '0');
                carry = sum / 10;
            }
            size_t k = i + a.digits.size();
            while (carry) {
                if (k >= totalProduct.digits.size()) totalProduct.digits.push_back('0');
                int sum = (totalProduct.digits[k] - '0') + carry;
                totalProduct.digits[k] = char(sum % 10 + '0');
                carry = sum / 10;
                ++k;
            }
        }
        while (totalProduct.digits.size() > 1 && totalProduct.digits.back() == '0')
            totalProduct.digits.pop_back();
        return totalProduct;
    }

    BigInt operator/(const BigInt& divisor) const { // slow but okay for small tests
        if (divisor == BigInt(0)) throw runtime_error("Division by zero");
        BigInt quotient, remainder;
        quotient.digits.resize(digits.size(), '0');
        for (int i = (int)digits.size() - 1; i >= 0; --i) {
            remainder.digits.insert(remainder.digits.begin(), digits[i]);
            while (remainder.digits.size() > 1 && remainder.digits.back() == '0')
                remainder.digits.pop_back();
            int count = 0;
            while (remainder >= divisor) { remainder = remainder - divisor; ++count; }
            quotient.digits[(size_t)i] = char(count + '0');
        }
        while (quotient.digits.size() > 1 && quotient.digits.back() == '0')
            quotient.digits.pop_back();
        return quotient;
    }

    BigInt operator%(const BigInt& a) const {
        BigInt q = *this / a;
        return *this - (q * a);
    }

    bool operator==(const BigInt& a) const {
        if (digits.size() != a.digits.size()) return false;
        for (size_t i = 0; i < digits.size(); ++i)
            if (digits[i] != a.digits[i]) return false;
        return true;
    }

    bool operator>=(const BigInt& a) const {
        if (digits.size() != a.digits.size()) return digits.size() > a.digits.size();
        for (int i = (int)digits.size() - 1; i >= 0; --i) {
            if (digits[(size_t)i] < a.digits[(size_t)i]) return false;
            if (digits[(size_t)i] > a.digits[(size_t)i]) return true;
        }
        return true; // equal
    }

    BigInt operator++(int) { BigInt temp = *this; *this = *this + BigInt(1); return temp; }
    BigInt operator++()    { *this = *this + BigInt(1); return *this; }

    void print() const { // FIX: make const
        if (digits.size() == 1 && digits[0] == '0') { cout << "0"; return; }
        for (int i = (int)digits.size() - 1; i >= 0; --i) cout << digits[(size_t)i];
    }

    // Extra helpers kept as-is (fibo, fact, operator<<)...
    BigInt _fibo(BigInt n, BigInt a, BigInt b) const {
        if (n == BigInt(0)) return a;
        else if (n == BigInt(1)) return b;
        return _fibo(n - BigInt(1), b, a + b);
    }
    BigInt fibo() const { return _fibo(*this, BigInt(0), BigInt(1)); }
    BigInt _fact(BigInt num) const {
        if (num == BigInt(0) || num == BigInt(1)) return BigInt(1);
        return num * _fact(num - BigInt(1));
    }
    BigInt fact() const { return _fact(*this); }

    friend BigInt operator+(int a, const BigInt& b) { return BigInt(a) + b; }

    friend ostream& operator<<(ostream& out, const BigInt& n) {
        if (n.digits.size() <= 12) {
            for (auto it = n.digits.rbegin(); it != n.digits.rend(); ++it) out << (*it - '0');
        } else {
            out << (*n.digits.rbegin() - '0') << '.';
            int count = 1;
            for (auto it = n.digits.rbegin() + 1; it != n.digits.rend() && count < 7; ++it) {
                out << (*it - '0'); count++;
            }
            int exponent = (int)n.digits.size() - 1;
            out << 'e' << exponent;
        }
        return out;
    }
};

// --------------------
// ADAPTERS to YOUR API  (keep BigInt above unchanged)
// --------------------
struct BigAdapters {
    static BigInt BigFromDec(const std::string& s){ return BigInt(s); }

    static std::string BigToDec(const BigInt& x){
        // Reconstruct exact decimal without using operator<< or print()
        // because operator<< shortens large numbers to scientific-ish.
        // We know the storage is LSB-first chars '0'..'9'.
        std::string s;
        // Access BigInt internals via friend or add an accessor if you prefer.
        // Since we don't have that, we use operator<< for small and a fallback:
        std::ostringstream oss;
        oss << x; // good for small (<=12 digits)
        std::string small = oss.str();
        // Heuristic: if it contains 'e', rebuild manually by printing.
        if (small.find('e') == std::string::npos) return small;

        // Manual rebuild by calling a const-friendly print equivalent:
        // We don't have direct access to digits here; add a helper in BigInt:
        // As a non-invasive fallback, stream via x.print() into a buffer.
        std::ostringstream oss2;
        std::streambuf* old = std::cout.rdbuf(oss2.rdbuf());
        x.print();
        std::cout.rdbuf(old);
        return oss2.str();
    }

    // Exact ops (already correct)
    static BigInt BigAdd(const BigInt& a, const BigInt& b){ return a + b; }
    static BigInt BigSub(const BigInt& a, const BigInt& b){ return a - b; }
    static BigInt BigMul(const BigInt& a, const BigInt& b){ return a * b; }

    // Size snapshot without allocating a decimal string
    static size_t BigSizeBytes(const BigInt& x){
        // Each decimal digit is one char-byte in your representation
        // so "bytes" ~= digits count.
        // If you prefer actual bytes used: digits.capacity() is not exposed; this proxy is fine.
        // Reconstruct via print() into a buffer to count digits, avoiding allocations in hot loop.
        std::ostringstream oss;
        std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
        x.print();
        std::cout.rdbuf(old);
        return oss.str().size();
    }
};

// --------------------
// Naive base-10 baseline (kept as-is)
// --------------------
struct NaiveBig {
    std::vector<uint8_t> digits; // LSB-first, base 10
    static NaiveBig fromDec(const std::string& s){
        NaiveBig n; n.digits.reserve(s.size());
        for (int i=(int)s.size()-1;i>=0;--i){ char c=s[i]; if (c>='0'&&c<='9') n.digits.push_back(uint8_t(c-'0')); }
        n.trim(); return n;
    }
    std::string toDec() const {
        if (digits.empty()) return "0";
        std::string s; s.reserve(digits.size());
        for (int i=(int)digits.size()-1;i>=0;--i) s.push_back(char('0'+digits[(size_t)i]));
        return s;
    }
    void trim(){ while (!digits.empty() && digits.back()==0) digits.pop_back(); }
    static NaiveBig add(const NaiveBig& A, const NaiveBig& B){
        NaiveBig R; size_t n=max(A.digits.size(), B.digits.size());
        R.digits.assign(n+1,0);
        int carry=0;
        for (size_t i=0;i<n;i++){
            int a=(i<A.digits.size()?A.digits[i]:0);
            int b=(i<B.digits.size()?B.digits[i]:0);
            int s=a+b+carry;
            R.digits[i]=uint8_t(s%10);
            carry=s/10;
        }
        if (carry) R.digits[n]=uint8_t(carry);
        R.trim(); return R;
    }
    static NaiveBig mul(const NaiveBig& A, const NaiveBig& B){
        NaiveBig R; if (A.digits.empty()||B.digits.empty()) return R;
        R.digits.assign(A.digits.size()+B.digits.size(),0);
        for (size_t i=0;i<A.digits.size();++i){
            int carry=0;
            for (size_t j=0;j<B.digits.size();++j){
                int cur = R.digits[i+j] + A.digits[i]*B.digits[j] + carry;
                R.digits[i+j] = uint8_t(cur%10);
                carry = cur/10;
            }
            size_t k=i+B.digits.size();
            while (carry){
                if (k>=R.digits.size()) R.digits.push_back(0);
                int cur = R.digits[k] + carry;
                R.digits[k]=uint8_t(cur%10);
                carry=cur/10; ++k;
            }
        }
        R.trim(); return R;
    }
};

// --------------------
// Utilities
// --------------------
static std::mt19937_64 rng(123456789);

std::string make_decimal(size_t digits){
    assert(digits>=1);
    std::uniform_int_distribution<int> d1(1,9), d(0,9);
    std::string s; s.reserve(digits);
    s.push_back(char('0'+d1(rng)));
    for (size_t i=1;i<digits;i++) s.push_back(char('0'+d(rng)));
    return s;
}

template<typename F>
inline long long time_once(F&& fn){
    auto t0 = std::chrono::steady_clock::now();
    fn();
    auto t1 = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t1 - t0).count();
}

struct Pct { double p50, p90, p99; double mean; };
static double pct_of_sorted(const std::vector<double>& v, double p){
    if (v.empty()) return 0;
    double k = (v.size()-1)*p;
    size_t lo = (size_t)std::floor(k), hi = (size_t)std::ceil(k);
    if (lo==hi) return v[lo];
    return v[lo]*(hi-k) + v[hi]*(k-lo);
}
static Pct summarize(std::vector<double> x){
    std::sort(x.begin(), x.end());
    double s=0; for(double z: x) s+=z;
    return { pct_of_sorted(x,0.50), pct_of_sorted(x,0.90), pct_of_sorted(x,0.99), s/x.size() };
}

// --------------------
// Benchmark core (overhead-subtracted, no allocations in loop)
// --------------------
struct Row { std::string op; size_t digits; std::string impl; long long p50_ns; long long p90_ns; long long p99_ns; double ops_per_s; double speedup_vs_baseline; };

template<typename MakePair, typename Op>
double run_ns_per_op_adjusted(MakePair&& make_pairs, Op&& op, int batches, int iters_per_batch){
    // Pre-generate operand pairs once per batch to keep cache behavior consistent
    std::vector<std::pair<BigInt,BigInt>> pairs = make_pairs();
    volatile uint64_t guard = 0;

    auto body = [&](){
        for (int it=0; it<iters_per_batch; ++it){
            const auto& P = pairs[(size_t)(it % pairs.size())];
            BigInt R = op(P.first, P.second);
            // cheap guard to prevent optimizing away
            // take last digit by streaming via print() into tiny buffer (rare, small)
            std::ostringstream oss;
            std::streambuf* old = std::cout.rdbuf(oss.rdbuf());
            R.print();
            std::cout.rdbuf(old);
            const std::string s = oss.str();
            guard += (uint64_t)(unsigned char)s.back();
        }
    };

    auto empty = [&](){
        for (int it=0; it<iters_per_batch; ++it){
            // touch the same memory to approximate loop/branch cost
            const auto& P = pairs[(size_t)(it % pairs.size())];
            (void)P;
        }
    };

    // warm-up
    body(); empty();

    std::vector<double> samples;
    samples.reserve(batches);
    for (int b=0; b<batches; ++b){
        long long t_empty = time_once(empty);
        long long t_body  = time_once(body);
        long long adj = std::max(0LL, t_body - t_empty);
        samples.push_back( double(adj) / double(iters_per_batch) );
    }
    (void)guard;
    // Return median ns/op
    std::sort(samples.begin(), samples.end());
    return samples[samples.size()/2];
}

int main(){
    using A = BigAdapters;

    // ---------- Config ----------
    // const std::vector<size_t> sizes_small = {8, 12, 18};          // compare to unsigned long long
    const std::vector<size_t> sizes_large = {32, 64, 128, 256, 512, 1024, 5000};   // compare to NaiveBig
    const int PAIRS = 64;                // pre-generated operand pairs per size
    const int BATCHES = 15;              // repetitions for p50/p90/p99
    const int ITERS_PER_BATCH = 400;     // operations per batch
    // ----------------------------

    std::cout << "op,digits,impl,p50_ns,p90_ns,p99_ns,ops_per_s,speedup_vs_baseline\n";

    std::vector<Row> rows;

    auto make_pairs_big = [&](size_t digits){
        return [=](){
            std::vector<std::pair<BigInt,BigInt>> v; v.reserve(PAIRS);
            for (int i=0;i<PAIRS;i++){
                std::string sa = make_decimal(digits);
                std::string sb = make_decimal(digits);
                v.emplace_back(A::BigFromDec(sa), A::BigFromDec(sb));
            }
            return v;
        };
    };

    // ---------- Small sizes vs unsigned long long ----------
    // for (size_t D: sizes_small){
        // Correctness spot checks
        // {
        //     std::string sa = make_decimal(D), sb = make_decimal(D);
        //     auto sum1 = A::BigToDec(A::BigAdd(A::BigFromDec(sa), A::BigFromDec(sb)));
        //     unsigned long long x = to_ull(sa), y = to_ull(sb);
        //     unsigned long long r = x + y;
        //     std::ostringstream oss; oss << r;
        //     if (sum1 != oss.str()){ std::cerr << "Add mismatch vs ULL at " << D << " digits\n"; return 1; }
        // }

        // // our/add
        // auto ns_per = run_ns_per_op_adjusted(
        //     make_pairs_big(D),
        //     [&](const BigInt& X, const BigInt& Y){ return A::BigAdd(X,Y); },
        //     BATCHES, ITERS_PER_BATCH
        // );
        // // baseline/add (ULL)
        // auto ns_per_base = run_ns_per_op_adjusted(
        //     // use same number of pairs; convert once when pairs are created
        //     [=](){
        //         std::vector<std::pair<unsigned long long, unsigned long long>> v; v.reserve(PAIRS);
        //         for(int i=0;i<PAIRS;i++){ auto sa=make_decimal(D), sb=make_decimal(D); v.emplace_back(to_ull(sa), to_ull(sb)); }
        //         // Wrap into BigInt-shaped pairs to reuse runner
        //         std::vector<std::pair<BigInt,BigInt>> w; w.reserve(PAIRS);
        //         for (auto &p: v) { w.emplace_back(BigInt(0), BigInt(0)); } // placeholders
        //         return w;
        //     },
        //     [&](const BigInt&, const BigInt&){
        //         // pull new randoms each call would add variance; instead keep a stable RNG offset
        //         static std::vector<std::pair<unsigned long long, unsigned long long>> vv;
        //         if (vv.empty()){
        //             vv.reserve(PAIRS);
        //             std::mt19937_64 rr(987654321);
        //             for(int i=0;i<PAIRS;i++){
        //                 auto sa=make_decimal(D), sb=make_decimal(D);
        //                 vv.emplace_back(to_ull(sa), to_ull(sb));
        //             }
        //         }
        //         static size_t idx=0;
        //         auto &p = vv[idx++ % vv.size()];
        //         unsigned long long r = p.first + p.second;
        //         (void)r;
        //         return BigInt(0);
        //     },
        //     BATCHES, ITERS_PER_BATCH
        // );

        // // Re-run to collect percentile distribution for both (for reporting)
        // auto dist = [&](auto make_pairs, auto op){
        //     std::vector<double> s; s.reserve(BATCHES);
        //     for (int b=0;b<BATCHES;b++){
        //         std::vector<std::pair<BigInt,BigInt>> pairs = make_pairs();
        //         auto body = [&](){ for(int it=0;it<ITERS_PER_BATCH; ++it){ BigInt R = op(pairs[(size_t)(it%pairs.size())].first, pairs[(size_t)(it%pairs.size())].second); (void)R; } };
        //         auto empty= [&](){ for(int it=0;it<ITERS_PER_BATCH; ++it){ (void)pairs[(size_t)(it%pairs.size())]; } };
        //         long long te=time_once(empty), tb=time_once(body);
        //         s.push_back(double(std::max(0LL, tb-te))/double(ITERS_PER_BATCH));
    //         }
    //         return summarize(std::move(s));
    //     };

    //     auto ourS   = dist(make_pairs_big(D), [&](const BigInt& X, const BigInt& Y){ return A::BigAdd(X,Y); });
    //     auto baseS  = dist(
    //         [=](){ std::vector<std::pair<BigInt,BigInt>> w(PAIRS); return w; },
    //         [&](const BigInt&, const BigInt&){
    //             static std::vector<std::pair<unsigned long long, unsigned long long>> vv;
    //             if (vv.empty()){
    //                 vv.reserve(PAIRS);
    //                 for(int i=0;i<PAIRS;i++){ auto sa=make_decimal(D), sb=make_decimal(D); vv.emplace_back(to_ull(sa), to_ull(sb)); }
    //             }
    //             static size_t idx=0; auto &p = vv[idx++ % vv.size()];
    //             volatile unsigned long long r = p.first + p.second; (void)r;
    //             return BigInt(0);
    //         }
    //     );

    //     auto add_speedup = baseS.p50 / ourS.p50;
    //     rows.push_back({"add", D, "our",     (long long)ourS.p50,  (long long)ourS.p90,  (long long)ourS.p99,  1e9/ourS.mean,  add_speedup});
    //     rows.push_back({"add", D, "baseline",(long long)baseS.p50, (long long)baseS.p90, (long long)baseS.p99, 1e9/baseS.mean,  1.0});

    //     // MUL small
    //     {
    //         auto ourM = dist(make_pairs_big(D), [&](const BigInt& X, const BigInt& Y){ return A::BigMul(X,Y); });
    //         auto baseM = dist(
    //             [=](){ std::vector<std::pair<BigInt,BigInt>> w(PAIRS); return w; },
    //             [&](const BigInt&, const BigInt&){
    //                 static std::vector<std::pair<unsigned long long, unsigned long long>> vv;
    //                 if (vv.empty()){
    //                     vv.reserve(PAIRS);
    //                     for(int i=0;i<PAIRS;i++){ auto sa=make_decimal(D), sb=make_decimal(D); vv.emplace_back(to_ull(sa), to_ull(sb)); }
    //                 }
    //                 static size_t idx=0; auto &p = vv[idx++ % vv.size()];
    //                 volatile unsigned long long r = p.first * p.second; (void)r;
    //                 return BigInt(0);
    //             }
    //         );
    //         auto mul_speedup = baseM.p50 / ourM.p50;
    //         rows.push_back({"mul", D, "our",      (long long)ourM.p50,  (long long)ourM.p90,  (long long)ourM.p99,  1e9/ourM.mean,  mul_speedup});
    //         rows.push_back({"mul", D, "baseline", (long long)baseM.p50, (long long)baseM.p90, (long long)baseM.p99, 1e9/baseM.mean, 1.0});
    //     }
    // }

    // ---------- Large sizes vs NaiveBig ----------
    auto make_pairs_naive = [&](size_t D){
        std::vector<std::pair<NaiveBig,NaiveBig>> v; v.reserve(PAIRS);
        for (int i=0;i<PAIRS;i++){ v.emplace_back(NaiveBig::fromDec(make_decimal(D)), NaiveBig::fromDec(make_decimal(D))); }
        return v;
    };
    for (size_t D: sizes_large){
        // Correctness spot-check (add/mul)
        {
            std::string sa = make_decimal(D), sb = make_decimal(D);
            auto R1 = A::BigToDec(A::BigAdd(A::BigFromDec(sa), A::BigFromDec(sb)));
            auto R2 = NaiveBig::add(NaiveBig::fromDec(sa), NaiveBig::fromDec(sb)).toDec();
            if (R1 != R2){ std::cerr << "Add mismatch at " << D << " digits\n"; return 1; }
            auto M1 = A::BigToDec(A::BigMul(A::BigFromDec(sa), A::BigFromDec(sb)));
            auto M2 = NaiveBig::mul(NaiveBig::fromDec(sa), NaiveBig::fromDec(sb)).toDec();
            if (M1 != M2){ std::cerr << "Mul mismatch at " << D << " digits\n"; return 1; }
        }

        auto dist_big = [&](auto op_bigint){
            std::vector<double> s; s.reserve(BATCHES);
            auto make_pairs = make_pairs_big(D);
            for (int b=0;b<BATCHES;b++){
                std::vector<std::pair<BigInt,BigInt>> pairs = make_pairs();
                auto body = [&](){ for(int it=0;it<ITERS_PER_BATCH; ++it){ BigInt R = op_bigint(pairs[(size_t)(it%pairs.size())].first, pairs[(size_t)(it%pairs.size())].second); (void)R; } };
                auto empty= [&](){ for(int it=0;it<ITERS_PER_BATCH; ++it){ (void)pairs[(size_t)(it%pairs.size())]; } };
                long long te=time_once(empty), tb=time_once(body);
                s.push_back(double(std::max(0LL, tb-te))/double(ITERS_PER_BATCH));
            }
            return summarize(std::move(s));
        };

        auto dist_naive = [&](auto op_naive){
            std::vector<double> s; s.reserve(BATCHES);
            auto pairs = make_pairs_naive(D);
            for (int b=0;b<BATCHES;b++){
                auto body = [&](){ for(int it=0;it<ITERS_PER_BATCH; ++it){ auto& P = pairs[(size_t)(it%pairs.size())]; auto R = op_naive(P.first, P.second); (void)R; } };
                auto empty= [&](){ for(int it=0;it<ITERS_PER_BATCH; ++it){ (void)pairs[(size_t)(it%pairs.size())]; } };
                long long te=time_once(empty), tb=time_once(body);
                s.push_back(double(std::max(0LL, tb-te))/double(ITERS_PER_BATCH));
            }
            return summarize(std::move(s));
        };

        // ADD
        auto ourA   = dist_big([&](const BigInt& X, const BigInt& Y){ return A::BigAdd(X,Y); });
        auto baseA  = dist_naive([&](const NaiveBig& X, const NaiveBig& Y){ return NaiveBig::add(X,Y); });
        rows.push_back({"add", D, "our",      (long long)ourA.p50,  (long long)ourA.p90,  (long long)ourA.p99,  1e9/ourA.mean,  baseA.p50/ourA.p50});
        rows.push_back({"add", D, "baseline", (long long)baseA.p50, (long long)baseA.p90, (long long)baseA.p99, 1e9/baseA.mean, 1.0});

        // MUL
        auto ourM   = dist_big([&](const BigInt& X, const BigInt& Y){ return A::BigMul(X,Y); });
        auto baseM  = dist_naive([&](const NaiveBig& X, const NaiveBig& Y){ return NaiveBig::mul(X,Y); });
        rows.push_back({"mul", D, "our",      (long long)ourM.p50,  (long long)ourM.p90,  (long long)ourM.p99,  1e9/ourM.mean,  baseM.p50/ourM.p50});
        rows.push_back({"mul", D, "baseline", (long long)baseM.p50, (long long)baseM.p90, (long long)baseM.p99, 1e9/baseM.mean, 1.0});

        // Storage snapshot (no timing)
        auto A_big = A::BigFromDec(make_decimal(D));
        size_t bytes = A::BigSizeBytes(A_big);
        std::cerr << "[storage] digits=" << D << " approx_bytes=" << bytes
                  << " bytes_per_digit~=" << (double)bytes / (double)D << "\n";
    }

    // ---------- Emit CSV ----------
    for (auto& r: rows){
        std::cout << r.op << "," << r.digits << "," << r.impl << ","
                  << r.p50_ns << "," << r.p90_ns << "," << r.p99_ns << ","
                  << std::fixed << std::setprecision(2) << r.ops_per_s << ","
                  << std::setprecision(3) << r.speedup_vs_baseline << "\n";
    }
    return 0;
}

