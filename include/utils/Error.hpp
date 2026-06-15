#pragma once

struct Error {
    const char* err;

    Error(const char* err);

    const char* what() const throw();
};
