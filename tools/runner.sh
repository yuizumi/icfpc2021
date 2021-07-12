set -e

TIMEOUT="$3"
if [ -z "$TIMEOUT" ]; then
    TIMEOUT="3"
fi

timeout $TIMEOUT ./$1 < ../problems/$2.problem | python3 min.py ../problems/$2.problem ../solutions/$2.json
