# lightweight-metagenomics

Exploration and prototyping of lightweight metagenomic classification algorithms
for resource-constrained environments.

## Motivation

Current metagenomic classification tools (Kraken2, DIAMOND) require significant
computational resources — often tens of gigabytes of RAM and stable internet
connectivity. This project explores algorithmic approaches to achieve robust
classification with a target memory footprint under 4 GB, running on CPU only,
without network access.

The core research question: **how to classify metagenomic sequences robustly
when reference databases are incomplete**, as is the case for the vast majority
of microbial diversity.

## Approach

A hierarchical multi-level index architecture:

- **Level 1** — Universal motifs (permanent RAM, few MB)
  Detects the biological domain (Bacteria / Archaea / Eukaryote)

- **Level 2** — Family motifs (permanent RAM, ~500 MB)
  Identifies the taxonomic family via weighted discriminant motifs

- **Level 3** — Discriminant k-mers (on-disk, lazy loading)
  Species-level classification, partitioned by family

Unknown organisms — those absent from reference databases — receive
a probabilistic response from levels 1 and 2 rather than a silent failure.

## Key ideas under exploration

- Hierarchical index with lazy loading by taxonomic bucket
- Skip heuristic on functional motifs (Boyer-Moore inspired)
- Probabilistic multi-level voting with confidence scores
- Weighted evolutionary distance on binary-encoded k-mers
- Bucket scheduling to minimize memory peak
- Online scheduling with lookahead for parallel processing

## Status

Early prototype — work in progress.
Benchmarking against Kraken2 on NCBI reference datasets is planned.

## Structure

```
src/
├── mains/          → entry points (benchmark, ...)
├── encoding/       → Nucleotide, Kmer, Read
├── extraction/     → KmerExtractor, SkipKmerExtractor, ExtractorFactory
└── exceptions/     → custom exception hierarchy

tests/
├── encoding/       → test_nucleotide.cpp, test_kmer.cpp, test_read.cpp
└── extraction/     → test_extractor.cpp

benchmarks/
├── results/        → JSON results (gitignored)
│   └── BENCHMARK_RESULT_DIR/
│       └── mode{MODE}_type{TYPE}_k{KMER_SIZE}.json
└── scripts/
    └── benchmark_report.py  → HTML report generator

logbook/            → research notes, ideas, open questions
```

## Requirements

```bash
# Compiler
sudo apt install g++ make

# Tests
sudo apt install catch2

# Benchmarks
sudo apt install libbenchmark-dev

# Coverage
sudo apt install lcov

# Benchmark report (optional)
pip install pandas matplotlib --break-system-packages
```

## Build

```bash
make all        # build all targets
make clean      # remove build artifacts
```

## Compilation parameters

| Parameter | Values | Default | Description |
|-----------|--------|---------|-------------|
| `KMER_SIZE` | integer | 2 | K-mer length |
| `EXTRACTION_MODE` | 0 (strict) / 1 (tolerant) | 1 | Error handling mode |
| `EXTRACTION_TYPE` | 0 (naive) / 1 (skip) | 0 | Extraction strategy |

## Tests

```bash
# Run tests with default parameters
make tests run_tests

# Run tests with specific parameters
make tests run_tests KMER_SIZE=31 EXTRACTION_MODE=0 EXTRACTION_TYPE=0

# Run all mode combinations
make tests_all

# Debug build
make tests_debug
```

## Benchmarks

Benchmarks use synthetic reads of 1000 bases with a 5% error rate
to simulate real Nanopore sequencing conditions.
Always compiled with `-O2` for meaningful measurements.

```bash
# Run benchmark — results saved automatically with timestamp
make run_benchmark KMER_SIZE=31 EXTRACTION_MODE=0 EXTRACTION_TYPE=0

# Run benchmark in a named directory
make run_benchmark KMER_SIZE=31 EXTRACTION_MODE=0 EXTRACTION_TYPE=0 \
     BENCHMARK_RESULT_DIR=step-01-baseline

# Results saved to:
# benchmarks/results/{BENCHMARK_RESULT_DIR}/mode{MODE}_type{TYPE}_k{KMER_SIZE}.json
# Default BENCHMARK_RESULT_DIR = YYYY-MM-DD_HH-MM (current timestamp)
```

### Benchmark parameters

| Parameter | Meaning |
|-----------|---------|
| `EXTRACTION_MODE=0` | Strict — throws on invalid character |
| `EXTRACTION_MODE=1` | Tolerant — skips invalid k-mers |
| `EXTRACTION_TYPE=0` | Naive extractor — no skip heuristic |
| `EXTRACTION_TYPE=1` | Skip extractor — Boyer-Moore inspired skip |

### Generate comparison report

```bash
python benchmarks/scripts/benchmark_report.py benchmarks/results/step-01-baseline
xdg-open benchmarks/results/step-01-baseline/report.html
```

See `benchmarks/scripts/README.md` for full documentation.

### Current results (2026-04-08, -O2, k=31, 1000 bases, 5% error rate)

| Configuration | Perfect read | Nanopore read (5% errors) |
|---------------|-------------|--------------------------|
| Strict + Naive | 31 635 ns | 781 042 ns |
| Tolerant + Skip | 31 512 ns | 784 440 ns |

**Key observation** : The skip heuristic does not improve performance significantly.
The bottleneck is likely `std::string::substr` allocation, not the number of
k-mer evaluations. Next step: replace `substr` with `std::string_view` to
eliminate heap allocations.

## Code coverage

```bash
# Coverage report — opens automatically in browser
make coverage

# Coverage with specific parameters
make coverage KMER_SIZE=31 EXTRACTION_MODE=1 EXTRACTION_TYPE=1
```

Current coverage: **100%** across all 4 mode combinations.

## References

- Wood & Salzberg (2014) — Kraken: ultrafast metagenomic sequence classification
- Buchfink et al. (2015) — Fast and sensitive protein alignment using DIAMOND
- Kimura (1980) — A simple method for estimating evolutionary rates
- Ondov et al. (2016) — Mash: fast genome and metagenome distance estimation

## License

MIT