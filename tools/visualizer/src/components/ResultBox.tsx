import * as React from "react";
import { Solution } from "../entities/Solution";

export type Props = {
    dislikes: number | undefined;
    errors: string[] | undefined;
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
                    value={JSON.stringify(props.errors)}
                />
            </div>
        </div>
    );
};
