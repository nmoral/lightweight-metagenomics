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

### Decision

- Stored as char internally, encoded on demand → minimise storage
- explicit constructors → no silent conversions
- explicit cast operators → no implicit conversions
- Round-trip XOR validation → guarantee no silent bit flip

### Results

- [x] Nucleotide encode/decode — tests green
- [x] Kmer encode/decode — in progress
- [ ] KmerExtractor sliding window — todo

### Open questions
### Observation

Object implementation is not a ram problem in bioinformatics. But manipulating a list of object is less compact than an u_int64_t. So When we're implementing K-mer, we need to represent K-mer as an u_int64 and transform bit at position N and N+1 into Nucleotide object. 