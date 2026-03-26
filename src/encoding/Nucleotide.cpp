
#include "Nucleotide.h"
#include <iostream>


Nucleotide::Nucleotide(const char& nucleotide): n_(nucleotide) {}

Nucleotide::Nucleotide(const int& nucleotide) {
    n_ = decode(nucleotide);
}

char Nucleotide::to_char() const {
    return n_;
}


u_int8_t Nucleotide::encode(const char& nucleotide) const {
    switch (nucleotide)
    {
        case 'A': return 0b00;
        case 'C': return 0b01;
        case 'G': return 0b10;
        case 'T': return 0b11;
        default: return -1;
    }
}


char Nucleotide::decode(const int& nucleotide) const {
    switch (nucleotide)
    {
        case 0: return 'A';
        case 1: return 'C';
        case 2: return 'G';
        case 3: return 'T';
        default: return 'z';
    }
}

bool Nucleotide::operator==(const int& other) const {
    return encode(n_) == other;
}


bool Nucleotide::operator==(const char& other) const {
    return n_ == other;
}