import React from "react";
import ReactDOM from "react-dom";
import { Container } from "./containers/Container";
import { Store } from "./stores/Store";

const store = new Store();

ReactDOM.render(<Container store={store} />, document.getElementById("app"));
