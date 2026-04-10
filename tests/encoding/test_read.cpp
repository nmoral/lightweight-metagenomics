#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_template_test_macros.hpp>

#include "encoding/Read.h"
#include "encoding/Kmer.h"
#include "exception/exceptions.h"
#include "extraction/ExtractorFactory.h"
#include "extraction/KmerExtractor.h"
#include "extraction/SkipKmerExtractor.h"



TEMPLATE_TEST_CASE("Extraction valid read", "[extractor]",
                    KmerExtractor, SkipKmerExtractor) {
    SECTION("ENCODING CORRECT READ") {
        auto extractor = std::make_unique<TestType>();
        Read r("ATCG", *extractor);

        REQUIRE(r.size() == 3);

        Kmer k("AT");
        auto k1 = r.next();
        REQUIRE_THROWS_AS(k1 == k, ValidateException);
        REQUIRE(k.valid());
        REQUIRE(k1->valid());
        REQUIRE(k1 == k);
        k = Kmer("TC");
        k1 = r.next();
        REQUIRE(k.valid());
        REQUIRE(k1->valid());
        REQUIRE(k1 == k);

        REQUIRE(!r.done());
        r.next();
        REQUIRE(r.done());
    }

    SECTION("ENCODING INCORRECT READ") {
        auto extractor = std::make_unique<TestType>();
        Read r("ATXG", *extractor);

        REQUIRE(r.size() == 3);

        Kmer k("AT");
        auto k1 = r.next();
        REQUIRE_THROWS_AS(k1 == k, ValidateException);
        REQUIRE(k.valid());
        REQUIRE(k1->valid());
        REQUIRE(k1 == k);
        r.next();
        REQUIRE(r.done());
    }
}