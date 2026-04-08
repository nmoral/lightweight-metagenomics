# Benchmark Report Generator

Python script that reads all JSON benchmark results from a directory
and generates a comparison report (HTML + PNG chart).

---

## Requirements

```bash
pip install pandas matplotlib --break-system-packages
```

---

## Usage

```bash
# With a named directory
python benchmarks/scripts/benchmark_report.py benchmarks/results/step-01-baseline

# With a timestamp directory
python benchmarks/scripts/benchmark_report.py benchmarks/results/2026-04-08_14-25
```

---

## Input — expected directory structure

Results are organised by directory. Each JSON file corresponds to one benchmark
run with a specific configuration.

```
benchmarks/results/
├── step-01-baseline/               ← named directory
│   ├── mode0_type0_k31.json
│   ├── mode1_type0_k31.json
│   └── mode1_type1_k31.json
│
└── 2026-04-08_14-25/               ← timestamp directory (default)
    ├── mode0_type0_k31.json
    └── mode1_type1_k31.json
```

Filename format : `mode{MODE}_type{TYPE}_k{KMER_SIZE}.json`

| Segment | Values | Meaning |
|---------|--------|---------|
| `mode0` | 0 = strict, 1 = tolerant | Error handling mode |
| `type0` | 0 = naive, 1 = skip | Extraction strategy |
| `k31` | integer | K-mer size |

---

## Output

Generated in the same directory as the input JSON files.

```
benchmarks/results/step-01-baseline/
├── mode0_type0_k31.json   ← input
├── mode1_type1_k31.json   ← input
├── comparison.png         ← generated bar chart (log scale)
└── report.html            ← generated full report
```

Open the report :

```bash
xdg-open benchmarks/results/step-01-baseline/report.html
```

---

## Generate benchmark JSON files

```bash
# Named directory — recommended for meaningful comparisons
make run_benchmark KMER_SIZE=31 EXTRACTION_MODE=0 EXTRACTION_TYPE=0 \
     BENCHMARK_RESULT_DIR=step-01-baseline

make run_benchmark KMER_SIZE=31 EXTRACTION_MODE=1 EXTRACTION_TYPE=1 \
     BENCHMARK_RESULT_DIR=step-01-baseline

# Then generate the report
python benchmarks/scripts/benchmark_report.py benchmarks/results/step-01-baseline
```

---

## Notes

- JSON files are **not versioned** — `benchmarks/results/` is in `.gitignore`
- Significant results should be documented manually in the main `README.md`
- Always use release build (`-O2`) for meaningful benchmarks — debug builds are ~3x slower
- The report includes a log-scale chart — useful when comparing values
  that differ by orders of magnitude (e.g. perfect read vs Nanopore read)