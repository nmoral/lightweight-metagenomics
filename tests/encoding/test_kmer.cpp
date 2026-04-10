#include <catch2/catch_test_macros.hpp>
#include "encoding/Kmer.h"
#include "encoding/Nucleotide.h"
#include "exception/exceptions.h"


TEST_CASE("Kmer encoding", "[Kmer][encoding]") {
    SECTION("Encoding Kmer") {
        Kmer k("AT");
        
        REQUIRE_THROWS_AS(k.value(), ValidateException);
        REQUIRE(k.valid());
        REQUIRE(k.value() == 3);

        k = Kmer("AX");
        REQUIRE_THROWS_AS(k.error(), ValidateException);
        REQUIRE(!k.valid());
        REQUIRE(k.error() == 1);

    }
} 