#ifndef H_IO
#define H_IO

#include <iostream>
#include <string>

void process_error(std::istream& in);

template<typename T>
T readval(std::istream& in) {
    T res;
    in >> res;
    while(!in.good()) {
        process_error(in);
        in >> res;
    }
    return res;
}

#endif