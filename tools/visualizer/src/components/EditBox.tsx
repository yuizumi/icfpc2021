import * as React from "react";
import { Solution } from "../entities/Solution";

export type Props = {
    solution: Solution | undefined;
    dislikes: number | undefined;
    errors: string[] | undefined;
    onEdit: (solution: string) => void;
    onClear: () => void;
    onInit: () => void;
};

export const EditBox = (props: Props) => {
    return (
        <form
            onSubmit={e => {
                e.preventDefault();
                props.onEdit((e.target as EventTarget & { solution: { value: string; }; }).solution.value);
            }}
            onReset={e => {
                props.onClear();
            }}
        >
            <div>
                <textarea
                    name="solution"
                    cols={80}
                    rows={20}
                    defaultValue={props.solution != null ? JSON.stringify(props.solution, undefined, 2) : ""}
                />
            </div>
            <div>
                <input type="submit" />
                <input type="reset" />
                <button onClick={() => props.onInit()}>初期ポーズを読み込む</button>
            </div>
            <div>
                <input type="text" readOnly value={"dislikes: " + props.dislikes} />
            </div>
            <div>
                <textarea
                    cols={80}
                    rows={20}
                    readOnly
                    value={JSON.stringify(props.errors)}
                />
            </div>
        </form>
    );
};
