import * as React from "react";

export type Props = {
    onSubmit: (v: number, from: number, to: number) => void;
};

export const SegRotation = (props: Props) => {
    return (
        <form
            onSubmit={e => {
                e.preventDefault();
                const target = (e.target as EventTarget & { v: { value: string; };  from: { value: string; }; to: { value: string; }; });
                props.onSubmit(parseInt(target.v.value, 10), parseInt(target.from.value, 10), parseInt(target.to.value, 10));
            }}
        >
            <input type="text" name="v" placeholder="移動したい頂点番号" required />
            <input type="text" name="from" placeholder="線分の端点の頂点番号1" required />
            <input type="text" name="to" placeholder="線分の端点の頂点番号2" required />
            <input type="submit" value="鏡映移動する" />
        </form>
    );
};
