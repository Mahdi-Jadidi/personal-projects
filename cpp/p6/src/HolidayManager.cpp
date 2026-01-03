#include "HolidayManager.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

void HolidayManager::loadFromFile(const std::string& filePath) {
    std::ifstream fin(filePath);
    if (!fin.is_open()) {
        std::cerr << "Failed to open holiday file.\n";
        return;
    }
    std::string line;
    getline(fin, line);
    while (getline(fin, line)) {
        std::istringstream ss(line);
        std::string dayStr, monthStr, yearStr;
        getline(ss, dayStr, ',');
        getline(ss, monthStr, ',');
        getline(ss, yearStr, ',');
        int d = std::stoi(dayStr);
        int m = std::stoi(monthStr);
        int y = std::stoi(yearStr);
        holidays.insert(Date(y, m, d));
    }
}
bool HolidayManager::isHoliday(const Date& date) const {
    return holidays.count(date) > 0;
}
