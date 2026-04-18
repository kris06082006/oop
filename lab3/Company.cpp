#include "Company.h"
#include "Department.h"
#include "IEmployee.h"
#include "serialization_helpers.h"
#include <iostream>
#include <algorithm>
#include <fstream> 
#include <sstream>
#include <map>
#include <yaml-cpp/yaml.h>
#include <utility>
#include <format>
#include <future>
#include <vector>

using namespace std; 

void Company::clear() {
    for (auto const& [id, employee] : globalEmployees) {
        delete employee;
    }
    globalEmployees.clear();

    for (Department* dept : departments) {
        delete dept;
    }
    departments.clear();
}

void Company::addDepartment(Department* dept) {
    if (dept && findDepartment(dept->getName()) == nullptr){
        departments.push_back(dept);
    }
    else {
        return;
    }
}

Department* Company::findDepartment(const std::string& deptName) const {
    for (Department* topDept : departments) {
        Department* found = const_cast<Company*>(this)->findDepartmentRecursive(deptName, topDept);
        if (found) {
            return found;
        }
    }
    return nullptr;
}

Department* Company::findDepartmentRecursive(const std::string& deptName, Department* startDept) const{
    if (!startDept) return nullptr;
    if (startDept->getName() == deptName) return startDept;

    for (Department* subDept : startDept->getSubDepartments()) {
        if (Department* found = findDepartmentRecursive(deptName, subDept)) {
            return found;
        }
    }
    return nullptr;
}

IEmployee* Company::findEmployeeById(int id) {
    auto it = globalEmployees.find(id);
    if(it != globalEmployees.end()) {
        return it->second;
    }
    return nullptr;
}

bool Company::hireEmployee(const std::string& deptName, IEmployee* employee) {
    if (!employee) return false;

    Department* targetDept = findDepartment(deptName);

    if (!targetDept) {
        return false;
    }

    if (findEmployeeById(employee->getId()) != nullptr) {
        return false; 
    }

    globalEmployees[employee->getId()] = employee;

    targetDept->addEmployee(employee);

    Manager* m = dynamic_cast<Manager*>(employee);
    if (m != nullptr) {
        targetDept->setManager(m);
    }

    return true;
}

bool Company::dismissEmployee(int id) {
    auto it = globalEmployees.find(id);
    if (it == globalEmployees.end()) return false;

    for (auto* root : departments) {
        root->cleanRecursive(id);
    }

    delete it->second; 
    globalEmployees.erase(it);
    
    return true;
}

bool Company::transferEmployee(int employeeId, const std::string& toDeptName){
    IEmployee* employee = findEmployeeById(employeeId);
    if (!employee) {
        return false;
    }
    Department* oldDept = nullptr;
    Department* newDept = nullptr;

    for (Department* topDept : departments) {
        Department* found = findDepartmentRecursive(toDeptName, topDept);
        if (found) newDept = found;
    }

    if (!newDept) return false;

    for (Department* topDept : departments) {
        Department* found = topDept->findDepartmentContainingEmployee(employeeId);
        if (found) {
            oldDept = found;
            break;
        }
    }

    if (oldDept == newDept) {
        return true;
    }

    oldDept->removeEmployee(employeeId);
    newDept->addEmployee(employee);

    return true;
}

bool Company::promoteEmployee(int employeeId, const std::string& newDeptName){
    IEmployee* oldEmployee = findEmployeeById(employeeId);
    if (!oldEmployee) {
        return false; 
    }
    if (dynamic_cast<Manager*>(oldEmployee)) {
        return false;
    }

    Department* targetDept = nullptr;
    for (Department* topDept : departments) {
        targetDept = findDepartmentRecursive(newDeptName, topDept);
        if (targetDept) break;
    }

    if (!targetDept) {
        return false;
    }

    for (Department* topDept : departments) {
        Department* currentDept = topDept->findDepartmentContainingEmployee(employeeId);
        if (currentDept) {
            currentDept->removeEmployee(employeeId);
            break;
        }
    }

    Manager* previosManager = targetDept->getManager();
    if (previosManager){
        targetDept->setManager(nullptr);
    }

    Manager* newManager = new Manager(
        oldEmployee->getId(),
        oldEmployee->getName(),
        oldEmployee->getBirthYear(),
        oldEmployee->getPosition(),
        oldEmployee->getEducation(),
        oldEmployee->getSalary()
    );

    globalEmployees.erase(employeeId);
    delete oldEmployee;
    globalEmployees[employeeId] = newManager;

    targetDept->setManager(newManager);
    targetDept->addEmployee(newManager);

    return true;
}

void Company::showCompanyStructure() const {
    cout << "\n___ Company Structure: " << name << " ___" << endl;
    if (departments.empty()) {
        cout << "The company has no top-level departments." << endl;
        return;
    }
    for (Department* dept : departments){
        printDepartment(dept, 0);
    }
}

void Company::printDepartment(Department* dept, int level) const {
    if (!dept) return;

    string indent(level * 4, ' ');
    string prefix = (level == 0) ? "- " : "└─ ";

    Manager* mgr = dept->getManager();
    string mgrInfo = mgr ? (" (Manager: " + mgr->getName() + ")") : " (No Manager)";

    cout << indent << prefix << dept->getName() << mgrInfo << endl;

    for (Department* subDept : dept->getSubDepartments()) {
        printDepartment(subDept, level + 1);
    }
}

void Company::showDepartmentStaff(const string& deptName) {
    Department* targetDept = nullptr;
    
    for (Department* topDept : departments) {
        targetDept = findDepartmentRecursive(deptName, topDept);
        if (targetDept) break;
    }

    if (!targetDept) {
        cerr << "Error: Department '" << deptName << "' not found in structure." << endl;
        return;
    }

    cout << "\n___ Department Staff: " << targetDept->getName() << " ___" << endl;

    Manager* mgr = targetDept->getManager();
    if (mgr) {
        cout << "Manager: " << mgr->getName() << " (ID: " << mgr->getId() << ")" << endl;
    } else {
        cout << "Manager: None" << endl;
    }
    
    const auto& staff = targetDept->getEmployees(); 

    if (staff.empty()) {
        cout << "The department has no employees (other than the manager, if appointed)." << endl;
    } else {
        cout << "\nEmployee List (" << staff.size() << "):" << endl;
        cout << "ID | Name | Position | Salary" << endl;
        cout << "---------------------------------------" << endl;
        
        for (const auto& pair : staff) {
            IEmployee* emp = pair.second;
            
            cout << emp->getId() 
                      << " | " << emp->getName() 
                      << " | " << emp->getPosition() 
                      << " | " << emp->getSalary() << endl;
        }
    }
    cout << "---------------------------------------" << endl;
}

std::vector<IEmployee*> Company::getSubordinates(Manager* manager) {
    std::vector<IEmployee*> subordinates;
    if (!manager) {
        return subordinates;
    }
    Department* managedDept = nullptr;
    
    for (Department* topDept : departments) {
        managedDept = findDepartmentByManager(manager, topDept);
        if (managedDept) {
            break;
        }
    }

    if (!managedDept) {
        return subordinates; 
    }

    const auto& staffMap = managedDept->getEmployees();

    for (const auto& pair : staffMap) {
        IEmployee* emp = pair.second;
        if (emp->getId() != manager->getId()) {
            subordinates.push_back(emp);
        }
    }

    return subordinates;
}

Department* Company::findDepartmentByManager(Manager* manager, Department* startDept) {
    if (!startDept) return nullptr;

    if (startDept->getManager() == manager) {
        return startDept;
    }

    for (Department* subDept : startDept->getSubDepartments()) {
        if (Department* found = findDepartmentByManager(manager, subDept)) {
            return found;
        }
    }
    return nullptr;
}

bool Company::saveToFile(const std::string& filename) const {
    YAML::Emitter out;
    out << YAML::BeginMap; 
    out << YAML::Key << "company_name" << YAML::Value << name;

    out << YAML::Key << "employees" << YAML::Value << YAML::BeginSeq; 
    for (auto const& [id, emp] : globalEmployees) {
        out << YAML::BeginMap;
        out << YAML::Key << "id" << YAML::Value << emp->getId();
        out << YAML::Key << "type" << YAML::Value << (dynamic_cast<Manager*>(emp) ? "Manager" : "Employee");
        out << YAML::Key << "name" << YAML::Value << emp->getName();
        out << YAML::Key << "year" << YAML::Value << emp->getBirthYear();
        out << YAML::Key << "position" << YAML::Value << emp->getPosition();
        out << YAML::Key << "education" << YAML::Value << emp->getEducation();
        out << YAML::Key << "salary" << YAML::Value << emp->getSalary();
        out << YAML::EndMap;
    }
    out << YAML::EndSeq;

    out << YAML::Key << "departments" << YAML::Value << YAML::BeginSeq;
    for (Department* dept : departments) {
        serializeDepartment(out, dept);
    }
    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream ofs(filename);
    if (!ofs.is_open()) return false;
    ofs << out.c_str();
    return true;
}

void Company::serializeDepartment(YAML::Emitter& out, Department* dept) const {
    out << YAML::BeginMap;
    out << YAML::Key << "name" << YAML::Value << dept->getName();
    
    int mid = dept->getManager() ? dept->getManager()->getId() : -1;
    out << YAML::Key << "manager_id" << YAML::Value << mid;

    out << YAML::Key << "staff_ids" << YAML::Flow << YAML::BeginSeq;
    for (auto const& [id, emp] : dept->getEmployees()) {
        out << id;
    }
    out << YAML::EndSeq;

    auto subs = dept->getSubDepartments();
    if (!subs.empty()) {
        out << YAML::Key << "sub_departments" << YAML::Value << YAML::BeginSeq;
        for (Department* sub : subs) {
            serializeDepartment(out, sub);
        }
        out << YAML::EndSeq;
    }
    out << YAML::EndMap;
}

bool Company::internalLoadLogic(const std::string& filename) {
    YAML::Node root;
    try {
        root = YAML::LoadFile(filename);
    } catch (const std::exception& e) {
        std::cerr << "YAML Error: " << e.what() << std::endl;
        return false;
    }

    UnorderedMap<int, IEmployee*> tempEmps;
    std::vector<Department*> tempDepts;

    try {
        if (root["company_name"]) {
            this->name = root["company_name"].as<std::string>();
        }

        if (root["employees"]) {
            for (auto const& node : root["employees"]) {
                IEmployee* emp = nullptr;
                int id = node["id"].as<int>();
                std::string type = node["type"].as<std::string>();
                std::string n = node["name"].as<std::string>();
                int y = node["year"].as<int>();
                std::string p = node["position"].as<std::string>();
                std::string ed = node["education"].as<std::string>();
                double s = node["salary"].as<double>();

                if (type == "Manager") {
                    emp = new Manager(id, n, y, p, ed, s);
                } else {
                    emp = new Employee(id, n, y, p, ed, s);
                }
                tempEmps[id] = emp;
            }
        }

        if (root["departments"]) {
            for (auto const& dNode : root["departments"]) {
                tempDepts.push_back(deserializeDepartment(dNode, tempEmps));
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Error during YAML parsing: " << e.what() << std::endl;
        for (auto& p : tempEmps) delete p.second;
        for (auto* d : tempDepts) delete d;
        return false;
    }

    this->globalEmployees = std::move(tempEmps);
    this->departments = std::move(tempDepts);
    return true;
}
Department* Company::deserializeDepartment(const YAML::Node& node, UnorderedMap<int, IEmployee*>& tempEmps) {
    Department* dept = new Department(node["name"].as<std::string>());

    int mid = node["manager_id"].as<int>();
    if (mid != -1 && tempEmps.count(mid)) {
        dept->setManager(dynamic_cast<Manager*>(tempEmps[mid]));
    }

    if (node["staff_ids"]) {
        for (auto const& idNode : node["staff_ids"]) {
            int eid = idNode.as<int>();
            if (tempEmps.count(eid)) {
                dept->addEmployee(tempEmps[eid]);
            }
        }
    }

    if (node["sub_departments"]) {
        for (auto const& subNode : node["sub_departments"]) {
            dept->addSubDepartment(deserializeDepartment(subNode, tempEmps));
        }
    }

    return dept;
} 

bool Company::loadFromFile(const std::string& filename) {
    this->clear(); 

    if (!this->internalLoadLogic(filename)) {
        std::cerr << "Critical error: Load failed. Company is now empty." << std::endl;
        return false;
    }

    return true; 
}

int Company::getMaxEmployeeId() const {
    int maxId = 0;
    for (auto const& [id, emp] : globalEmployees) {
        if (id > maxId) maxId = id;
    }
    return maxId;
}

bool Company::departmentExistsRecursive(const std::string& name) {
    for (Department* topDept : departments) {
        if (findDepartmentRecursive(name, topDept) != nullptr) {
            return true;
        }
    }
    return false;
}

void Company::showCategorizedStaff() const {
    std::cout << "\n--- LIST OF MANAGERS ---" << std::endl;
    bool managersExist = false;
    
    for (const auto& pair : globalEmployees) { 
        IEmployee* emp = pair.second;   
        
        if (emp && dynamic_cast<Manager*>(emp)) {
            emp->printInfo();
            managersExist = true;
        }
    }
    if (!managersExist) std::cout << "No managers in the system." << std::endl;

    std::cout << "\n--- LIST OF REGULAR EMPLOYEES ---" << std::endl;
    bool employeesExist = false;
    
    for (const auto& pair : globalEmployees) { 
        IEmployee* emp = pair.second; 
        
        if (emp && dynamic_cast<Employee*>(emp)) {
            emp->printInfo();
            employeesExist = true;
        }
    }
    if (!employeesExist) std::cout << "No regular employees in the system." << std::endl;
}

void Company::getSubRecursiveSeq(Department* dept, UnorderedMap<int, IEmployee*>& result) {
    if (!dept) return;

    IEmployee* mgr = dept->getManager();
    if (mgr){
        result[mgr->getId()] = mgr;
        std::this_thread::sleep_for(std::chrono::nanoseconds(30));
    }
    for (auto const& pair : dept->getEmployees()) {
        IEmployee* e = pair.second;
        if (e){
            result[e->getId()] = e;
            std::this_thread::sleep_for(std::chrono::nanoseconds(30));
        }
    }

    for (Department* subDept : dept->getSubDepartments()) {
        getSubRecursiveSeq(subDept, result);
    }
}

UnorderedMap<int, IEmployee*> Company::getAllSubordinatesSeq(int managerId) {
    UnorderedMap<int, IEmployee*> result;
    
    IEmployee* emp = findEmployeeById(managerId);
    if (!emp) {
        return result;
    }
    
    Manager* mgr = dynamic_cast<Manager*>(emp);
    if (!mgr) {
        return result; 
    }

    Department* dept = mgr->getDepartment();
    if (dept) {
        getSubRecursiveSeq(dept, result);
        
        if (result.find(managerId) != result.end()) {
            result.erase(managerId);
        }
    }
    
    return result;
}

std::vector<IEmployee*> Company::getSubRecursivePar(Department* dept) {
    if (!dept) return {};

    std::vector<IEmployee*> localResult;

    for (auto const& pair : dept->getEmployees()) {
        localResult.push_back(pair.second);
        std::this_thread::sleep_for(std::chrono::nanoseconds(30));
    }

    auto subDepts = dept->getSubDepartments();
    if (subDepts.empty()) return localResult;

    std::vector<std::future<UnorderedMap<int, IEmployee*>>> futures;
    
    for (Department* sub : subDepts) {
        if (sub) {
            futures.push_back(std::async(std::launch::async, [this, sub]() {
                UnorderedMap<int, IEmployee*> tempRes;
                this->getSubRecursiveSeq(sub, tempRes);
                return tempRes;
            }));
        }
    }

    for (auto& fut : futures) {
        UnorderedMap<int, IEmployee*> subResMap = fut.get();
        for (auto const& pair : subResMap) {
            localResult.push_back(pair.second);
        }
    }

    return localResult;
}

UnorderedMap<int, IEmployee*> Company::getAllSubordinatesPar(int managerId) {
    UnorderedMap<int, IEmployee*> result;
    IEmployee* emp = findEmployeeById(managerId);
    Manager* mgr = dynamic_cast<Manager*>(emp);

    if (mgr) {
        Department* managedDept = nullptr;
        for (Department* topDept : departments) {
            managedDept = findDepartmentByManager(mgr, topDept);
            if (managedDept) break;
        }

        if (managedDept) {
            std::vector<IEmployee*> vec = getSubRecursivePar(managedDept);
            for (auto* e : vec) {
                if (e && e->getId() != managerId) {
                    result[e->getId()] = e;
                }
            }
        }
    }
    return result;
}

Company::~Company(){
    clear();
}

