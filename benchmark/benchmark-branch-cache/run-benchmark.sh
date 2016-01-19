#!/bin/bash

set -e

readonly min=10
readonly max=10000000
readonly exe="./benchmark"

readonly vector="vector"
readonly polycontainer="polycontainer"
readonly contiguous_polycontainer="contiguous-polycontainer"

declare -a vector_branch_misses=()
declare -a polycontainer_branch_misses=()
declare -a contiguous_polycontainer_branch_misses=()

declare -a vector_cache_misses=()
declare -a polycontainer_cache_misses=()
declare -a contiguous_polycontainer_cache_misses=()

format_number() {
## taken from http://www.linuxquestions.org/questions/programming-9/format-numbers-using-bash-672031/#post4269641
    echo "$1" | sed -r '
                  :L
                  s=([0-9]+)([0-9]{3})=\1,\2=
                  t L'
}

filter() {
    echo "$1" | grep "$2" | awk '{print $1;}'
}

run_perf() {
    local -r type="$1"
    local -r size="$2"
    local -r output=$(perf stat -e branch-misses -e cache-misses "$exe" "$type" "$size" 2>&1)
    local branch_misses=$(filter "$output" "branch-misses")
    local cache_misses=$(filter "$output" "cache-misses")

    if [[ "$branch_misses" == "<not" || "$cache_misses" == "<not" ]]; then
        echo "Error running 'perf stat': branch misses and/or cache-misses cannot be measured by this OS's perf tool"
        exit 1
    fi

    if [[ "$type" == "$vector" ]]; then
        vector_cache_misses+=("$cache_misses")
        vector_branch_misses+=("$branch_misses")

    elif [[ "$type" == "$polycontainer" ]]; then
        polycontainer_cache_misses+=("$cache_misses")
        polycontainer_branch_misses+=("$branch_misses")

    elif [[ "$type" == "$contiguous_polycontainer" ]]; then
        contiguous_polycontainer_cache_misses+=("$cache_misses")
        contiguous_polycontainer_branch_misses+=("$branch_misses")
    fi
}

measure() {
    local -r type="$1"
    local size=$min
    while [[ $size -le $max ]]; do
        run_perf "$type" "$size"
        size=$(($size * 10));
    done
}

display_results() {
    echo -e "\n------------------------------------------------"
    echo "$1"
    printf "%15s %15s %15s\n" "size" "branch-misses" "cache-misses"
    echo "------------------------------------------------"
    local str="";
    local size=$min

    declare -a branch_misses=("${!2}")
    declare -a cache_misses=("${!3}")

    local -r trials="${#branch_misses[@]}"

    for i in $(seq 0 $((trials - 1))); do
        printf "%15s %15s %15s\n" $(format_number "$size") "${branch_misses[$i]}" "${cache_misses[$i]}"
        size=$(($size * 10));
    done
}

main() {
    if [[ ! -f "$exe" ]]; then
        echo "$exe is not found. (Did you compile benchmark.cpp?)"
        exit 1
    fi

    measure "vector"
    measure "polycontainer"
    measure "contiguous-polycontainer"

    display_results "std::vector" vector_branch_misses[@] vector_cache_misses[@]
    display_results "PolyContainer" polycontainer_branch_misses[@] polycontainer_cache_misses[@]
    display_results "ContiguousPolyContainer" contiguous_polycontainer_branch_misses[@] contiguous_polycontainer_cache_misses[@]
    echo ""
}

main
