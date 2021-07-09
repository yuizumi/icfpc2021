import argparse
import json
import sys
import urllib.parse
import urllib.request


_API_KEY = '36824061-b81f-4673-b997-474c4d9c39dd'
_API_URL = 'https://poses.live/api'


def _verify(solution):
    # TODO(yuizumi): Add more validations.
    json.loads(solution)


def _submit(solution, problem_id):
    req = urllib.request.Request(
        url=f'{_API_URL}/problems/{problem_id}/solutions',
        data=solution.encode('UTF-8'),
        headers={'Authorization': f'Bearer {_API_KEY}'})
    with urllib.request.urlopen(req) as resp:
        sys.stderr.write(resp.read().decode('UTF-8'))
    sys.stderr.write('\n')


def main(args):
    data = sys.stdin.read()
    _verify(data)
    _submit(data, args.problem_id)


def _parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('problem_id', metavar='PROBLEM_ID', type=int)
    return parser.parse_args()


if __name__ == '__main__':
    main(_parse_args())
