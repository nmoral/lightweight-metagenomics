# Repository Structure & Development Milestones

## Source organisation

```
lightweight-metagenomics/
│
├── src/
│   ├── encoding/           # K-mer binary encoding and decoding
│   │                       # uint64_t representation, nucleotide mapping
│   │                       # round-trip validation
│   │
│   ├── index/              # Index structures and search strategies
│   │                       # Naive hash table, adaptive search strategy
│   │                       # Lazy loading by taxonomic bucket
│   │                       # Level 3 partitioned k-mer index
│   │
│   ├── distance/           # Similarity measurement between k-mers
│   │                       # Hamming distance (baseline)
│   │                       # Weighted evolutionary distance
│   │                       # Substitution weight tables (Kimura, GTR, Resistome)
│   │
│   ├── classification/     # Read classification logic
│   │                       # Probabilistic multi-level voting
│   │                       # Functional motif layer (level 2)
│   │                       # Universal motif layer (level 1)
│   │                       # Confidence score computation
│   │
│   ├── pipeline/           # Pipeline orchestration
│   │                       # Skip heuristic (Boyer-Moore inspired)
│   │                       # Bucket scheduler (greedy overlap heuristic)
│   │                       # K-mer garbage collector (reference counting)
│   │                       # Lookahead streaming scheduler
│   │
│   ├── benchmark/          # Benchmarking and comparison tools
│   │                       # Precision / recall measurement
│   │                       # Memory and time profiling
│   │                       # Comparison vs Kraken2 on CAMI datasets
│   │
│   ├── exceptions/         # Custom exceptions
│   │
│   └── mains/              # Entry points
│       ├── main_encoding.cpp
│       ├── main_index.cpp
│       ├── main_classify.cpp
│       └── main_benchmark.cpp
│
├── tests/                  # Unit tests (Catch2)
│   ├── test_encoding.cpp
│   ├── test_index.cpp
│   ├── test_distance.cpp
│   ├── test_classification.cpp
│   ├── test_pipeline.cpp
│   └── test_benchmark.cpp
│
├── logbook/                # Research notes and ideas
│   ├── ORGANISATION.md
│   ├── carnet_prototype_T2.md
│   ├── development_roadbook.md
│   └── journal.md
│
├── data/                   # Reference data (gitignored if too large)
│   ├── genomes/            # Downloaded NCBI reference genomes
│   ├── reads/              # Test reads (synthetic + real SRA)
│   └── motifs/             # Hand-crafted functional motif sets
│
├── scripts/                # Utility scripts
│   ├── download_ncbi.py    # Download reference genomes from NCBI
│   ├── generate_reads.py   # Generate synthetic reads with mutations
│   └── run_benchmark.sh    # Full benchmark pipeline
│
├── Makefile
├── README.md
└── .gitignore
```

---

## Git milestone tags

Each tag marks a validated step in the development roadbook.
A step is tagged only when its acceptance criterion is met —
not when the code compiles, but when the results are verified.

```bash
# Tag a milestone
git tag -a step-01-encoding -m "K-mer extraction and round-trip validation complete"
git push origin step-01-encoding

# List all milestones
git tag -l "step-*"

# Go back to a specific milestone
git checkout step-01-encoding
```

### Milestone definitions

| Tag | Step | Acceptance criterion |
|-----|------|---------------------|
| `step-01-encoding` | K-mer extraction | Round-trip test passes on 10+ sequences, edge cases covered |
| `step-02-naive-index` | Naive index baseline | Precision > 90% on indexed genomes, memory and time measured |
| `step-03-mutations` | Robustness to mutations | Precision-vs-mutation-rate curve produced and documented |
| `step-04-weighted-distance` | Weighted evolutionary distance | Measurable precision improvement on 2+ mutation reads vs Hamming |
| `step-05-probabilistic-voting` | Probabilistic voting | Confidence scores calibrated, unknown organisms produce diffuse distributions |
| `step-06-functional-motifs` | Functional motif layer | Family-level precision > 80% on organisms not in level 3 index |
| `step-07-two-level-architecture` | Lazy loading | Two-phase results match single-phase within 2%, memory reduction measured |
| `step-08-skip-heuristic` | Skip heuristic | 50%+ fewer k-mer evaluations with less than 5% precision loss |
| `step-09-bucket-scheduling` | Bucket scheduling | Measurable peak memory reduction vs random ordering |
| `step-10-benchmark` | End-to-end benchmark | Full comparison vs Kraken2 on CAMI dataset, results documented |

---

## Dependency map

```
encoding
    └── index
    └── distance
            └── classification
                    └── pipeline
                            └── benchmark
```

Each module depends only on modules above it in the graph.
No circular dependencies. Each module is independently testable.

---

## .gitignore

```
# Build artifacts
output/

# Large data files
data/genomes/*.fna
data/genomes/*.fasta
data/reads/*.fastq
data/reads/*.fastq.gz

# Keep scripts that download data, not the data itself
!scripts/download_ncbi.py

# Compiled Python
__pycache__/
*.pyc

# OS
.DS_Store
Thumbs.db

# IDE
.vscode/settings.json
.idea/
```

---

## Naming conventions

```
Files       snake_case.cpp / snake_case.h
Classes     PascalCase
Functions   snake_case()
Constants   UPPER_SNAKE_CASE
Members     snake_case_  (trailing underscore for private)
```

---

## Key principle

> The roadbook lives in the logbook, not in the source tree.
> Code is organised by what it does, not by when it was written.
> Steps become git tags — visible in history, invisible in structure.