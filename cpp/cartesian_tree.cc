#include <vector>
#include <memory>
#include <stack>
#include <initializer_list>
#include <iostream>
#include <cassert>
#include <random>
using namespace std;

namespace cst_impl {

template <class I, class T>
class CartesianNode {
public:
    I idx;
    T val;
    CartesianNode *left, *right;

    CartesianNode() : left(nullptr), right(nullptr) {}
    CartesianNode(const I& idx, const T& val) : idx(idx), val(val), left(nullptr), right(nullptr) {}
    CartesianNode(I&& idx, T&& val)
        : idx(std::move(idx)), val(std::move(val)), left(nullptr), right(nullptr) {}
};

// Cartesian tree has the property o
template <class I, class T, class Compare, T NotFound>
class CartesianTree {
public:
    typedef I index_type;
    typedef T value_type;
    typedef Compare compare_type;
    typedef CartesianNode<I, T> node_type;
    typedef CartesianNode<I, T>* node_pointer;
    typedef const CartesianNode<I, T>* const_node_pointer;

public:
    CartesianTree(const vector<pair<I, T>>& data) {
        std::sort(data.begin(), data.end(), std::less<I>());

        node_pointer p = nullptr;
        stack<node_pointer> st;
        for (auto& it : data) {
            auto next_node = new node_type(it.first, it.second);
            if (!root) {
                p = root = next_node;
                st.push(root);
                continue;
            }
            assert(p == st.top());
            while (p && !value_compare(p->val, next_node->val)) {
                st.pop();
                if (!st.empty()) {
                    p = st.top();
                } else {
                    p = nullptr;
                }
            }
            if (!p) {
                next_node->left = root;
                p = root = next_node;
            } else {
                next_node->left = p->right;
                p = p->right = next_node;
            }
            st.push(next_node);
        }
    }

    CartesianTree(const initializer_list<pair<I, T>>& data)
        : CartesianTree(vector<pair<I, T>>(data)) {}

    template <typename = typename std::enable_if<std::numeric_limits<I>::is_integer>::type>
    CartesianTree(const vector<T>& data) {
        assert(data.size() <= std::numeric_limits<I>::max());

        node_pointer p = nullptr;
        stack<node_pointer> st;
        for (index_type i = 0; i < index_type(data.size()); ++i) {
            auto next_node = new node_type(i, data[i]);
            if (!root) {
                p = root = next_node;
                st.push(root);
                continue;
            }
            assert(p == st.top());
            while (p && !value_compare(p->val, next_node->val)) {
                st.pop();
                if (!st.empty()) {
                    p = st.top();
                } else {
                    p = nullptr;
                }
            }
            if (!p) {
                next_node->left = root;
                p = root = next_node;
            } else {
                next_node->left = p->right;
                p = p->right = next_node;
            }
            st.push(next_node);
        }
    }

    template <typename = typename std::enable_if<std::numeric_limits<I>::is_integer>::type>
    CartesianTree(const initializer_list<T>& data) : CartesianTree(vector<T>(data)) {}

    ~CartesianTree() {
        if (!root) return;

        stack<node_pointer> st;
        st.push(root);

        while (!st.empty()) {
            auto p = st.top();
            st.pop();
            if (p->right) st.push(p->right);
            if (p->left) st.push(p->left);
            delete p;
        }
    }

    const value_type& rangeQuery(index_type l, index_type r) const {
        auto p = root;
        while (p) {
            if (p->idx < l) {
                p = p->right;
            } else if (r < p->idx) {
                p = p->left;
            } else {
                return p->val;
            }
        }
        return not_found;
    }

    // todo range k query

    static constexpr value_type not_found = NotFound;

private:
    node_pointer root = nullptr;
    compare_type value_compare;
};

template <class I, class T, class C, T n>
constexpr T CartesianTree<I, T, C, n>::not_found;

}  // namespace cst_impl

template <class T, class Compare = std::less<T>, class I = std::size_t, T NotFound = -1>
using CartesianTree = cst_impl::CartesianTree<I, T, Compare, -1>;

template <class F>
void random_test(string target_func, int default_value) {
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
    CartesianTree<int, F> ct_test(test);

    cout << "Begin random test on " << target_func << "!" << endl;
    int t = 10;
    for (int i = 0; i < t; ++i) {
        int l = uniform_dist(eng) % n, r = l + ((uniform_dist(eng) % (n - l)) >> (i / 2));
        auto to_verify = ct_test.rangeQuery(l, r);
        auto expected = default_value;

        for (int j = l; j <= r; ++j) {
            expected = f(expected, test[j]) ? expected : test[j];
        }
        assert(to_verify == expected);
        cout << " + query range(" << l << "," << r << ")\t= " << to_verify << endl;
    }

    cout << "Begin testing invalid ranges!" << endl;
    for (int i = 0; i < t; ++i) {
        assert(ct_test.rangeQuery(-i - 100, -i - 90) == decltype(ct_test)::not_found);
    }
    cout << "Test passed!" << endl;
}

int main() {
    random_test<std::less<int>>("min", INT_MAX);
    random_test<std::greater<int>>("max", INT_MIN);

    return 0;
}
