export type Problem = {
    hole: [[number, number]]
    figure: {
        edges: [[number, number]];
        vertices: [[number, number]];
    };
    epsilon: number;
};
