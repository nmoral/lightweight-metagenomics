#pragma once

#include <stdexcept>
#include <string>


class ValidateException : public std::invalid_argument {
    using std::invalid_argument::invalid_argument;
};


class EncodingException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};
