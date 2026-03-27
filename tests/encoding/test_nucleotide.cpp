#include <catch2/catch_test_macros.hpp>
#include "encoding/Nucleotide.h"

TEST_CASE("Nucleotide encoding", "[nucleotide][encoding]") {
    SECTION("Encoding Nucleotides") {
        Nucleotide n('A');

        REQUIRE(n == 0);

        n = Nucleotide(0);
        REQUIRE(n == 'A');

        REQUIRE((u_int8_t)n == 0b00);
    }

    SECTION("Round-trip char → int → char") {
        for (char c : {'A', 'C', 'G', 'T'}) {
            Nucleotide n(c);
            REQUIRE(n == c);  
        }
    }

    SECTION("Round-trip int → char → int") {
        for (int i = 0; i < 4; i++) {
            Nucleotide n(i);
            REQUIRE(n == i);
        }
    }

    SECTION("Round-trip XOR — no silent bit flip") {
        for (char c : {'A', 'C', 'G', 'T'}) {
            Nucleotide n(c);
            Nucleotide reencoded((uint8_t)n);
            REQUIRE(((uint8_t)n ^ (uint8_t)reencoded) == 0);
        }
    }
}