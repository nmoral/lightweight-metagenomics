#pragma once

#include <stdexcept>
#include <string>
#include "encoding/exception/exceptions.h"


class ExtractException : public std::runtime_error {
    public: 
        explicit ExtractException(const KmerException& e, const int index): 
        std::runtime_error(e.what()),
        index_(index),
        previous_(e) {};

        KmerException previous() const {
            return previous_;
        }

        int index() const {
            return index_;
        }

    private: 
        int index_;
        KmerException previous_;
};