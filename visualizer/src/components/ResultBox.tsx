import * as React from "react";
import { EvalError } from "../entities/EvalError";

export type Props = {
    dislikes: number | undefined;
    errors: EvalError[] | undefined;
};

export const ResultBox = (props: Props) => {
    return (
        <div>
            <div>
                <input type="text" readOnly value={"dislikes: " + props.dislikes} />
            </div>
            <div>
                <textarea
                    cols={80}
                    rows={20}
                    readOnly
                    value={props.errors && props.errors.map(e => e.message).join("\n")}
                />
            </div>
        </div>
    );
};
