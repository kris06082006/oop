/**
 * @file forecast.h
 * @brief Заголовочный файл, содержащий определение класса Daily_Weather_Forecast
 */

#ifndef FORECAST_H
#define FORECAST_H

#include <string>
#include <compare>
#include <iostream>
#include <ctime> 
#include <format>

/**
 * @brief Перечисление погодных явлений
 */
enum class Weather_Phenomenon{
    SUNNY,  ///< Солнечно
    CLOUDY, ///< Облачно
    RAIN,   ///< Дождь
    SNOW    ///< Снег
};

/**
 * @brief Класс для представления суточного прогноза погоды
 */

class Daily_Weather_Forecast{

private:
    time_t time_utc;                ///< Дата в формате UTC
    double temp_morning;            ///< Температура утром
    double temp_afternoon;          ///< Температура днем
    double temp_evening;            ///< Температура вечером
    Weather_Phenomenon phenomenon;  ///< Погодное явление
    double precipitation;           ///< Количество осадков
    
    /**
     * @brief Определяет погодное явление на основе осадков и температуры
     * @param precipitation Количество осадков
     * @param avg_temp Средняя температура
     * @return Соответствующее погодное явление
     */
    Weather_Phenomenon identify_Phenomenon(double precipitation, double avg_temp) const;

public:
    /**
     * @brief Конструктор по умолчанию
     */
    Daily_Weather_Forecast()
        : time_utc(0), temp_morning(0), temp_afternoon(0), 
        temp_evening(0), phenomenon(Weather_Phenomenon::SUNNY), 
        precipitation(0) {};

    /**
     * @brief Конструктор по дате, температуре, погодному явлению и количеству осадков
     * @param date Дата в формате UTC
     * @param morning_temp Температура утром
     * @param afternoon_temp Температура днем 
     * @param evening_temp Температура вечером
     * @param phen Погодное явление
     * @param precip Количество осадков
     * @throws std::invalid_argument При значениях температуры, выходящих за диапазон [-100; 60], или отрицательных осадках
     */
    Daily_Weather_Forecast(time_t date, double morning_temp, 
                                              double afternoon_temp, double evening_temp,
                                              Weather_Phenomenon phen, double precip)
    : time_utc(date), phenomenon(phen) {
    
        if (morning_temp < -273 || afternoon_temp < -273 || evening_temp < -273) {
            throw std::invalid_argument("Invalid temperature");
        }
        if (precip < 0) {
            throw std::invalid_argument("Invalid precipitation");
        }

        temp_morning = morning_temp;
        temp_afternoon = afternoon_temp; 
        temp_evening = evening_temp;
        precipitation = precip;
    }

    /**
     * @brief Конструктор по дате, температуре, количеству осадков и с автоматическим определением погодного явления
     * @param date Дата в формате UTC
     * @param morning_temp Температура утром
     * @param afternoon_temp Температура днем 
     * @param evening_temp Температура вечером
     * @param precip Количество осадков
     * @throws std::invalid_argument При значениях температуры, выходящих за диапазон [-100; 60], или отрицательных осадках
     */        
    Daily_Weather_Forecast(time_t date, double morning_temp, 
                                              double afternoon_temp, double evening_temp,
                                              double precip)
    : time_utc(date) {
    
        if (morning_temp < -273 || afternoon_temp < -273 || evening_temp < -273) {
            throw std::invalid_argument("Invalid temperature");
        }
        if (precip < 0) {
            throw std::invalid_argument("Invalid precipitation");
        }

        temp_morning = morning_temp;
        temp_afternoon = afternoon_temp; 
        temp_evening = evening_temp;
        precipitation = precip;

        double avg_temp = averageTemperature();
        phenomenon = identify_Phenomenon(precip, avg_temp);
    }

    /**
     * @brief Возвращает дату прогноза 
     * @return Дата в формате UTC
     */
    time_t getTimeUTC() const;

    /**
     * @brief Возвращает температуру утром
     * @return Температура утром в °C
     */
    double getTempMorning() const;

    /**
     * @brief Возвращает температуру днем
     * @return Температура днем в °C
     */
    double getTempAfternoon() const;

    /**
     * @brief Возвращает температуру вечером
     * @return Температура вечером в °C
     */
    double getTempEvening() const;

    /**
     * @brief Возвращает погодное явление
     * @return Погодное явление
     */
    Weather_Phenomenon getPhenomenon() const;

    /**
     * @brief Возвращает количество осадков 
     * @return Количество осадков в мм
     */
    double getPrecipitation() const;

    /**
     * @brief Устанавливает дату
     * @param time Дата в формате UTC
     */
    void setTimeUTC(time_t time);

    /**
     * @brief Устанавливает температуру утром
     * @param temp Температура утром
     * @throws std::invalid_argument При температуре вне диапазона [-100, 60]
     */
    void setTempMorning(double temp);

    /**
     * @brief Устанавливает температуру днем
     * @param temp Температура днем
     * @throws std::invalid_argument При температуре вне диапазона [-100, 60]
     */
    void setTempAfternoon(double temp);

    /**
     * @brief Устанавливает температуру вечером
     * @param temp Температура вечером
     * @throws std::invalid_argument При температуре вне диапазона [-100, 60]
     */
    void setTempEvening(double temp);

    /**
     * @brief Устанавливает погодное явление
     * @param phen Погодное явление
     */
    void setPhenomenon(Weather_Phenomenon phen);

    /**
     * @brief Устанавливает количество осадков
     * @param precip Количество осадков
     * @throws std::invalid_argument При отрицательном количестве осадков
     */
    void setPrecipitation(double precip);

    /**
     * @brief Рассчитывает среднюю температуру за день
     * @return Средняя температура в °C
     */
    double averageTemperature() const;

    /**
     * @brief Проверяет валидность прогноза
     * @return true если прогноз валиден, false в противном случае
     */
    bool isForecastValid() const;

    /**
     * @brief Оператор сложения двух прогнозов для одного дня
     * @param other Другой прогноз погоды
     * @return Ссылка на текущий объект
     * @throws std::invalid_argument Если прогнозы не для одного дня
     */
    Daily_Weather_Forecast& operator+=(const Daily_Weather_Forecast& other);
    
    /**
     * @brief Оператор сравнения для сортировки по дате
     * @param lhs Левый операнд сравнения
     * @param rhs Правый операнд сравнения  
     * @return Результат сравнения (меньше, равно, больше)
     */
    friend std::strong_ordering operator<=>(const Daily_Weather_Forecast& lhs, const Daily_Weather_Forecast& rhs);
    
    /**
     * @brief Оператор проверки на равенство двух прогнозов
     * @param lhs Первый прогноз для сравнения
     * @param rhs Второй прогноз для сравнения
     * @return true если даты прогнозов совпадают, false в противном случае
     */
    friend bool operator==(const Daily_Weather_Forecast& lhs, const Daily_Weather_Forecast& rhs);

    /**
     * @brief Оператор логического отрицания для проверки валидности прогноза
     * @return true если прогноз невалиден, false если валиден
     */
    bool operator!() const;

    /**
     * @brief Выводит прогноз в консоль
     * @brief Отображает все данные прогноза в форматированном виде
     */
    void print_Forecast(std::ostream& out) const;

    /**
     * @brief Осуществляет ввод данных прогноза с консоли
     * @brief Запрашивает у пользователя все необходимые данные для прогноза
     */
    void input_Forecast(std::istream& in);

    /**
     * @brief Преобразует погодное явление в строку
     * @param phen Погодное явление
     * @return Строковое представление погодного явления
     */
    static std::string phenomenonToString(Weather_Phenomenon phen);

    /**
     * @brief Оператор получения средней температуры за день
     * @return Средняя температура в °C (усреднение утренней, дневной и вечерней)
     */
    double operator~() const;

    /**
     * @brief Оператор ввода данных прогноза из потока
     * @param in Входной поток
     * @param forecast Прогноз для заполнения
     * @return Ссылка на входной поток
     * @throws std::invalid_argument При вводе некорректных данных
     * @throws std::runtime_error При ошибках потока ввода
     */
    friend std::istream& operator >> (std::istream& in, Daily_Weather_Forecast& forecast);
   
    /**
     * @brief Оператор вывода данных прогноза в поток
     * @param out Выходной поток  
     * @param forecast Прогноз для вывода
     * @return Ссылка на выходной поток
     */
    friend std::ostream& operator << (std::ostream& out, const Daily_Weather_Forecast& forecast);

};

#endif