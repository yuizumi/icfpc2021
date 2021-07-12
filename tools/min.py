import json
import os
import subprocess
from subprocess import PIPE
import sys

newSolution = sys.stdin.read()
if not newSolution:
    sys.exit()

path = sys.argv[2]

if not os.path.exists(path):
    file = open(path, 'w')
    file.write(newSolution)
    file.close()
    sys.exit()
 
tmpPath = "/tmp/solution.json"
tmp = open(tmpPath, 'w')
tmp.write(newSolution)
tmp.close()

oldDislikes = json.loads(subprocess.run("../yuizumi/eval {} {}".format(sys.argv[1], path), shell=True, stdout=PIPE, text=True).stdout)["dislikes"]
newDislikes = json.loads(subprocess.run("../yuizumi/eval {} {}".format(sys.argv[1], tmpPath), shell=True, stdout=PIPE, text=True).stdout)["dislikes"]

if oldDislikes > newDislikes:
    file = open(path, 'w')
    file.write(newSolution)
    file.close()
    print("{} updated: {} -> {}".format(path, oldDislikes, newDislikes))
