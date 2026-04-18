#include "Manager.h"
#include "Department.h"
#include <iostream>
#include <format>

void Manager::printInfo() const {
    std::cout << std::format("___ Manager Information ___\n"
                            "ID: {}\n"
                            "Name: {}\n"
                            "Birth Year: {}\n"
                            "Position: {}\n"
                            "Education: {}\n"
                            "Salary: {:.2f}\n"
                            "Type: Manager\n",
                            id, name, birthYear, position, education, salary);

    if (department) {
        std::cout << std::format("Manages Department: {}\n", department->getName());
    }
    else {
        std::cout << "Currently not managing any dapartment\n";
    }

}

void Manager::writeToStream(std::ostream& out) const {
    out << std::format("MANAGER|{}|{}|{}|{}|{}|{:.2f}|{}", 
                        id, name, birthYear, position, education, salary, 
                        department ? department->getName() : "null");
}