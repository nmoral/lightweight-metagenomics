#include <catch2/catch_test_macros.hpp>

#include "extraction/KmerExtractor.h"
#include "extraction/SkipKmerExtractor.h"
#include "extraction/ExtractorFactory.h"

TEST_CASE("ExtractorFactory", "[factory]") {
#if EXTRACTION_TYPE == 0
    SECTION("Creates NaiveKmerExtractor in NAIVE mode") {
        auto extractor = createExtractor();
        REQUIRE(dynamic_cast<KmerExtractor*>(extractor.get()) != nullptr);
    }
#else
    SECTION("Creates SkipKmerExtractor in SKIP mode") {
        auto extractor = createExtractor();
        REQUIRE(dynamic_cast<SkipKmerExtractor*>(extractor.get()) != nullptr);
    }
#endif
}