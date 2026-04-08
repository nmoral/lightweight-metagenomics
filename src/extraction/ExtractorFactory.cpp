#include "ExtractorFactory.h"
#include "KmerExtractor.h"


std::unique_ptr<KmerExtractor> createExtractor() {
    if constexpr (CURRENT_TYPE == ExtractionType::NAIVE) {
        return std::make_unique<KmerExtractor>();
    }

    return std::make_unique<SkipKmerExtractor>();
}