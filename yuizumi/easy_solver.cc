#include <algorithm>
#include <iostream>
#include "common.h"

using namespace std;

std::optional<Pose> Solve(const Problem& prob)
{
    const int m = prob.vertices().size();
    const int n = prob.hole().size();

    if (m > n) return std::nullopt;

    vector<int> map(n);
    for (int i = 0; i < n; i++) map[i] = i;

    Pose pose(m);
    do {
        for (int i = 0; i < m; i++) pose[i] = prob.hole().vertices()[map[i]];
        if (Validate(prob, pose)) return pose;
        reverse(map.begin() + m, map.end());
    } while (next_permutation(map.begin(), map.end()));

    return std::nullopt;
}

int main()
{
    Json json;
    cin >> json;

    const Problem prob = Problem::FromJson(json);
    const std::optional<Pose> pose = Solve(prob);
    if (pose.has_value()) {
        cout << ToJson(*pose) << endl;
        cerr << "dislikes = " << Evaluate(prob, *pose) << endl;
    } else {
        cerr << "dislikes = (n/a)" << endl;
    }
    return 0;
}
