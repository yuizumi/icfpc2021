import * as React from "react";

export type Props = {
    onSubmit: (cx: number, cy: number, deg: number) => void;
};

export const Rotation = (props: Props) => {
    return (
        <form
            onSubmit={e => {
                e.preventDefault();
                const target = (e.target as EventTarget & { cx: { value: string; }; cy: { value: string; }; deg: { value: string; }; });
                props.onSubmit(parseInt(target.cx.value, 10), parseInt(target.cy.value, 10), parseInt(target.deg.value, 10));
            }}
        >
            <input type="text" name="cx" placeholder="cx" />
            <input type="text" name="cy" placeholder="cy" />
            <input type="text" name="deg" placeholder="deg" />
            <input type="submit" value="回転移動する" />
        </form>
    );
};
