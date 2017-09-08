#include <vector>

class BinaryIndexedTree {
private:
    std::vector<long> bit;

    static int lowbit(int x) {
        return x & -x;
    }
public:
    BinaryIndexedTree(const std::vector<int>& nums) {
        int n = nums.size();
        bit = std::vector<long>(n);

        for (int i = 0; i < n; ++ i) {
            bit[i] = nums[i];
            for (int j = i - 1; j > i - lowbit(i + 1); -- j) {
                bit[i] += nums[j];
            }
        }
    }

    void add(int i, int delta) {
        for (int j = i; j < int(bit.size()); j += lowbit(j + 1)) {
            bit[j] += delta;
        }
    }

    long sum(int k) {
        int res = 0;
        for (int i = k; i >= 0; i -= lowbit(i + 1)) {
            res += bit[i];
        }
        return res;
    }
};
