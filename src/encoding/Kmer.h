#pragma once

#include <iostream>
#include "Nucleotide.h"


#ifndef KMER_SIZE
    #define KMER_SIZE 2
#endif

class Kmer {
    public:
        explicit Kmer(const std::string& kmer);

        bool valid();

        u_int64_t value() const;
        u_int8_t error() const;

        bool operator==(const Kmer& kmer) const;
    
    private: 
        u_int64_t kmer_ = 0;
        u_int8_t error_index_ = 0;
        u_int8_t state_ = 0;

        void check_state() const;
};