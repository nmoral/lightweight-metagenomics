
#pragma once

#include <optional>
#include "KmerExtractor.h"

class Read;
class Kmer;

class SkipKmerExtractor: public KmerExtractor {
    using KmerExtractor::KmerExtractor;
    public: 
        std::optional<Kmer> extract(Read& read, int& at) override;
};
