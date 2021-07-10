#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "common.h"

namespace {

Json LoadJson(const std::string& filename)
{
    Json json;

    if (filename == "-") {
        std::cin >> json;
    } else {
        std::ifstream fin(filename);
        fin >> json;
        fin.close();
    }

    return json;
}

Json FullValidate(const Problem& prob, const Pose& pose) {
    static constexpr double kEpsDenom = 1e+6;

    Json errors = Json::array();

    const Polygon& hole = prob.hole();
    const int n = hole.size();

    const std::vector<Complex>& orig = prob.vertices();

    for (const Edge& e : prob.edges()) {
        bool good = hole.Contains(pose[e.u]) && hole.Contains(pose[e.v]);

        for (int i = 0; good && i < n; i++) {
            const Complex zi = hole.vertices()[(i + 0) % n];
            const Complex zj = hole.vertices()[(i + 1) % n];
            if (Intersects({pose[e.u], pose[e.v]}, {zi, zj}) == kCrossing) {
                good = false;
            }
        }

        if (!good) {
            std::ostringstream msg;
            msg << "The edge at " << pose[e.u] << "-" << pose[e.v] << " is not "
                << "inside the hole.";
            errors.push_back(
                {{"type", "not_inside_hole"}, {"edge", {e.u, e.v}}, {"message", msg.str()}});
        }

        const double d_pose = norm(pose[e.u] - pose[e.v]);
        const double d_orig = norm(orig[e.u] - orig[e.v]);

        if (abs(d_pose - d_orig) * kEpsDenom > prob.epsilon() * d_orig) {
            std::ostringstream msg;
            msg << "The edge {" << e.u << ", " << e.v << "} has an invalid length. "
                << "orig: " << d_orig << ", pose: " << d_pose;
            errors.push_back(
                {{"type", "invalid_length"}, {"edge", {e.u, e.v}}, {"message", msg.str()}});
        }
    }

    Json json = {{"errors", errors}};

    if (Validate(prob, pose)) {
        json.emplace("dislikes", Evaluate(prob, pose));
    }

    return json;
}

}  // namespace

int main(int argc, char* argv[])
{
    if (argc != 3) {
        std::cerr << "Usage: eval PROBLEM POSE" << std::endl;
        return 1;
    }

    const Problem prob = Problem::FromJson(LoadJson(argv[1]));
    const Pose pose = PoseFromJson(LoadJson(argv[2]));

    std::cout << FullValidate(prob, pose) << std::endl;

    return 0;
}
