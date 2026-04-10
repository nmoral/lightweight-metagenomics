#!/bin/bash
# run_all_benchmarks.sh

DIR=${1:?Usage: ./all_benchmarks.sh <BENCHMARK_RESULT_DIR>}
KMER_SIZE=${2:-31}

for MODE in 0 1; do
    for TYPE in 0 1; do
        echo "==> MODE=$MODE TYPE=$TYPE KMER_SIZE=$KMER_SIZE"
        make clean run_benchmark run_memory \
            EXTRACTION_MODE=$MODE \
            EXTRACTION_TYPE=$TYPE \
            KMER_SIZE=$KMER_SIZE \
            BENCHMARK_RESULT_DIR=$DIR
    done
done

echo "==> Done. Generating report..."
make report BENCHMARK_RESULT_DIR=$DIR