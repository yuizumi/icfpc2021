import * as React from "react";
import { ViewCanvas, Props } from "./ViewCanvas";

export { Props };

export const View = (props: Props) => {
    return (
        <div>
            <ViewCanvas {...props} />
        </div>
    );
};
