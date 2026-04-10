
#include <iostream>
#include "Nucleotide.h"
#include "exception/exceptions.h"


Nucleotide::Nucleotide(const char nucleotide) {
    n_ = value(nucleotide);
}

bool Nucleotide::valid() 
{
    state_ = 1;

    return !error_;
}

bool Nucleotide::error() const 
{
    check_state();
    return error_;
}

u_int8_t Nucleotide::value() const 
{
    check_state();

    return n_;
}

u_int8_t Nucleotide::value(char n) {

    switch (n)
    {
        case 'A': return 0b00;
        case 'C': return 0b01;
        case 'G': return 0b10;
        case 'T': return 0b11;
        default: 
            error_ = true;
            return 0b100;
    }
}

void Nucleotide::check_state() const 
{
    if (state_ == 0) {
        throw ValidateException("You must validate object before reading value");
    }
}