import { observable, action, computed, makeAutoObservable } from "mobx";
import { EvalError } from "../entities/EvalError";
import { Problem } from "../entities/Problem";
import { Solution } from "../entities/Solution";

export type VisualizerState = {
  apiKey: string | undefined;
  selectedId: number | undefined;
  problem: Problem | undefined;
  solution: Solution | undefined;
  numOfProblems: number;
  dislikes: number | undefined;
  errors: EvalError[] | undefined;
};

export class Store {
  constructor() {
    makeAutoObservable(this)
  }

  @observable state: VisualizerState = {
    apiKey: undefined,
    selectedId: undefined,
    problem: undefined,
    solution: undefined,
    numOfProblems: 132,
    dislikes: undefined,
    errors: undefined
  };

  @action.bound async onMove(idx: number): Promise<void> {
    this.state.selectedId = idx;
    await this.fetchProblem(idx);
    await this.fetchSolution(idx);
    await this.fetchScore(idx);
  }

  private fetchProblem(idx: number): Promise<void> {
    return window.fetch(`http://localhost:3000/problems/${("00" + String(idx)).substr(-3)}.problem`)
      .then(res => res.ok ? Promise.resolve(res.json()) : Promise.reject())
      .then(
        action((json: Problem) => {
          this.state.problem = json;
        }),
        action(() => {
          this.state.solution = undefined;
        })
      );
  }

  private fetchSolution(idx: number): Promise<void> {
    return window.fetch(`http://localhost:3000/solutions/${("00" + String(idx)).substr(-3)}.json`)
      .then(res => res.ok ? Promise.resolve(res.json()) : Promise.reject())
      .then(
        action((json: Solution) => {
          this.state.solution = json;
        }),
        action(() => {
          this.state.solution = undefined;
        })
      )
  }

  private fetchScore(idx: number): Promise<void> {
    if (this.state.solution != null) {
      return window.fetch(`http://localhost:3000/solutions/eval/${idx}`, {
        method: "POST",
        headers: {
          "Content-Type": "application/json"
        },
        body: JSON.stringify(this.state.solution)
      })
        .then(res => res.ok ? Promise.resolve(res.json()) : Promise.reject())
        .then(
          action((json: {
            dislikes: number;
            errors: EvalError[]
          }) => {
            this.state.dislikes = json.dislikes;
            this.state.errors = json.errors;
          }),
          action((e) => {
            this.state.dislikes = undefined;
            this.state.errors = [e];
          })
        )
    } else {
      this.state.dislikes = undefined;
      this.state.errors = undefined;
      return Promise.resolve();
    }
  }

  @action.bound onEdit(solution: string | undefined): void {
    if (solution != null) {
      try {
        this.state.solution = JSON.parse(solution);
        if (this.state.selectedId != null) {
          this.fetchScore(this.state.selectedId);
        }
      } catch (e) { }
    }
  }

  @action.bound onClear(): void {
    this.state.solution = undefined;
    this.state.dislikes = undefined;
    this.state.errors = undefined;
  }

  @action.bound onInit(): void {
    this.state.solution = this.state.problem && { vertices: this.state.problem.figure.vertices };
  }

  @action.bound onTraslate(dx: number, dy: number): void {
    if (this.state.solution != null) {
      this.state.solution = { vertices: this.state.solution.vertices.map(v => [v[0] + dx, v[1] + dy]) };
    }
  }

  @action.bound onRotate(cx: number, cy: number, deg: number): void {
    if (this.state.solution != null) {
      this.state.solution = {
        vertices: this.state.solution.vertices.map(v => {
          const dx = v[0] - cx;
          const dy = v[1] - cy;
          const rad = deg * Math.PI / 180;
          const dx_ = Math.cos(rad) * dx - Math.sin(rad) * dy;
          const dy_ = Math.sin(rad) * dx + Math.cos(rad) * dy;
          return [cx + dx_, cy + dy_];
        })
      };
    }
  }

  @action.bound onRound(): void {
    if (this.state.solution != null) {
      this.state.solution = { vertices: this.state.solution.vertices.map(v => [Math.round(v[0]), Math.round(v[1])]) };
    }
  }

  @action.bound onCeil(): void {
    if (this.state.solution != null) {
      this.state.solution = { vertices: this.state.solution.vertices.map(v => [Math.ceil(v[0]), Math.ceil(v[1])]) };
    }
  }

  @action.bound onFloor(): void {
    if (this.state.solution != null) {
      this.state.solution = { vertices: this.state.solution.vertices.map(v => [Math.floor(v[0]), Math.floor(v[1])]) };
    }
  }

  @action.bound onZRotate(v: number, base: number, axis: "x" | "y"): void {
    if (this.state.solution != null && this.state.solution.vertices[v] != null && this.state.solution.vertices[base] != null) {
      const a = Array.of(...this.state.solution.vertices);
      if (axis === "x") {
        a[v] =
          [this.state.solution.vertices[v][0], 2 * this.state.solution.vertices[base][1] - this.state.solution.vertices[v][1]];
      } else {
        a[v] =
          [2 * this.state.solution.vertices[base][0] - this.state.solution.vertices[v][0], this.state.solution.vertices[v][1]];
      }
      this.state.solution = { vertices: a };
    }
  }

  @action.bound onSegRotate(v: number, from: number, to: number): void {
    if (this.state.solution != null && this.state.solution.vertices[v] != null
      && this.state.solution.vertices[from] != null && this.state.solution.vertices[to] != null) {
      const a = Array.of(...this.state.solution.vertices);
      const f = this.state.solution.vertices[from];
      const seg = [this.state.solution.vertices[to][0] - f[0], this.state.solution.vertices[to][1] - f[1]];
      const norm = Math.hypot(...seg);
      const normalized = seg.map(v => v / norm);
      const d = [this.state.solution.vertices[v][0] - f[0], this.state.solution.vertices[v][1] - f[1]];
      const det = normalized[0] * d[0] + normalized[1] * d[1];
      const p = [d[0] - det * normalized[0], d[1] - det * normalized[1]];
      a[v] = [f[0] + det * normalized[0] - p[0], f[1] + det * normalized[1] - p[1]];
      this.state.solution = { vertices: a };
    }
  }

  @action.bound onFit(r: number): void {
    if (this.state.problem != null && this.state.solution != null) {
      const hole = this.state.problem.hole;
      this.state.solution = {
        vertices: this.state.solution.vertices.map(v => {
          for (const h of hole) {
            if (Math.hypot(v[0] - h[0], v[1] - h[1]) <= r) {
              return h;
            }
          }
          return v;
        })
      };
    }
  }

  @action.bound showHints(): void {
    if (this.state.problem != null && this.state.solution != null) {
      const hole = this.state.problem.hole;
      const hints: [number, [number, number]][] = [];
      this.state.solution.vertices.forEach((v, index) => {
        for (const z of hole) {
          if (v[0] == z[0] && v[1] == z[1]) {
            hints.push([ index, z ]);
          }
        }
      });
      console.log(JSON.stringify(hints));
    }
  }
}
