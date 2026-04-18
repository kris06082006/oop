#include "forecast.h"
#include "io.h"
#include <stdexcept>
#include <compare>
#include <format>

time_t Daily_Weather_Forecast::getTimeUTC() const { return time_utc; }
double Daily_Weather_Forecast::getTempMorning() const { return temp_morning; }
double Daily_Weather_Forecast::getTempAfternoon() const { return temp_afternoon; }
double Daily_Weather_Forecast::getTempEvening() const { return temp_evening; }
Weather_Phenomenon Daily_Weather_Forecast::getPhenomenon() const { return phenomenon; }
double Daily_Weather_Forecast::getPrecipitation() const { return precipitation; }

void Daily_Weather_Forecast::setTimeUTC(time_t time) { time_utc = time; }

void Daily_Weather_Forecast::setTempMorning(double temp) {
    if (temp < -100 || temp > 60) {
        throw std::invalid_argument("Invalid morning temperature");
    }
    temp_morning = temp;
}

void Daily_Weather_Forecast::setTempAfternoon(double temp) {
    if (temp < -100 || temp > 60) {
        throw std::invalid_argument("Invalid afternoon temperature");
    }
    temp_afternoon = temp;
}

void Daily_Weather_Forecast::setTempEvening(double temp) {
    if (temp < -100 || temp > 60) {
        throw std::invalid_argument("Invalid evening temperature");
    }
    temp_evening = temp;
}

void Daily_Weather_Forecast::setPhenomenon(Weather_Phenomenon phen) {
    phenomenon = phen;
}

void Daily_Weather_Forecast::setPrecipitation(double precip) {
    if (precip < 0 || precip > 1500) {
        throw std::invalid_argument("Invalid precipitation");
    }
    precipitation = precip;
}

Weather_Phenomenon Daily_Weather_Forecast::identify_Phenomenon(double precipitation, double avg_temp) const {
    if (precipitation == 0) {
        return avg_temp > 0 ? Weather_Phenomenon::SUNNY : Weather_Phenomenon::CLOUDY;
    }
    return avg_temp > 0 ? Weather_Phenomenon::RAIN : Weather_Phenomenon::SNOW;
}

double Daily_Weather_Forecast::averageTemperature() const {
    return (temp_morning + temp_afternoon + temp_evening) / 3.0;
}

double Daily_Weather_Forecast::operator~() const{
    return averageTemperature();
}


Daily_Weather_Forecast& Daily_Weather_Forecast::operator+=(const Daily_Weather_Forecast& other) {
    if (time_utc != other.time_utc) {
        throw std::invalid_argument("Forecast are not for the same day");
    }
    
    temp_morning = (temp_morning + other.temp_morning) / 2.0;
    temp_afternoon = (temp_afternoon + other.temp_afternoon) / 2.0;
    temp_evening = (temp_evening + other.temp_evening) / 2.0;


    precipitation = (precipitation + other.precipitation) / 2.0;

    phenomenon = std::max(phenomenon, other.phenomenon);

    return *this;
}

bool Daily_Weather_Forecast::isForecastValid() const {
    if ((phenomenon == Weather_Phenomenon::SUNNY || phenomenon == Weather_Phenomenon::CLOUDY) 
        && precipitation != 0) {
        return false;
    }
    if (temp_morning < -100 || temp_morning > 60 ||
        temp_afternoon < -100 || temp_afternoon > 60 ||
        temp_evening < -100 || temp_evening > 60) {
        return false;
    }
    if (phenomenon == Weather_Phenomenon::SNOW && averageTemperature() > 0) {
        return false;
    }
    if (precipitation > 1500) {
        return false;
    }
    return true;
}

std::string Daily_Weather_Forecast::phenomenonToString(Weather_Phenomenon phen) {
    if (phen == Weather_Phenomenon::SUNNY) {
        return "SUNNY";
    } else if (phen == Weather_Phenomenon::CLOUDY) {
        return "CLOUDY";
    } else if (phen == Weather_Phenomenon::RAIN) {
        return "RAIN";
    } else {
        return "SNOW";
    }
}

std::strong_ordering operator<=>(const Daily_Weather_Forecast& lhs, const Daily_Weather_Forecast& rhs){
    return lhs.time_utc <=> rhs.time_utc;
}

bool operator==(const Daily_Weather_Forecast& lhs,  const Daily_Weather_Forecast& rhs) {
    return (lhs <=> rhs) == 0;
}

bool Daily_Weather_Forecast::operator!() const {
    return !isForecastValid();
}

std::istream& operator>>(std::istream& in, Daily_Weather_Forecast& forecast) {
    forecast.time_utc = readval<time_t>(in);
    forecast.temp_morning = readval<double>(in);
    forecast.temp_afternoon = readval<double>(in);
    forecast.temp_evening = readval<double>(in);
    forecast.precipitation = readval<double>(in);
    
    int phen = readval<int>(in);
    
    if (phen < 0 || phen > 3) {
        forecast.phenomenon = Weather_Phenomenon::SUNNY;
    } else {
        forecast.phenomenon = static_cast<Weather_Phenomenon>(phen);
    }
    
    return in;
}

std::ostream& operator << (std::ostream& out, const Daily_Weather_Forecast& forecast) {
    out << std::format("___Weather Forecast___\n"
                     "Date (UTC): {}\n"
                     "Temperatures: Morning ={}°C, Afternoon ={}°C, Evening ={}°C\n"
                     "Weather phenomenon: {}\n"
                     "Precipitation: {} mm\n"
                     "Forecast is valid: {}\n",
                     forecast.time_utc,
                     forecast.temp_morning, forecast.temp_afternoon, forecast.temp_evening,
                     forecast.phenomenonToString(forecast.phenomenon),
                     forecast.precipitation,
                     forecast.isForecastValid() ? "Yes" : "No");
    return out;
}

void Daily_Weather_Forecast::print_Forecast(std::ostream& out) const{
    out << *this;
}

void Daily_Weather_Forecast::input_Forecast(std::istream& in) {
    in >> *this;
}