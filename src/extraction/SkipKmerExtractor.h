#pragma once


#include "KmerExtractor.h"

class Read;
class Kmer;

class SkipKmerExtractor: public KmerExtractor {
    using KmerExtractor::KmerExtractor;
    public: 

        Kmer extract(const Read& read, const int at) const override;

};