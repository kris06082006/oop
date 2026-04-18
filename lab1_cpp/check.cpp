#include"check.h"
#include <cctype>

void check_format_of_array(const std::string& str){
    if (str.empty()) {
        throw std::invalid_argument("Empty string");
    }

    for (size_t i = 0; i < str.length(); i++){
        if (!std::isdigit(str[i]) && str[i] != ','){
            throw std::invalid_argument("The string does not match the format");
        }
    }

    if (str.find(",,") != std::string::npos || str[0] == ',' || str[str.length() - 1] == ','){
            throw std::invalid_argument("The string does not match the format");
    }
}


