#include <cassert>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "v2.h"

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
    const std::vector<Complex>& orig = prob.vertices();
    Json errors = Json::array();

    for (const Edge& e : prob.edges()) {
        if (!prob.hole().Contains(LineSeg{pose[e.u], pose[e.v]})) {
            std::ostringstream msg;
            msg << "The edge at " << pose[e.u] << "-" << pose[e.v] << " is not "
                << "inside the hole.";
            errors.push_back(
                {{"type", "not_inside_hole"}, {"edge", {e.u, e.v}}, {"message", msg.str()}});
        }

        const double d_pose = norm(pose[e.u] - pose[e.v]);
        const double d_orig = norm(orig[e.u] - orig[e.v]);

        if (abs(d_pose - d_orig) * kEpsDivisor > prob.epsilon() * d_orig) {
            std::ostringstream msg;
            msg << "The edge {" << e.u << ", " << e.v << "} has an invalid length. "
                << "orig: " << d_orig << ", pose: " << d_pose;
            errors.push_back(
                {{"type", "invalid_length"}, {"edge", {e.u, e.v}}, {"message", msg.str()}});
        }
    }

    Json json = {{"errors", errors}};

    if (Validate(prob, pose)) {
        assert(errors.empty());
        json.emplace("dislikes", Dislikes(prob, pose));
    } else {
        assert(!errors.empty());
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
