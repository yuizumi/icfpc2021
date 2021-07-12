#ifndef YUIZUMI_V2_H_
#define YUIZUMI_V2_H_

#include <algorithm>
#include <complex>
#include <limits>
#include <unordered_map>
#include <utility>
#include <vector>
#include "json.hpp"

using Complex = std::complex<double>;
using Json = nlohmann::json;

constexpr double kEpsDivisor = 1e+06;
constexpr double kInf = std::numeric_limits<double>::infinity();


//------------------------
//  Utility

int Sign(double x) { return (x > 0.0) - (x < 0.0); }

namespace std {
template <> struct hash<Complex>
{
    size_t operator()(Complex z) const
    {
        static constexpr size_t kMixer = 0x9e3779b97f4a7c15u;

        size_t h = 0;
        h ^= hash<double>{}(z.real()) + kMixer + (h << 12) + (h >> 4);
        h ^= hash<double>{}(z.imag()) + kMixer + (h << 12) + (h >> 4);
        return h;
    }
};
}  // namespace std

namespace impl {
std::vector<Complex> ParseVertexArray(const Json& json)
{
    std::vector<Complex> vertices;
    vertices.reserve(json.size());
    for (const Json& xy : json) {
        vertices.emplace_back(xy[0].get<int>(), xy[1].get<int>());
    }
    return vertices;
}
}  // namespace impl


//------------------------
//  LineSeg

struct LineSeg { Complex z1, z2; };

enum class IntersectsResult
{
    kSeparate = 0,
    kTouching = 1,
    kCrossing = 2,
};

constexpr IntersectsResult kSeparate = IntersectsResult::kSeparate;
constexpr IntersectsResult kTouching = IntersectsResult::kTouching;
constexpr IntersectsResult kCrossing = IntersectsResult::kCrossing;

inline int Ccw(const LineSeg& l, Complex z)
{
    const Complex z1 = z - l.z1;
    const Complex z2 = l.z2 - l.z1;
    return Sign(z1.real() * z2.imag() - z2.real() * z1.imag());
}

inline IntersectsResult Intersects(const LineSeg& l, const LineSeg& m)
{
    const int sign_l = Ccw(l, m.z1) * Ccw(l, m.z2);
    const int sign_m = Ccw(m, l.z1) * Ccw(m, l.z2);
    return static_cast<IntersectsResult>(1 - std::max(sign_l, sign_m));
}


//------------------------
//  Hole

class Hole
{
public:
    explicit Hole(std::vector<Complex> vertices);

    Hole(const Hole&) = delete;
    Hole& operator=(const Hole&) = delete;

    const std::vector<Complex>& vertices() const { return vertices_; }
    Complex operator[](int i) const { return vertices_[i]; }
    const int size() const { return vertices_.size(); }

    const std::vector<LineSeg>& borders() const { return borders_; }

    int xmin() const { return xmin_; }
    int ymin() const { return ymin_; }
    int xmax() const { return xmax_; }
    int ymax() const { return ymax_; }

    // Works for integer coordinates only.
    bool Contains(Complex z) const { return GetState(z) != State::kOutside; }
    bool Contains(const LineSeg& line) const;

private:
    enum class State : char { kInside, kBorder, kOutside };

    State ComputeState(Complex z) const;
    State GetState(Complex z) const;

    std::vector<Complex> vertices_;
    std::vector<LineSeg> borders_;
    int xmin_, ymin_, xmax_, ymax_;
    std::vector<std::vector<State>> state_;
};

Hole::Hole(std::vector<Complex> vertices)
    : vertices_(std::move(vertices)),
      borders_(vertices_.size())
{
    for (int i = 0; i < size(); i++) {
        borders_[i] = {vertices_[i], vertices_[(i + 1) % size()]};
    }

    double xmin = +kInf, xmax = -kInf;
    double ymin = +kInf, ymax = -kInf;

    for (const Complex z: vertices_) {
        xmin = std::min(xmin, z.real()), xmax = std::max(xmax, z.real());
        ymin = std::min(ymin, z.imag()), ymax = std::max(ymax, z.imag());
    }

    xmin_ = static_cast<int>(xmin);
    ymin_ = static_cast<int>(ymin);
    xmax_ = static_cast<int>(xmax);
    ymax_ = static_cast<int>(ymax);

    const int x_size = xmax_ - xmin_ + 1;
    const int y_size = ymax_ - ymin_ + 1;

    state_.assign(y_size, std::vector<State>(x_size));

    for (int y = ymin_; y <= ymax_; y++)
    for (int x = xmin_; x <= xmax_; x++) {
        state_[y - ymin_][x - xmin_] = ComputeState(Complex(x, y));
    }
}

bool Hole::Contains(const LineSeg& line) const
{
    const State q1 = GetState(line.z1);
    const State q2 = GetState(line.z2);

    if (q1 == State::kOutside || q2 == State::kOutside) {
        return false;
    }

    std::vector<Complex> touching;

    if (q1 == State::kBorder) touching.push_back(line.z1);
    if (q2 == State::kBorder) touching.push_back(line.z2);

    for (const LineSeg& border : borders_) {
        switch (Intersects(line, border)) {
            case kSeparate: break;
            case kTouching: {
                if (Ccw(line, border.z1) == 0) touching.push_back(border.z1);
                if (Ccw(line, border.z2) == 0) touching.push_back(border.z2);
                break;
            }
            case kCrossing: return false;
        }
    }

    sort(touching.begin(), touching.end(), [&](Complex z1, Complex z2) {
        return (z1.real() != z2.real())
            ? z1.real() < z2.real() : z1.imag() < z2.imag();
    });

    for (int i = 1; i < touching.size(); i++) {
        if (touching[i - 1] != touching[i]) {
            const Complex middle = (touching[i - 1] + touching[i]) / 2.0;
            if (!Contains(middle)) return false;
        }
    }

    return true;
}

Hole::State Hole::ComputeState(Complex z) const
{
    static constexpr Complex kFaraway(1e+6, 0.5);

    const LineSeg line = {z, kFaraway};
    bool outside = true;
    for (const LineSeg& b : borders_) {
        switch (Intersects(line, b)) {
            case kSeparate: break;
            case kCrossing: outside = !outside; break;
            case kTouching: return State::kBorder;
        }
    }
    return outside ? State::kOutside : State::kInside;
}

Hole::State Hole::GetState(Complex z) const
{
    const int x = static_cast<int>(z.real());
    const int y = static_cast<int>(z.imag());
    if (x < xmin_ || x > xmax_ || y < ymin_ || y > ymax_)
        return State::kOutside;
    return state_[y - ymin_][x - xmin_];
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
    Figure figure;

    figure.vertices = impl::ParseVertexArray(json.at("vertices"));
    figure.edges.reserve(json.at("edges").size());
    for (const Json& uv : json.at("edges")) {
        figure.edges.push_back({uv[0].get<int>(), uv[1].get<int>()});
    }

    return figure;
}


//------------------------
//  Problem

class Problem
{
public:
    Problem(std::vector<Complex> hole, Figure figure, int epsilon)
        : hole_(std::move(hole)),
          figure_(std::move(figure)),
          epsilon_(epsilon) {}

    Problem(const Problem&) = delete;
    Problem operator=(const Problem&) = delete;

    static Problem FromJson(const Json& json)
    {
        return Problem(impl::ParseVertexArray(json.at("hole")),
                       Figure::FromJson(json.at("figure")),
                       json.at("epsilon").get<int>());
    }

    const Hole& hole() const { return hole_; }
    const Figure& figure() const { return figure_; }

    int epsilon() const { return epsilon_; }

    const std::vector<Complex>& vertices() const { return figure_.vertices; }
    const std::vector<Edge>& edges() const { return figure_.edges; }

    int GetMinNorm(const Edge& edge) const
    {
        const std::vector<Complex>& vertices = figure_.vertices;
        const double d_orig = std::norm(vertices[edge.u] - vertices[edge.v]);
        return static_cast<int>(std::ceil(d_orig * (1.0 - epsilon_)));
    }

    int GetMaxNorm(const Edge& edge) const
    {
        const std::vector<Complex>& vertices = figure_.vertices;
        const double d_orig = std::norm(vertices[edge.u] - vertices[edge.v]);
        return static_cast<int>(std::floor(d_orig * (1.0 + epsilon_)));
    }

    bool IsValidNorm(const Edge& edge, const double d_pose) const
    {
        const std::vector<Complex>& vertices = figure_.vertices;
        const double d_orig = std::norm(vertices[edge.u] - vertices[edge.v]);
        return abs(d_pose - d_orig) * kEpsDivisor <= epsilon_ * d_orig;
    }

private:
    const Hole hole_;
    const Figure figure_;
    const int epsilon_;
};


//------------------------
//  Pose

using Pose = std::vector<Complex>;

Json PoseToJson(const Pose& pose)
{
    Json vertices = Json::array();
    for (const Complex z : pose) {
        const int x = static_cast<int>(z.real());
        const int y = static_cast<int>(z.imag());
        vertices.push_back({x, y});
    }
    return {{"vertices", vertices}};
}

Pose PoseFromJson(const Json& json)
{
    return impl::ParseVertexArray(json.at("vertices"));
}


//------------------------
//  MoveTable

class MoveTable
{
public:
    MoveTable(int max_dx, int max_dy);

    MoveTable(const MoveTable&) = delete;
    MoveTable& operator=(const MoveTable&) = delete;

    const std::vector<Complex>& Get(double norm) const {
        return Get(static_cast<int>(norm));
    }
    const std::vector<Complex>& Get(int norm) const {
        const auto iter = map_.find(norm);
        return (iter != map_.end()) ? iter->second : empty_;
    }

private:
    std::unordered_map<int, std::vector<Complex>> map_;
    const std::vector<Complex> empty_;
};

MoveTable::MoveTable(int max_dx, int max_dy)
{
    for (int dy = 0; dy <= max_dy; dy++)
    for (int dx = 0; dx <= max_dx; dx++) {
        const int norm = dx * dx + dy * dy;
        map_[norm].emplace_back(-dx, -dy); map_[norm].emplace_back(-dx, +dy);
        map_[norm].emplace_back(+dx, -dy); map_[norm].emplace_back(+dx, +dy);
    }
}


//------------------------
//  Validate

bool Validate(const Problem& prob, const Pose& pose)
{
    const std::vector<Edge>& edges = prob.edges();

    return std::all_of(edges.begin(), edges.end(), [&](const Edge& e) {
        return prob.IsValidNorm(e, std::norm(pose[e.u] - pose[e.v]))
            && prob.hole().Contains(LineSeg{pose[e.u], pose[e.v]});
    });
}

long Dislikes(const Problem& prob, const Pose& pose)
{
    double dislikes = 0.0;

    for (const Complex zh : prob.hole().vertices()) {
        double min = kInf;
        for (const Complex zp : pose) min = std::min(min, norm(zh - zp));
        dislikes += min;
    }

    return static_cast<long>(dislikes);
}

#endif  // YUIZUMI_V2_H_
