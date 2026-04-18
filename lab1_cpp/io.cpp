#include "io.h"

#include <iostream>
#include <string>
#include <functional>
#include <limits>
#include <stdexcept>

void process_error(std::istream& in) {
    if (in.bad()) {
        throw std::runtime_error("Corrupted input stream");
    }
    if (in.eof()) {
        throw std::runtime_error("End of file");
    }
    if (in.fail()) {
        in.clear();
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

std::string readline(std::istream& in){
    std::string res;
    std::getline(in, res);

    while(!in.good()) {
        process_error(in);
        std::getline(in, res);
    }
    return res;
}
