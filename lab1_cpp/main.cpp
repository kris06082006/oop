#include "java_properties.h"
#include "check.h"
#include "io.h"

#include <iostream>
#include <string>
#include <stdexcept>
#include <algorithm>

int main() {
    try {
        std::cout << "Input name of integer array: ";
        std::string name = readline(std::cin);

        std::cout << "Input integer array (format: \"1,2,3...\"): ";
        std::string array = readline(std::cin);

        check_format_of_array(array);

        size_t size = std::count(array.begin(), array.end(), ',') + 1;

        const char* c_array = array.c_str();
        const char* c_name = name.c_str();

        std::string java_properties_s = int_array_to_properties(array, name);
        std::cout << "Your array as java properties (string):\n" << java_properties_s << "\n";

        char* java_properties_c = int_array_to_properties(c_array, c_name);
        std::cout << "Your array as java properties (char*):\n" << java_properties_c << "\n";

        char* java_properties_c_size = int_array_to_properties(size, c_array, c_name);
        std::cout << "Your array as java properties (size_t, char*):\n" << java_properties_c_size << "\n";


        std::string integer_array_s = properties_to_int_array(java_properties_s, name);
        std::cout << "Your array as integers (string):\n" << integer_array_s << "\n";

        char* integer_array_c = properties_to_int_array(java_properties_c, c_name);
        std::cout << "Your array as integers (char*):\n" << integer_array_c << "\n";

        char* integer_array_c_size = properties_to_int_array(size, java_properties_c_size, c_name);
        std::cout << "Your array as integers (size_t, char*):\n" << integer_array_c_size << "\n";

        delete[] java_properties_c;
        delete[] java_properties_c_size;
        delete[] integer_array_c;
        delete[] integer_array_c_size;


    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}

