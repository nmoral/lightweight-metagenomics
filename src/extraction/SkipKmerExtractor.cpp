
#include <optional>
#include "SkipKmerExtractor.h"
#include "KmerExtractor.h"
#include "encoding/Read.h"
#include "encoding/Kmer.h"
#include "exception/exceptions.h"



std::optional<Kmer> SkipKmerExtractor::extract(Read& read, int& at)
{
    auto k = read.window(at, KMER_SIZE);
    ++at;

    if (k.valid()) {
        return k;
    }

    if (CURRENT_MODE == ExtractionMode::STRICT) {
        at = read.size();
         
        return std::nullopt;
    }

    at += k.error();

    if (read.done()) {
        return std::nullopt;
    }

    return read.next();
}

