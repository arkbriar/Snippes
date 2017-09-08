#include <algorithm>
#include <vector>
using namespace std;

// Segment tree supports range maximum query, with range update and lazy propagation.
class SegmentTree {
private:
    int n;
    vector<int> max_val, to_add;

    static inline int left(int i) { return 2 * i + 1; }

    static inline int right(int i) { return 2 * i + 2; }

    void push(int i, int tl, int tr) {
        max_val[i] += to_add[i];
        if (tl != tr - 1) {
            max_val[left(i)] += to_add[i];
            max_val[right(i)] += to_add[i];
        }
        to_add[i] = 0;
    }

    void add(int i, int tl, int tr, int l, int r, int delta) {
        push(i, tl, tr);
        if (tl >= r || tr <= l) {
            return;
        }
        if (l <= tl && tr <= r) {
            to_add[i] += delta;
            push(i, tl, tr);
            return;
        }
        int tm = (tl + tr) / 2;
        add(left(i), tl, tm, l, r, delta);
        add(right(i), tm, tr, l, r, delta);
        max_val[i] = max(max_val[2 * i + 1], max_val[2 * i + 2]);
    }

    int get(int i, int tl, int tr, int l, int r) {
        push(i, tl, tr);
        if (tl >= r || tr <= l) {
            return 0;
        }
        if (l <= tl && tr <= r) {
            return max_val[i];
        }
        int tm = (tl + tr) / 2;
        return max(get(left(i), tl, tm, l, r), get(right(i), tm, tr, l, r));
    }

public:
    SegmentTree(int k) {
        n = 1;
        while (n < k) {
            n *= 2;
        }
        max_val = vector<int>(2 * n, 0);
        to_add = vector<int>(2 * n, 0);
    }

    void add(int l, int r, int delta) { add(0, 0, n, l, r, delta); }

    int get(int l, int r) { return get(0, 0, n, l, r); }
};
