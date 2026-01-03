#ifndef COMMANDPARSER_HPP
#define COMMANDPARSER_HPP
#include "CalendarManager.hpp"
#include "UserManager.hpp"
#include <string>

class CommandParser {
    CalendarManager &calendarManager;
    UserManager &userManager;
public:
    CommandParser(CalendarManager &calendarManager, UserManager &userManager);
    std::string process(const std::string &line);
};
#endif