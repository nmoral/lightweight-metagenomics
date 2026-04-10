#pragma once

#include <iostream>
#include <optional>
#include "Kmer.h"
#include "extraction/KmerExtractor.h"

class Read {
    public:
        explicit Read(const std::string& read, KmerExtractor& extractor);
        std::optional<Kmer> next();
        int size() const;
        bool done() const;
        Kmer window(int at, int size) const;
        
    private:
        std::string read_;
        int size_ = 0;
        int index_ = 0;
        KmerExtractor* extractor_;
};