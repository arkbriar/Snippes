#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <algorithm>
#include <vector>
#include <string>
#include <stack>
#include <cmath>
#include <deque>
#include <queue>
#include <map>
#include <bitset>
#include <set>
#include <list>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <numeric>
#include <climits>
#include <utility>
#include <iomanip>
#include <cassert>

using namespace std;

using ii = pair<int, int>;
using iii = pair<int, ii>;
template <class T>
using vv = vector<vector<T>>;

#define rep(i, b) for (int i = 0; i < int(b); ++i)
#define reps(i, a, b) for (int i = int(a); i < int(b); ++i)
#define rrep(i, b) for (int i = int(b) - 1; i >= 0; --i)
#define rreps(i, a, b) for (int i = int(b) - 1; i >= a; --i)
#define repe(i, b) for (int i = 0; i <= int(b); ++i)
#define repse(i, a, b) for (int i = int(a); i <= int(b); ++i)
#define rrepe(i, b) for (int i = int(b); i >= 0; --i)
#define rrepse(i, a, b) for (int i = int(b); i >= int(a); --i)

#define all(a) a.begin(), a.end()
#define rall(a) a.rbegin(), a.rend()
#define sz(a) int(a.size())
#define mp(a, b) make_pair(a, b)

#define inf (INT_MAX / 2)
#define infl (LONG_MAX / 2)
#define infll (LLONG_MAX / 2)

#define X first
#define Y second
#define pb push_back
#define eb emplace_back

// clang-format off
vector<string> __macro_split(const string& s) { vector<string> v; int d = 0, f = 0; string t; for (char c : s) { if (!d && c == ',') v.pb(t), t = ""; else t += c; if (c == '\"' || c == '\'') f ^= 1; if (!f && c == '(') ++d; if (!f && c == ')') --d; } v.pb(t); return v; }
void __args_output(vector<string>::iterator, vector<string>::iterator) { cerr << endl; }
template <typename T, typename... Args>
void __args_output(vector<string>::iterator it, vector<string>::iterator end, T a, Args... args) { cerr << it->substr((*it)[0] == ' ', it->length()) << " = " << a; if (++it != end) { cerr << ", "; } __args_output(it, end, args...); }
#define out(args...) { vector<string> __args = __macro_split(#args); __args_output(__args.begin(), __args.end(), args); }
// clang-format on

const int MAX_N = 200000;

struct Command {
    int op, id, x1, y1, x2, y2;

    bool operator<(const Command& other) const {
        if (x1 == other.x1) return op < other.op;
        return x1 < other.x1;
    }
};

int n;
int bit[MAX_N * 10];

void add(int i, int delta) {
    while (i <= n) {
        bit[i] += delta;
        i += (i & -i);
    }
}

int sum(int i) {
    int ans = 0;
    while (i > 0) {
        ans += bit[i];
        i -= (i & -i);
    }
    return ans;
}

int range_sum(int i, int j) {
    return sum(j) - sum(i - 1);
}

Command com[MAX_N], com_t[MAX_N];
int ans[MAX_N];

void cdq(int l, int r) {
    if (l >= r) return;
    int m = (l + r) / 2;
    cdq(l, m);
    cdq(m + 1, r);

    int cnt = 0;
    reps(i, 0, m) { com_t[cnt++] = com[i]; }
    reps(i, m + 1, r) {
        if (com[i].op == 2) {
            com_t[cnt] = com[i];
            com_t[cnt++].op = 0;
            com_t[cnt] = com[i];
            com_t[cnt++].x1 = com[i].x2;
        }
    }

    sort(com_t, com_t + cnt);

    rep(i, cnt) {
        if (com_t[i].op == 0) {
            ans[com_t[i].id] -= range_sum(com_t[i].y1, com_t[i].y2);
        } else if (com_t[i].op == 1) {
            add(com_t[i].y1, com_t[i].x2);
        } else {
            ans[com_t[i].id] += range_sum(com_t[i].y1, com_t[i].y2);
        }
    }
}

int main() {
    int op, a, b, c, d;
    scanf("%d %d", &op, &n);

    int cnt = 0, cnt_q = 0;
    while (op != 3) {
        scanf("%d", &op);
        if (op == 1) {
            scanf("%d %d %d", &a, &b, &c);
            com[cnt++] = Command{op, -1, a, b, c, 0};
        } else if (op == 2) {
            scanf("%d %d %d %d", &a, &b, &c, &d);
            com[cnt++] = Command{op, cnt_q++, a, b, c, d};
        }
    }

    cdq(0, cnt);

    rep(i, cnt) { printf("%d ", ans[i]); }
    printf("\n");

    return 0;
}
