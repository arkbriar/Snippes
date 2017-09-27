#include <string>
#include <vector>
using namespace std;

vector<int> make_next(const string &str) {
    int n = str.size();
    vector<int> next(n);
    for (int q = 1, k = 0; q < n; ++q) {
        while (k > 0 && str[q] != str[k]) {
            k = next[k - 1];
        }
        if (str[q] == str[k]) ++k;
        next[q] = k;
    }
    return next;
}

int kmp(const string &str, const string &pattern) {
    vector<int> next = make_next(pattern);

    int res = 0;
    int n = str.size(), m = pattern.size();
    for (int i = 0, q = 0; i < n; ++i) {
        // when q == m, string::operator[] returns null char '\0'
        while (q > 0 && pattern[q] != str[i]) {
            q = next[q - 1];
        }
        // here q < m always holds
        if (pattern[q] == str[i]) ++q;
        if (q == m) res++;
    }
    return res;
}
