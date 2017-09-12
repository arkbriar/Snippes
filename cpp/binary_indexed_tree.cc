#include <vector>
#include <iostream>
#include <cassert>
using namespace std;

class BinaryIndexedTree {
protected:
    vector<long> bit;

    static int lowbit(int x) { return x & -x; }

    BinaryIndexedTree(BinaryIndexedTree&& other) { bit = std::move(other.bit); }

public:
    BinaryIndexedTree(int n) { bit = vector<long>(n); }

    BinaryIndexedTree(const vector<long>& nums) {
        int n = nums.size();
        bit = vector<long>(n);

        for (int i = 0; i < n; ++i) {
            for (int j = i; j > i - lowbit(i + 1); --j) {
                bit[i] += nums[j];
            }
        }
    }

    void add(int i, long delta) {
        for (int j = i; j < int(bit.size()); j += lowbit(j + 1)) {
            bit[j] += delta;
        }
    }

    long sum(int k) {
        long res = 0;
        for (int i = k; i >= 0; i -= lowbit(i + 1)) {
            res += bit[i];
        }
        return res;
    }
};

class PointUpdateRangeQueryExectuor {
private:
    int n;
    BinaryIndexedTree tree;

    long prefixSum(int r) {
        if (r < 0) return 0;
        return tree.sum(r);
    }

public:
    PointUpdateRangeQueryExectuor(int n) : n(n), tree(n) {}
    PointUpdateRangeQueryExectuor(const vector<long>& nums) : n(nums.size()), tree(nums) {}

    void update(int i, long delta) {
        assert(i >= 0 && i < n);
        tree.add(i, delta);
    }

    long rangeSum(int l, int r) {
        assert(l <= r && l >= 0 && r < n);
        return prefixSum(r) - prefixSum(l - 1);
    }
};

class RangeUpdatePointQueryExecutor {
private:
    int n;
    BinaryIndexedTree tree;

    // Tear array into pieces
    static vector<long> rangePieces(const vector<long>& nums) {
        int n = nums.size();
        vector<long> res(n);
        // make sure that prefix_sum(res, i) = nums[i]
        if (n != 0) res[0] = nums[0];
        for (int i = 1; i < n; ++i) {
            res[i] = nums[i] - nums[i - 1];
        }
        return res;
    }

    friend class RangeUpdateRangeQueryExecutor;

public:
    RangeUpdatePointQueryExecutor(int n) : n(n), tree(n) {}

    RangeUpdatePointQueryExecutor(const vector<long>& nums)
        : n(nums.size()), tree(rangePieces(nums)) {}

    void update(int l, int r, long delta) {
        assert(l <= r && l >= 0 && r < n);
        tree.add(l, delta);
        if (r + 1 < n) tree.add(r + 1, -delta);
    }

    long get(int i) {
        assert(i >= 0 && i < n);
        return tree.sum(i);
    }
};

class RangeUpdateRangeQueryExecutor {
private:
    long n;
    BinaryIndexedTree tree;
    BinaryIndexedTree tree2;

    static vector<long> prefixPieces(const vector<long>& nums) {
        int n = nums.size();
        vector<long> res(n);
        // make sure that nums[i] * i - res[i] = prefix_sum(nums, i),
        // so that the following prefixSum works.
        // Then run rangePieces, so that we get res[i] = (nums[i] - nums[i - 1]) * (i - 1);
        if (n != 0) res[0] = -nums[0];
        for (long i = 0; i < n; ++i) {
            res[i] = (nums[i] - nums[i - 1]) * (i - 1);
        }
        return res;
    }

    long prefixSum(long r) {
        if (r < 0) return 0;
        return tree.sum(r) * r - tree2.sum(r);
    }

    static constexpr auto rangePieces = RangeUpdatePointQueryExecutor::rangePieces;

public:
    RangeUpdateRangeQueryExecutor(long n) : n(n), tree(n), tree2(n) {}

    RangeUpdateRangeQueryExecutor(const vector<long>& nums)
        : n(nums.size()), tree(rangePieces(nums)), tree2(prefixPieces(nums)) {}

    void update(long l, long r, long delta) {
        assert(l <= r && l >= 0 && r < n);
        tree.add(l, delta);
        if (r + 1 < n) tree.add(r + 1, -delta);
        tree2.add(l, delta * (l - 1));
        if (r + 1 < n) tree2.add(r + 1, -delta * r);
    }

    long rangeSum(long l, long r) {
        assert(l <= r && l >= 0 && r < n);
        return prefixSum(r) - prefixSum(l - 1);
    }
};

int main() {
    // point update range query
    PointUpdateRangeQueryExectuor purq(5);
    purq.update(0, 2);
    purq.update(3, 3);
    purq.update(4, 5);
    cout << purq.rangeSum(0, 1) << endl;  // 2
    cout << purq.rangeSum(2, 3) << endl;  // 3
    cout << purq.rangeSum(3, 4) << endl;  // 8

    PointUpdateRangeQueryExectuor purq2({2, 1, 2, 3, 5});
    cout << purq2.rangeSum(0, 1) << endl;  // 3
    cout << purq2.rangeSum(2, 3) << endl;  // 5
    cout << purq2.rangeSum(3, 4) << endl;  // 8

    // range update point query
    RangeUpdatePointQueryExecutor rupq(5);
    rupq.update(0, 4, 2);
    rupq.update(3, 4, 3);
    cout << rupq.get(0) << endl;  // 2
    cout << rupq.get(3) << endl;  // 5

    RangeUpdatePointQueryExecutor rupq2({11, 3, 2, 6, 5});
    cout << rupq2.get(0) << endl;  // 11
    cout << rupq2.get(3) << endl;  // 6

    // range update range query
    RangeUpdateRangeQueryExecutor rurq(5);
    rurq.update(0, 4, 2);
    rurq.update(3, 4, 3);
    cout << rurq.rangeSum(2, 4) << endl;  // 12

    RangeUpdateRangeQueryExecutor rurq2({2, 2, 3, 6, 5});
    cout << rurq2.rangeSum(2, 4) << endl;  // 14

    return 0;
}
