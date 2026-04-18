#include "Department.h"
#include "Manager.h"
#include "IEmployee.h"
#include <iostream>
#include <iomanip>
#include <format>

using namespace std;

void Department::addEmployee(IEmployee* employee) {
    if(employee) {
        employees[employee->getId()] = employee;
    }
}


bool Department::removeEmployee(int id) {
    size_t erased = employees.erase(id);
    if (erased > 0) {
        return true; 
    } else {
        return false;
    }
}

IEmployee* Department::findEmployeeById(int id){
    auto it = employees.find(id);
    if (it != employees.end()){
        return it->second;
    }
    return nullptr;
}

void Department::setManager(Manager* mgr) {
    manager = mgr;
}

void Department::addSubDepartment(Department* subDept) {
    if (subDept) {
        subDepartments.push_back(subDept);
    }
}

void Department::cleanRecursive(int id) {
    if (manager && manager->getId() == id) {
        manager = nullptr;
    }

    employees.erase(id);

    for (auto* sub : subDepartments) {
        sub->cleanRecursive(id);
    }
}

Department* Department::findSubDepartment(const std::string& subDeptName){
    for (Department* subDept : subDepartments) { 
        if (subDept->getName() == subDeptName){
            return subDept;
        }
    }
    return nullptr;
}

void Department::showStaff() const {
    if (manager) {
        cout << "  Manager: " << manager->getName() << " (ID: " << manager->getId() << ", Position: " << manager->getPosition() << ")" << endl;
    }
    else {
        cout << "  Manager: None" << endl;
    }

    cout << "  --- Staff (" << employees.size() << " total) ---" << endl;
    for (const auto& pair : employees) {
        IEmployee* emp = pair.second;
        if (emp && emp != manager) {
            emp->printInfo();
        }
    }

    if (!subDepartments.empty()) {
        cout << "  --- Sub-Departments (" << subDepartments.size() << " total) ---" << endl;
        for (Department* subDept : subDepartments) {
            cout << "  |--> " << subDept->getName() << endl;        
        }  
    }
}

std::vector<IEmployee*> Department::getAllEmployeesRecursive() const {
    std::vector<IEmployee*> all;
    
    for (const auto& pair : employees) {
        all.push_back(pair.second);
    }

    for (const Department* subDept : subDepartments) {
        std::vector<IEmployee*> subStaff = subDept->getAllEmployeesRecursive();
        all.insert(all.end(), subStaff.begin(), subStaff.end());
    }

    return all;
}

void Department::showStructure(int level) const {
    for (int i = 0; i < level; ++i) {
        cout << "  |  ";
    }

    cout << "+--[" << name << "]";
    
    if (manager) {
        cout << " (Manager: " << manager->getName() << ", ID: " << manager->getId() << ")";
    }
    cout << " (" << employees.size() << " employees)" << endl;

    for (const Department* subDept : subDepartments) {
        subDept->showStructure(level + 1);
    }
}

Department* Department::findDepartmentContainingEmployee(int employeeId) {
    if (employees.count(employeeId) > 0) {
        return this;
    }

    for (Department* subDept : subDepartments){
        if (Department* found = subDept->findDepartmentContainingEmployee(employeeId)) {
            return found;
        }
    }

    return nullptr;
}

void Department::writeToStream(std::ostream& out, const std::string& parentName) const {
    int mgrId = manager ? manager->getId() : -1;

    std::stringstream employeeIdsListStream;
    for (const auto& pair : employees) {
        employeeIdsListStream << pair.first << " ";
    }
    
    std::string employeeIdsList = employeeIdsListStream.str();
    if (!employeeIdsList.empty()) {
        employeeIdsList.pop_back(); 
    }

    out << std::format(
        "DEPT|{}|{}|{}|{}",
        name, parentName, mgrId, employeeIdsList
    ); 
        
    for (const Department* subDept : subDepartments) {
        out << "\n"; 
        subDept->writeToStream(out, name); 
    }
}

Department::~Department() {
    for (Department* subDept : subDepartments){
        delete subDept;
    }
}

