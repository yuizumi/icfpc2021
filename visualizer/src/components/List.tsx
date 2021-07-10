import * as React from "react";
import ListGroup from "react-bootstrap/ListGroup";

export type Props = {
    selectedId: number | undefined;
    numOfProblems: number;
    onMove: (idx: number) => void;
};

export const List = (props: Props) => {
    const list = [...Array(props.numOfProblems)].map((_, i) => i + 1);
    return (
        <ListGroup>
            {list.map(idx => (
                <ListGroup.Item key={idx} active={props.selectedId === idx} onClick={() => props.onMove(idx)}>
                    {idx}
                </ListGroup.Item>
            ))}
        </ListGroup>
    );
};
