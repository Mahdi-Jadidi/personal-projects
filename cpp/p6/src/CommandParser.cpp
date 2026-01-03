#include "CommandParser.hpp"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <vector>
#define METHOD_POST "POST"
#define METHOD_GET "GET"
#define METHOD_PUT "PUT"
#define METHOD_DELETE "DELETE"
#define ACTION_SIGNUP "signup"
#define ACTION_LOGIN "login"
#define ACTION_LOGOUT "logout"
#define ACTION_EVENT "event"
#define ACTION_TASK "task"
#define ACTION_PERIODIC "periodic_event"
#define ACTION_REPORT "report"
#define ACTION_J_EVENT "join_event"
#define ACTION_CONFIRM_JOIN "confirm_join_event"
#define ACTION_REJECT_JOIN "reject_join_event"
#define QUERY_MARK "?"
#define QUERY_USERNAME "username"
#define QUERY_PASSWORD "password"
#define QUERY_DATE "date"
#define QUERY_START_TIME "start_time"
#define QUERY_DURATION "duration"
#define QUERY_TITLE "title"
#define QUERY_DESCRIPTION "description"
#define QUERY_TIME "time"
#define QUERY_TASK_ID "task_id"
#define QUERY_INVITATION_ID "invitation_id"
#define QUERY_END_TIME "end_time"
#define QUERY_START_DATE "start_date"
#define QUERY_END_DATE "end_date"
#define QUERY_TYPE "type"
#define QUERY_DAY "day"
#define QUERY_WEEK_DAYS "week_days"
#define QUERY_GUESTS "guests"
#define KEY_HOST "host"
#define BAD_REQUEST "Bad Request"
#define RESPONSE_EMPTY "Empty"
#define RESPONSE_PERMISSION_DENIED "Permission Denied"
#define DEFAULT_FROM_DATE "1400/01/01"
#define MAX_DAY_OF_MONTH 30
#define TYPE_DAILY "Daily"
#define TYPE_WEEKLY "Weekly"
#define TYPE_MONTHLY "Monthly"

namespace
{
    int weekDayStrToInt(const std::string &day)
    {
        if (day == "Sunday") return 0;
        if (day == "Monday") return 1;
        if (day == "Tuesday") return 2;
        if (day == "Wednesday") return 3;
        if (day == "Thursday") return 4;
        if (day == "Friday") return 5;
        if (day == "Saturday") return 6;
        return -1;
    }
}

CommandParser::CommandParser(CalendarManager &calendarManager, UserManager &userManager)
    : calendarManager(calendarManager), userManager(userManager) {}

std::string CommandParser::process(const std::string &line)
{
    std::istringstream iss(line);
    std::string method;
    iss >> method;

    if (method != METHOD_POST && method != METHOD_GET && method != METHOD_PUT && method != METHOD_DELETE)
        return BAD_REQUEST;

    std::string action;
    iss >> action;

    if (method == METHOD_POST && action == ACTION_SIGNUP)
    {
        std::string qm, key1, key2, username, password;
        iss >> qm >> key1 >> std::quoted(username) >> key2 >> std::quoted(password);
        if (qm != QUERY_MARK || key1 != QUERY_USERNAME || key2 != QUERY_PASSWORD)
            return BAD_REQUEST;
        return userManager.signup(username, password);
    }

    if (method == METHOD_POST && action == ACTION_LOGIN)
    {
        std::string qm, key1, key2, username, password;
        iss >> qm >> key1 >> std::quoted(username) >> key2 >> std::quoted(password);
        if (qm != QUERY_MARK || key1 != QUERY_USERNAME || key2 != QUERY_PASSWORD)
            return BAD_REQUEST;
        return userManager.login(username, password);
    }

    if (method == METHOD_POST && action == ACTION_LOGOUT)
    {
        std::string qm;
        iss >> qm;
        if (qm != QUERY_MARK)
            return BAD_REQUEST;
        return userManager.logout();
    }

    if (method == METHOD_POST && action == ACTION_EVENT)
    {
        std::string qm, k1, dateStr, k2, k3, k4, title, description = "";
        int start, duration;
        iss >> qm >> k1 >> dateStr >> k2 >> start >> k3 >> duration >> k4 >> std::quoted(title);
        if (qm != QUERY_MARK || k1 != QUERY_DATE || k2 != QUERY_START_TIME || k3 != QUERY_DURATION || k4 != QUERY_TITLE)
            return BAD_REQUEST;

        std::string dummy;
        if (iss >> dummy)
        {
            if (dummy != QUERY_DESCRIPTION || !(iss >> std::quoted(description)))
                return BAD_REQUEST;
        }
        Date date(dateStr);
        return calendarManager.addEvent(date, start, duration, title, description);
    }

    if (method == METHOD_POST && action == ACTION_TASK)
    {
        std::string qm, k1, dateStr, k2, k3, title, description, dummy;
        int time;
        iss >> qm >> k1 >> dateStr >> k2 >> time >> k3 >> std::quoted(title) >> dummy >> std::quoted(description);
        if (qm != QUERY_MARK || k1 != QUERY_DATE || k2 != QUERY_TIME || k3 != QUERY_TITLE || dummy != QUERY_DESCRIPTION)
            return BAD_REQUEST;
        Date date(dateStr);
        return calendarManager.addTask(date, time, title, description);
    }

    if (method == METHOD_PUT && action == ACTION_TASK)
    {
        std::string qm, key, title, description;
        int id = -1, time = -1;
        Date newDate;
        bool hasChange = false, hasBad = false;

        iss >> qm;
        if (qm != QUERY_MARK)
            return BAD_REQUEST;

        while (iss >> key)
        {
            if (key == QUERY_TASK_ID)
                iss >> id;
            else if (key == QUERY_DATE)
            {
                std::string dateStr;
                iss >> dateStr;
                newDate = Date(dateStr);
                hasChange = true;
            }
            else if (key == QUERY_TIME)
            {
                iss >> time;
                hasChange = true;
            }
            else if (key == QUERY_TITLE)
            {
                iss >> std::quoted(title);
                hasChange = true;
            }
            else if (key == QUERY_DESCRIPTION)
            {
                iss >> std::quoted(description);
                hasChange = true;
            }
            else
            {
                hasBad = true;
                break;
            }
        }
        if (id == -1 || !hasChange || hasBad)
            return BAD_REQUEST;
        return calendarManager.editTask(id, newDate, time, title, description);
    }

    if (method == METHOD_POST && action == ACTION_J_EVENT)
    {
        std::string qm;
        iss >> qm;
        if (qm != QUERY_MARK)
            return BAD_REQUEST;

        std::string key;
        std::string dateStr, title, description;
        int start = -1, end = -1;
        std::vector<std::string> guestUsernames;

        while (iss >> key)
        {
            if (key == QUERY_GUESTS)
            {
                std::string guestListStr;
                if (!(iss >> std::quoted(guestListStr)))
                    return BAD_REQUEST;
                std::stringstream ss(guestListStr);
                std::string guest;
                while (std::getline(ss, guest, ','))
                {
                    if (!guest.empty())
                        guestUsernames.push_back(guest);
                }
            }
            else if (key == QUERY_DATE)
            {
                if (!(iss >> dateStr))
                    return BAD_REQUEST;
            }
            else if (key == QUERY_START_TIME)
            {
                if (!(iss >> start))
                    return BAD_REQUEST;
            }
            else if (key == QUERY_END_TIME)
            {
                if (!(iss >> end))
                    return BAD_REQUEST;
            }
            else if (key == QUERY_TITLE)
            {
                if (!(iss >> std::quoted(title)))
                    return BAD_REQUEST;
            }
            else if (key == QUERY_DESCRIPTION)
            {
                if (!(iss >> std::quoted(description)))
                    return BAD_REQUEST;
            }
            else
            {
                return BAD_REQUEST;
            }
        }

        if (guestUsernames.empty() || dateStr.empty() || title.empty() || start == -1 || end == -1)
            return BAD_REQUEST;

        if (end <= start)
            return BAD_REQUEST;

        Date date(dateStr);
        return calendarManager.addJointEvent(guestUsernames, date, start, end, title, description);
    }

    if (method == METHOD_GET && action == ACTION_J_EVENT)
    {
        std::string qm;
        iss >> qm;
        if (qm != QUERY_MARK)
            return BAD_REQUEST;

        if (!userManager.isLoggedIn())
            return RESPONSE_PERMISSION_DENIED;

        int currentUserId = userManager.getCurrentUserId();
        auto invites = calendarManager.getReceivedJoinEvents(currentUserId);
        if (invites.empty())
            return RESPONSE_EMPTY;

        std::string result;
        for (const auto &invite : invites)
        {
            int joinEventId = invite.globalId;
            std::string title = invite.title;
            std::string dateStr = invite.date.toString();
            int startTime = invite.startTime;
            int endTime = startTime + invite.duration;

            result += std::to_string(joinEventId) + ": \"" + title + "\" - " + dateStr + " - " +
                      std::to_string(startTime) + " - " + std::to_string(endTime) + "\n";
        }
        if (!result.empty())
            result.pop_back();

        return result;
    }

    if (method == METHOD_POST && action == ACTION_CONFIRM_JOIN)
    {
        std::string qm, key;
        int invitationId;
        iss >> qm >> key >> invitationId;
        if (qm != QUERY_MARK || key != QUERY_INVITATION_ID)
            return BAD_REQUEST;
        return calendarManager.confirmJoinEvent(invitationId);
    }

    if (method == METHOD_POST && action == ACTION_REJECT_JOIN)
    {
        std::string qm, key;
        int joinEventId;
        iss >> qm >> key >> joinEventId;
        if (qm != QUERY_MARK || key != QUERY_INVITATION_ID)
            return BAD_REQUEST;
        return calendarManager.rejectJoinEvent(joinEventId);
    }

    if (method == METHOD_POST && action == ACTION_PERIODIC)
    {
        std::string qm, startDateStr, endDateStr, typeStr, title, description = "";
        int startTime = -1, duration = -1, dayOfMonth = -1;
        std::vector<int> weekDays;

        iss >> qm;
        if (qm != QUERY_MARK)
            return BAD_REQUEST;

        std::string key, value;
        while (iss >> key)
        {
            if (key == QUERY_START_DATE)
                iss >> startDateStr;
            else if (key == QUERY_END_DATE)
                iss >> endDateStr;
            else if (key == QUERY_START_TIME)
                iss >> startTime;
            else if (key == QUERY_DURATION)
                iss >> duration;
            else if (key == QUERY_TYPE)
                iss >> typeStr;
            else if (key == QUERY_DAY)
                iss >> dayOfMonth;
            else if (key == QUERY_WEEK_DAYS)
            {
                std::string daysStr;
                iss >> daysStr;
                std::stringstream ss(daysStr);
                std::string part;
                while (std::getline(ss, part, ','))
                {
                    int wd = weekDayStrToInt(part);
                    if (wd == -1)
                        return BAD_REQUEST;
                    weekDays.push_back(wd);
                }
            }
            else if (key == QUERY_TITLE)
                iss >> std::quoted(title);
            else if (key == QUERY_DESCRIPTION)
                iss >> std::quoted(description);
            else
                return BAD_REQUEST;
        }

        if (startDateStr.empty() || endDateStr.empty() || startTime < 0 || duration <= 0 || typeStr.empty() || title.empty())
            return BAD_REQUEST;

        Date start(startDateStr), end(endDateStr);
        if (end < start)
            return BAD_REQUEST;

        if (typeStr == TYPE_DAILY)
            return calendarManager.addPeriodicEvent(start, end, startTime, duration, title, description, typeStr);
        if (typeStr == TYPE_WEEKLY)
        {
            if (weekDays.empty())
                return BAD_REQUEST;
            return calendarManager.addPeriodicEvent(start, end, startTime, duration, title, description, typeStr, weekDays);
        }
        if (typeStr == TYPE_MONTHLY)
        {
            if (dayOfMonth < 1 || dayOfMonth > MAX_DAY_OF_MONTH)
                return BAD_REQUEST;
            return calendarManager.addPeriodicEvent(start, end, startTime, duration, title, description, typeStr, {}, dayOfMonth);
        }
        return BAD_REQUEST;
    }

    if (method == METHOD_GET && action == ACTION_REPORT)
    {
        std::string qm;
        iss >> qm;
        if (qm != QUERY_MARK)
            return BAD_REQUEST;

        std::string key, value, fromStr = "", toStr = "", typeValue = "";
        bool badRequest = false;

        while (iss >> key >> value)
        {
            if (key == "from")
                fromStr = value;
            else if (key == "to")
                toStr = value;
            else if (key == QUERY_TYPE)
                typeValue = value;
            else
            {
                badRequest = true;
                break;
            }
        }

        if (badRequest || toStr.empty())
            return BAD_REQUEST;
        if (fromStr.empty())
            fromStr = DEFAULT_FROM_DATE;

        Date from(fromStr), to(toStr);
        if (to < from)
            return BAD_REQUEST;

        std::vector<std::string> report;
        if (!typeValue.empty())
            report = calendarManager.getReport(from, to, typeValue);
        else
            report = calendarManager.getReport(from, to);

        std::string output;
        for (const auto &entry : report)
            output += entry + "\n";
        return output.empty() ? RESPONSE_EMPTY : output.substr(0, output.size() - 1);
    }

    if (method == METHOD_DELETE && action == ACTION_TASK)
    {
        std::string qm, key;
        int taskId;
        iss >> qm >> key >> taskId;
        if (qm != QUERY_MARK || key != QUERY_TASK_ID)
            return BAD_REQUEST;
        return calendarManager.removeTask(taskId);
    }

    return BAD_REQUEST;
}
