import * as React from "react";

export type Props = {
    onSubmit: (v: number, base: number, axis: "x" | "y") => void;
};

export const ZRotation = (props: Props) => {
    return (
        <form
            onSubmit={e => {
                e.preventDefault();
                const target = (e.target as EventTarget & { v: { value: string; };  base: { value: string; }; axis: { value: "x" | "y"; }; });
                props.onSubmit(parseInt(target.v.value, 10), parseInt(target.base.value, 10), target.axis.value);
            }}
        >
            <input type="text" name="v" placeholder="移動したい頂点番号" required />
            <input type="text" name="base" placeholder="基点の頂点番号" required />
            <select name="axis" required>
                <option value="x">x軸方向を軸に</option>
                <option value="y">y軸方向を軸に</option>
            </select>
            <input type="submit" value="鏡映移動する" />
        </form>
    );
};
