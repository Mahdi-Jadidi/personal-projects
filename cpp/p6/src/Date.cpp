#include "Date.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <string>
#include <unordered_map>
#define MONTHS_IN_YEAR     12
#define FEBRUARY_MONTH     2
#define MAX_DAY_FEBRUARY   29
#define BASE_YEAR          0
#define BASE_MONTH         0
#define BASE_DAY           0
#define DAY_OF_WEEK_OFFSET 4
#define ZELLER_DIV1        5
#define ZELLER_DIV2        4
#define ZELLER_M_ADJUST    12
static const int DAYS_IN_MONTH[] = {31,28,31,30,31,30,31,31,30,31,30,31};
Date::Date() : year(BASE_YEAR), month(BASE_MONTH), day(BASE_DAY) {}
Date::Date(int y, int m, int d) : year(y), month(m), day(d) {}
Date::Date(const std::string& dateStr) {
    std::istringstream iss(dateStr);
    char sep1, sep2;
    iss >> year >> sep1 >> month >> sep2 >> day;
}

std::vector<std::string> Date::splitCommaSeparated(const std::string& input) {
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string token;
    while (std::getline(ss, token, ',')) {
        token.erase(0, token.find_first_not_of(" \t\n\r\f\v"));
        token.erase(token.find_last_not_of(" \t\n\r\f\v") + 1);
        result.push_back(token);
    }
    return result;
}

bool Date::operator<(const Date& other) const {
    if (year != other.year)
        return year < other.year;
    if (month != other.month)
        return month < other.month;
    return day < other.day;
}

bool Date::operator<=(const Date& other) const {
    return (*this < other) || (*this == other);
}

bool Date::operator>=(const Date& other) const {
    return (*this > other) || (*this == other);
}

bool Date::operator==(const Date& other) const {
    return year == other.year && month == other.month && day == other.day;
}

bool Date::operator>(const Date& other) const {
    return !(*this <= other);
}

bool Date::operator!=(const Date& other) const {
        return !(*this == other);
    }

Date Date::operator++(int) {
    Date temp = *this;
    ++(*this);
    return temp;
}

Date& Date::operator++() {
    day++;
    if (day > daysInMonth(year, month)) {
        day = 1;
        month++;
        if (month > MONTHS_IN_YEAR) {
            month = 1;
            year++;
        }
    }
    return *this;
}

int Date::weekDay() const {
    int y = year;
    int m = month;
    int d = day;
    if (m < 3) {
        m += ZELLER_M_ADJUST;
        y--;
    }
    int k = y % 100;
    int j = y / 100;
    int h = (d + 13*(m + 1)/ZELLER_DIV1 + k + k/ZELLER_DIV2 + j/ZELLER_DIV2 + 5*j) % 7;
    int dayOfWeek = (h + DAY_OF_WEEK_OFFSET) % 7;
    return dayOfWeek;
}

std::string Date::toString() const {
    std::ostringstream oss;
    oss << year << "/";
    oss << std::setw(2) << std::setfill('0') << month << "/";
    oss << std::setw(2) << std::setfill('0') << day;
    return oss.str();
}

bool Date::isLeapYear(int y) {
    return (y % 400 == 0) || ((y % 4 == 0) && (y % 100 != 0));
}

int Date::daysInMonth(int y, int m) {
    if (m == FEBRUARY_MONTH && isLeapYear(y))
        return MAX_DAY_FEBRUARY;
    return DAYS_IN_MONTH[m - 1];
}

std::vector<int> Date::parseWeekDays(const std::string& input) {
    std::vector<int> result;
    std::unordered_map<std::string, int> dayMap = {
        {"Sun", 0}, {"Mon", 1}, {"Tue", 2}, {"Wed", 3},
        {"Thu", 4}, {"Fri", 5}, {"Sat", 6}
    };

    std::stringstream ss(input);
    std::string day;
    while (std::getline(ss, day, ',')) {
        day.erase(std::remove_if(day.begin(), day.end(), ::isspace), day.end());  // حذف فاصله‌ها
        if (dayMap.find(day) != dayMap.end()) {
            result.push_back(dayMap[day]);
        }
    }

    return result;
}

Date Date::parse(const std::string& str) {
    std::istringstream ss(str);
    std::string token;
    int y, m, d;

    if (std::count(str.begin(), str.end(), '-') != 2)
        throw std::invalid_argument("Invalid date format");

    std::getline(ss, token, '-');
    y = std::stoi(token);
    std::getline(ss, token, '-');
    m = std::stoi(token);
    std::getline(ss, token, '-');
    d = std::stoi(token);

    return Date(y, m, d);
}