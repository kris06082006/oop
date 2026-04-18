#include "java_properties.h"
#include "cut.h"
#include <stdexcept>
#include <sstream>

std::string int_array_to_properties(std::string source, std::string name_array){
    std::stringstream raw_res;

    for (size_t i = 0; ; i++) {
        try {
            std::string number = cut(source, i, ',');
            raw_res << name_array << "." << (i + 1) << "=" << number << "\n";           
        } catch (const std::out_of_range& e) {
            break;
        }
    }
    std::string result = raw_res.str();
    result.pop_back();
    return result;
}

char* int_array_to_properties(const char* source, const char* name_array){
    if (source == nullptr) {
        throw std::logic_error("str is nullptr");
    }
    if (name_array == nullptr) {
        throw std::logic_error("str is nullptr");
    }
    std::string copy_source(source);
    std::string copy_name_array(name_array);

    std::string raw_res = int_array_to_properties(copy_source, copy_name_array);

    char* result = new char[raw_res.size() + 1];
    std::copy(raw_res.begin(), raw_res.end(), result);
    result[raw_res.size()] = '\0';

    return result;
}

char* int_array_to_properties(size_t count_elem, const char* source, const char* name_array){
    if (source == nullptr) {
        throw std::logic_error("str is nullptr");
    }
    if (name_array == nullptr) {
        throw std::logic_error("str is nullptr");
    }
    std::string copy_source(source);
    std::string copy_name_array(name_array);

    std::stringstream raw_res;
     for (size_t i = 0; i < count_elem; i++) {
        try {
            std::string number = cut(copy_source, i, ',');
            raw_res << copy_name_array << "." << (i + 1) << "=" << number << "\n";            
        } catch (const std::out_of_range& e) {
            throw std::out_of_range("The number of requested elements exceeds the total number");
        }
    }
    std::string new_raw_res = raw_res.str();
    new_raw_res.pop_back();
    char* result = new char[new_raw_res.size() + 1];
    std::copy(new_raw_res.begin(), new_raw_res.end(), result);
    result[new_raw_res.size()] = '\0';

    return result;
}

std::string properties_to_int_array(std::string source, std::string name_array){
    std::string result;

    for (size_t i = 0; ; i++) {
        try {
            std::string elem = cut(source, i, '\n');
            size_t pos = elem.find("=");
            std::string sub_elem = elem.substr(pos + 1);
            result += sub_elem;
            result += ',';
            
        } catch (const std::out_of_range& e) {
            break;
        }
    }
    result.pop_back();
    return result;
}


char* properties_to_int_array(const char* source, const char*name_array){
    if (source == nullptr) {
        throw std::logic_error("str is nullptr");
    }
    if (name_array == nullptr) {
        throw std::logic_error("str is nullptr");
    }
    std::string copy_source(source);
    std::string copy_name_array(name_array);

    std::string raw_res = properties_to_int_array(copy_source, copy_name_array);

    char* result = new char[raw_res.size() + 1];
    std::copy(raw_res.begin(), raw_res.end(), result);
    result[raw_res.size()] = '\0';

    return result;
}

char* properties_to_int_array(size_t count_elem, const char* source, const char* name_array){
    if (source == nullptr) {
        throw std::logic_error("str is nullptr");
    }
    if (name_array == nullptr) {
        throw std::logic_error("str is nullptr");
    }
    std::string copy_source(source);
    std::string copy_name_array(name_array);

    std::string raw_res;
     for (size_t i = 0; i < count_elem; i++) {
        try {
            std::string elem = cut(copy_source, i, '\n');
            size_t pos = elem.find("=");
            std::string sub_elem = elem.substr(pos + 1);
            raw_res += sub_elem;
            raw_res += ',';
        } catch (const std::out_of_range& e) {
            throw std::out_of_range("The number of requested elements exceeds the total number");
        }
    }
    raw_res.pop_back();
    char* result = new char[raw_res.size() + 1];
    std::copy(raw_res.begin(), raw_res.end(), result);
    result[raw_res.size()] = '\0';

    return result;
}

