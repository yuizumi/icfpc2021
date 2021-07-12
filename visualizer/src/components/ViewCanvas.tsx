import * as React from "react";
import { Problem } from "../entities/Problem";
import { Solution } from "../entities/Solution";

export type Props = {
    problem: Problem | undefined;
    solution: Solution | undefined;
};

export class ViewCanvas extends React.Component<Props> {
    render() {
        try {
            const viewBox = this.props.problem && this.computeViewBox(this.props.problem);
            const edges = this.props.problem?.figure.edges;
            const ok: [number, number][] = [];
            const ng: [number, number][] = [];
            if (this.props.problem != null && this.props.solution != null && edges != null) {
                for (let i = 0; i < edges.length; i++) {
                    const u = this.props.problem.figure.vertices[edges[i][0]];
                    const v = this.props.problem.figure.vertices[edges[i][1]];
                    const u_ = this.props.solution.vertices[edges[i][0]];
                    const v_ = this.props.solution.vertices[edges[i][1]];
                    const d = Math.abs(Math.hypot(v_[0] - u_[0], v_[1] - u_[1]) / Math.hypot(v[0] - u[0], v[1] - u[1]) - 1) * 1000000;
                    if (d < this.props.problem.epsilon + 1) {
                        ok.push(edges[i]);
                    } else {
                        ng.push(edges[i]);
                    }
                }
            }
            return (
                <svg
                    id="svg"
                    xmlns="http://www.w3.org/2000/svg"
                    xmlnsXlink="http://www.w3.org/1999/xlink"
                    style={{
                        border: "1px solid #333",
                        height: "600px",
                        width: "600px"
                    }}
                    viewBox={(() => {
                        if (viewBox != null) {
                            const sx = viewBox[0];
                            const sy = viewBox[1];
                            const tx = viewBox[2];
                            const ty = viewBox[3];
                            return `${sx} ${sy} ${tx - sx} ${ty - sy}`;
                        } else {
                            return undefined;
                        }
                    })()}
                >
                    <path
                        id="hole"
                        style={{
                            fill: "#666",
                            fillRule: "evenodd",
                            stroke: "none"
                        }}
                        d={(() => {
                            if (this.props.problem != null && viewBox != null) {
                                return this.buildHolePath(this.props.problem.hole, viewBox);
                            } else {
                                return undefined;
                            }
                        })()}
                    />
                    <path
                        id="figure"
                        style={{
                            fill: "none",
                            stroke: "#fdd",
                            strokeLinecap: "round"
                        }}
                        d={(() => {
                            if (this.props.problem != null) {
                                return this.buildFigurePath(this.props.problem.figure.vertices, this.props.problem.figure.edges);
                            } else {
                                return undefined;
                            }
                        })()}
                    />
                    <path
                        id="output-ok"
                        style={{
                            fill: "none",
                            stroke: "#f00",
                            strokeLinecap: "round"
                        }}
                        d={(() => {
                            if (this.props.problem != null && this.props.solution != null) {
                                return this.buildFigurePath(this.props.solution.vertices, ok);
                            } else {
                                return undefined;
                            }
                        })()}
                    />
                    <path
                        id="output-ng"
                        style={{
                            fill: "none",
                            stroke: "#00f",
                            strokeLinecap: "round"
                        }}
                        d={(() => {
                            if (this.props.problem != null && this.props.solution != null) {
                                return this.buildFigurePath(this.props.solution.vertices, ng);
                            } else {
                                return undefined;
                            }
                        })()}
                    />
                    {
                        this.props.problem?.hole.map((v, i) => (
                            <text x={v[0]} y={v[1]} fontSize="3">
                                {i}
                            </text>
                        ))
                    }
                    {
                        this.props.solution?.vertices.map((v, i) => (
                            <text x={v[0]} y={v[1]} fontSize="3">
                                {i}
                            </text>
                        ))
                    }
                </svg>
            );
        } catch (e) {
            return <div>error</div>;
        }
    }

    private computeViewBox(problem: Problem): [number, number, number, number] {
        let xmin = +Infinity;
        let ymin = +Infinity;
        let xmax = -Infinity;
        let ymax = -Infinity;

        for (const v of problem.hole) {
            const x = v[0], y = v[1];
            xmin = Math.min(xmin, x); ymin = Math.min(ymin, y);
            xmax = Math.max(xmax, x); ymax = Math.max(ymax, y);
        }
        for (const v of problem.figure.vertices) {
            const x = v[0], y = v[1];
            xmin = Math.min(xmin, x); ymin = Math.min(ymin, y);
            xmax = Math.max(xmax, x); ymax = Math.max(ymax, y);
        }

        xmin -= 5; ymin -= 5;
        xmax += 5; ymax += 5;

        return [xmin, ymin, xmax, ymax];
    }

    private buildHolePath(vertices: [number, number][], viewBox: [number, number, number, number]) {
        let path = "";

        for (const v of vertices) {
            path += path ? "L" : "M";
            path += ` ${v[0]} ${v[1]} `;
        }

        const xmin = viewBox[0];
        const ymin = viewBox[1];
        const xmax = viewBox[2];
        const ymax = viewBox[3];
        path += `Z M ${xmin} ${ymin} L ${xmax} ${ymin} L ${xmax} ${ymax} `
            + `L ${xmin} ${ymax}`;

        return path;
    }

    private buildFigurePath(vertices: [number, number][], edges: [number, number][]) {
        let path = "";

        for (const e of edges) {
            const u = vertices[e[0]], ux = u[0], uy = u[1];
            const v = vertices[e[1]], vx = v[0], vy = v[1];
            path += `M ${ux},${uy} L ${vx},${vy} `;
        }

        return path;
    }

};