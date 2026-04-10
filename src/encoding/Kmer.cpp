
#include "Kmer.h"
#include "Nucleotide.h"
#include "exception/exceptions.h"


Kmer::Kmer(const std::string& kmer) {
    u_int8_t index = 0;
    kmer_ = 0;
    bool valid = true;
    for (char c: kmer) {
        Nucleotide n(c);
        if (!n.valid()) {
            valid = false;
            break;
        }
        ++index;

        kmer_ = kmer_ << 2 | n.value();
    }
    if (!valid) {
        error_index_ = index;
    } else {
        error_index_ = KMER_SIZE;
    }
}


bool Kmer::valid() {
    state_ = 1;

    return KMER_SIZE == error_index_;
}

u_int64_t Kmer::value() const {
    check_state();

    return kmer_;
}


u_int8_t Kmer::error() const {
    check_state();

    return error_index_;
}

bool Kmer::operator==(const Kmer& kmer) const
{
    return kmer_ == kmer.value();
}

void Kmer::check_state() const 
{
    if (state_ == 0) {
        throw ValidateException("You must validate object before reading value");
    }
}