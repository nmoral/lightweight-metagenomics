#pragma once

#include <iostream>
#include "Nucleotide.h"

class Kmer {
    public:
        explicit Kmer(const std::string& kmer);

        bool operator==(const u_int64_t kmer) const;

        Nucleotide operator[](const std::size_t index) const;

        int size() const;

        operator std::string() const;
    
    private: 
        u_int64_t kmer_;
        int length_;

        u_int64_t encode(const std::string& kmer) const;
};