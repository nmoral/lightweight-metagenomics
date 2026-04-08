#include "KmerExtractor.h"
#include "encoding/Read.h"
#include "encoding/Kmer.h"
#include "encoding/exception/exceptions.h"
#include "exception/exceptions.h"

KmerExtractor::KmerExtractor() {}

Kmer KmerExtractor::extract(const Read& read, const int at) const 
{
    
    try {
        return read.window(at, KMER_SIZE);
    } catch (const KmerException& e) {
        std::throw_with_nested(ExtractException(e, at + 1));
    }
}

