#include <iostream>
#include <string>
#include "CommandParser.hpp"
#include "HolidayManager.hpp"
#include "UserManager.hpp"
#include "CalendarManager.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: ./UTrello <holiday_file>\n";
        return 1;
    }
    HolidayManager holidayManager;
    holidayManager.loadFromFile(argv[1]);
    UserManager userManager;
    CalendarManager calendarManager(holidayManager, userManager);
    CommandParser parser(calendarManager, userManager);
    std::string line;
    while (std::getline(std::cin, line))
    {
        std::string output = parser.process(line);
        std::cout << output << "\n";
    }
    return 0;
}
