#ifndef SERIALIZATION_HELPERS_H
#define SERIALIZATION_HELPERS_H

#include <string>
#include <vector>
#include <sstream>
#include "IEmployee.h"
#include "Manager.h"
#include "Employee.h" 

/**
 * @brief Structure for temporarily storing department data and relationship IDs 
 * during file loading (deserialization).
 */
struct ParsedDepartmentData {
    std::string name;
    std::string parentName;
    int managerId = -1;
    std::vector<int> employeeIds;
};
/**
 * @brief Parses a department line from the file and returns the structure containing 
 * relationship IDs.
 * @param data The serialized string (e.g., DEPT|Name|ParentName|ManagerID|EmployeeIDs...).
 * @return ParsedDepartmentData structure.
 * @throws std::runtime_error if required fields are missing.
 */
ParsedDepartmentData parseDepartmentLine(const std::string& data);
/**
 * @brief Polymorphically creates an IEmployee object (Employee or Manager) 
 * from the serialized string data.
 * @param data The serialized employee string (e.g., TYPE|ID|Name|...).
 * @return Pointer to the newly created IEmployee object, or nullptr on failure.
 */
IEmployee* createEmployeeFromSerializedString(const std::string& data);

#endif 