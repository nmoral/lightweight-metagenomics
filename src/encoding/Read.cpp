
#include <iostream>
#include <optional>
#include "Read.h"
#include "extraction/KmerExtractor.h"
#include "exception/exceptions.h"
#include "extraction/exception/exceptions.h"

Read::Read(const std::string& read, KmerExtractor& extractor): read_(read), extractor_(&extractor) {
    totalKmer_ = read.size() - KMER_SIZE + 1;
    index_ = 0;
}


std::optional<Kmer> Read::next() 
{   
    Kmer k;


    try {
        k = Kmer(extractor_->extract((*this), index_));
    } catch (const ExtractException& e ) {
        if constexpr (ExtractionMode::STRICT == CURRENT_MODE) {
            std::throw_with_nested(e);
        }

        index_ = e.index();

        // Le jump dépasse la fin — plus rien à lire
        if ((index_ + KMER_SIZE - 1) >= size()) {
            return std::nullopt;  // ou throw selon le design
        }
        return next();

    }
    ++index_;

    return k;
}

Kmer Read::window(const int at, const int size) const 
{
    return Kmer(read_.substr(at, size));
}

bool Read::done() const {
    return size() == index_ + KMER_SIZE - 1;
}

int Read::size() const 
{
    return read_.size();
}
