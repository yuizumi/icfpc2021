import * as React from "react";

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
            <input type="text" name="dx" placeholder="dx" required />
            <input type="text" name="dy" placeholder="dy" required />
            <input type="submit" value="平行移動する" />
        </form>
    );
};
