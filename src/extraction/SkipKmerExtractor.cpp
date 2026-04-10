
#include <optional>
#include "SkipKmerExtractor.h"
#include "KmerExtractor.h"
#include "encoding/Read.h"
#include "encoding/Kmer.h"
#include "exception/exceptions.h"



std::optional<Kmer> SkipKmerExtractor::extract(Read& read, int& at)
{
    auto k =  KmerExtractor::extract(read, at);

    if (k->valid()) {
        return k;
    }

    at += k->error();

    if (read.done()) {
        return std::nullopt;
    }

    return read.next();
}

