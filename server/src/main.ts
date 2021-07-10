import express from "express";
import path from "path";
import crypto from "crypto";
import childProcess from "child_process";
import fs from "fs";
const app = express();
const port = 3000;

app.use(express.json())
app.use(express.urlencoded({ extended: true }));
app.use("/tools", express.static(path.resolve(__dirname, "..", "..", "tools")));
app.use("/problems", express.static(path.resolve(__dirname, "..", "..", "problems")));
app.use("/solutions", express.static(path.resolve(__dirname, "..", "..", "solutions")));
app.get("/", (req, res) => {
    res.send(`<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8" />
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@4.6.0/dist/css/bootstrap.min.css"
        integrity="sha384-B0vP5xmATw1+K9KRQjQERJvTumQW0nPEzvF6L/Z6nronJ3oUOFUFpCjEUQouq2+l" crossorigin="anonymous" />
    <script defer src="/tools/visualizer/dist/main.js"></script>
</head>
<body>
    <div id="app"></div>
</body>
</html>
    `);
});
app.post("/solutions/eval/:id", (req, res) => {
    const problemId: string = req.params.id;
    const problemFile =
        path.resolve(__dirname, "..", "..", "problems", `${("00" + String(problemId)).substr(-3)}.problem`);
    const solution = JSON.stringify(req.body);
    const solutionFile =
        path.resolve("/tmp", crypto.createHash("sha256").update(solution).digest("hex"));
    fs.writeFileSync(solutionFile, solution);
    const evalBin = path.resolve(__dirname, "..", "eval");
    const result = JSON.parse(childProcess.execSync(`${evalBin} ${problemFile} ${solutionFile}`).toString());
    res.json(result);
});

app.listen(port, () => {
    console.log(`Listening at http://localhost:${port}`)
});
