#ifndef PROPERTIES_H
#define PROPERTIES_H

#include <string>

char* int_array_to_properties(const char*, const char*);
char* int_array_to_properties(size_t, const char*, const char*);
std::string int_array_to_properties(std::string, std::string);

char* properties_to_int_array(const char*, const char*);
char* properties_to_int_array(size_t, const char*, const char*);
std::string properties_to_int_array(std::string, std::string);

#endif
