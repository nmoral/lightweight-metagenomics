
#pragma once

#include <iostream>

class Nucleotide {
    public:
        explicit Nucleotide(const char& nucleotide);
        explicit Nucleotide(const int& nucleotide);

        u_int8_t encode(const char& nucleotide) const;

        operator uint8_t() const;

        char to_char() const;

        bool operator==(const int& other) const;
        bool operator==(const char& other) const;

    private:
        char n_;

        char decode(const int& nucleotide) const;
};