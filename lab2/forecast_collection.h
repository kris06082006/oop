/**
 * @brief Класс для работы с коллекцией прогнозов погоды
 */
#ifndef FORECAST_COLLECTION_H
#define FORECAST_COLLECTION_H

#include "forecast.h"
#include <format>
#include <stdexcept>

class Forecast_Collection {
private:
    Daily_Weather_Forecast* forecasts;  ///< Динамический массив прогнозов
    size_t current_size;                ///< Текущее количество прогнозов
    size_t capacity;                    ///< Вместимость массива

    /**
     * @brief Изменяет вместимость массива
     * @param new_capacity Новая вместимость
     */
    void resize(size_t new_capacity);

    /**
     * @brief Проверяет, принадлежит ли дата указанному месяцу и году
     * @param date Проверяемая дата
     * @param month Месяц (1-12)
     * @param year Год
     * @return true если дата принадлежит указанному месяцу и году, false в противном случае
     */
    bool is_date_in_month(time_t date, int month, int year) const;

public:
    /**
     * @brief Конструктор по умолчанию
     * Создает пустую коллекцию
     */
    Forecast_Collection() : forecasts(nullptr), current_size(0), capacity(0) {} 

    /**
     * @brief Конструктор из массива прогнозов
     * @param forecasts_array Массив прогнозов для копирования
     * @param count Количество прогнозов в массиве
     * @throws std::invalid_argument Если передан нулевой указатель при ненулевом количестве
     */
    explicit Forecast_Collection(const Daily_Weather_Forecast* forecasts_array, size_t count) {
        if (forecasts_array == nullptr && count > 0) {
            throw std::invalid_argument("Null point with non-zero count");
        }

        if (count == 0){
            forecasts = nullptr;
            current_size = 0;
            capacity = 0;
        }
        else {
            forecasts = new Daily_Weather_Forecast[count];
            current_size = count;
            capacity = count;
        
            for(size_t i = 0; i < count; i++) {
                forecasts[i] = forecasts_array[i];
            }
        }
    }

    /**
     * @brief Конструктор из одного прогноза
     * @param forecast Прогноз для добавления в коллекцию
     */
    explicit Forecast_Collection(const Daily_Weather_Forecast& forecast)
    : current_size(1), capacity(1){

        forecasts = new Daily_Weather_Forecast[1];
        forecasts[0] = forecast;
    }

    /**
     * @brief Конструктор копирования
     * @param other Коллекция для копирования
     */
    Forecast_Collection(const Forecast_Collection& other);

    /**
     * @brief Копирующий оператор присваивания
     * @param other Коллекция для копирования
     * @return Ссылка на текущую коллекцию
     */
    Forecast_Collection& operator=(const Forecast_Collection& other);

    /**
     * @brief Конструктор перемещения
     * @param other Коллекция для перемещения
     */
    Forecast_Collection(Forecast_Collection &&other) noexcept;

    /**
     * @brief Перемещающий оператор присваивания
     * @param other Коллекция для перемещения
     * @return Ссылка на текущую коллекцию
     */
    Forecast_Collection& operator=(Forecast_Collection &&other) noexcept;

    /**
     * @brief Деструктор
     * Освобождает память, занятую массивом прогнозов
     */
    ~Forecast_Collection() {
        delete[] forecasts;
    }

    /**
     * @brief Возвращает текущее количество прогнозов в коллекции
     * @return Количество прогнозов
     */
    size_t getSize() const;

    /**
     * @brief Возвращает текущую вместимость коллекции
     * @return Вместимость массива
     */
    size_t getCapacity() const;

    /**
     * @brief Проверяет, пуста ли коллекция
     * @return true если коллекция пуста, false в противном случае
     */
    bool empty() const;

    /**
     * @brief Добавляет прогноз в коллекцию
     * @param new_forecast Прогноз для добавления
     * @return Ссылка на текущую коллекцию
     */
    Forecast_Collection& operator+=(const Daily_Weather_Forecast& new_forecast);

    /**
     * @brief Оператор доступа к прогнозу по индексу (неконстантная версия)
     * @param index Индекс прогноза
     * @return Ссылка на прогноз
     * @throws std::out_of_range Если индекс выходит за границы
     */
    Daily_Weather_Forecast& operator[] (size_t index);

    /**
     * @brief Оператор доступа к прогнозу по индексу (константная версия)
     * @param index Индекс прогноза
     * @return Константная ссылка на прогноз
     * @throws std::out_of_range Если индекс выходит за границы
     */
    const Daily_Weather_Forecast& operator[] (size_t index) const;

    /**
     * @brief Удаляет суточный прогноз по его номеру
     * @param index Индекс прогноза для удаления
     * @throw std::out_of_range Усли индекс выходит за границы
     */
    void remove_forecast(size_t index);

    /**
     * @brief Оператор вывода коллекции в поток
     * @param out Выходной поток
     * @param collection Коллекция для вывода
     * @return Ссылка на выходной поток
     */
    friend std::ostream& operator<<(std::ostream& out, const Forecast_Collection&);

    /**
     * @brief Оператор ввода коллекции из потока
     * @param in Входной поток
     * @param collection Коллекция для заполнения
     * @return Ссылка на входной поток
     */
    friend std::istream& operator>>(std::istream& in, Forecast_Collection&);

    /**
     * @brief Поиск самого холодного дня за указанный промежуток
     * @param start Начальная дата (UTC)
     * @param end Конечная дата (UTC)
     * @return Индекс самого холодного дня или -1 если не найден
     */
    int find_coldest_day(time_t start, time_t end) const;

    /**
     * @brief Поиск ближайшего солнечного дня для текущей даты
     * @param current_date Текущая дата (UTC)
     * @return Индекс ближайшего солнечного дня или -1 если не найден
     */
    int find_nearest_sunny_day(time_t current_date) const;

    /**
     * @brief Удаляет все невалидные прогнозы из коллекции
     */
    void remove_invalid_forecasts();

    /**
     * @brief Объединяет дублирующиеся прогнозы за одни сутки
     */
    void merge_duplicate_days();

    /**
     * @brief Упорядочивает прогнозы по дате
     */
    void sort_by_date();

    /**
     * @brief Возвращает подмножество прогнозов за указанный месяц
     * @param month Месяц (1-12)
     * @param year Год
     * @return Новая коллекция прогнозов за указанный месяц
     */
    Forecast_Collection get_monthly_forecast(int month, int years) const;

};

#endif