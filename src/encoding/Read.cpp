
#include <iostream>
#include <optional>
#include "Read.h"
#include "Kmer.h"
#include "exception/exceptions.h"

Read::Read(const std::string& read, KmerExtractor& extractor): 
    read_(read), 
    extractor_(&extractor) {
    size_ = read.size() - KMER_SIZE + 1;
}


std::optional<Kmer> Read::next() {
    return extractor_->extract((*this), index_);
}

int Read::size() const {
    return size_;
}

bool Read::done() const {
    return index_ >= size_;
}

Kmer Read::window(int at, int size) const 
{
    return Kmer(read_.substr(at, size));
}

