import * as React from "react";

export type Props = {
    onSubmit: (r: number) => void;
};

export const Fit = (props: Props) => {
    return (
        <form
            onSubmit={e => {
                e.preventDefault();
                const target = (e.target as EventTarget & { r: { value: string; }; });
                props.onSubmit(parseInt(target.r.value, 10));
            }}
        >
            <input type="text" name="r" placeholder="r" required />
            <input type="submit" value="holeの頂点付近を合わせる" />
        </form>
    );
};
