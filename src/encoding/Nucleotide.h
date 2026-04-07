
#pragma once

#include <iostream>

class Nucleotide {
    public:
        explicit Nucleotide(const char nucleotide);
        explicit Nucleotide(const int nucleotide);

        u_int8_t encode(const char& nucleotide) const;

        explicit operator char() const;

        explicit operator u_int8_t() const;

        operator std::string() const;

        bool operator==(const int other) const;
        bool operator==(const char other) const;
        bool operator==(const Nucleotide& other) const;

    private:
        char n_;

        char decode(const int& nucleotide) const;

        static char validate(char nucleotide); 
};