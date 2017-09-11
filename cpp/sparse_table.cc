#include <vector>
#include <cassert>
#include <cstring>
#include <iostream>
#include <limits>
#include <type_traits>
#include <random>
using namespace std;

namespace st_impl {

template <class T, class F>
class SparseTable {
public:
    typedef F func_type;
    typedef unsigned size_type;
    typedef T value_type;

    SparseTable(const vector<T>& init) : _size(init.size()), _idx_size(flsl(_size)) {
        table.resize(_size);
        for (auto& row : table) {
            row.resize(_idx_size, func_type::default_value);
        }

        // initialize sparse table
        for (size_type i = 0; i < _size; ++i) {
            table[i][0] = init[i];
        }
        for (size_type j = 1; j < _idx_size; ++j) {
            for (size_type i = 0; i <= _size - (1 << j); ++i) {
                table[i][j] = f(table[i][j - 1], table[i + (1 << (j - 1))][j - 1]);
            }
        }
    }

    SparseTable(const initializer_list<T>& init) : SparseTable(vector<T>(init)) {}

    SparseTable(const vector<T>& init, F f) : SparseTable(init) { this->f = f; }
    SparseTable(const initializer_list<T>& init, F f) : SparseTable(vector<T>(init), f) {}

    T rangeQuery(size_type l, size_type r) const {
        if (!(l <= r && r < _size)) {
            throw std::out_of_range("Bad query!");
        }

        // if the function is idempotent, which means f(x, x) = x holds for
        // all x with definition, then we can deduce that
        // f(range(l, s), range(t, r)) == f(range(l, r)) always
        // holds for all l, s, t, r which satisfies l <= t && s <= r && t <= s + 1
        // then rangeQuery will be executed in O(1).
        // otherwise it should be finished in O(lgN).
        if (func_type::idempotent) {
            size_type idx = flsl(r - l + 1) - 1;
            return f(table[l][idx], table[r - (1 << idx) + 1][idx]);
        } else {
            T res = func_type::default_value;
            for (size_type i = 0; i < _idx_size; ++i) {
                size_type idx = _idx_size - 1 - i;
                if (l + (1 << idx) - 1 <= r) {
                    res = f(res, table[l][idx]);
                    l += 1 << idx;
                }
            }
            return res;
        }
    }

private:
    func_type f;

    size_type _size;
    size_type _idx_size;
    vector<vector<T>> table;
};

}  // namespace st_impl

template <class T, T v = T{}>
struct sum_f {
    static constexpr T default_value = v;
    static constexpr bool idempotent = false;
    T operator()(const T& a, const T& b) const { return a + b; }
};
template <class T, T v>
constexpr const T sum_f<T, v>::default_value;

template <class T, T v = numeric_limits<T>::min(),
          typename = typename enable_if<numeric_limits<T>::is_specialized>::type>
struct max_f {
    static constexpr T default_value = v;
    static constexpr bool idempotent = true;
    T operator()(const T& a, const T& b) const { return max(a, b); }
};
template <class T, T v, typename R>
constexpr const T max_f<T, v, R>::default_value;

template <class T, T v = numeric_limits<T>::max(),
          typename = typename enable_if<numeric_limits<T>::is_specialized>::type>
struct min_f {
    static constexpr T default_value = v;
    static constexpr bool idempotent = true;
    T operator()(const T& a, const T& b) const { return min(a, b); }
};
template <class T, T v, typename R>
constexpr const T min_f<T, v, R>::default_value;

uint64_t gcd(uint64_t a, uint64_t b) {
    if (a < b) swap(a, b);
    while (b != 0) {
        auto t = b;
        b = a % b;
        a = t;
    }
    return a;
}

template <class T, T v = T{}, typename = typename enable_if<numeric_limits<T>::is_integer>::type>
struct gcd_f {
    static constexpr T default_value = v;
    static constexpr bool idempotent = true;
    T operator()(const T& a, const T& b) const { return gcd(a, b); }
};
template <class T, T v, typename R>
constexpr const T gcd_f<T, v, R>::default_value;

template <class T, class F = max_f<T>>
using SparseTable = st_impl::SparseTable<T, F>;

template <class F>
void random_test(string target_func) {
    int n = 400;
    vector<int> test(n);

    // generate random numbers
    random_device r;
    default_random_engine eng(r());
    uniform_int_distribution<int> uniform_dist(0, 2000);

    for (int i = 0; i < n; ++i) {
        test[i] = uniform_dist(eng);
    }

    // query and verify
    F f;
    SparseTable<int, F> st_test(test, f);

    cout << "Begin random test on " << target_func << "!" << endl;
    int t = 10;
    for (int i = 0; i < t; ++i) {
        int l = uniform_dist(eng) % n, r = l + ((uniform_dist(eng) % (n - l)) >> (i / 2));
        auto to_verify = st_test.rangeQuery(l, r);
        auto expected = decltype(f)::default_value;

        for (int j = l; j <= r; ++j) {
            expected = f(expected, test[j]);
        }
        assert(to_verify == expected);
        cout << " + query range(" << l << "," << r << ")\t= " << to_verify << endl;
    }
    cout << "Test passed!" << endl;
}

void regular_test() {
    SparseTable<int> st_max({3, 1, 2, 5, 2, 10, 8});

    assert(st_max.rangeQuery(0, 2) == 3);
    assert(st_max.rangeQuery(3, 6) == 10);
    assert(st_max.rangeQuery(0, 6) == 10);
    assert(st_max.rangeQuery(2, 4) == 5);

    SparseTable<int, min_f<int>> st_min({3, 1, 2, 5, 2, 10, 8});

    assert(st_min.rangeQuery(0, 2) == 1);
    assert(st_min.rangeQuery(3, 6) == 2);
    assert(st_min.rangeQuery(0, 6) == 1);
    assert(st_min.rangeQuery(2, 4) == 2);

    SparseTable<int, sum_f<int>> st_sum({3, 1, 2, 5, 2, 10, 8});

    assert(st_sum.rangeQuery(0, 2) == 6);
    assert(st_sum.rangeQuery(3, 6) == 25);
    assert(st_sum.rangeQuery(0, 6) == 31);
    assert(st_sum.rangeQuery(2, 4) == 9);
}

int main() {
    regular_test();

    random_test<max_f<int>>("max");
    random_test<min_f<int>>("min");
    random_test<sum_f<int>>("sum");
    random_test<gcd_f<int>>("gcd");

    return 0;
}
