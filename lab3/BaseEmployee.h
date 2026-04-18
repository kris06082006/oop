#pragma once
#include "IEmployee.h"
#include <string>
#include <format>

/**
 * @class BaseEmployee
 * @brief Базовая реализация интерфейса IEmployee
 * * Содержит общие поля и методы для всех типов сотрудников компании.
 * * Является абстрактным классом, так как методы printInfo и writeToStream остаются чисто виртуальными.
 */
class BaseEmployee : public IEmployee {
protected:
    int id = 0; ///< Уникальный идентификатор сотрудника
    std::string name; ///< Полное имя (ФИО) сотрудника
    int birthYear = 0; ///< Год рождения сотрудника
    std::string position; ///< Текущая занимаемая должность
    std::string education; ///< Сведения об образовании
    double salary = 0.0; ///< Текущий уровень заработной платы

public:
    /**
     * @brief Конструктор базового класса сотрудника
     * @param id_arg Уникальный идентификатор
     * @param name_arg ФИО сотрудника
     * @param birthYear_arg Год рождения
     * @param position_arg Должность
     * @param education_arg Образование
     * @param salary_arg Заработная плата
     */
    BaseEmployee(int id_arg, const std::string& name_arg, int birthYear_arg, 
                const std::string& position_arg, const std::string& education_arg,
                double salary_arg) :
                id(id_arg), name(name_arg), birthYear(birthYear_arg), position(position_arg),
                education(education_arg), salary(salary_arg) {}

    /** @brief Получение уникального идентификатора сотрудника */
    int getId() const override { return id; }

    /** @brief Получение имени сотрудника */
    std::string getName() const override { return name; }

    /** @brief Получение года рождения сотрудника */
    int getBirthYear() const override { return birthYear; }

    /** @brief Получение информации об образовании */
    std::string getEducation() const override { return education; }

    /** @brief Получение названия текущей должности */
    std::string getPosition() const override { return position; }

    /** @brief Изменение названия должности сотрудника */
    void setPosition(const std::string& newPosition) override { position = newPosition; }

    /** @brief Получение значения заработной платы */
    double getSalary() const override { return salary; }

    /** @brief Изменение значения заработной платы */
    void setSalary(double newSalary) override { salary = newSalary; }

    /** @brief Чисто виртуальный метод вывода информации (реализуется в наследниках) */
    void printInfo() const override = 0;

    /** @brief Чисто виртуальный метод сериализации в поток (реализуется в наследниках) */
    void writeToStream(std::ostream& out) const override = 0;

    /** @brief Виртуальный деструктор по умолчанию */
    virtual ~BaseEmployee() = default;
    
};

/**
 * @brief Перегрузка оператора вывода в поток для объектов IEmployee
 * @param out Ссылка на поток вывода
 * @param employee Ссылка на интерфейс сотрудника
 * @return Ссылка на поток вывода
 */
inline std::ostream& operator<<(std::ostream& out, const IEmployee& employee) {
    employee.writeToStream(out);
    return out;
}