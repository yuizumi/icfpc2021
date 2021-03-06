import React from "react";
import { Container as BootstrapContainer, Col, Row } from "react-bootstrap";
import { observer } from "mobx-react";
import { Store } from "../stores/Store";
import { List } from "../components/List"
import { View } from "../components/View"
import { EditBox } from "../components/EditBox";
import { ResultBox } from "../components/ResultBox";
import { Translation } from "../components/Translation";
import { Rotation } from "../components/Rotation";
import { ZRotation } from "../components/ZRotation";
import { Round } from "../components/Round";
import { SegRotation } from "../components/SegRotation";
import { Fit } from "../components/Fit";

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
                <h1>{store.state.selectedId}</h1>
                <View
                    problem={store.state.problem}
                    solution={store.state.solution}
                />
                <Translation
                    onSubmit={(dx, dy) => store.onTraslate(dx, dy)}
                />
                <Rotation
                    onSubmit={(cx, cy, deg) => store.onRotate(cx, cy, deg)}
                />
                <ZRotation
                    onSubmit={(v, base, axis) => store.onZRotate(v, base, axis)}
                />
                <SegRotation
                    onSubmit={(v, from, to) => store.onSegRotate(v, from, to)}
                />
                <Fit
                    onSubmit={(r) => store.onFit(r)}
                />
                <Round
                    onRound={() => store.onRound()}
                    onCeil={() => store.onCeil()}
                    onFloor={() => store.onFloor()}
                    logHints={() => store.showHints()}
                />
                <EditBox
                    solution={store.state.solution}
                    onEdit={(solution) => store.onEdit(solution)}
                    onClear={() => store.onClear()}
                    onInit={() => store.onInit()}
                />
                <ResultBox
                    dislikes={store.state.dislikes}
                    errors={store.state.errors}
                />
            </Col>
        </Row>
    </BootstrapContainer>
));
