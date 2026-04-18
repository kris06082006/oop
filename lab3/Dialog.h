#pragma once
#include "Company.h"
#include "IEmployee.h"
#include <string>

/**
 * @class Dialog
 * @brief Класс для управления пользовательским интерфейсом через консоль.
 * * Данный класс реализует паттерн "Контроллер", связывая логику класса Company 
 * с командами пользователя. Предоставляет текстовое меню для управления сотрудниками.
 */
class Dialog {
private:
    Company company; ///< Объект компании, с которым работает интерфейс.
    int next_id;     ///< Счетчик для генерации уникальных ID новых сотрудников.

    /** @name Вспомогательные методы ввода */
    ///@{
    int readEmployeeId();
    std::string readStringInput(const std::string& prompt);
    std::string readDepartmentName();
    std::string readPosition();
    IEmployee* createNewEmployee(int next_id);
    ///@}

    /**
     * @brief Выводит на экран главное меню программы.
     */
    void printMenu();

    /** @name Обработчики пунктов меню */
    ///@{
    void addEmployee();    ///< Нанять нового сотрудника (Option 1).
    void addDept();        ///< Создать новый департамент (Option 2).
    void findEmployee();   ///< Поиск сотрудника по ID (Option 3).
    void findDept();       ///< Поиск департамента по названию (Option 4).
    void dismissEmployee();///< Уволить сотрудника (Option 5).
    void setPos();         ///< Изменить должность сотрудника (Option 6).
    void setSal();         ///< Изменить зарплату (Option 7).
    void printFullInfo();  ///< Вывести полную информацию о сотруднике (Option 8).
    void promote();        ///< Повысить сотрудника до менеджера (Option 9).
    void transfer();       ///< Перевести в другой отдел (Option 10).
    void showStructure();  ///< Показать иерархию компании (Option 11).
    void showDeptStaff();  ///< Показать персонал конкретного отдела (Option 12).
    void showRTTI();       ///< Информация о типе объекта через RTTI (Option 13).
    void showSubordinates(); ///< Список подчиненных менеджера (Option 14).
    void saveToFile();     ///< Сохранить состояние в YAML (Option 15).
    void loadFromFile();   ///< Загрузить состояние из YAML (Option 16).
    void showCategorized();///< Показать список, разделенный на менеджеров и рабочих (Option 17).
    ///@}

public:
    /**
     * @brief Конструктор по умолчанию.
     * Инициализирует компанию и устанавливает начальный ID для сотрудников.
     */
    Dialog();

    /**
     * @brief Запускает основной цикл обработки команд.
     * @return Код завершения программы (обычно 0).
     */
    int run();
};