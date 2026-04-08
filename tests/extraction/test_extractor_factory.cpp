#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "encoding/Read.h"
#include "extraction/KmerExtractor.h"
#include "extraction/ExtractorFactory.h"
#include "extraction/exception/exceptions.h"

TEST_CASE("ExtractorFactory", "[factory]") {

    SECTION("Creates correct extractor type") {
        auto extractor = createExtractor();
        REQUIRE(extractor != nullptr);
        
        // Vérifie que l'extracteur fonctionne
        Read r("ATCG", *extractor);
        REQUIRE(r.next() == "AT");
    }

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
