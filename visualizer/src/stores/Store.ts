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
    numOfProblems: 88,
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
}
