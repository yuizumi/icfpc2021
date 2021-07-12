# Team undefined

## Solvers

`yuizumi/` contains several automatic solvers. `hybrid_pose` is the best one
of ours, though presumably not as cool as the solvers from other teams.

Each solver is written in C++ and can be compiled alone with Clang:

```bash
$ clang++ -O3 -Wall --std=c++17 -o hybrid_pose yuizumi/hybrid_pose.cc
```

NOTE: GCC should also do, but has not been checked.

We have solved a number of problems by hand as well, using Emacs(!?) and/or
the visualizer (see below).

## Strategies

Random, random, random. Prefer the vertices on the hole. Period.

## Visualizer

`visualizer/` contains our visualizer, simple but useful enough. It comes with
some aids for humans to solve the problems (e.g. translation, rotation).

The visualizer runs as a node server, thus requires node.js to run:

```bash
$ cd server
$ npm install
$ npm build:eval && npm start
```
## Other tools

`tools/` contains small scripts used during the contest.
