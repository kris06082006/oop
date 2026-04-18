#ifndef H_CUT
#define H_CUT
#include <string>

char* cut(const char* str, size_t n, char sep);

std::string cut(const std::string& str, size_t n, char sep);

std::string_view cut(std::string_view str, size_t n, char sep);

#endif
