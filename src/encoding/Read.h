#pragma once

#include <iostream>
#include <optional>
#include "Kmer.h"
#include "extraction/KmerExtractor.h"

class Read {
    public:
        explicit Read(const std::string& read, KmerExtractor& extractor);


        Kmer window(const int at, const int size) const;
        std::optional<Kmer> next();
        bool done() const;

    private: 

        int size() const;

        std::string read_;
        KmerExtractor* extractor_;
        int totalKmer_;
        int index_;
};