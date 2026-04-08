// src/mains/main_benchmark.cpp
#include <benchmark/benchmark.h>
#include <string>
#include <random>
#include "encoding/Read.h"
#include "extraction/ExtractorFactory.h"

// --- Générateur de reads synthétiques ---

static std::string generate_read(int length, float error_rate = 0.0) {
    const std::string nucleotides = "ACGT";
    const std::string errors      = "XNZ";

    std::mt19937 gen(42);  // seed fixe — résultats reproductibles
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

// Reads générés une seule fois
static const std::string PERFECT_READ  = generate_read(1000, 0.0);
static const std::string NANOPORE_READ = generate_read(1000, 0.05);  // 5% erreurs

// --- Benchmarks ---

static void BM_Perfect_Read(benchmark::State& state) {
    auto extractor = createExtractor();
    for (auto _ : state) {
        Read r(PERFECT_READ, *extractor);
        while (!r.done()) r.next();
    }
}

static void BM_Nanopore_Read(benchmark::State& state) {
    auto extractor = createExtractor();
    for (auto _ : state) {
        #if EXTRACTION_MODE == 0
                // Mode strict — pas d'erreurs dans le read
                Read r(PERFECT_READ, *extractor);
        #else
                // Mode tolerant — erreurs acceptées
                Read r(NANOPORE_READ, *extractor);
        #endif
        while (!r.done()) r.next();
    }
}

BENCHMARK(BM_Perfect_Read);
BENCHMARK(BM_Nanopore_Read);

BENCHMARK_MAIN();