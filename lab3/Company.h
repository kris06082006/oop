#pragma once
#include "Department.h"
#include "IEmployee.h"
#include "Manager.h"
#include <vector>
#include "unordered_map.h"
#include <yaml-cpp/yaml.h>
#include <utility>
#include <string>
#include <map>
#include <format>

/**
 * @class Company
 * @brief Класс для управления организационной структурой компании
 */
class Company {
private:
    std::string name; ///< Название компании
    std::vector<Department*> departments; ///< Список корневых отделов компании
    UnorderedMap<int, IEmployee*> globalEmployees; ///< Общий реестр всех сотрудников компании по ID

    /**
     * @brief Рекурсивный поиск отдела по его названию
     * @param deptName Название искомого отдела
     * @param startDept Отдел, с которого начинается поиск (корень поддерева)
     * @return Указатель на найденный отдел или nullptr
     */
    Department* findDepartmentRecursive(const std::string& deptName, Department* startDept) const;

    /**
     * @brief Поиск отдела, за которым закреплен конкретный менеджер
     * @param manager Указатель на объект менеджера
     * @param startDept Отдел, с которого начинается поиск
     * @return Указатель на отдел, где данный менеджер является руководителем
     */
    Department* findDepartmentByManager(Manager* manager, Department* startDept);

    /**
    * @brief Очищает все динамически выделенные ресурсы (сотрудников и отделы).
    */
    void clear();

    /**
     * @brief Сериализация иерархии отдела в формат YAML
     * @param out Объект эмиттера YAML для записи
     * @param dept Указатель на сериализуемый отдел
     */
    void serializeDepartment(YAML::Emitter& out, Department* dept) const;

    /**
     * @brief Десериализация отдела и его структуры из YAML
     * @param node Узел YAML, содержащий данные отдела
     * @param tempEmps Карта загруженных сотрудников для восстановления связей
     * @return Указатель на воссозданный объект отдела
     */
    Department* deserializeDepartment(const YAML::Node& node, UnorderedMap<int, IEmployee*>& tempEmps);

    /**
     * @brief Рекурсивный поиск подчиненных для однопоточного режима.
     * @param dept Текущее подразделение для обхода.
     * @param result Таблица для записи найденных сотрудников.
     */
    void getSubRecursiveSeq(Department* dept, UnorderedMap<int, IEmployee*>& result);

    /**
     * @brief Метод для быстрого добавления сотрудника в общую таблицу сотрудников.
     */
    void addEmployee(IEmployee* emp) {
        if (emp) globalEmployees[emp->getId()] = emp;
    }
    
public:
    /**
     * @brief Конструктор по умолчанию
     */
    Company() : name("") {}

    /**
     * @brief Конструктор компании с заданным именем
     * @param companyName Название компании
     */
    Company(const std::string& companyName) : name(companyName) {}

    /**
     * @brief Получение названия компании
     * @return Строка с названием компании
     */
    std::string getName() const { return name; }

    /**
     * @brief Добавление нового корневого отдела в компанию
     * @param dept Указатель на новый отдел
     */
    void addDepartment(Department* dept);

    /**
     * @brief Поиск корневого отдела по имени
     * @param deptName Название отдела
     * @return Указатель на отдел или nullptr
     */
    Department* findDepartment(const std::string& deptName) const;

    /**
     * @brief Поиск сотрудника в глобальном реестре по ID
     * @param id Уникальный идентификатор сотрудника
     * @return Указатель на сотрудника или nullptr
     */
    IEmployee* findEmployeeById(int id);

    /**
     * @brief Наем сотрудника в конкретный отдел
     * @param depName Название отдела для приема на работу
     * @param employee Указатель на объект нового сотрудника
     * @return true если наем прошел успешно, иначе false
     */
    bool hireEmployee(const std::string& depName, IEmployee* employee);

    /**
     * @brief Перевод сотрудника в другой отдел
     * @param employeeId ID переводимого сотрудника
     * @param toDeptName Название целевого отдела
     * @return true если перевод выполнен, false если сотрудник или отдел не найдены
     */
    bool transferEmployee(int employeeId, const std::string& toDeptName);

    /**
     * @brief Увольнение сотрудника из компании
     * @param employeeId ID сотрудника
     * @return true если сотрудник найден и удален, иначе false
     */
    bool dismissEmployee(int employeeId);

    /**
     * @brief Отображение полной древовидной структуры компании
     */
    void showCompanyStructure() const;

    /**
     * @brief Вывод информации о персонале конкретного отдела
     * @param deptName Название отдела
     */
    void showDepartmentStaff(const std::string& deptName);

    /**
     * @brief Вспомогательный метод для рекурсивной печати отдела
     * @param dept Указатель на текущий отдел
     * @param level Текущий уровень вложенности для формирования отступов
     */
    void printDepartment(Department* dept, int level) const;

    /**
     * @brief Получение списка всех подчиненных для данного менеджера
     * @param manager Указатель на менеджера
     * @return Вектор указателей на сотрудников в ведении менеджера
     */
    std::vector<IEmployee*> getSubordinates(Manager* manager);

    /**
     * @brief Повышение сотрудника до должности менеджера в указанном отделе
     * @param id ID сотрудника
     * @param newDeptName Название отдела, где он станет менеджером
     * @return true если повышение успешно, иначе false
     */
    bool promoteEmployee(int id, const std::string& newDeptName);

    /**
     * @brief Сохранение данных всей компании в файл
     * @param filename Путь к файлу сохранения
     * @return true если сохранение успешно, иначе false
     */
    bool saveToFile(const std::string& filename) const;

    /**
     * @brief Внутренняя логика парсинга данных при загрузке
     * @param filename Путь к файлу
     * @return true если данные валидны и считаны, иначе false
     */
    bool internalLoadLogic(const std::string& filename);

    /**
     * @brief Загрузка состояния компании из файла (с полной очисткой текущих данных)
     * @param filename Путь к файлу
     * @return true если загрузка прошла успешно, иначе false
     */
    bool loadFromFile(const std::string& filename);

    /**
     * @brief Получение самого большого значения ID среди всех сотрудников
     * @return Целочисленное значение максимального ID
     */
    int getMaxEmployeeId() const;

    /**
     * @brief Рекурсивная проверка существования отдела с данным названием
     * @param name Название отдела
     * @return true если отдел существует в структуре, иначе false
     */
    bool departmentExistsRecursive(const std::string& name);

    /**
     * @brief Отображение списка сотрудников, сгруппированных по категориям (Менеджеры/Обычные)
     */
    void showCategorizedStaff() const;

    /**
     * @brief Рекурсивный поиск подчиненных в многопоточном режиме.
     * @param dept Текущее подразделение для обхода.
     * @return Вектор указателей на найденных сотрудников.
     */
    std::vector<IEmployee*> getSubRecursivePar(Department* dept);

    /**
     * @brief Метод для поиска всех подчиненных (прямых и косвенных) в однопоточном режиме.
     * @param managerId ID руководителя, чьих подчиненных нужно найти.
     * @return Таблица UnorderedMap с объектами сотрудников.
     */
    UnorderedMap<int, IEmployee*> getAllSubordinatesSeq(int managerId);

    /**
     * @brief Метод для поиска всех подчиненных (прямых и косвенных) в многопоточном режиме 
     * @param managerId ID руководителя.
     * @return Таблица UnorderedMap с объектами сотрудников.
     */
    UnorderedMap<int, IEmployee*> getAllSubordinatesPar(int managerId);

    /**
     * @brief Функция для создания тестовой компании.
     * @param comp Ссылка на компанию.
     * @param levels Количество уровней вложенности (глубина дерева).
     * @param sub_count Сколько подотделов создавать в каждом отделе.
     */
    friend void buildCompany(Company& comp, int levels, int sub_count);
  
    /**
     * @brief Деструктор класса Company
     */
    ~Company();
};