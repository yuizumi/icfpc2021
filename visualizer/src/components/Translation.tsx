import * as React from "react";
import { Solution } from "../entities/Solution";

export type Props = {
    onSubmit: (dx: number, dy: number) => void;
};

export const Translation = (props: Props) => {
    return (
        <form
            onSubmit={e => {
                e.preventDefault();
                const target = (e.target as EventTarget & { dx: { value: string; }; dy: { value: string; }; });
                props.onSubmit(parseInt(target.dx.value, 10), parseInt(target.dy.value, 10));
            }}
        >
            <div>
                <input type="text" name="dx" defaultValue={0} />
                <input type="text" name="dy" defaultValue={0} />
                <input type="submit" value="平行移動する" />
            </div>
        </form>
    );
};
