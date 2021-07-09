(function() {
    function computeViewBox(problem) {
        let xmin = +Infinity;
        let ymin = +Infinity;
        let xmax = -Infinity;
        let ymax = -Infinity;

        for (const v of problem.hole) {
            const x = v[0], y = v[1];
            xmin = Math.min(xmin, x); ymin = Math.min(ymin, y);
            xmax = Math.max(xmax, x); ymax = Math.max(ymax, y);
        }
        for (const v of problem.figure.vertices) {
            const x = v[0], y = v[1];
            xmin = Math.min(xmin, x); ymin = Math.min(ymin, y);
            xmax = Math.max(xmax, x); ymax = Math.max(ymax, y);
        }

        xmin -= 5; ymin -= 5;
        xmax += 5; ymax += 5;

        return [xmin, ymin, xmax, ymax];
    }

    function buildHolePath(vertices, viewBox) {
        let path = '';

        for (const v of vertices) {
            const x = v[0], y = v[1];
            path += path ? 'L' : 'M';
            path += ` ${v[0]} ${v[1]} `;
        }

        const xmin = viewBox[0];
        const ymin = viewBox[1];
        const xmax = viewBox[2];
        const ymax = viewBox[3];
        path += `Z M ${xmin} ${ymin} L ${xmax} ${ymin} L ${xmax} ${ymax} `
            + `L ${xmin} ${ymax}`;

        return path;
    }

    function buildFigurePath(vertices, edges) {
        let path = '';

        for (const e of edges) {
            const u = vertices[e[0]], ux = u[0], uy = u[1];
            const v = vertices[e[1]], vx = v[0], vy = v[1];
            path += `M ${ux},${uy} L ${vx},${vy} `;
        }

        return path;
    }

    const $ = (query) => document.querySelector(query);

    let problem = null;

    $('#problem').addEventListener('change', async () => {
        const content = await $('#problem').files[0].text();

        problem = JSON.parse(content);

        const viewBox = computeViewBox(problem);
        const sx = viewBox[0];
        const sy = viewBox[1];
        const tx = viewBox[2];
        const ty = viewBox[3];
        $('#svg').setAttribute('viewBox', `${sx} ${sy} ${tx - sx} ${ty - sy}`);
        $('#hole').setAttribute('d', buildHolePath(problem.hole, viewBox));
        $('#figure').setAttribute(
            'd', buildFigurePath(problem.figure.vertices, problem.figure.edges));
        $('#output').removeAttribute('d');
    });

    $('#solution').addEventListener('change', async () => {
        if (!problem) return;

        const content = await $('#solution').files[0].text();
        solution = JSON.parse(content);
        $('#output').setAttribute(
            'd', buildFigurePath(solution.vertices, problem.figure.edges));
    });
})();
