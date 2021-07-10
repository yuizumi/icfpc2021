import * as React from "react";
import { ViewCanvas, Props } from "./ViewConvas";

export { Props };

export const View = (props: Props) => {
    return (
        <div>
            <ViewCanvas {...props} />
        </div>
    );
};