
#pragma once

#include <iostream>

class Nucleotide {
    public:
        explicit Nucleotide(const char nucleotide);
        bool valid();
        u_int8_t value() const;
        bool error() const;

    private:
        u_int8_t n_;
        bool error_;
        u_int8_t state_ = 0;

        void check_state() const;
        u_int8_t value(char n);
};