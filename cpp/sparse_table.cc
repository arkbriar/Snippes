#include <vector>
#include <cassert>
#include <cstring>
#include <iostream>
#include <limits>
#include <type_traits>
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

    T rangeQuery(size_type l, size_type r) const {
        if (!(l <= r && r < _size)) {
            throw std::out_of_range("Bad query!");
        }

        // if the function allows overlap, which is to say,
        // f(range(l, s), range(t, r)) == f(range(l, r)) always
        // holds for all l <= t <= s <= r,
        // then rangeQuery will be executed in O(1).
        // otherwise it should be finished in O(lgN).
        if (func_type::allow_overlap) {
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
    static constexpr bool allow_overlap = false;
    T operator()(const T& a, const T& b) const { return a + b; }
};
template <class T, T v>
constexpr const T sum_f<T, v>::default_value;

template <class T, T v = numeric_limits<T>::min(),
          typename = typename enable_if<numeric_limits<T>::is_specialized>::type>
struct max_f {
    static constexpr T default_value = v;
    static constexpr bool allow_overlap = true;
    T operator()(const T& a, const T& b) const { return max(a, b); }
};
template <class T, T v, typename R>
constexpr const T max_f<T, v, R>::default_value;

template <class T, T v = numeric_limits<T>::max(),
          typename = typename enable_if<numeric_limits<T>::is_specialized>::type>
struct min_f {
    static constexpr T default_value = v;
    static constexpr bool allow_overlap = true;
    T operator()(const T& a, const T& b) const { return min(a, b); }
};
template <class T, T v, typename R>
constexpr const T min_f<T, v, R>::default_value;

template <class T, class F = max_f<T>>
using SparseTable = st_impl::SparseTable<T, F>;

int main() {
    SparseTable<int> st_max({3, 1, 2, 5, 2, 10, 8});

    cout << st_max.rangeQuery(0, 2) << endl;  // 3
    cout << st_max.rangeQuery(3, 6) << endl;  // 10
    cout << st_max.rangeQuery(0, 6) << endl;  // 10
    cout << st_max.rangeQuery(2, 4) << endl;  // 5

    SparseTable<int, min_f<int>> st_min({3, 1, 2, 5, 2, 10, 8});

    cout << st_min.rangeQuery(0, 2) << endl;  // 1
    cout << st_min.rangeQuery(3, 6) << endl;  // 2
    cout << st_min.rangeQuery(0, 6) << endl;  // 1
    cout << st_min.rangeQuery(2, 4) << endl;  // 2

    SparseTable<int, sum_f<int>> st_sum({3, 1, 2, 5, 2, 10, 8});

    cout << st_sum.rangeQuery(0, 2) << endl;  // 6
    cout << st_sum.rangeQuery(3, 6) << endl;  // 25
    cout << st_sum.rangeQuery(0, 6) << endl;  // 31
    cout << st_sum.rangeQuery(2, 4) << endl;  // 9

    return 0;
}
