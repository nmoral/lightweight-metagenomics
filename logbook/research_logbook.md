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
Edge cases tested: k = sequence length, k = 1, sequences with all 4 nucleotides, K > sequences.


### Objective

Given a DNA sequence (string), extract all k-mers of length k

### Reasoning

so each nucleotide must be encoded from char to two bits. Not only because it's a standart but because it's easier to apply mathematical solution on int. 
We need to verify that no silent bit-level modifications are taking place. 


kmer extractor, should allow two options : 
    - strict
    - tolerant

Strict will reject the read if at least one kmer is invalid. 

Extractor, should return a vector of Kmer, it's a boundary between reading and indexing. We need a real contract between each module, not an array on int. 

Extractor should take a read as input. This allow to get multiple source of read possible and give more flexibility to extractor. 

Extractor can skip if an error is detected in a READ. skip formula is 
I = I + J +1 
Where : 
I = current index in read
J = error index in kmer

### Decision

- Stored as char internally, encoded on demand → minimise storage
- explicit constructors → no silent conversions
- explicit cast operators → no implicit conversions
- Round-trip XOR validation → guarantee no silent bit flip
- using classic string
- throw exception 

### Results

- [x] Nucleotide encode/decode — tests green
- [x] Kmer encode/decode — tests green
- [x] KmerExtractor sliding window — tests green
- [x] Benchmark — strict+naive k=31 : 31 635 ns / 781 042 ns (perfect/nanopore)

### Open questions

- What happens when string_view is used instead of string?
- What happens when an error code is used instead of an exception?

### Observation

Object implementation is not a ram problem in bioinformatics. But manipulating a list of object is less compact than an u_int64_t. So When we're implementing K-mer, we need to represent K-mer as an u_int64 and transform bit at position N and N+1 into Nucleotide object. 

The Boyer-Moore inspired skip heuristic does not appear effective in a context combining exceptions and substring allocation.
Three possible explanations:

- the skip itself provides no performance gain
- the gain is masked by the cost of throwing exceptions
- the gain is masked by the cost of substr allocation

## 08/04/2026

### Observation :
Exceptions carry a significant performance cost compared to error codes. A system allowing clean skip testing without exceptions needs to be designed. The first attempt failed, using negative values on unsigned integers is not possible.