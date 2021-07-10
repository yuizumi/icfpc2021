// -*- C++ -*-
#ifndef YUIZUMI_COMMON_H_
#define YUIZUMI_COMMON_H_

#include <complex>
#include <limits>
#include <optional>
#include <utility>
#include <vector>
#include "json.hpp"

using Complex = std::complex<double>;
using Json = nlohmann::json;

constexpr double kInfinity = std::numeric_limits<double>::infinity();

//------------------------
//  dblcmp

inline int dblcmp(double x, double y)
{
    static constexpr double kEpsilon = 1e-8;

    if (x > y)
        return (x - y >= kEpsilon) ? +1 : 0;
    else
        return (y - x >= kEpsilon) ? -1 : 0;
}

//------------------------
//  LineSegment

struct LineSegment { Complex z1, z2; };

enum class Intersection
{
    kSeparate = 0,
    kTouching = 1,
    kCrossing = 2,
};

constexpr Intersection kSeparate = Intersection::kSeparate;
constexpr Intersection kTouching = Intersection::kTouching;
constexpr Intersection kCrossing = Intersection::kCrossing;

inline Intersection Intersects(const LineSegment& l,
                               const LineSegment& m)
{
    int sign1;
    {
        const Complex z1 = (m.z1 - l.z1) / (l.z2 - l.z1);
        const Complex z2 = (m.z2 - l.z1) / (l.z2 - l.z1);
        sign1 = dblcmp(z1.imag(), 0.0) * dblcmp(z2.imag(), 0.0);
    }
    int sign2;
    {
        const Complex z1 = (l.z1 - m.z1) / (m.z2 - m.z1);
        const Complex z2 = (l.z2 - m.z1) / (m.z2 - m.z1);
        sign2 = dblcmp(z1.imag(), 0.0) * dblcmp(z2.imag(), 0.0);
    }
    const int value = 1 - std::max(sign1, sign2);
    return static_cast<Intersection>(value);
}

//------------------------
//  Polygon

class Polygon
{
public:
    explicit Polygon(std::vector<Complex> vertices)
        : vertices_(std::move(vertices)) {}

    Polygon(const Polygon&) = default;
    Polygon& operator=(const Polygon&) = default;

    static Polygon FromJson(const Json& json);

    const std::vector<Complex>& vertices() const { return vertices_; }
    const int size() const { return vertices_.size(); }

    bool Contains(Complex z) const;

private:
    std::vector<Complex> vertices_;
};

Polygon Polygon::FromJson(const Json& json)
{
    std::vector<Complex> v;
    v.reserve(json.size());
    for (const Json& xy : json) {
        v.emplace_back(xy[0].get<double>(), xy[1].get<double>());
    }
    return Polygon(std::move(v));
}

bool Polygon::Contains(Complex z) const
{
    const LineSegment line = {z, Complex(1e+8, 0.5)};

    bool result = false;

    for (int i = 0; i < size(); i++) {
        const int j = (i + 1) % size();
        const Intersection is = Intersects(line, {vertices_[i], vertices_[j]});
        if (is == kTouching) return true;
        result ^= (is == kCrossing);
    }

    return result;
}

//------------------------
//  Figure

struct Edge { int u, v; };

struct Figure
{
    std::vector<Complex> vertices;
    std::vector<Edge> edges;

    static Figure FromJson(const Json& json);
};

Figure Figure::FromJson(const Json& json)
{
    Figure fig;
    {
        const Json& src = json.at("vertices");
        fig.vertices.reserve(src.size());
        for (const Json& xy : src) {
            fig.vertices.emplace_back(xy[0].get<double>(), xy[1].get<double>());
        }
    }
    {
        const Json& src = json.at("edges");
        fig.edges.reserve(src.size());
        for (const Json& uv : src) {
            fig.edges.push_back({uv[0].get<int>(), uv[1].get<int>()});
        }
    }
    return fig;
}

//------------------------
//  Problem

class Problem
{
public:
    explicit Problem(Polygon hole, Figure fig, long epsilon)
        : hole_(std::move(hole)),
          fig_(std::move(fig)),
          epsilon_(epsilon) {}

    Problem(const Problem&) = delete;
    Problem& operator=(const Problem&) = delete;

    static Problem FromJson(const Json& json);

    const Polygon& hole() const { return hole_; }
    const Figure& fig() const { return fig_; }

    const long epsilon() const { return epsilon_; }

    const std::vector<Complex>& vertices() const { return fig_.vertices; }
    const std::vector<Edge>& edges() const { return fig_.edges; }

private:
    const Polygon hole_;
    const Figure fig_;
    const long epsilon_;
};

Problem Problem::FromJson(const Json& json)
{
    return Problem(Polygon::FromJson(json.at("hole")), Figure::FromJson(json.at("figure")),
                   json.at("epsilon").get<long>());
}

//------------------------
//  Pose

using Pose = std::vector<Complex>;

bool Validate(const Problem& problem, const Pose& pose)
{
    constexpr double kEpsDenom = 1e+6;

    const Polygon& hole = problem.hole();
    const int n = hole.size();

    for (const Complex z : pose) {
        if (!hole.Contains(z)) return false;
    }

    const std::vector<Complex>& orig = problem.vertices();

    for (const Edge& e : problem.edges()) {
        for (int i = 0; i < n; i++) {
            const Complex zi = hole.vertices()[(i + 0) % n];
            const Complex zj = hole.vertices()[(i + 1) % n];
            if (Intersects({pose[e.u], pose[e.v]}, {zi, zj}) == kCrossing)
                return false;
        }

        const double d_pose = norm(pose[e.u] - pose[e.v]);
        const double d_orig = norm(orig[e.u] - orig[e.v]);
        if (abs(d_pose - d_orig) * kEpsDenom > problem.epsilon() * d_orig)
            return false;
    }

    return true;
}

long Evaluate(const Problem& problem, const Pose& pose)
{
    double sum = 0.0;

    for (const Complex z_hole : problem.hole().vertices()) {
        double min = kInfinity;
        for (const Complex z_pose : pose) min = std::min(min, norm(z_hole - z_pose));
        sum += min;
    }

    return static_cast<long>(sum);
}

Json ToJson(const Pose& pose)
{
    Json vertices = Json::array();
    for (Complex z : pose) {
        vertices.push_back({static_cast<int>(z.real()), static_cast<int>(z.imag())});
    }
    return Json::object({{"vertices", vertices}});
}

#endif  // YUIZUMI_COMMON_H_
