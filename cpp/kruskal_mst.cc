#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
using namespace std;

int __get_root(vector<int>& root, int v) {
    if (v == root[v]) return v;
    return root[v] = __get_root(root, root[v]);
}

bool __unite(vector<int>& root, int u, int v) {
    u = __get_root(root, u);
    v = __get_root(root, v);
    if (u == v) return false;
    root[v] = u;
    return true;
}

// Kruskal minimum spanning tree, vertexs start from 0
// edge is in the form of (w, (u, v))
long kruskal_mst(vector<pair<int, pair<int, int>>>& edges, int n) {
    vector<int> root(n, 0), size(n, 1);
    iota(root.begin(), root.end(), 0);

    auto unite = [&](int u, int v) -> bool { return __unite(root, u, v); };

    long mst_weight = 0;
    sort(edges.begin(), edges.end());
    for (auto it : edges) {
        int w = it.first, u = it.second.first, v = it.second.second;
        if (unite(u, v)) {
            mst_weight += w;
        }
    }

    return mst_weight;
}

int main() {
    int n = 4;
    vector<pair<int, pair<int, int>>> edges = {
        {1, {0, 1}}, {3, {0, 2}}, {7, {0, 3}}, {2, {1, 2}}, {3, {1, 3}}, {3, {2, 3}},
    };

    cout << kruskal_mst(edges, n) << endl;
    return 0;
}
