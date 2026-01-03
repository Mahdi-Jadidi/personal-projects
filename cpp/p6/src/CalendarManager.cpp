#include "CalendarManager.hpp"
#include <algorithm>
#include <sstream>
#include <iostream>

CalendarManager::CalendarManager(HolidayManager &hm, UserManager &um)
    : holidayManager(hm), userManager(um)
{
}

int CalendarManager::getNextGlobalId()
{
    return nextGlobalId++;
}

int CalendarManager::getNextUserLevelId(int userId, const std::string &entityType)
{
    if (entityType == "task")
    {
        int &lastId = lastUserIdForTasks[userId];
        lastId++;
        return lastId;
    }
    else if (entityType == "event")
    {
        int &lastId = lastUserIdForEvents[userId];
        lastId++;
        return lastId;
    }
    else if (entityType == "periodic_event")
    {
        int &lastId = lastUserIdForPeriodicEvents[userId];
        lastId++;
        return lastId;
    }
    return 0;
}

bool CalendarManager::isConflict(const Event &newEvent) const
{
    for (const auto &ev : events)
    {
        if (ev.date == newEvent.date)
        {
            int evStart = ev.startTime;
            int evEnd = ev.startTime + ev.duration;
            int newStart = newEvent.startTime;
            int newEnd = newEvent.startTime + newEvent.duration;
            if (!(newEnd <= evStart || newStart >= evEnd))
            {
                return true;
            }
        }
    }
    for (const auto &task : tasks)
    {
        if (task.date == newEvent.date)
        {
            int tStart = task.startTime;
            int tEnd = task.startTime + task.duration;
            int newStart = newEvent.startTime;
            int newEnd = newEvent.startTime + newEvent.duration;
            if (!(newEnd <= tStart || newStart >= tEnd))
            {
                return true;
            }
        }
    }
    return false;
}

bool CalendarManager::isConflictWithPeriodic(const Event &newEvent) const
{
    for (const auto &pe : periodicEvents)
    {
        if (newEvent.date < pe.startDate || newEvent.date > pe.endDate)
            continue;

        if (pe.type == "Daily")
        {
        }
        else if (pe.type == "Weekly")
        {
            if (std::find(pe.weekDays.begin(), pe.weekDays.end(), newEvent.date.weekDay()) == pe.weekDays.end())
                continue;
        }
        else if (pe.type == "Monthly")
        {
            if (newEvent.date.day != pe.dayOfMonth)
                continue;
        }
        else
        {
            continue;
        }

        int peStart = pe.startTime;
        int peEnd = pe.startTime + pe.duration;
        int newStart = newEvent.startTime;
        int newEnd = newEvent.startTime + newEvent.duration;
        if (!(newEnd <= peStart || newStart >= peEnd))
        {
            return true;
        }
    }
    return false;
}

std::string CalendarManager::addEvent(const Date &date, int startTime, int duration,
                                      const std::string &title, const std::string &description)
{
    if (!userManager.isLoggedIn())
        return "Permission Denied";

    int userId = userManager.getCurrentUserId();

    Event newEvent{date, startTime, duration, title, description};
    newEvent.userLevelId = getNextUserLevelId(userId, "event");
    newEvent.globalId = getNextGlobalId();

    if (isConflict(newEvent))
        return "Overlap";

    if (isConflictWithPeriodic(newEvent))
        return "Overlap";

    if (holidayManager.isHoliday(date))
        return "Holiday Found";

    events.push_back(newEvent);
    return "OK";
}

std::string CalendarManager::addTask(const Date &date, int time,
                                     const std::string &title, const std::string &description)
{
    if (!userManager.isLoggedIn())
        return "Permission Denied";

    int userId = userManager.getCurrentUserId();

    Event newTask{date, time, 1, title, description};
    newTask.userLevelId = getNextUserLevelId(userId, "task");
    newTask.globalId = getNextGlobalId();

    if (isConflict(newTask) || isConflictWithPeriodic(newTask))
        return "Overlap";

    tasks.push_back(newTask);
    return "OK";
}

std::string CalendarManager::removeTask(int taskUserLevelId)
{
    if (!userManager.isLoggedIn())
        return "Permission Denied";

    auto it = std::find_if(tasks.begin(), tasks.end(), [&](const Event &e)
                           { return e.userLevelId == taskUserLevelId; });

    if (it == tasks.end())
        return "Not Found";

    tasks.erase(it);
    return "OK";
}

std::string CalendarManager::editTask(int taskUserLevelId,
                                      const std::optional<Date> &newDate,
                                      const std::optional<int> &newTime,
                                      const std::optional<std::string> &newTitle,
                                      const std::optional<std::string> &newDescription)
{
    if (!userManager.isLoggedIn())
        return "Permission Denied";

    auto it = std::find_if(tasks.begin(), tasks.end(), [&](const Event &e)
                           { return e.userLevelId == taskUserLevelId; });

    if (it == tasks.end())
        return "Not Found";

    Event edited = *it;

    if (newDate)
        edited.date = *newDate;
    if (newTime)
        edited.startTime = *newTime;
    if (newTitle)
        edited.title = *newTitle;
    if (newDescription)
        edited.description = *newDescription;

    tasks.erase(it);

    if (isConflict(edited) || isConflictWithPeriodic(edited))
    {
        tasks.push_back(*it);
        return "Overlap";
    }

    tasks.push_back(edited);
    return "OK";
}

std::string CalendarManager::addPeriodicEvent(const Date &startDate, const Date &endDate, int startTime,
                                              int duration, const std::string &title,
                                              const std::string &description, const std::string &type,
                                              const std::vector<int> &weekDays, int dayOfMonth)
{
    if (!userManager.isLoggedIn())
        return "Permission Denied";

    int userId = userManager.getCurrentUserId();

    PeriodicEvent newPeriodic{startDate, endDate, startTime, duration, title, description, type, weekDays, dayOfMonth};
    newPeriodic.userLevelId = getNextUserLevelId(userId, "periodic_event");
    newPeriodic.globalId = getNextGlobalId();

    periodicEvents.push_back(newPeriodic);
    return "OK";
}

std::vector<std::string> CalendarManager::getReport(const Date &from, const Date &to, const std::string &type) const
{

    struct ReportEntry
    {
        std::string type;
        Date date;
        int startTime; 
        int globalId;
        std::string description;

        ReportEntry(const std::string &t, const Date &d, int st, int gid, const std::string &desc)
            : type(t), date(d), startTime(st), globalId(gid), description(desc) {}
    };

    auto isTypeMatch = [&](const std::string &t)
    {
        return type.empty() || t == type;
    };

    std::vector<ReportEntry> entries;

    for (const auto &task : tasks)
    {
        if (task.date >= from && task.date <= to && isTypeMatch("task"))
        {
            std::stringstream ss;
            ss << "task \"" << task.title << "\" on " << task.date.toString()
               << " at " << (task.startTime < 10 ? "0" : "") << task.startTime << ":00"
               << ": \"" << task.description << "\"";
            entries.emplace_back("task", task.date, task.startTime, task.globalId, ss.str());
        }
    }
    for (const auto &event : events)
    {
        if (event.date >= from && event.date <= to && isTypeMatch("event"))
        {
            std::stringstream ss;
            ss << "event \"" << event.title << "\" on " << event.date.toString()
               << " from " << (event.startTime < 10 ? "0" : "") << event.startTime << ":00"
               << " for " << event.duration << " hours: \"" << event.description << "\"";
            entries.emplace_back("event", event.date, event.startTime, event.globalId, ss.str());
        }
    }
    for (const auto &pEvent : periodicEvents)
    {
        if (!(pEvent.endDate < from || pEvent.startDate > to) && isTypeMatch("periodic_event"))
        {
            Date occurDate = from > pEvent.startDate ? from : pEvent.startDate;
            std::stringstream ss;
            ss << "periodic_event \"" << pEvent.title << "\" on " << occurDate.toString()
               << " from " << (pEvent.startTime < 10 ? "0" : "") << pEvent.startTime << ":00"
               << " for " << pEvent.duration << " hours " << pEvent.type << ": \"" << pEvent.description << "\"";
            entries.emplace_back("periodic_event", occurDate, pEvent.startTime, pEvent.globalId, ss.str());
        }
    }

    for (const auto &jEvent : jointEvents)
    {
        if (jEvent.date >= from && jEvent.date <= to && isTypeMatch("join_event"))
        {
            int end_time = jEvent.end_time;
            std::stringstream ss;
            ss << "join_event \"" << jEvent.title << "\" on " << jEvent.date.toString()
               << " from " << (jEvent.startTime < 10 ? "0" : "") << jEvent.startTime << ":00"
               << " to " << (end_time < 10 ? "0" : "") << end_time << ":00"
               << " hosted by \"" << jEvent.hostUsername << "\" : \"" << jEvent.description << "\"";
            entries.emplace_back("join_event", jEvent.date, jEvent.startTime, jEvent.globalId, ss.str());
        }
    }

    if (entries.empty())
        return {};

    auto typePriority = [](const std::string &t) -> int
    {
        if (t == "join_event")
            return 0;
        if (t == "periodic_event")
            return 1;
        if (t == "event")
            return 2;
        if (t == "task")
            return 3;
        return 4;
    };

    std::sort(entries.begin(), entries.end(), [&](const ReportEntry &a, const ReportEntry &b)
              {
        if (a.date != b.date)
            return a.date < b.date;
        if (a.startTime != b.startTime)
            return a.startTime < b.startTime;
        int prA = typePriority(a.type);
        int prB = typePriority(b.type);
        if (prA != prB)
            return prA < prB;
        return a.globalId < b.globalId; });
    std::vector<std::string> result;
    for (const auto &e : entries)
    {
        result.push_back(e.description);
    }
    return result;
}

std::string CalendarManager::addJointEvent(const std::vector<std::string> &guestUsernames,
                                           const Date &date, int startTime, int endTime,
                                           const std::string &title, const std::string &description)
{
    if (!userManager.isLoggedIn())
        return "Permission Denied";

    if (guestUsernames.empty() || startTime >= endTime || title.empty())
        return "Bad Request";

    int hostId = userManager.getCurrentUserId();
    int duration = endTime - startTime;

    std::vector<int> guestIds;
    for (const std::string &username : guestUsernames)
    {
        if (!userManager.userExists(username))
            return "Not Found";
        int guestId = userManager.getUserIdByUsername(username);
        if (guestId == hostId) 
            return "Bad Request";
        guestIds.push_back(guestId);
    }
    Event tempEvent{date, startTime, duration, title, description};
    if (isConflict(tempEvent) || isConflictWithPeriodic(tempEvent))
    {
        return "Overlap";
    }
    if (holidayManager.isHoliday(date))
    {
        return "Holiday Found";
    }
    JointEvent newJE;
    newJE.globalId = getNextGlobalId();
    newJE.hostUserId = hostId;
    newJE.guestUserIds = guestIds;
    newJE.date = date;
    newJE.startTime = startTime;
    newJE.duration = duration;
    newJE.title = title;
    newJE.description = description;

    for (const std::string &username : guestUsernames)
    {
        newJE.guestAccepted[username] = false; 
    }
    jointEvents.push_back(newJE);
    return "OK";
}

std::vector<JointEvent> CalendarManager::getReceivedJoinEvents(int userId) const
{
    std::vector<JointEvent> result;
    for (const auto &event : jointEvents)
    {
        if (std::find(event.guestUserIds.begin(), event.guestUserIds.end(), userId) != event.guestUserIds.end())
        {
            result.push_back(event);
        }
    }
    return result;
}

int CalendarManager::getNextJoinEventId()
{
    return nextJoinEventId++;
}

std::string CalendarManager::confirmJoinEvent(int joinEventId)
{
    if (!userManager.isLoggedIn())
        return "Permission Denied";

    std::string username = userManager.getCurrentUsername();

    auto it = std::find_if(jointEvents.begin(), jointEvents.end(), [&](const JointEvent &je)
                           { return je.globalId == joinEventId &&
                                    je.guestAccepted.count(username) &&
                                    !je.guestAccepted.at(username); });

    if (it == jointEvents.end())
        return "Not Found";

    Event event{it->date, it->startTime, it->duration, it->title, it->description};

    if (isConflict(event) || isConflictWithPeriodic(event) || isConflictWithConfirmedJointEvents(event, username))
        return "Overlap";

    it->guestAccepted[username] = true;

    return "OK";
}

bool CalendarManager::isConflictWithConfirmedJointEvents(const Event &event, const std::string &username) const
{
    for (const auto &je : jointEvents)
    {
        auto it = je.guestAccepted.find(username);
        if (it == je.guestAccepted.end() || !it->second)
            continue; 

        if (je.date == event.date)
        {
            int evStart = je.startTime;
            int evEnd = je.startTime + je.duration;
            int newStart = event.startTime;
            int newEnd = event.startTime + event.duration;

            if (std::max(evStart, newStart) < std::min(evEnd, newEnd))
            {
                return true;
            }
        }
    }
    return false;
}

std::string CalendarManager::rejectJoinEvent(int joinEventId)
{
    if (!userManager.isLoggedIn())
        return "Permission Denied";

    std::string username = userManager.getCurrentUsername();
    auto it = std::find_if(jointEvents.begin(), jointEvents.end(), [&](const JointEvent &je)
                           { return je.globalId == joinEventId &&
                                    je.guestAccepted.count(username) &&
                                    !je.guestAccepted.at(username); });

    if (it == jointEvents.end())
        return "Not Found";

    it->guestAccepted.erase(username);

    return "OK";
}

std::vector<JointEvent> CalendarManager::getPendingInvitations(const std::string& username) {
    std::vector<JointEvent> result;
    for (const JointEvent& je : jointEvents) {
        if (je.guestAccepted.count(username) && !je.guestAccepted.at(username)) {
            result.push_back(je);
        }
    }
    return result;
}
