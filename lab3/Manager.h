#pragma once
#include "BaseEmployee.h"
#include <string>
#include <iostream>
#include <format>

class Department;

/**
 * @class Manager
 * @brief Класс, представляющий руководителя подразделения
 * * Наследуется от BaseEmployee и расширяет его функционал связью с конкретным отделом.
 */
class Manager : public BaseEmployee{
private:
    Department* department; ///< Указатель на отдел, которым управляет менеджер
public:
    /**
     * @brief Конструктор класса Manager
     * @param id Уникальный идентификатор сотрудника
     * @param name ФИО сотрудника
     * @param birthYear Год рождения
     * @param position Должность
     * @param education Образование
     * @param salary Заработная плата
     * @param dept Указатель на управляемый отдел (по умолчанию nullptr)
     */
    Manager(int id, const std::string& name, int birthYear,
                const std::string& position, const std::string& education, 
                double salary, Department* dept = nullptr): 
                BaseEmployee(id, name, birthYear, position, education, salary), 
                department(dept) {}

    /**
     * @brief Получение указателя на управляемый отдел
     * @return Указатель на объект Department
     */
    Department* getDepartment() const { return department; }

    /**
     * @brief Установка (назначение) отдела для управления
     * @param dept Указатель на объект Department
     */
    void setDepartment(Department* dept) { department = dept; }

    /**
     * @brief Вывод полной информации о менеджере в консоль
     */
    void printInfo() const override;

    /**
     * @brief Сериализация данных менеджера в поток
     * @param out Поток для записи данных
     */
    void writeToStream(std::ostream& out) const override;
};