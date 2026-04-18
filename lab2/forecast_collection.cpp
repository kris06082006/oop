#include "forecast_collection.h"
#include "forecast.h"
#include <stdexcept>
#include <compare>
#include <format>
#include <algorithm>

Forecast_Collection::Forecast_Collection(const Forecast_Collection& other) {
    current_size = other.current_size;
    capacity = other.capacity;

    if (other.forecasts != nullptr && other.current_size > 0) {
        forecasts = new Daily_Weather_Forecast[other.capacity];

        std::copy(other.forecasts, other.forecasts + other.current_size, forecasts);
    }
    else {
        forecasts = nullptr;
    }
}

Forecast_Collection& Forecast_Collection::operator=(const Forecast_Collection& other){
    if (this == &other)
        return *this;

    Daily_Weather_Forecast* new_forecasts = nullptr;
    if (other.current_size != 0) {
        new_forecasts = new Daily_Weather_Forecast[other.current_size];
        std::copy(other.forecasts, other.forecasts + other.current_size, new_forecasts);
    }

    delete [] forecasts;
    forecasts = new_forecasts;
    current_size = other.current_size;
    capacity = other.capacity;
    return *this;
}

Forecast_Collection::Forecast_Collection(Forecast_Collection&& other) noexcept
    : forecasts(other.forecasts), current_size(other.current_size), capacity(other.capacity) {
        other.forecasts = nullptr;
        other.current_size = 0;
        other.capacity = 0;
}


Forecast_Collection& Forecast_Collection::operator=(Forecast_Collection&& other) noexcept{
    delete[] forecasts;
    current_size = other.current_size;
    capacity = other.capacity;
    forecasts = other.forecasts;
    other.current_size = 0;
    other.capacity = 0;
    other.forecasts = nullptr;
    return *this;
}

Forecast_Collection& Forecast_Collection::operator+=(const Daily_Weather_Forecast& new_forecast) {
    if (current_size < capacity) {
        forecasts[current_size] = new_forecast;
        current_size++;
    }
    else {
        size_t new_capacity = (capacity == 0) ? 1 : capacity * 2;
        resize(new_capacity);

        forecasts[current_size++] = new_forecast;
        capacity = new_capacity;
    }

    return *this;
}

size_t Forecast_Collection::getSize() const { return current_size; }

size_t Forecast_Collection::getCapacity() const { return capacity; }

bool Forecast_Collection::empty() const {return current_size == 0; }




Daily_Weather_Forecast& Forecast_Collection::operator[] (size_t index) {
    if (index >= current_size) {
        throw std::out_of_range("Index out of range for collection of size");
    }

    return forecasts[index];
}

const Daily_Weather_Forecast& Forecast_Collection::operator[] (size_t index) const {
    if (index >= current_size) {
        throw std::out_of_range("Index out of range for collection of size");
    }

    return forecasts[index];
}

void Forecast_Collection::resize(size_t new_capacity) {
    if (new_capacity <= capacity) {
        return;
    }

    Daily_Weather_Forecast *new_forecasts = new Daily_Weather_Forecast[new_capacity];

    std::copy(forecasts, forecasts + current_size, new_forecasts);

    delete [] forecasts;
    forecasts = new_forecasts;
    capacity = new_capacity;

}

void Forecast_Collection::remove_forecast(size_t index) {
    if (index >= current_size){
        throw std::out_of_range("Index out of range");
    }

    std::copy(forecasts + index + 1, forecasts + current_size, forecasts + index);

    current_size--;
}

std::ostream& operator<<(std::ostream& out, const Forecast_Collection& collection) {
    out << std::format("___Forecast Collection___\n"
                "Size: {}, Capacity: {}\n" "Forecasts:\n", collection.current_size, collection.capacity);

    for (size_t i = 0; i < collection.current_size; i++) {
        out << "[" << i << "] " << collection.forecasts[i] << "\n";
    }

    return out;
}

std::istream& operator>>(std::istream& in, Forecast_Collection& collection) {
    collection.current_size = 0;
    
    size_t count;
    in >> count;
    
    if (count == 0) {
        return in;
    }
    
    if (count > collection.capacity) {
        collection.resize(count);
    }
    
    for (size_t i = 0; i < count; ++i) {        
        Daily_Weather_Forecast forecast;
        in >> forecast;  
        
        collection.forecasts[collection.current_size] = forecast;
        collection.current_size++;
    }
    
    return in;
}

int Forecast_Collection::find_coldest_day(time_t start, time_t end) const{
    if (empty()) {return -1;}

    int coldest_index = -1;
    double min_avg_temp = 100.0;

    for (size_t i = 0; i < current_size; i++) {
        time_t forecast_time = forecasts[i].getTimeUTC();

        if (forecast_time >= start && forecast_time <= end){
            double avg_temp = forecasts[i].averageTemperature();
            if (avg_temp < min_avg_temp) {
                min_avg_temp = avg_temp;
                coldest_index = static_cast<int>(i);
            }
        }
    }
    return coldest_index;
}

int Forecast_Collection::find_nearest_sunny_day(time_t current_date) const {
    if (empty()){
        return -1;
    }
    int nearest_index = -1;
    for (size_t i = 0; i < current_size; i++){
        if (forecasts[i].getPhenomenon() == Weather_Phenomenon::SUNNY){
            if (nearest_index == -1 || std::abs(forecasts[i].getTimeUTC() - current_date) < std::abs(forecasts[nearest_index].getTimeUTC() - current_date)) {
                nearest_index = static_cast<int>(i);
            }
        }
    }

    return nearest_index;
}

void Forecast_Collection::remove_invalid_forecasts(){
    auto new_end = std::copy_if(forecasts, forecasts + current_size, forecasts, 
                            [](const Daily_Weather_Forecast& forecast) {
                                return forecast.isForecastValid();
                            });

    current_size = new_end - forecasts;
}

void Forecast_Collection::merge_duplicate_days() {
    for (size_t i = 0; i < current_size; i++){
        for (size_t j = i + 1; j < current_size; j++){
            if (forecasts[i].getTimeUTC() == forecasts[j].getTimeUTC()){
                forecasts[i] += forecasts[j];
                remove_forecast(j);
                j--;

            }
        }
    }
}

void Forecast_Collection::sort_by_date(){
    std::sort(forecasts, forecasts + current_size);
}

Forecast_Collection Forecast_Collection::get_monthly_forecast(int month, int year) const {
    Forecast_Collection monthly_forecast;

    for (size_t i = 0; i < current_size; i++){
        if (is_date_in_month(forecasts[i].getTimeUTC(), month, year)){ 
            monthly_forecast += forecasts[i];
        }
    }

    monthly_forecast.sort_by_date();

    return monthly_forecast;
}

bool Forecast_Collection::is_date_in_month(time_t date, int month, int year) const {
    std::tm* time_info = std::gmtime(&date); 
    if (time_info == nullptr) {
        return false;
    }
    
    int date_month = time_info->tm_mon + 1;  
    int date_year = time_info->tm_year + 1900; 
    
    return (date_month == month) && (date_year == year);
}