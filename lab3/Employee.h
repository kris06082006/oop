#pragma once
#include "BaseEmployee.h"
#include <string>
#include <iostream>
#include <format>

/**
 * @class Employee
 * @brief Класс, представляющий рядового сотрудника компании
 * * Наследуется от BaseEmployee и реализует стандартное поведение для штатных единиц.
 */
class Employee : public BaseEmployee{
public:
    /**
     * @brief Конструктор класса Employee
     * @param id Уникальный идентификатор сотрудника
     * @param name ФИО сотрудника
     * @param birthYear Год рождения
     * @param position Должность
     * @param education Образование
     * @param salary Заработная плата
     */
    Employee(int id, const std::string& name, int birthYear, 
            const std::string position, const std::string& education, 
            double salary) :
            BaseEmployee(id, name, birthYear, position, education, salary) {}

    /**
     * @brief Вывод полной информации о сотруднике в консоль
     */
    void printInfo() const override;

    /**
     * @brief Сериализация данных сотрудника в текстовый поток
     * @param out Ссылка на поток вывода
     */
    void writeToStream(std::ostream& out) const override;

};