#pragma once
#include "IEmployee.h"
#include "unordered_map.h"
#include <vector>
#include <string>
#include <format>

class Manager;

/**
 * @class Department
 * @brief Класс, представляющий отдел компании и управляющий его сотрудниками и подотделами
 */
class Department {
private:
    std::string name; ///< Название отдела
    UnorderedMap<int, IEmployee*> employees; ///< Коллекция сотрудников отдела (ID -> Указатель)
    std::vector<Department*> subDepartments; ///< Список дочерних отделов
    Manager* manager = nullptr; ///< Указатель на руководителя отдела
public:
    /**
     * @brief Конструктор отдела с инициализацией имени
     * @param deptName Название отдела
     */
    Department(const std::string& deptName) :
                name(deptName), manager(nullptr) {};
    
    /**
     * @brief Добавление сотрудника в текущий отдел
     * @param employee Указатель на объект сотрудника
     */
    void addEmployee(IEmployee* employee); 

    /**
     * @brief Удаление сотрудника из отдела по его идентификатору
     * @param id Уникальный номер сотрудника
     * @return true если сотрудник был найден и удален, иначе false
     */
    bool removeEmployee(int id);

    /**
     * @brief Поиск сотрудника в локальном реестре отдела
     * @param id Уникальный номер сотрудника
     * @return Указатель на сотрудника или nullptr, если он не числится в отделе
     */
    IEmployee* findEmployeeById(int id);

    /**
     * @brief Назначение руководителя (менеджера) отдела
     * @param mgr Указатель на объект менеджера
     */
    void setManager(Manager* mgr); 

    /**
     * @brief Добавление вложенного подразделения
     * @param subDept Указатель на объект подотдела
     */
    void addSubDepartment(Department* subDept); 

    /**
     * @brief Рекурсивная очистка ссылок на сотрудника в дереве департаментов.
     * @param id ID увольняемого сотрудника.
     */
    void cleanRecursive(int id);

    /**
     * @brief Поиск подотдела первого уровня по названию
     * @param deptName Название искомого подотдела
     * @return Указатель на подотдел или nullptr
     */
    Department* findSubDepartment(const std::string& deptName); 
    
    /** @brief Получение названия отдела */
    std::string getName() const { return name; }; 

    /** @brief Получение указателя на текущего руководителя */
    Manager* getManager() const { return manager; };

    /** @brief Получение доступа к списку сотрудников отдела */
    const UnorderedMap<int, IEmployee*>& getEmployees() const { return employees; };

    /** @brief Получение списка всех дочерних подразделений */
    const std::vector<Department*>& getSubDepartments() const { return subDepartments; };
    
    /** @brief Вывод информации о персонале отдела в консоль */
    void showStaff() const;

    /**
     * @brief Рекурсивный сбор всех сотрудников отдела и всех его подотделов
     * @return Вектор указателей на всех найденных сотрудников
     */
    std::vector<IEmployee*> getAllEmployeesRecursive() const;

    /**
     * @brief Отображение иерархической структуры отдела
     * @param level Текущий уровень вложенности для формирования отступов
     */
    void showStructure(int level = 0) const;

    /**
     * @brief Поиск конкретного подразделения, в котором находится сотрудник
     * @param employeeId Идентификатор искомого сотрудника
     * @return Указатель на отдел, содержащий сотрудника, или nullptr
     */
    Department* findDepartmentContainingEmployee(int employeeId);

    /**
     * @brief Запись данных об отделе в поток (формат DEPT|...)
     * @param out Поток для записи данных
     * @param parentName Имя родительского отдела для сохранения связей
     */
    void writeToStream(std::ostream& out, const std::string& parentName) const;
    
    /** @brief Деструктор класса Department (выполняет рекурсивное удаление подотделов) */
    ~Department();

};