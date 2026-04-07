
#include "Kmer.h"
#include "Nucleotide.h"
#include "exception/exceptions.cpp"

Kmer::Kmer(const std::string& kmer) {
    if (kmer.size() > 31 || kmer.size() == 0) {
        throw LengthException("kmer size must be between 1 and 31 char, got "+ std::to_string(kmer.size()) + " instead");
    }
    kmer_ = encode(kmer);
    length_ = kmer.size();
}


u_int64_t Kmer::encode(const std::string& kmer) const {
    u_int64_t value = 0;

    for (const char c: kmer) {
        value = value << 2 | (u_int8_t) Nucleotide(c);
    }

    return value;
}


Kmer::operator std::string() const {
    std::string result;
    for (int i = 0; i < length_; ++i) {
        result += (char)(*this)[i];
    }

    return result;
} // LCOV_EXCL_LINE


Nucleotide Kmer::operator[](const std::size_t index) const {
    return Nucleotide((int) ((kmer_ >> (length_ - 1 - index) * 2) & 0b11));
}

bool Kmer::operator==(const u_int64_t kmer) const {
    return kmer_ == kmer;
}

int Kmer::size() const {
    return length_;
}


