#!/usr/bin/env bash
# run_experiment.sh

set -u

# Resolve script directory
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
EXE="${SCRIPT_DIR}/experiment_io"
CACHE="${SCRIPT_DIR}/cache_file.sh"

# Check prerequisites
if [[ ! -f "$CACHE" ]]; then
    echo "cache_file.sh not found at: $CACHE" >&2
    exit 1
fi

if [[ ! -x "$EXE" ]]; then
    echo "experiment executable not found or not executable at: $EXE" >&2
    exit 1
fi

# Run cache step first
echo "Running cache_file.sh..."
bash "$CACHE"

# Define option combinations
combos=(
    "read random"
    "read sequential"
    "write random"
    "write sequential"
)

# Always use order=random
for combo in "${combos[@]}"; do
    read -r rw order <<<"$combo"
    echo "Switching options: backing=file, share=shared order=$order rw=$rw"
    for i in {1..5}; do
        echo "  Run $i/5..."
        taskset -c 1 "$EXE" --backing file --share shared --order "$order" --rw "$rw"
    done
    echo
done
