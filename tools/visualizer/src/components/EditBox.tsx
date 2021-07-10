import * as React from "react";
import { Solution } from "../entities/Solution";

export type Props = {
    solution: Solution | undefined;
    onEdit: (solution: string) => void;
};

export const EditBox = (props: Props) => {
    return (
        <form onSubmit={e => {
            e.preventDefault();
            props.onEdit((e.target as EventTarget & { solution: { value: string; }; }).solution.value);
        }} >
            <textarea
                name="solution"
                cols={80}
                rows={20}
                defaultValue={props.solution && JSON.stringify(props.solution, undefined, 2)}
            />
            <input type="submit" />
        </form>
    );
};
