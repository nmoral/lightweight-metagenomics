# Research Notes — T2 Prototype
## Lightweight hierarchical metagenomic index

**Last updated** : March 2026  
**Status** : Hypotheses and ideas under exploration  
**Objective** : Centralise conceptual ideas developed around the T2 research topic.
This document evolves as understanding deepens and experiments progress.

---

## 📋 TODO

### Open questions — priority
- [ ] Skip calibration: how much precision do we lose vs speed gained?
- [ ] Index construction: how much RAM is needed vs RAM at query time?
- [ ] How to implement streaming construction to control RAM usage?
- [ ] Optimal bucket granularity (phylum? family? genus?)
- [ ] Level 3 cache policy (LRU? size threshold? time?)
- [ ] Confidence threshold for bucket assignment in ambiguous cases?
- [ ] How to handle Nanopore reads with sequencing errors in the skip heuristic?
- [ ] What default startup threshold for lookahead scheduling?
- [ ] How to compute the overlap graph without waiting for phase 1 to complete?
- [ ] Does the parallelism gain justify the scheduler complexity?
- [ ] Does the optimal scheduling policy depend on sample type (clinical vs environmental)?
- [ ] Which weight table for evolutionary distance? Kimura? GTR? empirical?
- [ ] Does weighted distance improve the functional motif skip?

### Ideas to prototype
- [ ] Levels 1+2 index in Python on synthetic NCBI data
- [ ] Measure skip on reads with controlled mutations
- [ ] Compare probabilistic voting vs exact classification on mini-dataset
- [ ] Test lazy loading on a family-partitioned dataset
- [ ] Benchmark construction vs query time across increasing volumes
- [ ] Test weighted evolutionary distance vs Hamming on real data
- [ ] Validate k-mer materialisation on Nanopore vs Illumina reads

### Related reading
- [ ] Kraken2 source code — index construction and compact hash table
- [ ] Minimap2 source code — minimizers and k-mer encoding
- [ ] EPA-ng paper — lightweight phylogenetic placement
- [ ] Mash paper — MinHash applied to genomes
- [ ] Kimura (1980) — two-parameter substitution model
- [ ] GTR model — generalised nucleotide substitution rates

---

## 1. Topic — Formulation and context

> *"Robust metagenomic classification algorithms for incomplete reference databases
> in resource-limited settings — methods combining lightweight phylogenetic placement,
> novelty detection and approximate functional homology, optimised for a memory
> footprint below 4 GB and CPU-only execution without network access."*

### Core problem

Less than 1% of microbial species are referenced. Databases are biased toward
wealthy countries and cultivable organisms. Current tools (Kraken2) rely on exact
k-mer matching — fragile as soon as an organism diverges from its reference,
and silent when facing unknown organisms.

### Why T2 is the priority topic

It is the most transversal — a contribution here directly benefits all other
identified topics (T1, T3, T4, T7, T8, T9). Solving T2 lays a foundational
building block for the entire research programme.

### Target users

Physicians, laboratory technicians, field researchers in resource-limited settings —
low-income countries, conflict zones, humanitarian emergencies. The tool must be
usable by non-technical users, on modest hardware, without network connectivity.
Error messages must be understandable by a physician, not a developer.

---

## 2. Architecture — Multi-level hierarchical index

### Overview

```
Level 1 — Universal motifs          (permanent RAM, a few MB)
  16S rRNA, ATP synthase, ribosomes...
  Present in all living organisms, extremely conserved
  → Determines the domain (Bacteria / Archaea / Eukaryote)

Level 2 — Family motifs             (permanent RAM, ~500 MB)
  Characteristic signatures for each phylum/family
  With discriminant weights computed from reference genomes
  → Determines the taxonomic family

Level 3 — Discriminant k-mers       (on disk, lazy loading)
  Classical k-mer index partitioned by family
  Only the partition for the family identified by levels 1+2 is loaded
  → Determines the species if referenced
```

Target memory budget: ~700 MB — compatible with a basic laptop.

### Why three levels

Levels 1 and 2 are small and stable — few universal motifs and a limited number
of taxonomic families. They remain in RAM permanently.

Level 3 is large but partitioned — only the partition for the identified family
is loaded. Zero unnecessary loading.

For unknown organisms — absent from level 3 — a partial response from levels 1+2
with its confidence score is still available. This is clinically useful:
"probably a bacterium from family X, confidence 67%, species not referenced"
is better than silence or an error.

### Adaptive search strategy

The search algorithm at each level is not fixed — it adapts to the size
of the indexed structure:

- Small index (rare family, few k-mers) → naive search
- Medium index → binary search
- Large index (dominant family, millions of k-mers) → hash table

The abstraction encapsulates this choice. The caller does not know how it is done.
New strategies can be added without modifying the existing interface.
This is the open/closed principle applied to index structures.

---

## 3. Algorithmic ideas

### 3.1 Bucket-based lazy loading

Instead of loading the entire level 3 index:

**Phase 1** — Lightweight classification (levels 1+2 only)
For each read → assign to a taxonomic bucket.
Store the OFFSET of the read in the source file (8 bytes, not the read itself).

**Phase 2** — Fine classification (level 3)
For each bucket: load the family index exactly once,
classify all reads in the bucket via their offsets,
unload, move to next bucket.

Advantages:
- Each level 3 index is loaded exactly once
- Buckets in RAM = lightweight offset lists (8 bytes per read vs ~300 bytes)
- Zero data duplication on disk
- Natural parallelism on more powerful machines

Recognised pattern: external sort / partition-then-process,
equivalent of the partitioned hash join in database systems.

### 3.2 K-mer materialisation

Extract all k-mers from a read once during phase 1 and store them temporarily,
rather than recomputing them at each level.

- O(n) extraction once instead of three times
- All three index levels work on the same array of encoded k-mers
- Cost: ~970 k-mers x 8 bytes = ~7.8 KB per Nanopore read — negligible

Open question: materialise in memory or on disk?
For long Nanopore reads — probably in memory.
For short Illumina reads — recomputing may be faster.

### 3.3 Per-bucket validity mask

Do not remove k-mers from the array when a read belongs to multiple buckets.
Instead, maintain an independent validity mask per bucket.

Each bucket has its own boolean array parallel to the k-mer array.
The k-mer array remains intact and shared.
Each bucket only consults k-mers marked as valid for it,
including skip information specific to that bucket.

### 3.4 Functional motif skip

Inspired by Boyer-Moore (good suffix rule):

If a k-mer contains a functional motif and no match is found,
skip len(motif) positions instead of sliding one position at a time.

If the motif is sufficiently specific, k-mers in the next window containing
that motif will not match either — no need to test them.

Potential reduction: from ~470 to ~50-100 k-mers evaluated on a 500-base read.

This is a heuristic, not a guarantee. Requires experimental calibration.
Target result to publish: "95% precision with 5x fewer operations".

### 3.5 Multi-level probabilistic voting

For each read, accumulate weighted votes:

- Exact k-mer match → strong vote for the species
- Family motif detected → weighted vote for the family
- Universal motif detected → domain confirmation
- Nothing found → "unknown" signal with partial information

Result: a probability vector per family.
For known organisms → probability concentrated on one species.
For unknown organisms → probability spread across a family.
In both cases → an honest response with an explicit confidence level.

### 3.6 Weighted evolutionary distance on k-mers

XOR between two encoded k-mers reveals positions that differ.
Classical Hamming distance treats all differences as equivalent.

Problem: not all mutations are biologically equivalent. 10 bits of difference
from innocuous mutations may indicate a closer organism than another with only
6 bits of difference but significant mutations.

Solution: weight by substitution type.

```
Transition   (same chemical family) : A↔G, C↔T
  → frequent, innocuous → low weight

Transversion (different family)     : A↔C, A↔T, G↔C, G↔T
  → rare, significant → high weight
```

In binary encoding (A=00, C=01, G=10, T=11), the XOR pattern alone identifies
the mutation type without knowing the original nucleotides. This is an emergent
property of the chosen encoding.

The weight table is abstract — it can be swapped depending on context:
- Kimura table — transitions/transversions, biologically validated
- Hamming table — all differences equal, baseline for comparison
- Resistome table — calibrated on antibiotic resistance mutations
- Learned table — weights computed statistically from a reference corpus

Only 16 possible pairs (4x4 nucleotides) → precomputed O(1) lookup table.
This is the equivalent of BLOSUM/PAM matrices at the protein level,
but applied to binary-encoded nucleotide k-mers.
Lighter, faster, directly computable via bitwise operations.

### 3.7 Phylogenetic placement for unknown organisms

Even without an exact reference, an unknown organism can be placed in the
tree of life using ultra-conserved sequences (16S/23S rRNA, ribosomal proteins).

These sequences are so fundamental that an organism never previously sequenced
will still have a recognisable version. They allow placement up to the clade
even when the species is unknown.

Existing tools too heavy for field use: EPA-ng, pplacer.
T2 objective: a lightweight version compatible with CPU-only execution,
< 4 GB RAM, usable in streaming on short reads.

### 3.8 Functional motifs as classification keys

Functional motifs (sequences conserved by evolution) are more stable than
raw k-mers. Two highly divergent organisms still share their fundamental
functional motifs.

Cascade filter:
- Presence of a ~9-base characteristic motif eliminates 90% of the database
- K-mer comparison only on the remaining subset

---

## 4. Pipeline optimisations

### 4.1 Optimal bucket scheduling

Compute the order of bucket processing that minimises k-mer loading
and unloading between successive buckets.

Transition cost between two buckets = k-mers to unload + k-mers to load.
Find the order that minimises the total cost.

Greedy heuristic: always choose the next bucket with maximum read overlap
with the current bucket.

Isomorphism: this is the Travelling Salesman Problem applied to buckets.

### 4.2 Exclusivity-priority processing

Within a bucket, process exclusive reads first (absent from the next bucket)
and free them immediately. Reads shared with the next bucket are processed last
and kept in memory for that bucket.

The memory peak is minimised continuously rather than at fixed points.

### 4.3 K-mer garbage collector

Maintain a reference counter for each k-mer in memory —
the number of buckets still needing it. Free when it drops to zero.

- On load: counter = number of buckets using this read
- After processing by a bucket: counter--
- When counter = 0: immediate release

This mirrors Rust's ownership mechanism — reference counting with
deterministic release. Rust would be the natural language for this implementation.

### 4.4 Lookahead streaming scheduler

Do not wait for phase 1 to complete before launching phase 2 workers.
As soon as a bucket exceeds a fill threshold, launch the worker in anticipation.

At time T, when bucket X exceeds the threshold:
find bucket Y with the most reads in common among known buckets,
launch Worker(X) in anticipation,
reads shared and discovered after launch are a bonus.

The startup threshold is a tunable parameter:
- Low threshold → fast launch, less sharing exploited → medical emergencies
- High threshold → longer wait, maximum efficiency → epidemic surveillance

This is online optimisation — decisions under uncertainty with available information.

### 4.5 Emergent parallelism

The combination of optimisations 4.1, 4.2 and 4.3 enables planning
which buckets can run in parallel without exceeding the memory budget.

At any given moment we know:
- Which k-mers are in memory and for how long (GC)
- Which buckets have the most reads in common (scheduling)
- Which reads will be freed soon (exclusivity priority)

Multiple workers can therefore run simultaneously as long as the memory budget
allows. Parallelism is a consequence of good memory management, not an end in itself.

The scheduler is the central component that orchestrates everything.

---

## 5. End-to-end constraint

### Core principle

Optimising usage without thinking about construction means optimising one link
and letting another break.

If the index uses 700 MB at query time but requires 100 GB to build —
it loses its field value entirely.

**The constraint applies to the entire chain, not just one link.**

### The complete chain

```
Reference genome collection   → biased, incomplete databases
          ↓
Index construction            → limited RAM, reasonable time
          ↓
Compression and distribution  → low bandwidth, offline
          ↓
Incremental update            → offline, lightweight delta
          ↓
Read classification           → 700 MB RAM, CPU only, real-time
          ↓
Result with confidence score  → readable by non-technical users
```

### Abstractions between links

Interfaces between links allow independent optimisation without breaking contracts.

Each link consumes an abstraction and produces another.
It does not know how the previous link works.

Changing the construction algorithm without touching distribution,
changing compression without touching classification,
testing each link independently with mocks — all of this is only possible
if abstractions are well defined from the start.

### Economical construction

Construction is done once in the lab on a powerful server.
The index is distributed in compact form to the field.
Incremental update when a connection becomes available.

Approaches to control construction RAM:
- Streaming construction (external sort — sorting beyond available RAM)
- Incremental construction (adding a genome without rebuilding from scratch)
- Distributed construction (parallel partitions, final merge)

---

## 6. Pipeline overview

```
Read stream (MinION / FASTQ)
        ↓
K-mer extractor → materialisation + validity masks
        ↓
Levels 1+2 classifier → bucket assignment + offsets
        ↓
Scheduler  ←→  K-mer garbage collector
        ↓
Level 3 workers (parallel within memory budget)
  → weighted evolutionary distance
  → probabilistic voting
  → phylogenetic placement if unknown
        ↓
Result aggregator → confidence score per family
        ↓
Human-readable result
```

---

## 7. Existing tools and their limitations

**Kraken2** — exact k-mer matching, 50-100 GB RAM,
fragile to mutations, silent on unknown organisms.

**Kaiju** — protein-level classification, more robust to mutations
but slower and heavier.

**EPA-ng / pplacer** — accurate phylogenetic placement,
too heavy for field use, not designed for short metagenomic reads.

**Mash** — MinHash for genome comparison, lightweight and fast
but no fine classification and no handling of unknown organisms.

**What is missing**: a tool combining robustness to unknowns,
field-appropriate lightness, honest probabilistic response, and usability
by non-technical users. That is the T2 topic.

---

## 8. Thesis structure (tentative)

The thesis would cover one or two links of the chain.
Subsequent publications cover the remaining links.
Coherence across the whole is guaranteed by shared abstractions.

Possible chapters:
1. End-to-end constraint analysis — formalise constraints at each link
2. Economical construction — hierarchical index under memory constraint
3. Robust classification — multi-level index, skip, probabilistic voting
4. Field distribution and incremental update
5. End-to-end validation on real data under simulated field conditions

---

## 9. Structured logging as scientific instrumentation

### Core idea

Exceptions and anomalies are not just errors to handle — they are scientific
events to capitalise on. A field case that "breaks the paradigms" is a potential
discovery. Without structured logging it disappears silently. With it, it becomes
a data point that travels from the field to the research team.

### The distinction

```
EncodingException     → technical error, fix and continue
UnknownOrganismEvent  → scientific event, log and capitalise
LowConfidenceEvent    → weak signal, aggregate for analysis
AnomalyEvent          → potentially important, flag for review
```

### Today — simple structured text logs

No automation, no AI, no magic. Just a well-structured text file
that travels with the sync:

```
[2031-03-14 14:32:11] [ANOMALY] confidence=0.04 family=UNKNOWN
sequence=ATCG...TTGA version=index-v2.1.3 location=Mali-Bamako
```

Readable by a human, parseable by a 10-line Python script,
versioned with the index version that produced it.

### The value of versioning with the index

Each log is tagged with the index version that produced it.
If index v1.2 had a bias and v2.0 corrects it — logs from v1.2
can be reanalysed retrospectively with the new index.
Nothing is lost. Every field case is reinterpretable.

### The natural evolution

```
Now             → structured text logs, manual review
In 2 years      → automatic qualification script
In 5 years      → integration into index update pipeline
In 10 years     → distributed epidemiological surveillance system
```

Each step builds on the previous one.
Everything starts with a well-formatted text file today.

### Why this matters for T2

A tool that silently crashes in Mali is potentially a missed diagnosis.
A tool that logs "unknown sequence received, read skipped, analysis continues"
is a field tool. A tool that logs anomalies with full context and ships them
back at the next sync is a scientific instrument.

The error infrastructure is part of the scientific contribution,
not an afterthought.

---

## 10. Long-term vision — BioBase

The T2 hierarchical index is one building block toward something larger:
a native biological database.

Native types (KMER, READ, VARIANT, MOTIF, GENOME_INTERVAL),
native biological indexes (FM-index, suffix arrays, interval trees),
biological query operators (APPROX_MATCH, OVERLAPS, JACCARD_KMER),
a biological query planner that selects the optimal strategy
based on the statistical properties of the data.

Each search strategy from the T2 index would become a native index type
in this database. The query language would be the user-facing layer.

This is a 10-15 year research programme, not a thesis.
The T2 thesis is its first chapter.

---

*"I want to move things forward. If a single one of my papers is useful
and used one day, I will be happy."*

*March 2026*