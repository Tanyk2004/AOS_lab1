#!/usr/bin/env bash
# run_experiment.sh

set -u

# Resolve script directory
SCRIPT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" >/dev/null 2>&1 && pwd)"
EXE="${SCRIPT_DIR}/experiment"
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
    "file shared"
    "file private"
    "anon shared"
    "anon private"
)

# Always use order=random
for combo in "${combos[@]}"; do
    read -r backing share <<<"$combo"
    echo "Switching options: backing=$backing, share=$share (order=random)"
    for i in {1..5}; do
        echo "  Run $i/5..."
        taskset -c 2 "$EXE" --backing "$backing" --share "$share" --order random
    done
    echo
done