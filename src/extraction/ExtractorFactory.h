
#pragma once
#include <memory>
#include "KmerExtractor.h"

std::unique_ptr<KmerExtractor> createExtractor();
