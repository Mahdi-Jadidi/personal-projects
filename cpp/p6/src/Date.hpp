#ifndef DATE_HPP
#define DATE_HPP
#include <string>
#include <vector>

class Date {
public:
    int year;
    int month;
    int day;
    Date();             
    Date(int y, int m, int d); 
    Date(const std::string& dateStr);
    bool operator<(const Date& other) const;
    bool operator<=(const Date& other) const;
    bool operator>=(const Date& other) const;
    bool operator==(const Date& other) const;
    bool operator!=(const Date& other) const;
    bool operator>(const Date& other) const;
    Date operator++(int);     
    Date& operator++();      
    int weekDay() const;     
    std::string toString() const;
    static Date parse(const std::string& str);
    static std::vector<int> parseWeekDays(const std::string& input);
    static std::vector<std::string> splitCommaSeparated(const std::string& input);
private:
    static bool isLeapYear(int year);
    static int daysInMonth(int year, int month);
};
#endif
