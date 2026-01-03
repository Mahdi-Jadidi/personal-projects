#ifndef HOLIDAY_MANAGER_HPP
#define HOLIDAY_MANAGER_HPP
#include "Date.hpp"
#include <set>
#include <string>

class HolidayManager {
private:
    std::set<Date> holidays;

public:
    void loadFromFile(const std::string& filePath);
    bool isHoliday(const Date& date) const;
};
#endif