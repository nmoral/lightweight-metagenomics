#pragma once

#include <memory>
#include "KmerExtractor.h"
#include "SkipKmerExtractor.h"



std::unique_ptr<KmerExtractor> createExtractor();