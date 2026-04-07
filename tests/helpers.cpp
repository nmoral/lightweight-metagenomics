#pragma once
#include <catch2/catch_test_macros.hpp>
#include "encoding/Nucleotide.h"
#include "encoding/Kmer.h"

namespace Catch {
    template<>
    struct StringMaker<Nucleotide> {
        static std::string convert(const Nucleotide& n) {
            return std::string(n);
        }
    };

    template<>
    struct StringMaker<Kmer> {
        static std::string convert(const Kmer& k) {
            return std::string(k);
        }
    };
}