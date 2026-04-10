

#include <optional>
#include "KmerExtractor.h"
#include "encoding/Read.h"
#include "encoding/Kmer.h"

KmerExtractor::KmerExtractor() {}

std::optional<Kmer> KmerExtractor::extract(Read& read, int& at) 
{
    Kmer k = read.window(at, KMER_SIZE);
    ++at;

     if (k.valid() ) {
        return k;
    }

    if (CURRENT_MODE == ExtractionMode::STRICT) {
        at = read.size();
    }

    if (read.done()) {
        return std::nullopt;
    }

    return read.next();
}