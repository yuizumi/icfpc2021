import { observable, action, computed, makeAutoObservable } from "mobx";
import { Problem } from "../entities/Problem";
import { Solution } from "../entities/Solution";

export type VisualizerState = {
  apiKey: string | undefined;
  selectedId: number | undefined;
  problem: Problem | undefined;
  solution: Solution | undefined;
  numOfProblems: number;
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
    numOfProblems: 78
  };

  @computed get getScore(): number {
    return NaN; // TODO
  }

  @action.bound onMove(idx: number): void {
    this.state.selectedId = idx;
    console.log(`fetch ${idx}`, this.state); // TODO
    this.fetchProblem(idx);
    this.fetchSolution(idx);
  }

  private fetchProblem(idx: number): Promise<void> {
    return window.fetch(`http://localhost:3000/problems/${("00" + String(idx)).substr(-3)}.problem`)
      .then(res => res.ok ? Promise.resolve(res.json()) : Promise.reject())
      .then(
        action((json: Problem) => {
          console.log("problem", idx, json);
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
          console.log("solution", idx, json);
          this.state.solution = json;
        }),
        action(() => {
          this.state.solution = undefined;
        })
      )
  }

  @action.bound onEdit(solution: string | undefined): void {
    if(solution != null) {
      try {
        this.state.solution = JSON.parse(solution);
      } catch(e) {}
    }
  }
}
