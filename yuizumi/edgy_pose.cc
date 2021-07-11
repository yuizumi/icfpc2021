#include <algorithm>
#include <iostream>
#include <limits>
#include <optional>
#include <random>
#include <vector>
#include "v1.h"

namespace {

using namespace std;


//------------------------
//  Parameters

#if PARAM_SET == 1
constexpr int kMaxRetries = 100;
constexpr int kMaxTotalRetries = 1000000;
constexpr int kNumPoses = 100;
#else
constexpr int kMaxRetries = 50;
constexpr int kMaxTotalRetries = 50000;
constexpr int kNumPoses = 1000;
#endif

//------------------------
//  Utility

bool Intersects(const Polygon& hole, const LineSegment& line)
{
    const vector<Complex>& v = hole.vertices();

    for (int i = 0; i < hole.size(); i++) {
        const int j = (i + 1) % hole.size();
        if (Intersects({v[i], v[j]}, line) == kCrossing) return true;
    }
    return !hole.Contains((line.z1 + line.z2) / 2.0);
}


//------------------------
//  Poser

class Poser
{
public:
    explicit Poser(const Problem* prob);

    optional<Pose> MakePose() {
        Pose pose(prob_.vertices().size());
        trial_ = 0;
        InitOrder();
        if (MakePose(pose, 0)) return pose;
        return nullopt;
    }

private:
    bool MakePose(Pose& pose, int index);

    void InitXYChooser();
    void InitOrder();

    optional<Complex> PickPoint1(const Pose& pose, int v, int u);
    optional<Complex> PickPoint2(const Pose& pose, int v, int u, int t);
    optional<Complex> PickFromHole(const Pose& pose, int v);

    optional<Complex> PickPoint(const Pose& pose, int v);

    const Problem& prob_;

    vector<int> order_;
    vector<vector<int>> adj_;

    int trial_;

    mt19937 rng_;

    uniform_int_distribution<int> hole_chooser_;
    uniform_real_distribution<double> eps_chooser_;
    uniform_real_distribution<double> arg_chooser_;
    bernoulli_distribution bool_chooser_{0.5};
};

// TODO: Initialize rng_ with std::random_device?
Poser::Poser(const Problem* prob)
    : prob_(*prob),
      hole_chooser_(0, prob_.hole().size() - 1),
      eps_chooser_(1.0 - prob_.epsilon() / kEpsDenom,
                   1.0 + prob_.epsilon() / kEpsDenom),
      arg_chooser_(-M_PI, +M_PI)
{
}

void Poser::InitOrder()
{
    const int n = prob_.vertices().size();
    vector<vector<int>> adj(n);

    for (const Edge& e : prob_.edges()) {
        adj[e.u].push_back(e.v);
        adj[e.v].push_back(e.u);
    }

    order_.resize(n);
    adj_.resize(n);
    for (int i = 0; i < n; i++) {
        adj_[i].clear();
        order_[i] = i;
    }

    shuffle(order_.begin(), order_.end(), rng_);

    vector<int> done(n);

    for (int i = 0; i < n; i++) {
        const int u = order_[i];
        for (const int v : adj[u]) { if (!done[v]) adj_[v].push_back(u); }
        done[u] = true;
    }
}

optional<Complex> Poser::PickPoint1(const Pose& pose, const int v, const int u)
{
    const vector<Complex>& orig = prob_.vertices();

    const double dist = sqrt(norm(orig[v] - orig[u]) * eps_chooser_(rng_));
    return pose[u] + polar(dist, arg_chooser_(rng_));
}

optional<Complex> Poser::PickPoint2(const Pose& pose, const int v, const int u,
                                    const int t)
{
    const vector<Complex>& orig = prob_.vertices();

    const vector<Complex> zs = GetIntersections(
        Circle{pose[t], sqrt(norm(orig[v] - orig[t]) * eps_chooser_(rng_))},
        Circle{pose[u], sqrt(norm(orig[v] - orig[u]) * eps_chooser_(rng_))}
    );
    if (zs.empty()) return nullopt;

    return (bool_chooser_(rng_)) ? zs.front() : zs.back();
}

optional<Complex> Poser::PickFromHole(const Pose& pose, const int v)
{
    const vector<Complex>& orig = prob_.vertices();

    const int offset = hole_chooser_(rng_);
    const Polygon& hole = prob_.hole();

    for (int i = 0; i < hole.size(); i++) {
        const Complex z = hole.vertices()[(i + offset) % hole.size()];
        bool done = false;

        for (const int j : order_) {
            if (j == v) break;
            if (pose[j] == z) { done = true; break; }
        }
        if (done) continue;

        const bool verify = all_of(adj_[v].begin(), adj_[v].end(), [&](const int w) {
            const double d_orig = norm(orig[v] - orig[w]);
            const double d_pose = norm(z - pose[w]);
            if ((d_pose - d_orig) * kEpsDenom > prob_.epsilon() * d_orig) {
                return false;
            }
            return !Intersects(hole, LineSegment{pose[w], z});
        });

        if (verify) return z;
    }

    return nullopt;
}

optional<Complex> Poser::PickPoint(const Pose& pose, const int v)
{
    optional<Complex> z = PickFromHole(pose, v);
    if (z.has_value()) return z;

    int adj = -1;

    for (const int u : adj_[v]) {
        if (adj == -1) {
            adj = u;
        } else {
            if (pose[u] != pose[adj]) return PickPoint2(pose, v, u, adj);
        }
    }

    return PickPoint1(pose, v, adj);
}

bool Poser::MakePose(Pose& pose, const int index)
{
    if (index == order_.size()) {
        return true;
    }

    const int v = order_[index];

    const vector<Complex>& orig = prob_.vertices();
    vector<Complex> done;

    for (int trial = 0; trial < kMaxRetries; ++trial) {
        if (++trial_ >= kMaxTotalRetries) {
            return false;
        }

        const optional<Complex> z = PickPoint(pose, v);
        if (!z.has_value()) return false;

        pose[v] = Complex(round(z->real()), round(z->imag()));

        if (find(done.begin(), done.end(), pose[v]) != done.end())
            continue;
        done.push_back(pose[v]);

        if (!prob_.hole().Contains(pose[v])) continue;

        const bool verify = all_of(adj_[v].begin(), adj_[v].end(), [&](const int u) {
            const double d_pose = norm(pose[u] - pose[v]);
            const double d_orig = norm(orig[u] - orig[v]);
            if (abs(d_pose - d_orig) * kEpsDenom > prob_.epsilon() * d_orig)
                return false;
            return !Intersects(prob_.hole(), LineSegment{pose[u], pose[v]});
        });

        if (verify && MakePose(pose, index + 1)) return true;
    }

    return false;
}

optional<Pose> Solve(const Problem& prob)
{
    long best_dislikes = numeric_limits<long>::max();
    optional<Pose> best_pose;

    Poser poser(&prob);

    for (int i = 1; i <= kNumPoses; i++) {
        const optional<Pose> pose = poser.MakePose();
        if (pose.has_value()) {
            const long dislikes = Evaluate(prob, *pose);
            if (dislikes == 0) return pose;

            if (dislikes < best_dislikes) {
                cerr << "Trial #" << i << ": dislikes = " << dislikes << endl;
                best_dislikes = dislikes;
                best_pose = pose;
            }
        }
        if (i % 100 == 0) {
            cerr << "(" << i << " trials)" << endl;
        }
    }

    return best_pose;
}

}  // namespace

int main()
{
    Json json;
    cin >> json;
    const optional<Pose> pose = Solve(Problem::FromJson(json));
    if (pose.has_value()) cout << PoseToJson(*pose) << endl;

    return 0;
}
