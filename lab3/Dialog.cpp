#include "Dialog.h"
#include "IEmployee.h"
#include "Manager.h"
#include "Employee.h"
#include "io.h"
#include <iostream>
#include <string>
#include <limits>
#include <typeinfo>

Dialog::Dialog() : company("Clobal_Tech"), next_id(101) {
    company.addDepartment(new Department("IT_Core"));
}

int Dialog::run() {
    int option;
    while (true) {
        printMenu();

        try {
            std::cout << "\nChoose option: ";
            option = readval<int>(std::cin);
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        } 
        catch (const std::runtime_error& e){
            if (std::string(e.what()) == "EOF") break;
            std::cerr << "Error: " << e.what() << std::endl;
            continue;
        }

        if (option == 0) {
            std::cout << "Exiting program." << std::endl;
            return 0;
        }

        switch (option) {
            case 1:  addEmployee(); break;
            case 2:  addDept(); break;
            case 3:  findEmployee(); break;
            case 4:  findDept(); break;
            case 5:  dismissEmployee(); break;
            case 6:  setPos(); break;
            case 7:  setSal(); break;
            case 8:  printFullInfo(); break;
            case 9:  promote(); break;
            case 10: transfer(); break;
            case 11: showStructure(); break;
            case 12: showDeptStaff(); break;
            case 13: showRTTI(); break;
            case 14: showSubordinates(); break;
            case 15: saveToFile(); break;
            case 16: loadFromFile(); break;
            case 17: showCategorized(); break;
            default: std::cerr << "Invalid option... Please try again." << std::endl; break;
        }
    }
    return 0;
}

int Dialog::readEmployeeId() {
    std::cout << "Enter Employee ID: ";
    int id = readval<int>(std::cin);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
    return id;
}

std::string Dialog::readStringInput(const std::string& prompt) {
    std::string input;
    std::cout << prompt; 
    if (!std::getline(std::cin, input)) {
        if (std::cin.eof()) throw std::runtime_error("EOF");
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return "";
    }
    return input;
}

std::string Dialog::readDepartmentName() {
    return readStringInput("Enter Department Name: ");
}

std::string Dialog::readPosition() {
    return readStringInput("Enter Position: ");
}

IEmployee* Dialog::createNewEmployee(int id) {
    std::string name, position, education;
    int birthYear, type;
    double salary;
    std::cout << "\n--- New Employee Creation (ID: " << id << ") ---" << std::endl;
    std::cout << "Select Employee Type:\n 1. Regular Employee\n 2. Manager\nChoose type (1 or 2): ";
    type = readval<int>(std::cin);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 

    if (type != 1 && type != 2) {
        std::cerr << "Invalid type selection. Aborting creation." << std::endl;
        return nullptr;
    }

    name = readStringInput("Enter Name: ");
    std::cout << "Enter Birth Year: "; 
    birthYear = readval<int>(std::cin);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    position = readPosition();
    education = readStringInput("Enter Education: ");
    std::cout << "Enter Initial Salary: "; 
    salary = readval<double>(std::cin);
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (type == 1) return new Employee(id, name, birthYear, position, education, salary);
    if (type == 2) return new Manager(id, name, birthYear, position, education, salary);
    return nullptr;
}


void Dialog::printMenu() {
    std::cout << "\n___ HR System Menu: " << company.getName() << " ___" << std::endl;
    
    std::cout << "\n--- Основные операции с объектами (Создание/Поиск/Удаление) ---" << std::endl;
    std::cout << " 1. Create & Hire NEW Employee" << std::endl;
    std::cout << " 2. Add NEW Department" << std::endl;
    std::cout << " 3. Find Employee by ID" << std::endl;
    std::cout << " 4. Find Department by Name" << std::endl;
    std::cout << " 5. Dismiss Employee (Уволить по ID)" << std::endl;
    
    std::cout << "\n--- Работа с данными (Get/Set) ---" << std::endl;
    std::cout << " 6. Set Employee Position" << std::endl;
    std::cout << " 7. Set Employee Salary" << std::endl;
    std::cout << " 8. Print Employee Full Info" << std::endl;
    
    std::cout << "\n--- HR Логика и Структура ---" << std::endl;
    std::cout << " 9. PROMOTE Employee to Manager" << std::endl;
    std::cout << " 10. Transfer Employee to another Department" << std::endl;
    std::cout << " 11. Show Company Structure" << std::endl;
    std::cout << " 12. Show Department Staff" << std::endl;
    std::cout << " 13. Get Employee Type Info (RTTI)" << std::endl;
    std::cout << " 14. Get Subordinates List (Руководителя)" << std::endl;
    
    std::cout << "\n--- Работа с файлами ---" << std::endl;
    std::cout << " 15. Save State to File" << std::endl;   
    std::cout << " 16. Load State from File" << std::endl;  
    std::cout << " 17. Show categorized staff (Managers and Employees separately)\n" << std::endl; 
    
    std::cout << "\n 0. Exit Program" << std::endl;
}

void Dialog::addEmployee() {
    std::string deptName = readDepartmentName();
    Department* dept = company.findDepartment(deptName);
    
    if (dept == nullptr) { 
        std::cerr << "Error: Department '" << deptName << "' not found." << std::endl;
        return; 
    }

    IEmployee* emp = createNewEmployee(next_id);
    if (!emp) return;

    if (company.hireEmployee(deptName, emp)) {
        std::cout << "Success: " << emp->getName() << " hired to " << deptName << std::endl;
        next_id++;
    } else {
        delete emp;
    }
}

void Dialog::addDept() {
    std::string newDeptName = readDepartmentName();
    
    std::cout << "Enter Parent Department Name (press Enter for Top Level): ";
    std::string parentName;
    std::getline(std::cin, parentName);

    if (parentName.empty()) {
        if (company.findDepartment(newDeptName) != nullptr) {
            std::cerr << "Error: Department " << newDeptName << " already exists at top level." << std::endl;                
        } else {
            company.addDepartment(new Department(newDeptName));
            std::cout << "Top-level department '" << newDeptName << "' added." << std::endl;
        }
    } else {
        Department* parent = company.findDepartment(parentName);
        if (parent) {
            parent->addSubDepartment(new Department(newDeptName)); 
            std::cout << "Department '" << newDeptName << "' added inside '" << parentName << "'." << std::endl;
        } else {
            std::cerr << "Error: Parent department '" << parentName << "' not found." << std::endl;
        }
    }
}

void Dialog::findEmployee() {
    int id = readEmployeeId();
    IEmployee* emp = company.findEmployeeById(id);
    if (emp) std::cout << "Found: " << emp->getName() << " (" << emp->getPosition() << ")" << std::endl;
    else std::cout << "Not found..." << std::endl;
}

void Dialog::findDept() {
    std::string deptName = readDepartmentName();
    Department* dept = company.findDepartment(deptName);
    if (dept) std::cout << "Found: Department " << dept->getName() << std::endl;
    else std::cout << "Not found..." << std::endl;
}

void Dialog::dismissEmployee() {
    int id = readEmployeeId();
    if (company.dismissEmployee(id)) std::cout << "Employee " << id << " dismissed." << std::endl;
    else std::cerr << "Dismissal failed. Employee not found." << std::endl;
}

void Dialog::setPos() {
    int id = readEmployeeId();
    IEmployee* emp = company.findEmployeeById(id);
    if (emp) {
        std::string newPosition = readPosition();
        emp->setPosition(newPosition);
        std::cout << "Position updated to '" << newPosition << "' for ID " << id << std::endl; 
    } else std::cout << "Employee not found..." << std::endl;
}

void Dialog::setSal() {
    int id = readEmployeeId();
    IEmployee* emp = company.findEmployeeById(id);
    if (emp) {
        std::cout << "Enter new salary: ";
        double newSalary = readval<double>(std::cin);
        emp->setSalary(newSalary);
        std::cout << "Salary updated to " << newSalary << " for ID " << id << std::endl;
    } else std::cout << "Employee not found..." << std::endl;
}

void Dialog::printFullInfo() {
    int id = readEmployeeId();
    IEmployee* emp = company.findEmployeeById(id);
    if (emp) {
        std::cout << "___ Employee Info ___" << std::endl;
        std::cout << *emp << std::endl;
    }
}

void Dialog::promote() {
    int id = readEmployeeId();
    std::string deptName = readDepartmentName();
    if (company.promoteEmployee(id, deptName)) {
        std::cout << "Employee " << id << " promoted to Manager and assigned to " << deptName << std::endl;
    } else std::cerr << "Promotion failed.(Check ID, Department, or if already Manager)" << std::endl;
}

void Dialog::transfer() {
    int id = readEmployeeId();
    std::string deptName = readDepartmentName();
    if (company.transferEmployee(id, deptName)) {
        std::cout << "Employee " << id << " transferred to " << deptName << std::endl;
    } else std::cerr << "Transfer failed (Employee not found or Department doesn't exist)." << std::endl;
}

void Dialog::showStructure() {
    company.showCompanyStructure();
}

void Dialog::showDeptStaff() {
    std::string deptName = readDepartmentName();
    Department* dept = company.findDepartment(deptName);
    if (dept) {
        std::cout << " ___ Staff of Department " << deptName << " ___" << std::endl;
        dept->showStaff();
        std::cout << "___ End of Staff List ___" << std::endl;
    } else std::cout << "Department not found..." << std::endl;
}

void Dialog::showRTTI() {
    int id = readEmployeeId();
    IEmployee* emp = company.findEmployeeById(id);
    if (emp) {
        std::cout << "--- RTTI Info ---\nEmployee ID " << id << ": " << emp->getName() << std::endl;
        std::cout << "Actual Type: " << typeid(*emp).name() << std::endl;
    } else std::cout << "Employee not found..." << std::endl;
}

void Dialog::showSubordinates() {
    int id = readEmployeeId();
    IEmployee* emp = company.findEmployeeById(id);
    Manager* mgr = dynamic_cast<Manager*>(emp);
    
    if (!mgr) {
        std::cerr << "Error: Employee ID " << id << " not found or is not a Manager." << std::endl;
        return;
    }

    std::cout << "Starting parallel search for: " << mgr->getName() << "..." << std::endl;

    UnorderedMap<int, IEmployee*> subordinates = company.getAllSubordinatesPar(id);

    std::cout << "--- Found Subordinates ---" << std::endl;

    bool found = false;
    for (auto& pair : subordinates) {
        if (pair.second != nullptr) {
            std::cout << "- [ID " << pair.first << "] " << pair.second->getName() 
                      << " (" << pair.second->getPosition() << ")" << std::endl;
            found = true;
        }
    }

    if (!found) {
        std::cout << "This manager has no subordinates." << std::endl;
    }
}

void Dialog::saveToFile() {
    std::string filename = readStringInput("Enter filename to SAVE to (e.g., data.yaml): ");
    if (company.saveToFile(filename)) std::cout << "Success! Data saved to " << filename << std::endl;
    else std::cerr << "Error: Could not save to file." << std::endl;
}

void Dialog::loadFromFile() {
    std::string filename = readStringInput("Enter filename to LOAD from (e.g., data.yaml): ");
    if (company.loadFromFile(filename)) {
        std::cout << "--- SUCCESS ---" << std::endl;
        int maxExistingId = company.getMaxEmployeeId();
        next_id = (maxExistingId >= 100) ? maxExistingId + 1 : 101;
        std::cout << "Company state loaded. Next available ID: " << next_id << std::endl;
    } else std::cerr << "Restore failed." << std::endl;
}

void Dialog::showCategorized() {
    std::cout << "\n>>> Categorized Staff Report <<<\n";
    company.showCategorizedStaff();
}