// -*- C++ -*-
#ifndef YUIZUMI_COMMON_H_
#define YUIZUMI_COMMON_H_

#include <complex>
#include <limits>
#include <utility>
#include <vector>
#include "json.hpp"

using Complex = std::complex<double>;
using Json = nlohmann::json;

constexpr double kInfinity = std::numeric_limits<double>::infinity();
constexpr double kEpsDenom = 1e+6;

namespace impl {
inline std::vector<Complex> ParseVertices(const Json& json)
{
    std::vector<Complex> vertices;
    vertices.reserve(json.size());
    for (const Json& xy : json) {
        vertices.emplace_back(xy[0].get<double>(), xy[1].get<double>());
    }
    return vertices;
}
}  // namespace impl

//------------------------
//  Compare

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
//  Circle

struct Circle { Complex z; double r; };

inline std::vector<Complex> GetIntersections(
    const Circle& c1, const Circle& c2)
{
    const double a = c1.r / std::abs(c2.z - c1.z);
    const double b = c2.r / std::abs(c2.z - c1.z);

    const double cos = (a * a + 1.0 - b * b) / (2.0 * a);

    if (dblcmp(cos, -1.0) < 0 || dblcmp(cos, +1.0) > 0) {
        return {};
    }
    if (dblcmp(cos, +1.0) == 0) {
        return {c1.z + a * (c2.z - c1.z)};
    }
    if (dblcmp(cos, -1.0) == 0) {
        return {c1.z - a * (c2.z - c1.z)};
    }

    const Complex w = std::polar(a, std::acos(cos));
    return {c1.z + w * (c2.z - c1.z), c1.z + std::conj(w) * (c2.z - c1.z)};
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
    return Polygon(impl::ParseVertices(json));
}

bool Polygon::Contains(Complex z) const
{
    const LineSegment line = {z, Complex(1e+6, 0.5)};

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
    Figure fig = {
        .vertices = impl::ParseVertices(json.at("vertices")),
    };

    const Json& src = json.at("edges");
    fig.edges.reserve(src.size());
    for (const Json& uv : src) {
        fig.edges.push_back({uv[0].get<int>(), uv[1].get<int>()});
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

Pose PoseFromJson(const Json& json)
{
    return impl::ParseVertices(json.at("vertices"));
}

Json PoseToJson(const Pose& pose)
{
    Json vertices = Json::array();
    for (Complex z : pose) {
        vertices.push_back({static_cast<int>(z.real()), static_cast<int>(z.imag())});
    }
    return Json::object({{"vertices", vertices}});
}

bool Validate(const Problem& prob, const Pose& pose)
{
    const Polygon& hole = prob.hole();
    const int n = hole.size();

    for (const Complex z : pose) {
        if (!hole.Contains(z)) return false;
    }

    const std::vector<Complex>& orig = prob.vertices();

    for (const Edge& e : prob.edges()) {
        const double d_pose = norm(pose[e.u] - pose[e.v]);
        const double d_orig = norm(orig[e.u] - orig[e.v]);
        if (abs(d_pose - d_orig) * kEpsDenom > prob.epsilon() * d_orig)
            return false;

        if (!hole.Contains((pose[e.u] + pose[e.v]) / 2.0))
            return false;

        for (int i = 0; i < n; i++) {
            const Complex zi = hole.vertices()[(i + 0) % n];
            const Complex zj = hole.vertices()[(i + 1) % n];
            if (Intersects({pose[e.u], pose[e.v]}, {zi, zj}) == kCrossing)
                return false;
        }
    }

    return true;
}

long Evaluate(const Problem& prob, const Pose& pose)
{
    double sum = 0.0;

    for (const Complex z_hole : prob.hole().vertices()) {
        double min = kInfinity;
        for (const Complex z_pose : pose) min = std::min(min, norm(z_hole - z_pose));
        sum += min;
    }

    return static_cast<long>(sum);
}

#endif  // YUIZUMI_COMMON_H_
