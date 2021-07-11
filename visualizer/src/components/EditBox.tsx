import * as React from "react";
import { Solution } from "../entities/Solution";

export type Props = {
    solution: Solution | undefined;
    onEdit: (solution: string) => void;
    onClear: () => void;
    onInit: () => void;
};

export type State = {
    solution?: string;
    viewSolution?: string;
};

export class EditBox extends React.Component<Props, State> {
    constructor(props: Props) {
        super(props);
        this.state = {
            solution: props.solution != null ? JSON.stringify(props.solution, undefined, 2) : "",
            viewSolution: props.solution != null ? JSON.stringify(props.solution, undefined, 2) : ""
        };
    }

    render() {
        const s = this.props.solution != null ? JSON.stringify(this.props.solution, undefined, 2) : "";
        if (s !== this.state.solution) {
            this.setState({ solution: s, viewSolution: s });
        }
        return (
            <form
                onSubmit={e => {
                    e.preventDefault();
                    this.props.onEdit((e.target as EventTarget & { solution: { value: string; }; }).solution.value);
                }}
                onReset={e => {
                    this.props.onClear();
                }}
            >
                <div>
                    <textarea
                        name="solution"
                        cols={80}
                        rows={20}
                        value={this.state.viewSolution}
                        onChange={e => {
                            this.setState({ viewSolution: e.target.value })
                        }}
                    />
                </div>
                <div>
                    <input type="submit" />
                    <input type="reset" />
                    <button onClick={() => this.props.onInit()}>初期ポーズを読み込む</button>
                </div>
            </form>
        );
    };
}
