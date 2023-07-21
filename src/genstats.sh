#!/bin/bash

n=15
max_grid=4096

echo "impl,grid_size,num_threads,time"
for impl in {seq,pth,omp}; do
    if [ "$impl" == "seq" ]; then
        max_threads=1
    else
        max_threads=32
    fi

    grid_size=32
    while [ "$grid_size" -le "$max_grid" ]; do
        threads=1
        while [ "$threads" -le "$max_threads" ]; do
            for _ in $(seq 1 "$n"); do
                t=$(./time_test --grid_size "$grid_size" --impl "$impl" --num_threads "$threads")
                echo "$impl,$grid_size,$threads,$t"
            done
            threads=$((threads * 2))
        done
        grid_size=$((grid_size * 2))
    done
done

