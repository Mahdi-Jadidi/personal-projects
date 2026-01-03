#ifndef CALENDARMANAGER_HPP
#define CALENDARMANAGER_HPP
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "Date.hpp"
#include "HolidayManager.hpp"
#include "UserManager.hpp"

struct Event {
    Date date;
    int startTime;
    int duration;
    std::string title;
    std::string description;
    int userLevelId = 0;
    int globalId = 0;
};

struct PeriodicEvent {
    Date startDate;
    Date endDate;
    int startTime;
    int duration;
    std::string title;
    std::string description;
    std::string type;
    std::vector<int> weekDays;
    int dayOfMonth;
    int userLevelId = 0;
    int globalId = 0;
};

struct SharedEvent {
    int globalId;
    int hostId;
    std::vector<int> guestIds;
    std::vector<int> confirmedGuests;
    Date date;
    int startTime;
    int duration;
    std::string title;
    std::string description;
};

struct JointEvent {
    std::string hostUsername;
    int globalId;
    int hostUserId;
    std::vector<int> guestUserIds;
    Date date;
    int startTime;
    int duration;
    std::string title;
    std::string description;
    std::vector<std::string> guests;
    int id;
    int end_time;
    std::map<std::string, bool> guestAccepted;
};

class CalendarManager {
private:
    HolidayManager& holidayManager;
    UserManager& userManager;
    std::vector<JointEvent> jointEvents;
    std::vector<Event> events;
    std::vector<Event> tasks;
    std::vector<PeriodicEvent> periodicEvents;
    std::vector<SharedEvent> sharedEvents;
    int nextJoinEventId = 1;
    std::map<int, int> lastUserIdForTasks;
    std::map<int, int> lastUserIdForEvents;
    std::map<int, int> lastUserIdForPeriodicEvents;
    int nextGlobalId = 1;
    bool isConflict(const Event& newEvent) const;
    bool isConflictWithPeriodic(const Event& newEvent) const;
    int getNextUserLevelId(int userId, const std::string& entityType);
    int getNextGlobalId();

public:
    UserManager& getUserManager() { return userManager; }
    const UserManager& getUserManager() const { return userManager; }
    std::vector<JointEvent> getUserJointEventsBetweenDates(int userId, const Date& from, const Date& to) const;
    std::string rejectJoinEvent(int joinEventId);
    bool isConflictWithConfirmedJointEvents(const Event& event, const std::string& username) const;
    std::string confirmJoinEvent(int joinEventId);
    CalendarManager(HolidayManager& hm, UserManager& um);
    int getNextJoinEventId();
    std::string addEvent(const Date& date, int startTime, int duration,
                         const std::string& title, const std::string& description);

    std::string addTask(const Date& date, int time,
                        const std::string& title, const std::string& description);

    std::string removeTask(int taskUserLevelId);
    std::vector<JointEvent> getReceivedJoinEvents(int userId) const;
    std::string editTask(int taskUserLevelId,
                         const std::optional<Date>& newDate,
                         const std::optional<int>& newTime,
                         const std::optional<std::string>& newTitle,
                         const std::optional<std::string>& newDescription);

    std::string addPeriodicEvent(const Date& startDate, const Date& endDate, int startTime,
                                 int duration, const std::string& title,
                                 const std::string& description, const std::string& type,
                                 const std::vector<int>& weekDays = {}, int dayOfMonth = -1);

    std::string addJointEvent(const std::vector<std::string>& guestUsernames,
                              const Date& date, int startTime, int endTime,
                              const std::string& title, const std::string& description);

    std::vector<std::string> getReport(const Date& from, const Date& to, const std::string& type = "") const;
std::vector<JointEvent> getPendingInvitations(const std::string& username);

};

#endif
