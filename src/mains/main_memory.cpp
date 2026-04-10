// src/mains/main_memory.cpp
#include <string>
#include <random>
#include "encoding/Read.h"
#include "extraction/ExtractorFactory.h"

static std::string generate_read(int length, float error_rate = 0.0) {
    const std::string nucleotides = "ACGT";
    const std::string errors      = "XNZ";

    std::mt19937 gen(42);
    std::uniform_real_distribution<float> dist(0.0, 1.0);
    std::uniform_int_distribution<int>    nuc(0, 3);
    std::uniform_int_distribution<int>    err(0, 2);

    std::string read;
    read.reserve(length);

    for (int i = 0; i < length; i++) {
        if (dist(gen) < error_rate) {
            read += errors[err(gen)];
        } else {
            read += nucleotides[nuc(gen)];
        }
    }

    return read;
}

int main() {
    const std::string PERFECT_READ  = generate_read(1000, 0.0);
    const std::string NANOPORE_READ = generate_read(1000, 0.05);

    auto extractor = createExtractor();

    #if EXTRACTION_MODE == 0
        Read r(PERFECT_READ, *extractor);
    #else
        Read r(NANOPORE_READ, *extractor);
    #endif

    while (!r.done()) r.next();

    return 0;
}