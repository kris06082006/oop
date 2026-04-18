#include "serialization_helpers.h"
#include <stdexcept>
#include <iostream>
#include <iostream>

/**
 * Helper function to safely extract fields separated by '|' from a string stream.
 * @throws std::runtime_error if extraction fails.
 */
void getNextSegment(std::stringstream& ss, std::string& segment, const std::string& errorMsg) {
    if (!std::getline(ss, segment, '|')) {
        throw std::runtime_error(errorMsg);
    }
}

ParsedDepartmentData parseDepartmentLine(const std::string& data) {
    std::stringstream ss(data);
    std::string segment;
    ParsedDepartmentData result;
    
    std::getline(ss, segment, '|'); 
    
    try {
        getNextSegment(ss, result.name, "Missing Department Name");
        
        getNextSegment(ss, result.parentName, "Missing Parent Name");
        
        std::string mgrIdStr;
        getNextSegment(ss, mgrIdStr, "Missing Manager ID");
        result.managerId = mgrIdStr.empty() ? -1 : std::stoi(mgrIdStr);
        
    } catch (const std::exception& e) {
        throw std::runtime_error(std::string("Department parsing failed: ") + e.what());
    }
    
    std::string employeeIdsStr;
    if (std::getline(ss, employeeIdsStr)) { 
        std::stringstream idStream(employeeIdsStr);
        int id;
        while (idStream >> id) {
            result.employeeIds.push_back(id);
        }
    }

    return result;
}

void parseCommonEmployeeFields(std::stringstream& ss, int& id, std::string& name, int& birthYear, 
                               std::string& position, double& salary, std::string& education, const std::string& type) 
{
    std::string idStr, birthYearStr, salaryStr, segment;

    getNextSegment(ss, idStr, "Missing ID");
    getNextSegment(ss, name, "Missing Name");
    getNextSegment(ss, birthYearStr, "Missing BirthYear");
    getNextSegment(ss, position, "Missing Position");
    getNextSegment(ss, salaryStr, "Missing Salary");
    
    if (!std::getline(ss, education)) {
         throw std::runtime_error("Missing Education field.");
    }
    
    try {
        id = std::stoi(idStr);
        birthYear = std::stoi(birthYearStr);
        salary = std::stod(salaryStr);
    } catch (...) {
        throw std::runtime_error("Invalid number format for ID, BirthYear, or Salary.");
    }
}

IEmployee* createEmployeeFromSerializedString(const std::string& data) {
    std::stringstream ss(data);
    std::string type, name, position, education;
    int id = -1, birthYear = 0;
    double salary = 0.0;

    if (!std::getline(ss, type, '|')) return nullptr;

    try {
        parseCommonEmployeeFields(ss, id, name, birthYear, position, salary, education, type);

        if (type == "MANAGER") {
            return new Manager(id, name, birthYear, position, education, salary);
        } else if (type == "EMPLOYEE") {
            return new Employee(id, name, birthYear, position, education, salary); 
        }
        
    } catch (const std::runtime_error& e) {
        std::cerr << "Serialization Error (Type " << type << "): " << e.what() << ". Skipping data: " << data << std::endl;
    }
    return nullptr;
}