#include "Employee.h"
#include <iostream>
#include <format>

void Employee::printInfo() const {
    std::cout << std::format("___ Employee Information ___\n"
                            "ID: {}\n"
                            "Name: {}\n"
                            "Birth Year: {}\n"
                            "Position: {}\n"
                            "Education: {}\n"
                            "Salary: {:.2f}\n"
                            "Type: Regular Employee\n", 
                            id, name, birthYear, position, education, salary);
}

void Employee::writeToStream(std::ostream& out) const {
    out << std::format("EMPLOYEE|{}|{}|{}|{}|{}|{:.2f}",
            id, name, birthYear, position, education, salary);
}