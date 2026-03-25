# Development Roadbook — T2 Algorithmic Hypotheses
## A coherent progression from foundations to full prototype

**Created** : March 2026  
**Principle** : Each step validates the foundations for the next.
Never move forward until the current step is tested and understood.
Document results — including failures — in the logbook.

---

## Reading guide

Each step follows the same structure:

**What** — the idea to implement or validate  
**Why now** — why this step before the next  
**Validates** — what question this step answers  
**Output** — what concrete artifact is produced  
**Before moving on** — the condition to meet before proceeding  

---

## Step 1 — K-mer extraction from a read
### Foundation of everything else

**What**  
Given a DNA sequence (string), extract all k-mers of length k
as encoded uint64_t integers using the binary encoding (A=00, C=01, G=10, T=11).
Use a sliding window of 1 position at a time (no skip yet).

**Why now**  
Every subsequent hypothesis operates on encoded k-mers.
Without a correct, tested extractor, nothing else is reliable.

**Validates**  
- Binary encoding of nucleotides works correctly
- Round-trip encoding/decoding is lossless
- Sliding window produces the right number of k-mers (n - k + 1)

**Output**  
A tested function: `extract_kmers(sequence, k) → list of uint64_t`

**Before moving on**  
Round-trip test passes on at least 10 sequences of varying length.
Edge cases tested: k = sequence length, k = 1, sequences with all 4 nucleotides.

---

## Step 2 — Naive k-mer index (level 3 baseline)
### The simplest possible index — the baseline to beat

**What**  
Build a hash table mapping each k-mer (uint64_t) to a list of organism identifiers.
Load k-mers from 3-5 reference genomes downloaded from NCBI.
Classify a query read by voting: count matches per organism, return the winner.

**Why now**  
This is the Kraken2 approach in its simplest form.
Every subsequent optimisation is measured against this baseline.
If the baseline does not work, nothing built on top of it will either.

**Validates**  
- K-mer indexing and lookup work correctly
- Exact matching classification is functional
- Establishes a precision/recall baseline on known data

**Output**  
A working naive classifier with measured precision, recall, and memory usage.
A benchmark script to reuse for all subsequent comparisons.

**Before moving on**  
Precision > 90% on reads generated from the indexed genomes.
Memory and query time measured and recorded.

---

## Step 3 — Robustness to mutations
### Understanding where exact matching breaks

**What**  
Generate synthetic reads from indexed genomes with controlled mutations:
1 mutation, 2 mutations, 3 mutations, 5 mutations per read.
Measure how precision degrades as mutation rate increases.

**Why now**  
This step quantifies the core problem T2 is trying to solve.
Without this measurement, all subsequent improvements are ungrounded.

**Validates**  
- At what mutation rate does exact matching become unreliable?
- What is the practical limit of a naive k-mer index?
- How does k length affect robustness (k=15 vs k=21 vs k=31)?

**Output**  
A precision-vs-mutation-rate curve for the naive index.
The empirical answer to: "why exact matching is not enough."

**Before moving on**  
Clear identification of the mutation threshold where precision drops below
an acceptable level (target: understand the failure mode, not fix it yet).

---

## Step 4 — Weighted evolutionary distance
### First improvement: smarter similarity measurement

**What**  
Instead of exact match (0 or 1), compute a weighted distance between
a query k-mer and the closest indexed k-mer using XOR + substitution weights.
Start with the Kimura table (transitions = 0.5, transversions = 1.0).
Compare classification precision vs step 2 on the same mutation datasets.

**Why now**  
This is the simplest improvement to exact matching that has biological grounding.
It requires only the k-mer extractor (step 1) and the naive index (step 2).
If this does not improve precision on mutated reads, it is not worth building on.

**Validates**  
- Does weighted distance improve precision on mutated reads vs exact matching?
- By how much? At what computational cost?
- Is the Kimura table better than Hamming (uniform weights)?

**Output**  
Precision comparison: exact matching vs weighted distance vs Hamming distance
on reads with 1, 2, 3, 5 mutations.
A concrete answer to: "is the biological weighting worth the extra cost?"

**Before moving on**  
Measurable precision improvement on reads with 2+ mutations.
Computational cost measured — distance calculation must remain fast enough
for a read of 1000 bases to be processed in under 1 second on a basic laptop.

---

## Step 5 — Probabilistic voting
### Honest answers with confidence scores

**What**  
Instead of returning a single classification winner, return a probability
vector over all families. Each k-mer match contributes a weighted vote.
Unknown organisms produce a diffuse distribution rather than a forced assignment.

**Why now**  
Steps 2-4 classify but do not express uncertainty.
A tool for field use must say "I don't know" when it doesn't know.
This step is the foundation of the honest response principle.

**Validates**  
- Does probabilistic voting produce meaningful confidence scores?
- Are confidence scores well-calibrated (high confidence = actually correct)?
- How does the tool behave on reads from organisms NOT in the index?

**Output**  
A classifier that returns: `{family: probability}` for each read.
Calibration curve: confidence score vs actual precision.
Behaviour on completely unknown organisms documented.

**Before moving on**  
Confidence scores are meaningful — high confidence reads are correct more often
than low confidence reads. Unknown organisms produce diffuse distributions,
not false high-confidence assignments.

---

## Step 6 — Functional motif layer (level 2 prototype)
### Classification without exact reference

**What**  
Build a small hand-crafted level 2 index of functional motifs:
a few conserved sequences from 16S rRNA and ribosomal proteins for each family.
Test whether these motifs alone can correctly assign reads to family-level taxonomy
even for organisms not in the level 3 index.

**Why now**  
Steps 2-5 all rely on having the organism in the reference database.
This step is the first test of T2's core claim: classification beyond the database.
It requires understanding of biology (which motifs to use) before implementation.

**Validates**  
- Can functional motifs assign unknown organisms to the correct family?
- What precision is achievable at family level without exact k-mer matching?
- Are functional motifs stable enough across divergent organisms of the same family?

**Output**  
A level 2 classifier with measured family-level precision on:
- Known organisms (in level 3 index) — should match level 3 precision
- Unknown organisms (not in level 3 index) — should still assign correctly to family

**Before moving on**  
Family-level classification works for at least 3 families with > 80% precision
on reads from organisms not in the level 3 index.

---

## Step 7 — Two-level architecture
### Levels 1+2 guiding level 3

**What**  
Connect the functional motif layer (step 6) with the k-mer index (step 2).
Phase 1: classify all reads to family using levels 1+2, collect offsets.
Phase 2: for each family bucket, load only that family's k-mer index and classify.
Measure total memory usage compared to loading the full index.

**Why now**  
This is the first test of the lazy loading architecture.
It validates that the two-phase approach works end-to-end before adding complexity.

**Validates**  
- Does the two-phase approach correctly replicate single-phase results?
- How much memory is saved by lazy loading vs loading the full index?
- What is the cost of the two-phase overhead?

**Output**  
Memory comparison: full index load vs lazy loading by bucket.
Precision comparison: two-phase vs single-phase (should be equivalent).
First concrete measurement of the memory budget.

**Before moving on**  
Two-phase results match single-phase results within 1-2% precision.
Measurable memory reduction demonstrated.

---

## Step 8 — Functional motif skip heuristic
### Fewer k-mers evaluated, same result

**What**  
When a k-mer contains a known functional motif and no match is found,
skip len(motif) positions instead of sliding one position.
Measure: how many k-mers are evaluated vs step 7?
Measure: what precision is lost vs step 7?

**Why now**  
Steps 1-7 have validated the classification pipeline.
The skip is a pure optimisation — it should not change results significantly.
Testing it now, on a validated pipeline, means any regression is clearly visible.

**Validates**  
- How many k-mer evaluations does the skip eliminate?
- What precision is lost?
- Is the skip worth it? (target: 95% precision with 5x fewer evaluations)

**Output**  
Evaluations comparison: with skip vs without skip.
Precision comparison: with skip vs without skip.
The empirical answer to: "is the skip heuristic worth implementing in C++?"

**Before moving on**  
Skip eliminates at least 50% of k-mer evaluations with less than 5% precision loss.
If this threshold is not met — document why and reconsider the skip design.

---

## Step 9 — Bucket scheduling optimisation
### Memory peak minimisation

**What**  
Compute the overlap graph between buckets (which reads appear in multiple buckets).
Implement the greedy scheduling heuristic: always process the bucket
with maximum overlap with the current bucket next.
Measure peak memory vs random bucket ordering.

**Why now**  
Steps 1-8 validate the algorithmic core.
This step optimises memory management — only relevant once the pipeline works.
Premature optimisation would have obscured bugs in the core logic.

**Validates**  
- Does greedy scheduling reduce peak memory vs random ordering?
- By how much? Is the gain significant on 3-5 families?
- How does the gain scale with number of families?

**Output**  
Peak memory comparison: greedy scheduling vs random ordering.
Scaling curve: memory reduction vs number of families.

**Before moving on**  
Measurable peak memory reduction with greedy scheduling.
Even a 20-30% reduction justifies the added complexity.

---

## Step 10 — End-to-end benchmark vs Kraken2
### The moment of truth

**What**  
Run the complete T2 prototype and Kraken2 on the same public dataset
(CAMI challenge or equivalent). Measure for both:
- Precision and recall at species, genus, family level
- RAM usage at query time
- Query time per read
- Behaviour on organisms not in the reference database

**Why now**  
All individual hypotheses have been validated in isolation.
This step answers the only question that matters:
"Is T2 better than the state of the art on the problem it targets?"

**Validates**  
- Does T2 achieve better precision than Kraken2 on reads from unknown organisms?
- Is the memory footprint actually below 700 MB?
- Is query time acceptable for field use?

**Output**  
A complete benchmark table.
The foundation of a first publication or preprint.
A concrete answer to: "does the prototype work?"

---

## Summary — logical dependency graph

```
Step 1 — K-mer extraction
    ↓
Step 2 — Naive index (baseline)
    ↓
Step 3 — Robustness to mutations (understand the problem)
    ↓
Step 4 — Weighted distance (improve similarity)
    ↓
Step 5 — Probabilistic voting (honest answers)
    ↓
Step 6 — Functional motif layer (beyond the database)
    ↓
Step 7 — Two-level architecture (lazy loading)
    ↓
Step 8 — Skip heuristic (fewer evaluations)
    ↓
Step 9 — Bucket scheduling (memory optimisation)
    ↓
Step 10 — End-to-end benchmark vs Kraken2
```

Each arrow means: "the next step depends on the previous one being validated."
Skipping a step means building on unvalidated assumptions.

---

## Key principles

**Test before moving on** — each step has a clear acceptance criterion.
A step that fails is not wasted — it tells you something important.
Document it.

**Measure everything** — precision, recall, memory, time.
An idea without a measurement is an opinion.

**The baseline is sacred** — step 2 is always the reference.
Every subsequent step must be compared against it.

**Failures are results** — if the skip heuristic does not work,
that is a publishable finding. Document why.

---

*"each step counts by definition."*

*March 2026*