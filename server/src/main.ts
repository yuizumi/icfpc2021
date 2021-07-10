import express from "express";
import path from "path";
const app = express()
const port = 3000

app.use("/tools", express.static(path.resolve(__dirname, "..", "..", "tools")));
app.use("/problems", express.static(path.resolve(__dirname, "..", "..", "problems")));
app.use("/solutions", express.static(path.resolve(__dirname, "..", "..", "solutions")));
app.get("/", (req, res) => {
    res.send('<a href="/tools/visualizer.html">visualizer</a>');
});

app.listen(port, () => {
    console.log(`Listening at http://localhost:${port}`)
});
