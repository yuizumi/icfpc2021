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

Pose PoseFromJson(const Json& json) {
    const Json& v = json.at("vertices");
    Pose pose;
    pose.reserve(v.size());
    for (const auto& xy : v) {
        pose.emplace_back(xy[0].get<double>(), xy[1].get<double>());
    }
    return pose;
}

Json FullValidate(const Problem& prob, const Pose& pose) {
    static constexpr double kEpsDenom = 1e+6;

    Json errors = Json::array();

    const Polygon& hole = prob.hole();
    const int n = hole.size();

    for (int i = 0; i < pose.size(); i++) {
        if (!hole.Contains(pose[i])) {
            std::ostringstream msg;
            msg << "Vertex " << i << " is located outside the hole.";
            errors.push_back({{"type", "vertex_outside_hole"}, {"message", msg.str()},
                              {"culprit", i}});
        }
    }

    const std::vector<Complex>& orig = prob.vertices();

    for (const Edge& e : prob.edges()) {
        for (int i = 0; i < n; i++) {
            const Complex zi = hole.vertices()[(i + 0) % n];
            const Complex zj = hole.vertices()[(i + 1) % n];
            if (Intersects({pose[e.u], pose[e.v]}, {zi, zj}) == kCrossing) {
                std::ostringstream msg;
                msg << "Edge (" << e.u << ", " << e.v << ") is not fully inside the hole.";
                errors.push_back({{"type", "edge_outside_hole"}, {"message", msg.str()},
                                  {"culprit", Json::array({e.u, e.v})}});
            }
        }

        const double d_pose = norm(pose[e.u] - pose[e.v]);
        const double d_orig = norm(orig[e.u] - orig[e.v]);
        if (abs(d_pose - d_orig) * kEpsDenom > prob.epsilon() * d_orig) {
            std::ostringstream msg;
            msg << "Edge (" << e.u << ", " << e.v << ") has an invalid length. "
                << "orig: " << d_orig << ", pose: " << d_pose;
            errors.push_back({{"type", "edge_invalid_length"}, {"message", msg.str()},
                              {"culprit", Json::array({e.u, e.v})}});
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
