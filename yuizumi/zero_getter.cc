#include <algorithm>
#include <iostream>
#include "common.h"

using namespace std;

Solution Solve(const Problem& p)
{
    const int n = p.hole.size();

    if (p.figure.vertices.size() != n) {
        return Solution();
    }

    vector<int> map(n);
    for (int i = 0; i < n; i++) map[i] = i;

    do {
        const bool success = all_of(
            p.figure.edges.begin(),
            p.figure.edges.end(),
            [&] (const Edge& e) {
                const double d_orig = norm(p.figure.vertices[e.u] - p.figure.vertices[e.v]);
                const double d_pose = norm(p.hole[map[e.u]] - p.hole[map[e.v]]);
                return abs(d_pose - d_orig) * 1e+6 <= p.epsilon * d_orig;
            }
        );
        if (success) {
            Solution s;
            s.reserve(n);
            for (int i = 0; i < n; i++) s.push_back(p.hole[map[i]]);
            return s;
        }
    } while (next_permutation(map.begin(), map.end()));

    return Solution();
}

int main()
{
    Json json;
    cin >> json;
    cout << BuildJson(Solve(ParseJson(json))) << endl;
    return 0;
}
