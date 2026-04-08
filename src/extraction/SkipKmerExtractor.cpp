
#include "SkipKmerExtractor.h"
#include "KmerExtractor.h"
#include "encoding/Read.h"
#include "encoding/Kmer.h"
#include "exception/exceptions.h"



Kmer SkipKmerExtractor::extract(const Read& read, const int at) const
{
    try {
        return KmerExtractor::extract(read, at);
    } catch (const ExtractException& e) {
        std::throw_with_nested(ExtractException(e.previous(), e.index() + e.previous().index()));
    }
}
