(function() {
    let edges;
  
    function drawHole(hole) {
        let xmin, ymin, xmax, ymax;
        let d = '';

        for (const v of hole) {
            const x = v[0], y = v[1];
            if (!d) {
                xmin = x; ymin = y;
                xmax = x; ymax = y;
                d += `M ${x},${y} `;
            } else {
                xmin = Math.min(xmin, x);
                ymin = Math.min(ymin, y);
                xmax = Math.max(xmax, x);
                ymax = Math.max(ymax, y);
                d += `L ${x},${y} `;
            }
        }

        xmin -= 5; ymin -= 5; xmax += 5; ymax += 5;
        d += `Z M ${xmin} ${ymin} L ${xmax} ${ymin} L ${xmax} ${ymax} L ${xmin} ${ymax}`;
        document.getElementById('hole').setAttribute('d', d);

        document.getElementById('svg')
            .setAttribute('viewBox', `${xmin} ${ymin} ${xmax} ${ymax}`);
    }

    function drawFigure(id, vertices, edges) {
        let d = '';

        for (const e of edges) {
            const u = vertices[e[0]], v = vertices[e[1]];
            d += `M ${u[0]},${u[1]} L ${v[0]},${v[1]} `;
        }

        document.getElementById(id).setAttribute('d', d);
    }

    const problem = document.getElementById('problem');
    const solution = document.getElementById('solution');

    problem.addEventListener('change', () => {
        problem.files[0].text().then(content => {
            data = JSON.parse(content);
            edges = data['figure']['edges'];
            drawHole(data['hole']);
            drawFigure('figure', data['figure']['vertices'], edges);
            document.getElementById('output').setAttribute('d', '');
        });
    });
    solution.addEventListener('change', () => {
        solution.files[0].text().then(content => {
            data = JSON.parse(content);
            if (edges) {
                drawFigure('output', data['vertices'], edges);
            }
        });
    });
})();
