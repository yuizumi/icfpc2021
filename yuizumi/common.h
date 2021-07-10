// -*- C++ -*-
#ifndef YUIZUMI_COMMON_H_
#define YUIZUMI_COMMON_H_

#include <complex>
#include <vector>
#include "json.hpp"

using Complex = std::complex<double>;
using Json = nlohmann::json;

struct Edge { int u, v; };

using Hole = std::vector<Complex>;

struct Figure
{
    std::vector<Complex> vertices;
    std::vector<Edge> edges;
};

struct Problem
{
    int epsilon;
    Hole hole;
    Figure figure;
};

using Solution = std::vector<Complex>;

inline Json BuildJson(const Solution& sol)
{
    std::vector<std::vector<int>> vertices;
    vertices.reserve(sol.size());

    for (const Complex& z : sol) {
        const int x = static_cast<int>(z.real());
        const int y = static_cast<int>(z.imag());
        vertices.push_back({x, y});
    }

    Json json;
    json.emplace("vertices", vertices);
    return json;
}

inline Problem ParseJson(const Json& json)
{
    Problem p;

    {
        const auto& hole = json["hole"];
        p.hole.reserve(hole.size());
        for (const auto& xy : hole) {
            p.hole.emplace_back(xy[0].get<double>(), xy[1].get<double>());
        }
    }
    {
        const auto& vertices = json["figure"]["vertices"];
        p.figure.vertices.reserve(vertices.size());
        for (const auto& xy : vertices) {
            p.figure.vertices.emplace_back(xy[0].get<double>(), xy[1].get<double>());
        }
    }
    {
        const auto& edges = json["figure"]["edges"];
        p.figure.edges.reserve(edges.size());
        for (const auto& uv : edges) {
            p.figure.edges.push_back({uv[0].get<int>(), uv[1].get<int>()});
        }
    }

    p.epsilon = json["epsilon"].get<int>();

    return p;
}

#endif  // YUIZUMI_COMMON_H_
