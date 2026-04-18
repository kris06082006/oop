#include "io.h"
#include <string>
#include <functional>
#include <limits>
#include <stdexcept>

void process_error(std::istream& in) {
    if (in.bad()) {
        throw std::runtime_error("Corrupted input stream");
    }
    if (in.eof()) {
        throw std::runtime_error("EOF");
    }
    if (in.fail()) {
        in.clear();
        in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}