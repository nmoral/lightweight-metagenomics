#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "encoding/Read.h"
#include "extraction/KmerExtractor.h"
#include "extraction/ExtractorFactory.h"
#include "extraction/exception/exceptions.h"

TEMPLATE_TEST_CASE("Extraction valid read", "[extractor]",
                    KmerExtractor, SkipKmerExtractor) {
    SECTION("Valid read") {
        auto extractor = std::make_unique<TestType>();
        Read r("ATC", *extractor);
    
        REQUIRE(r.next() == "AT");
        REQUIRE(r.done() == false);
        REQUIRE(r.next() == "TC");
        REQUIRE(r.done());
    }


    SECTION("Encoding bad read with skip extractor") {
        auto extractor = std::make_unique<TestType>();
    
        Read r ("ATX", *extractor);
        REQUIRE(r.next() == "AT");
        REQUIRE_THROWS_AS(r.next(), OutofBoundsException);

        r = Read("ATXC", *extractor);
        REQUIRE(r.next() == "AT");
        REQUIRE_THROWS_AS(r.next(), OutofBoundsException);
    }
}


// Tests spécifiques STRICT
#if EXTRACTION_MODE == 0  // ou une constante que tu définis
TEMPLATE_TEST_CASE("Strict mode throws on invalid", "[extractor][strict]",
                    KmerExtractor, SkipKmerExtractor) {
    auto extractor = std::make_unique<TestType>();
    Read r("ATXCG", *extractor);

    REQUIRE(r.next() == "AT");
    REQUIRE_THROWS_AS(r.next(), KmerException);
}
#endif

// Tests spécifiques TOLERANT
#if EXTRACTION_MODE == 1
TEMPLATE_TEST_CASE("Tolerant mode skips invalid", "[extractor][tolerant]",
                    KmerExtractor, SkipKmerExtractor) {
    auto extractor = std::make_unique<TestType>();
    Read r("ATXCG", *extractor);

    REQUIRE(r.next() == "AT");
    REQUIRE(r.next() == "CG");
    REQUIRE(r.done());
}
#endif
