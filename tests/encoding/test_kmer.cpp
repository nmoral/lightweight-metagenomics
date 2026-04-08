#include <catch2/catch_test_macros.hpp>
#include "encoding/Kmer.h"
#include "encoding/Nucleotide.h"
#include "encoding/exception/exceptions.h"
#include "helpers.cpp"

TEST_CASE("Kmer encoding", "[Kmer][encoding]") {
    SECTION("Encoding Kmer") {
        Kmer k("A");

        REQUIRE(k == 0);
        REQUIRE(k == 0b00);
        REQUIRE(k.size() == 1);

        k = Kmer("C");

        REQUIRE(k == 1);
        REQUIRE(k == 0b01);

        k = Kmer("G");

        REQUIRE(k == 2);
        REQUIRE(k == 0b10);

        k = Kmer("T");

        REQUIRE(k == 3);
        REQUIRE(k == 0b11);

        k = Kmer("AA");

        REQUIRE(k == 0);
        REQUIRE(k == 0b0000);

        k = Kmer("TT");

        REQUIRE(k == 15);
        REQUIRE(k == 0b1111);

        k = Kmer("GT");

        REQUIRE(k == 11);
        REQUIRE(k == 0b1011);
        REQUIRE(k.size() == 2);
        REQUIRE(std::string(k) == "GT");
    }


    SECTION("Getting Nucleotide") {
        Kmer k("ATCG");
        Nucleotide n('T');

        REQUIRE(k[1] == n);
        k = Kmer("ATCG");
        n  = Nucleotide('G');

        REQUIRE(k[3] == n);
    }


    SECTION("expecting errors") {
        REQUIRE_THROWS_AS(Kmer("AZTG"), EncodingException);
        REQUIRE_THROWS_AS(Kmer("ATCGAZERTYUYUIIOPIUJHJHGGGFTTHYHGFFGHYYTFFGGHHGG"), LengthException);
    }
}