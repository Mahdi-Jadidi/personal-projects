#ifndef HANDLERS_HPP_INCLUDE
#define HANDLERS_HPP_INCLUDE
#include <map>
#include <string>
#include <vector>
#include "../server/server.hpp"
#include "../src/CalendarManager.hpp"
#include "../src/Date.hpp"
#include "../src/HolidayManager.hpp"
#include "../src/UserManager.hpp"

class GetReportHandler : public RequestHandler {
public:
    GetReportHandler(CalendarManager& cm) : calendarManager(cm) {}
    Response* callback(Request* req)override;

private:
    CalendarManager& calendarManager;
};

class InvitationsHandler : public RequestHandler {
public:
    explicit InvitationsHandler(CalendarManager& cm);
    Response* callback(Request* req) override;

private:
    CalendarManager &calendarManager;
};

class AddEventHandler : public RequestHandler {
public:
    AddEventHandler(CalendarManager& cm) : calendarManager(cm) {}
    Response* callback(Request* req) override;

private:
    CalendarManager& calendarManager;
};

class AddTaskHandler : public RequestHandler {
public:
    AddTaskHandler(CalendarManager& cm) : calendarManager(cm) {}
    Response* callback(Request* req) override;

private:
    CalendarManager& calendarManager;
};

class EditTaskHandler : public RequestHandler {
public:
    explicit EditTaskHandler(CalendarManager& calendarManager) : calendarManager(calendarManager) {}
    Response* callback(Request* req) override;

private:
    CalendarManager& calendarManager;
};

class RemoveTaskHandler : public RequestHandler {
public:
    RemoveTaskHandler(CalendarManager& cm) : calendarManager(cm) {}
    Response* callback(Request* req) override;

private:
    CalendarManager& calendarManager;
};

class SignupHandler : public RequestHandler {
private:
    UserManager* userManager;

public:
    SignupHandler(UserManager* manager) : userManager(manager) {}
    Response* callback(Request*) override;
};

class LogoutHandler : public RequestHandler {
public:
    LogoutHandler(UserManager* userManager);   // سازنده
    Response* callback(Request* req) override; // فقط اعلام (بدنه نباشد)

private:
    UserManager* userManager;
};

class LoginHandler : public RequestHandler {
private:
    UserManager* userManager;

public:
    LoginHandler(UserManager* manager) : userManager(manager) {}
    Response* callback(Request*) override;
};

#endif // HANDLERS_HPP_INCLUDE
