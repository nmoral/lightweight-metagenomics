
#pragma once
#include <optional>



#ifndef EXTRACTION_MODE
    #define EXTRACTION_MODE 1
#endif

#ifndef EXTRACTION_TYPE
    #define EXTRACTION_TYPE 0
#endif

enum class ExtractionMode {
    STRICT = 0 ,
    TOLERANT = 1
};

enum class ExtractionType {
    NAIVE = 0,
    SKIP = 1
};

constexpr ExtractionMode CURRENT_MODE = static_cast<ExtractionMode>(EXTRACTION_MODE);
constexpr ExtractionType CURRENT_TYPE = static_cast<ExtractionType>(EXTRACTION_TYPE);

class Read;
class Kmer;


class KmerExtractor {
    public: 
        explicit KmerExtractor();

        virtual std::optional<Kmer> extract(Read& read, int& at);
};