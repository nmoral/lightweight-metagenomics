#include <catch2/catch_test_macros.hpp>
#include "encoding/Nucleotide.h"
#include "exception/exceptions.h"

TEST_CASE("Nucleotide encoding", "[nucleotide][encoding]") {
    SECTION("Encoding Nucleotides") {
        Nucleotide n('A');
        REQUIRE(n.valid());
        REQUIRE(n.value() == 0b00);

        n = Nucleotide('T');
        REQUIRE_THROWS_AS(n.value(), ValidateException);

        n = Nucleotide('X');
        REQUIRE(!n.valid());
        
        REQUIRE(n.error());
    }
}