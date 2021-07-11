#include <iostream>
#include <optional>
#include <random>
#include "v1.h"

using namespace std;

namespace {

//------------------------
//  Parameters

constexpr int kWeightFlipVertex = 75;
constexpr int kWeightMoveVertex = 0;
constexpr int kWeightTranslate  = 25;

constexpr int kNumMutateSteps = 1000000;


//------------------------
//  Mutator

using Graph = vector<vector<int>>;

class Mutator
{
public:
    explicit Mutator(const Problem* prob);

    void Mutate(Pose& pose);

private:
    void FlipVertex(Pose& pose);
    void MoveVertex(Pose& pose);
    void Translate(Pose& pose);

    const Problem& prob_;
    Graph graph_;
    double x_size_, y_size_;
    mt19937 rng_;
};

Mutator::Mutator(const Problem* const prob)
    : prob_(*prob),
      graph_(prob_.vertices().size()),
    rng_()  // random_device()?
{
    for (const Edge& e : prob_.edges()) {
        graph_[e.u].push_back(e.v);
        graph_[e.v].push_back(e.u);
    }

    double xmin = +kInfinity, xmax = -kInfinity;
    double ymin = +kInfinity, ymax = -kInfinity;

    for (const Complex z : prob_.hole().vertices()) {
        xmin = min(xmin, z.real());
        ymin = min(ymin, z.imag());
        xmax = max(xmax, z.real());
        ymax = max(ymax, z.imag());
    }

    x_size_ = xmax - xmin, y_size_ = ymax - ymin;
}

void Mutator::Mutate(Pose& pose)
{
    discrete_distribution<> chooser({
        kWeightFlipVertex,
        kWeightMoveVertex,
        kWeightTranslate,
    });

    switch (chooser(rng_)) {
        case 0: FlipVertex(pose); break;
        case 1: MoveVertex(pose); break;
        case 2: Translate (pose); break;
    }
}

void Mutator::FlipVertex(Pose& pose)
{
    uniform_int_distribution<> index_dist(0, pose.size() - 1);
    const int i = index_dist(rng_);

    if (graph_[i].size() == 1) {
        const int j = graph_[i][0];

        uniform_real_distribution<> theta_dist(-M_PI, +M_PI);
        Complex z = polar(abs(pose[i] - pose[j]), theta_dist(rng_));
        z = Complex(round(z.real()), round(z.imag()));

        const double d_orig = norm(prob_.vertices()[i] - prob_.vertices()[j]);
        const double d_pose = norm(z - pose[j]);
        if (std::abs(d_pose - d_orig) * kEpsDenom > prob_.epsilon() * d_orig)
            return;

        pose[i] = z;
    }

    if (graph_[i].size() == 2) {
        const int j = graph_[i][0];
        const int k = graph_[i][1];

        const vector<Complex> zs = GetIntersections(
            Circle{pose[j], abs(pose[j] - pose[i])},
            Circle{pose[k], abs(pose[k] - pose[i])});
        if (zs.size() != 2) return;

        Complex z = (norm(pose[i] - zs[0]) > norm(pose[i] - zs[1])) ? zs[0] : zs[1];
        z = Complex(round(z.real()), round(z.imag()));

        for (const int j : graph_[i]) {
            const double d_orig = norm(prob_.vertices()[i] - prob_.vertices()[j]);
            const double d_pose = norm(z - pose[j]);
            if (std::abs(d_pose - d_orig) * kEpsDenom > prob_.epsilon() * d_orig)
                return;
        }

        pose[i] = z;
    }
}

void Mutator::MoveVertex(Pose& pose)
{
    // TODO: Implement.
}

void Mutator::Translate(Pose& pose)
{
    normal_distribution<> dx_dist(0.0, x_size_ / 2.0);
    const double dx = round(dx_dist(rng_));

    normal_distribution<> dy_dist(0.0, y_size_ / 2.0);
    const double dy = round(dy_dist(rng_));

    for (Complex& z : pose) z += Complex(dx, dy);
}


//------------------------
//  ComputeError

long ComputeError(const Problem& prob, const Pose& pose)
{
    vector<int> contains(pose.size());

    for (int i = 0; i < pose.size(); i++) {
        contains[i] = prob.hole().Contains(pose[i]);
    }

    const vector<Complex>& hole = prob.hole().vertices();
    long invalid = 0;

    for (const Edge& e : prob.edges()) {
        if (!contains[e.u] || !contains[e.v]) {
            ++invalid;
            continue;
        }
        for (int i = 0; i < hole.size(); i++) {
            const int j = (i + 1) % hole.size();
            if (Intersects({pose[e.u], pose[e.v]}, {hole[i], hole[j]}) == kCrossing) {
                ++invalid;
                break;
            }
        }
    }

    return invalid;
}


//------------------------
//  Solve

Pose Solve(const Problem& prob)
{
    Mutator mutator(&prob);

    Pose pose = prob.vertices();

    long best_score = 1L << 60;
    long best_error = ComputeError(prob, pose);
    Pose best_pose = pose;

    for (int step = 0; step < kNumMutateSteps; step++) {
        if (step % 1000 == 0) {
            cerr << "Step " << step << ": error = " << best_error << ", "
                 << "score = " << best_score << endl;
        }

        mutator.Mutate(pose);

        const long error = ComputeError(prob, pose);
        if (error == 0) {
            const long score = Evaluate(prob, pose);
            if (score < best_score) {
                best_score = score;
                best_pose = pose;
            }
            best_error = 0;
        } else if (error <= best_error) {
            best_error = error;
            best_pose = pose;
        }
    }

    return best_pose;
}

}  // namespace

//------------------------
//  Entrypoint

int main()
{
    Json json;
    cin >> json;

    const Problem prob = Problem::FromJson(json);
    const Pose pose = Solve(prob);

    if (Validate(prob, pose)) {
        cout << PoseToJson(pose) << endl;
        cerr << "dislikes = " << Evaluate(prob, pose) << endl;
    } else {
        cerr << "dislikes = (error)" << endl;
    }

    return 0;
}
