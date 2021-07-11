import csv
import json
import pathlib
import sys


def main(argv):
    fields = ['prob', 'hole', 'vert', 'edge', 'eps', 'min', 'max', 'bonus']

    writer = csv.DictWriter(sys.stdout, fields)
    writer.writeheader()

    for path in argv[1:]:
        with open(path, 'r') as fp:
            data = json.load(fp)

        xmin = min(x for x, y in data['hole'])
        ymin = min(y for x, y in data['hole'])
        xmax = max(x for x, y in data['hole'])
        ymax = max(y for x, y in data['hole'])
        writer.writerow({
            'prob': pathlib.PurePath(path).stem,
            'hole': len(data['hole']),
            'vert': len(data['figure']['vertices']),
            'edge': len(data['figure']['edges']),
            'eps': data['epsilon'],
            'min': f'({xmin},{ymin})',
            'max': f'({xmax},{ymax})',
            'bonus': ' / '.join(f"{b['bonus']} ({b['problem']})" for b in data['bonuses']),
        })


if __name__ == '__main__':
    main(sys.argv)
