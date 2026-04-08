#pragma once

#include <stdexcept>
#include <string>


class EncodingException : public std::invalid_argument {
    using std::invalid_argument::invalid_argument;
};

class NucleotideException: public EncodingException {
    public:
        explicit NucleotideException(const std::string message, char value):
        EncodingException(message),
        value_(value)
         {}

        char value() const {return value_;}

    private:
        char value_;
};

class KmerException: public NucleotideException {
    public:
        explicit KmerException(const std::string message, char value, int index = 0):
        NucleotideException(message, value),
        index_(index) 
        {}

        int index() const {return index_; }

    private: 
        int index_ = 0;
};

class LengthException: public std::invalid_argument {
    using std::invalid_argument::invalid_argument;
};

class OutofBoundsException: public std::runtime_error {
    using std::runtime_error::runtime_error;
};