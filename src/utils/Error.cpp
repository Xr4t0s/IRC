#include "utils/Error.hpp"

Error::Error(const char* error) : err(error) {}

const char* Error::what() const throw() {
    return err;
}