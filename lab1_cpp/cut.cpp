#include "cut.h"
#include <stdexcept>
#include <iostream>

char* cut(const char* str, size_t n, char sep) {
    if (str == nullptr) {
        throw std::logic_error("str is nullptr");
    }
    auto res = cut(std::string_view(str), n, sep);
    char* out = new char[res.size()+1];
    std::copy(res.begin(), res.end(), out);
    out[res.size()] = '\0';
    return out;
}

std::string cut(const std::string& str, size_t n, char sep) {
    std::string_view res = cut(std::string_view(str), n, sep);
    return std::string{ res };
}

std::string_view cut(std::string_view str, size_t n, char sep) {
    size_t start = 0;
    for(size_t i = 0; i < n; ++i) {
        start = str.find(sep, start);
        if(start == str.npos) {
            throw std::out_of_range("Word is out of range");
        }
        ++start;
    }

    size_t end = str.find(sep, start);
    size_t len = end != str.npos ? end - start : str.npos;

    return str.substr(start, len);
}
