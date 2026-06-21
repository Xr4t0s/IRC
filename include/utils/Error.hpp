#pragma once

#include <exception>

struct Error : std::exception {
    const char* err;

    Error(const char* err);

    const char* what() const throw();
};
