#include <algorithm>
#include <iostream>
#include "common.h"

using namespace std;

std::optional<Pose> Solve(const Problem& p)
{
    const int n = p.hole().size();

    if (p.vertices().size() != n)
        return std::nullopt;

    vector<int> map(n);
    for (int i = 0; i < n; i++) map[i] = i;

    Pose pose(n);
    do {
        for (int i = 0; i < n; i++) pose[i] = p.hole().vertices()[map[i]];
        if (Validate(p, pose)) return pose;
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
        cerr << "dislikes: " << Evaluate(prob, *pose) << endl;
    }
    return 0;
}
