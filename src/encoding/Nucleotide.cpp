
#include "Nucleotide.h"
#include <iostream>
#include "exception/exceptions.cpp"


Nucleotide::Nucleotide(const char nucleotide): n_(validate(nucleotide)) {}

Nucleotide::Nucleotide(const int nucleotide) {
    n_ = decode(nucleotide);
}


Nucleotide::operator char() const {
    return n_;
}

Nucleotide::operator u_int8_t() const {
    return encode(n_);
}


u_int8_t Nucleotide::encode(const char& nucleotide) const {
    switch (nucleotide)
    {
        case 'A': return 0b00;
        case 'C': return 0b01;
        case 'G': return 0b10;
        case 'T': return 0b11;
        default: throw EncodingException("Nucleotide char value must be A, C, T, G got : " + std::string(1 ,nucleotide) + " instead");
    }
}


char Nucleotide::decode(const int& nucleotide) const {
    switch (nucleotide)
    {
        case 0: return 'A';
        case 1: return 'C';
        case 2: return 'G';
        case 3: return 'T';
        default: throw EncodingException("Nucleotide int value must be 0, 1, 2, 3 got : " + std::string(1 ,nucleotide) + " instead");;
    }
}

bool Nucleotide::operator==(const int other) const {
    return encode(n_) == other;
}


bool Nucleotide::operator==(const char other) const {
    return n_ == other;
}

bool Nucleotide::operator==(const Nucleotide& other) const {
    return n_ == other.n_;
}

Nucleotide::operator std::string() const {
    return std::string(1, n_);
}

char Nucleotide::validate(char nucleotide) {
    switch (nucleotide)
    {
        case 'A': case 'T': case 'C': case 'G': return nucleotide;
        default: throw EncodingException("Invalid nucleotide expected A, T, C, G got "+std::string(1, nucleotide) + " instead");
    }
}