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
├── mains/          → entry points (index, bits, benchmark)
├── bits/           → binary encoding of nucleotides and k-mers
├── index/          → index structures and search strategies
└── exceptions/     → custom exceptions

logbook/            → ideas, reflections, open questions
```

## Requirements

- g++ with C++17 support
- make

```bash
make index      # build and run index tests
make bits       # build and run bitwise tests
make benchmark  # build and run benchmarks
make all        # build everything
```

## References

- Wood & Salzberg (2014) — Kraken: ultrafast metagenomic sequence classification
- Buchfink et al. (2015) — Fast and sensitive protein alignment using DIAMOND
- Kimura (1980) — A simple method for estimating evolutionary rates
- Ondov et al. (2016) — Mash: fast genome and metagenome distance estimation

## License

MIT
