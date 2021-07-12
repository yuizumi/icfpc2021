set -e

TIMEOUT="$3"
if [ -z "$TIMEOUT" ]; then
    TIMEOUT="3"
fi

if [ -e "../search_params/$2.config" ]; then
    CONFIG="../search_params/$2.config"
fi

timeout $TIMEOUT ./$1 $CONFIG < ../problems/$2.problem | python3 min.py ../problems/$2.problem ../solutions/$2.json
