#pragma once

#include <stdexcept>
#include <string>


class EncodingException : public std::invalid_argument {
    using std::invalid_argument::invalid_argument;
};

class LengthException: public std::invalid_argument {
    using std::invalid_argument::invalid_argument;
};