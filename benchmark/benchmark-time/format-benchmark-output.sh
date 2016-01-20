#!/bin/bash

set -e

readonly min=100
readonly max=10000000
readonly exe="./benchmark"

readonly input=$(cat)

declare -a time_vector=()
declare -a time_polycontainer=()
declare -a time_contiguous_polycontainer=()

format_number() {
## taken from http://www.linuxquestions.org/questions/programming-9/format-numbers-using-bash-672031/#post4269641
    echo "$1" | sed -r '
                  :L
                  s=([0-9]+)([0-9]{3})=\1,\2=
                  t L'
}

filter_time() {
    local -r output=$(echo "$input" | grep -e "$1")
    while read -r line; do
        local elapsed=$(echo "$line" | awk -v ORS="" '{print $3}')
        echo -n $(format_number "$elapsed")
        echo -n "|"
    done <<< "$output"
}

filter_sizes() {
    local -r output=$(echo "$input" | grep -e "$1")
    while read -r line; do
        echo ${line#*/} | awk -v ORS="|" '{print $1}'
    done <<< "$output"
}

main() {
    IFS='|' read -r -a sizes <<< "$(filter_sizes '^std_vector')"
    IFS='|' read -r -a vector <<< "$(filter_time '^std_vector')"
    IFS='|' read -r -a polycontainer <<< "$(filter_time '^PolyContainer')"
    IFS='|' read -r -a contiguous_polycontainer <<< "$(filter_time '^ContiguousPolyContainer')"

    echo "--------------------------------------------------------------------------------"
    printf "%16s | %16s | %16s | %16s\n" "size" "std::vector" "PolyContainer" "ContiguousPolyContainer"
    echo "--------------------------------------------------------------------------------"

    local -r trials="${#sizes[@]}"
    for i in $(seq 0 $((trials - 1))); do
        printf "%16s | %16s | %16s | %16s\n" "${sizes[$i]}" \
                                       "${vector[$i]}" \
                                       "${polycontainer[$i]}" \
                                       "${contiguous_polycontainer[$i]}"
    done
}

main
