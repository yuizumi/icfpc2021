import * as React from "react";

export type Props = {
    onRound: () => void;
    onFloor: () => void;
    onCeil: () => void;
};

export const Round = (props: Props) => {
    return (
        <form
            onSubmit={e => {
                e.preventDefault();
            }}
        >
            <input type="submit" value="切り捨て" onClick={() => props.onFloor()} />
            <input type="submit" value="切り上げ" onClick={() => props.onCeil()}/>
            <input type="submit" value="四捨五入" onClick={() => props.onRound()} />
        </form>
    );
};
