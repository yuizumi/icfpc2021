#include <cstdint>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <utility>
#include <vector>
#include "v2.h"

namespace {

using namespace std;


//------------------------
//  Random

class Random
{
public:
    explicit Random(uint_fast32_t seed) : rng_(seed) {}

    mt19937& rng() { return rng_; }

    bool Bernoulli(double prob)
    {
        return bernoulli_distribution(prob)(rng_);
    }
    
    double Get(double lo, double hi)
    {
        return uniform_real_distribution<double>(lo, hi)(rng_);
    }

    int Get(int lo, int hi)
    {
        return uniform_int_distribution<int>(lo, hi)(rng_);
    }

private:
    mt19937 rng_;
};


//------------------------
//  Hint

struct Hint
{
    int index;
    Complex z;

    static Hint FromJson(const Json& json);
};

Hint Hint::FromJson(const Json& json)
{
    return {
        .index = json[0].get<int>(),
        .z = Complex(json[1][0].get<int>(), json[1][1].get<int>()),
    };
}


//------------------------
//  Config

struct Config
{
    vector<Hint> hints;

    uint32_t seed = mt19937::default_seed;

    double prob_hole = 0.5;

    int num_poses = 1000;
    int max_total_steps = 1000000;
    int max_local_steps = 50;

    static Config FromJson(const Json& json);
};

Config Config::FromJson(const Json& json)
{
    Config config;

    if (json.contains("hints")) {
        config.hints.reserve(json.at("hints").size());
        for (const Json& hint : json.at("hints"))
            config.hints.push_back(Hint::FromJson(hint));
    }

    if (json.contains("seed")) {
        config.seed = json.at("seed").get<uint32_t>();
    }

    if (json.contains("prob_hole")) {
        config.prob_hole = json.at("prob_hole").get<double>();
    }
    if (json.contains("num_poses")) {
        config.num_poses = json.at("num_poses").get<int>();
    }
    if (json.contains("max_total_steps")) {
        config.max_total_steps = json.at("max_total_steps").get<int>();
    }
    if (json.contains("max_local_steps")) {
        config.max_local_steps = json.at("max_local_steps").get<int>();
    }

    return config;
}


//------------------------
//  Poser

class Poser
{
public:
    Poser(const Problem* prob, const Config* cfg)
        : prob_(*prob), cfg_(*cfg),
          random_(cfg_.seed) {}

    optional<Pose> MakePose();

private:
    bool MakePose(Pose& pose, int index);

    void Prepare(Pose& pose);

    bool IsFeasible(const Pose& pose, Complex z, int v) const;

    optional<Complex> LocateHole(const Pose& pose, int v);

    optional<Complex> LocateDeg0(const Pose& pose, int v);
    optional<Complex> LocateDeg1(const Pose& pose, int v, int u);
    optional<Complex> LocateDeg2(const Pose& pose, int v, int u, int t);

    optional<Complex> Locate(const Pose& pose, int v);

    const Problem& prob_;
    const Config& cfg_;

    int steps_left_;
    vector<vector<int>> adj_;
    vector<int> order_;
    Random random_;
};

optional<Pose> Poser::MakePose()
{
    Pose pose(prob_.vertices().size());
    steps_left_ = cfg_.max_total_steps;
    Prepare(pose);
    if (MakePose(pose, cfg_.hints.size())) return pose;
    return nullopt;
}

void Poser::Prepare(Pose& pose)
{
    const int n = prob_.vertices().size();
    vector<vector<int>> adj(n);

    for (const Edge& edge : prob_.edges()) {
        adj[edge.u].push_back(edge.v);
        adj[edge.v].push_back(edge.u);
    }

    order_.clear();
    adj_.clear();

    order_.reserve(n);
    adj_.resize(n);

    vector<int> done(n);

    for (const Hint& hint : cfg_.hints) {
        const int u = hint.index;
        pose[u] = hint.z;

        order_.push_back(u);

        shuffle(adj[u].begin(), adj[u].end(), random_.rng());
        for (const int v : adj[u]) { if (!done[v]) adj_[v].push_back(u); }
        done[u] = true;
    }

    vector<int> next;
    next.reserve(n);

    while (order_.size() < n) {
        int max_deg = -1;

        for (int v = 0; v < n; v++) {
            const int deg = adj_[v].size();
            if (!done[v] && deg >= max_deg) {
                if (deg != max_deg) next.clear();
                next.push_back(v);
                max_deg = deg;
            }
        }

        const int u = next[random_.Get(0, next.size() - 1)];

        order_.push_back(u);

        shuffle(adj[u].begin(), adj[u].end(), random_.rng());
        for (const int v : adj[u]) { if (!done[v]) adj_[v].push_back(u); }
        done[u] = true;
    }
}

bool Poser::IsFeasible(const Pose& pose, Complex z, int v) const
{
    return all_of(adj_[v].begin(), adj_[v].end(), [&](const int u) {
        return prob_.IsValidNorm(Edge{u, v}, norm(pose[u] - z))
            && prob_.hole().Contains(LineSeg{pose[u], z});
    });
}

optional<Complex> Poser::LocateHole(const Pose& pose, int v)
{
    optional<Complex> picked;
    int count = 0;

    for (const Complex z : prob_.hole().vertices()) {
        bool done = false;

        for (const int u : order_) {
            if (u == v) break;
            if (pose[u] == z) { done = true; break; }
        }
        if (done) continue;

        if (IsFeasible(pose, z, v) && random_.Get(0, count++) == 0)
            picked = z;
    }

    return picked;
}

optional<Complex> Poser::LocateDeg0(const Pose& pose, int v)
{
    const Hole& hole = prob_.hole();
    while (true) {
        const Complex z(random_.Get(hole.xmin(), hole.xmax()),
                        random_.Get(hole.ymin(), hole.ymax()));
        if (hole.Contains(z)) return z;
    }
}

optional<Complex> Poser::LocateDeg1(const Pose& pose, int v, int u)
{
    const double norm = random_.Get(prob_.GetMinNorm(Edge{u, v}),
                                    prob_.GetMaxNorm(Edge{u, v}));
    const double arg = random_.Get(-M_PI, +M_PI);
    return pose[u] + polar(sqrt(norm), arg);
}

optional<Complex> Poser::LocateDeg2(const Pose& pose, int v, int u, int t)
{
    const double rt_sq = random_.Get(prob_.GetMinNorm(Edge{t, v}),
                                     prob_.GetMaxNorm(Edge{t, v}));
    const double ru_sq = random_.Get(prob_.GetMinNorm(Edge{u, v}),
                                     prob_.GetMaxNorm(Edge{u, v}));

    const vector<Complex> zs = GetIntersections(Circle{pose[t], sqrt(rt_sq)},
                                                Circle{pose[u], sqrt(ru_sq)});
    if (zs.empty()) return nullopt;

    return (random_.Bernoulli(0.5)) ? zs.front() : zs.back();
}

optional<Complex> Poser::Locate(const Pose& pose, int v)
{
    if (random_.Bernoulli(cfg_.prob_hole)) {
        const optional<Complex> z = LocateHole(pose, v);
        if (z.has_value()) return z;
    }

    int adj = -1;

    for (const int u : adj_[v]) {
        if (adj == -1) {
            adj = u;
        } else {
            if (pose[u] != pose[adj]) return LocateDeg2(pose, v, u, adj);
        }
    }
    return (adj == -1) ? LocateDeg0(pose, v) :LocateDeg1(pose, v, adj);
}

bool Poser::MakePose(Pose& pose, int index)
{
    if (index == order_.size()) {
        return true;
    }

    const int v = order_[index];
    vector<Complex> done;

    for (int step = 0; step < cfg_.max_local_steps; step++) {
        if (--steps_left_ < 0)
            return false;

        const optional<Complex> z = Locate(pose, v);
        if (!z.has_value()) return false;

        pose[v] = Complex(round(z->real()), round(z->imag()));

        if (find(done.begin(), done.end(), pose[v]) != done.end())
            continue;
        done.push_back(pose[v]);

        if (IsFeasible(pose, pose[v], v) && MakePose(pose, index + 1))
            return true;
    }

    return false;
}


//------------------------
//  Solve

optional<Pose> Solve(const Problem& prob, const Config& cfg)
{
    long best_dislikes = numeric_limits<long>::max();
    optional<Pose> best_pose;

    Poser poser(&prob, &cfg);

    for (int i = 1; i <= cfg.num_poses; i++) {
        const optional<Pose> pose = poser.MakePose();
        if (pose.has_value()) {
            const long dislikes = Dislikes(prob, *pose);
            if (dislikes < best_dislikes) {
                cerr << "[" << i << ":" << dislikes << "]";
                best_dislikes = dislikes;
                best_pose = pose;
            }
            if (dislikes == 0) break;
        }
        if (i % 10 == 0) {
            (i % 100 == 0) ? (cerr << "(" << i << ")") : (cerr << ".");
        }
    }
    cerr << endl;
    return best_pose;
}

}  // namespace

//------------------------
//  Entrypoint

int main(int argc, char* argv[])
{
    Config cfg;

    if (argc >= 2) cfg = Config::FromJson(Json::parse(argv[1]));
    Json json;
    cin >> json;
    const optional<Pose> pose = Solve(Problem::FromJson(json), cfg);
    if (pose.has_value()) cout << PoseToJson(*pose) << endl;

    return 0;
}
