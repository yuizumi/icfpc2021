import express from "express";
import path from "path";
const app = express()
const port = 3000

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
    <script defer src="/tools/dist/main.js"></script>
</head>
<body>
    <div id="app"></div>
</body>
</html>
    `);
});

app.listen(port, () => {
    console.log(`Listening at http://localhost:${port}`)
});
