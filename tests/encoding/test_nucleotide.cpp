#include <catch2/catch_test_macros.hpp>
#include "encoding/Nucleotide.h"

TEST_CASE("Nucleotide encoding", "[nucleotide][encoding]") {
    SECTION("Encoding Nucleotides") {
        Nucleotide n('A');

        REQUIRE(n == 0);

        n = Nucleotide(0);
        REQUIRE(n == 'A');
    }
}