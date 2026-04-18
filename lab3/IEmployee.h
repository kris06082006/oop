#pragma once
#include <string>

/**
 * @class IEmployee
 * @brief Абстрактный интерфейс для всех типов сотрудников компании
 * * Определяет обязательный набор методов для работы с персональными данными,
 * должностными характеристиками и финансовой информацией.
 */
class IEmployee {
public:
    /**
     * @brief Получение уникального идентификатора сотрудника
     * @return Целочисленный ID
     */
    virtual int getId() const = 0;

    /**
     * @brief Получение ФИО сотрудника
     * @return Строка с полным именем
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Получение года рождения сотрудника
     * @return Год рождения (гггг)
     */
    virtual int getBirthYear() const = 0;

    /**
     * @brief Получение сведений об образовании
     * @return Строка с описанием образования
     */
    virtual std::string getEducation() const = 0;

    /**
     * @brief Получение текущей должности
     * @return Название должности
     */
    virtual std::string getPosition() const = 0;

    /**
     * @brief Обновление названия должности
     * @param newPosition Новое название должности
     */
    virtual void setPosition(const std::string& newPosition) = 0;

    /**
     * @brief Получение текущей заработной платы
     * @return Сумма заработной платы
     */
    virtual double getSalary() const = 0;

    /**
     * @brief Установка нового значения заработной платы
     * @param newSalary Новая сумма зарплаты
     */
    virtual void setSalary(double newSalary) = 0;

    /**
     * @brief Вывод полной информации о сотруднике в консоль
     */
    virtual void printInfo() const = 0;

    /**
     * @brief Сериализация данных объекта в текстовый поток
     * @param out Ссылка на поток вывода
     */
    virtual void writeToStream(std::ostream& out) const = 0;

    /**
     * @brief Виртуальный деструктор для корректного удаления объектов через указатель на интерфейс
     */
    virtual ~IEmployee() = default;

};