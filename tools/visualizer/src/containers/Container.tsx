import React from "react";
import { Container as BootstrapContainer, Col, Row } from "react-bootstrap";
import { observer } from "mobx-react";
import { Store } from "../stores/Store";
import { List } from "../components/List"
import { View } from "../components/View"
import { EditBox } from "../components/EditBox";

export const Container = observer(({ store }: { store: Store }) => (
    <BootstrapContainer>
        <Row>
            <Col md="2">
                <List
                    numOfProblems={store.state.numOfProblems}
                    selectedId={store.state.selectedId}
                    onMove={(idx: number) => store.onMove(idx)}
                />
            </Col>
            <Col md="auto">
                <View
                    problem={store.state.problem}
                    solution={store.state.solution}
                />
                <EditBox
                    solution={store.state.solution}
                    onEdit={(solution) => store.onEdit(solution)}
                />
            </Col>
        </Row>
    </BootstrapContainer>
));
