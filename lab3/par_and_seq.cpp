#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <string>
#include "Company.h"
#include "Employee.h"
#include "Manager.h"
#include "Department.h"

using namespace std;
using namespace std::chrono;

/**
 * @brief Функция для создания тестовой компании.
 */
void buildCompany(Company& comp, int levels, int sub_count) {
    Manager* boss = new Manager(0, "Boss", 1970, "CEO", "PhD", 10000);
    comp.addEmployee(boss); 
    
    Department* root = new Department("MainOffice");
    root->setManager(boss);
    
    comp.addDepartment(root);
    boss->setDepartment(root);

    vector<Department*> current_depts = {root};
    int last_id = 1;

    for (int i = 0; i < levels; ++i) {
        vector<Department*> next_depts;
        
        for (auto* parent : current_depts) {
            for (int j = 0; j < sub_count; ++j) {
                Manager* mgr = new Manager(last_id++, "Mgr", 1980, "Head", "High", 5000);
                comp.addEmployee(mgr);
                
                Department* sub = new Department("Dept_" + std::to_string(last_id));
                sub->setManager(mgr);
                
          //    comp.addDepartment(sub);
                mgr->setDepartment(sub);
                
                parent->addSubDepartment(sub);
                next_depts.push_back(sub);
                
                for(int e = 0; e < 2; ++e) {
                    Employee* worker = new Employee(last_id++, "Worker", 1995, "Staff", "High", 2000);
                    comp.addEmployee(worker);     
                    sub->addEmployee(worker);     
                }
            }
        }
        current_depts = next_depts;
    }
}

void test_speed(int levels, int subs) {
    Company comp;
    buildCompany(comp, levels, subs);

    auto start1 = steady_clock::now();
    auto result1 = comp.getAllSubordinatesSeq(0);
    auto end1 = steady_clock::now();
    
    auto time_seq = duration_cast<microseconds>(end1 - start1).count();

    auto start2 = steady_clock::now();
    auto result2 = comp.getAllSubordinatesPar(0);
    auto end2 = steady_clock::now();
    
    auto time_par = duration_cast<microseconds>(end2 - start2).count();

    cout << setw(10) << result1.size() << " | " 
         << setw(15) << time_seq << " | " 
         << setw(15) << time_par << " мкс" << endl;
}

int main() {
    setlocale(LC_ALL, "ru");

    cout << "Сравнение скорости" << endl;
    cout << "--------------------------------------------------------" << endl;
    cout << setw(10) << "Человек" << " | " 
         << setw(15) << "Один поток" << " | " 
         << setw(15) << "Много потоков" << endl;
    cout << "--------------------------------------------------------" << endl;

    test_speed(1, 1);
    test_speed(2, 2);
    test_speed(3, 2);
    test_speed(4, 2);
    test_speed(4, 3);
    test_speed(5, 3);
    test_speed(6, 3);
    test_speed(5, 4);
    test_speed(6, 4);
    test_speed(7, 4);


    cout << "--------------------------------------------------------" << endl;
    return 0;
}