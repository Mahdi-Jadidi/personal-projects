#include "handlers.hpp"

#include <any>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

Response* GetReportHandler::callback(Request* req) {
    try {
        std::string fromStr = req->getBodyParam("from");
        std::string toStr = req->getBodyParam("to");

        if (fromStr.empty() || toStr.empty()) {
            return Response::redirect("/report?error=Missing from or to date");
        }

        std::tm tmFrom = {}, tmTo = {};
        std::istringstream ssFrom(fromStr), ssTo(toStr);
        ssFrom >> std::get_time(&tmFrom, "%Y-%m-%d");
        ssTo >> std::get_time(&tmTo, "%Y-%m-%d");
        if (ssFrom.fail() || ssTo.fail()) {
            return Response::redirect("/report?error=Invalid date format");
        }

        Date fromDate(tmFrom.tm_year + 1900, tmFrom.tm_mon + 1, tmFrom.tm_mday);
        Date toDate(tmTo.tm_year + 1900, tmTo.tm_mon + 1, tmTo.tm_mday);

        if (toDate < fromDate) {
            return Response::redirect("/report?error=End date must be after start date");
        }

        auto reportLines = calendarManager.getReport(fromDate, toDate, "");

        std::stringstream html;
        html << "<!DOCTYPE html><html><head><title>Report</title>"
             << "<style>"
             << "body { font-family: Arial, sans-serif; margin: 20px; }"
             << "h1 { color: #333; }"
             << "ul { list-style-type: none; padding: 0; }"
             << "li { background: #f0f0f0; margin: 5px 0; padding: 10px; border-radius: 5px; }"
             << "</style></head><body>";

        html << "<h1>Report from " << fromStr << " to " << toStr << "</h1>";
        if (reportLines.empty()) {
            html << "<p>No tasks or events found in this period.</p>";
        }
        else {
            html << "<ul>";
            for (const auto& line : reportLines) {
                html << "<li>" << line << "</li>";
            }
            html << "</ul>";
        }
        html << "<p><a href=\"/report\">Back to Report Form</a></p>";
        html << "</body></html>";

        Response* res = new Response(Response::Status::ok);
        res->setHeader("Content-Type", "text/html; charset=utf-8");
        res->setBody(html.str());

        return res;
    }
    catch (const std::exception& ex) {
        return Response::redirect(std::string("/report?error=") + ex.what());
    }
}


InvitationsHandler::InvitationsHandler(CalendarManager& cm) : calendarManager(cm) {}

Response* InvitationsHandler::callback(Request* req) {
    if (!calendarManager.getUserManager().isLoggedIn()) {
        return Response::redirect("/login?error=Please login first");
    }

    std::string username = calendarManager.getUserManager().getCurrentUsername();

    if (req->getMethod() == Request::Method::POST) {
        std::string action = req->getBodyParam("action");
        int joinEventId = std::stoi(req->getBodyParam("joinEventId"));

        std::string result;
        if (action == "confirm") {
            result = calendarManager.confirmJoinEvent(joinEventId);
        }
        else if (action == "reject") {
            result = calendarManager.rejectJoinEvent(joinEventId);
        }
        else {
            result = "Invalid action";
        }

        if (result == "OK") {
            return Response::redirect("/invitations?success=" + action + "ed successfully");
        }
        else {
            return Response::redirect("/invitations?error=" + result);
        }
    }
    else if (req->getMethod() == Request::Method::GET) {
        auto invitations = calendarManager.getPendingInvitations(username);
        std::string url = "/invitations.html?";
        for (const auto& je : invitations) {
            url += "id=" + std::to_string(je.globalId) + "&";
            url += "title=" + je.title + "&";
            url += "date=" + je.date.toString() + "&";
        }
        return Response::redirect(url);
    }

    return Response::redirect("/invitations?error=Invalid request");
}

Response* AddEventHandler::callback(Request* req) {
    try {
        std::string type = req->getBodyParam("eventType"); // "normal", "weekly", or "joint"

        if (type == "normal") {
            std::string dateStr = req->getBodyParam("date");
            std::tm tm = {};
            std::istringstream ssDate(dateStr);
            ssDate >> std::get_time(&tm, "%Y-%m-%d");
            if (ssDate.fail()) throw std::runtime_error("Invalid date format");

            Date date(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
            int startTime = std::stoi(req->getBodyParam("startTime"));
            int duration = std::stoi(req->getBodyParam("duration"));
            std::string title = req->getBodyParam("title");
            std::string description = req->getBodyParam("description");

            std::string result = calendarManager.addEvent(date, startTime, duration, title, description);
            return Response::redirect(result == "OK"
                                          ? "/addevent?success=Normal event added successfully"
                                          : "/addevent?error=" + result);
        }
        if (type == "weekly") {
            std::string startStr = req->getBodyParam("startDate");
            std::string endStr = req->getBodyParam("endDate");
            std::tm tmStart = {}, tmEnd = {};
            std::istringstream ssStart(startStr), ssEnd(endStr);
            ssStart >> std::get_time(&tmStart, "%Y-%m-%d");
            ssEnd >> std::get_time(&tmEnd, "%Y-%m-%d");
            if (ssStart.fail() || ssEnd.fail()) throw std::runtime_error("Invalid date format");

            Date startDate(tmStart.tm_year + 1900, tmStart.tm_mon + 1, tmStart.tm_mday);
            Date endDate(tmEnd.tm_year + 1900, tmEnd.tm_mon + 1, tmEnd.tm_mday);
            int startTime = std::stoi(req->getBodyParam("startTime"));
            int duration = std::stoi(req->getBodyParam("duration"));
            std::string title = req->getBodyParam("title");
            std::string description = req->getBodyParam("description");
            std::string weekDaysStr = req->getBodyParam("weekDays");
            std::vector<int> weekDays;
            std::istringstream ssDays(weekDaysStr);
            std::string token;
            while (std::getline(ssDays, token, ',')) {
                try {
                    weekDays.push_back(std::stoi(token));
                }
                catch (...) {
                }
            }
            std::string result = calendarManager.addPeriodicEvent(startDate, endDate, startTime, duration, title, description, "weekly", weekDays, -1);
            return Response::redirect(result == "OK"
                                          ? "/addevent?success=Periodic event added successfully"
                                          : "/addevent?error=" + result);
        }
        if (type == "joint") {
            std::string dateStr = req->getBodyParam("date");
            std::tm tm = {};
            std::istringstream ssDate(dateStr);
            ssDate >> std::get_time(&tm, "%Y-%m-%d");
            if (ssDate.fail()) throw std::runtime_error("Invalid date format");

            Date date(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
            int startTime = std::stoi(req->getBodyParam("startTime"));
            int endTime = std::stoi(req->getBodyParam("endTime"));
            std::string title = req->getBodyParam("title");
            std::string description = req->getBodyParam("description");

            std::string guestsStr = req->getBodyParam("guests"); // e.g., "ali,mina,hasan"
            std::vector<std::string> guests;
            std::istringstream ssGuests(guestsStr);
            std::string name;
            while (std::getline(ssGuests, name, ',')) {
                if (!name.empty())
                    guests.push_back(name);
            }
            std::string result = calendarManager.addJointEvent(guests, date, startTime, endTime, title, description);
            return Response::redirect(result == "OK"
                                          ? "/addevent?success=Joint event added successfully"
                                          : "/addevent?error=" + result);
        }
        return Response::redirect("/addevent?error=Unknown event type");
    }
    catch (const std::exception& e) {
        return Response::redirect("/addevent?error=" + std::string(e.what()));
    }
}

Response* EditTaskHandler::callback(Request* req) {
    try {
        std::string taskIdStr = req->getBodyParam("taskId");
        int taskId = std::stoi(taskIdStr);

        std::string dateStr = req->getBodyParam("date");
        std::optional<Date> newDate = std::nullopt;
        if (!dateStr.empty()) {
            std::tm tm = {};
            std::istringstream ss(dateStr);
            ss >> std::get_time(&tm, "%Y-%m-%d");
            if (ss.fail()) {
                throw std::runtime_error("Invalid date format (should be YYYY-MM-DD)");
            }
            newDate = Date(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
        }

        std::string timeStr = req->getBodyParam("time");
        std::optional<int> newTime = std::nullopt;
        if (!timeStr.empty()) {
            int hour = std::stoi(timeStr);
            if (hour < 0 || hour > 23) {
                throw std::runtime_error("Hour must be between 0 and 23");
            }
            newTime = hour;
        }

        std::string newTitle = req->getBodyParam("title");
        std::optional<std::string> newTitleOpt = newTitle.empty() ? std::nullopt : std::make_optional(newTitle);

        std::string newDescription = req->getBodyParam("description");
        std::optional<std::string> newDescOpt = newDescription.empty() ? std::nullopt : std::make_optional(newDescription);

        std::string result = calendarManager.editTask(taskId, newDate, newTime, newTitleOpt, newDescOpt);

        if (result == "OK") {
            return Response::redirect("/edittask?success=Task updated successfully");
        }
        else {
            return Response::redirect("/edittask?error=" + result);
        }
    }
    catch (const std::exception& e) {
        return Response::redirect("/edittask?error=" + std::string(e.what()));
    }
}

Response* AddTaskHandler::callback(Request* req) {
    try {
        std::string dateStr = req->getBodyParam("date");
        std::tm tm = {};
        std::istringstream ss(dateStr);
        ss >> std::get_time(&tm, "%Y-%m-%d");
        if (ss.fail()) {
            throw std::runtime_error("Invalid date format (should be YYYY-MM-DD)");
        }
        Date date(tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);

        std::string timeStr = req->getBodyParam("time");
        int hour = std::stoi(timeStr);
        if (hour < 0 || hour > 23) {
            throw std::runtime_error("Hour must be between 0 and 23");
        }

        std::string title = req->getBodyParam("title");
        if (title.empty()) {
            throw std::runtime_error("Task title cannot be empty");
        }

        std::string description = req->getBodyParam("description");

        std::string result = calendarManager.addTask(date, hour, title, description);

        if (result == "OK") {
            return Response::redirect("/addtask?success=Task added successfully");
        }
        else if (result == "Permission Denied") {
            return Response::redirect("/addtask?error=You must be logged in to add a task");
        }
        else if (result == "Overlap") {
            return Response::redirect("/addtask?error=Task overlaps with an existing event");
        }
        else {
            return Response::redirect("/addtask?error=Unknown error occurred");
        }
    }
    catch (const std::exception& e) {
        return Response::redirect("/addtask?error=" + std::string(e.what()));
    }
}

Response* RemoveTaskHandler::callback(Request* req) {
    try {
        std::string taskIdStr = req->getBodyParam("taskId");
        int taskId = std::stoi(taskIdStr);

        std::string result = calendarManager.removeTask(taskId);

        if (result == "OK") {
            return Response::redirect("/removetask?success=Task successfully removed");
        }
        else {
            return Response::redirect("/removetask?error=" + result);
        }
    }
    catch (const std::exception& e) {
        return Response::redirect("/removetask?error=" + std::string("Invalid task ID"));
    }
}

Response* SignupHandler::callback(Request* req) {
    std::string username = req->getBodyParam("username");
    std::string password = req->getBodyParam("password");

    std::string signupResult = userManager->signup(username, password);

    if (signupResult == "OK") {
        Response* res = Response::redirect("/");
        res->setSessionId(username);
        userManager->login(username, password);
        return res;
    }
    else {
        Response* res = Response::redirect("/signup?error=" + signupResult);
        return res;
    }
}
LogoutHandler::LogoutHandler(UserManager* userManager)
    : userManager(userManager) {}

Response* LogoutHandler::callback(Request* req) {
    std::string logoutResult = userManager->logout();
    Response* res = Response::redirect("/login");
    res->setSessionId("");
    return res;
}

Response* LoginHandler::callback(Request* req) {
    std::string username = req->getBodyParam("username");
    std::string password = req->getBodyParam("password");

    std::string loginResult = userManager->login(username, password);

    if (loginResult == "OK") {
        Response* res = Response::redirect("/");
        res->setSessionId(username); // استفاده از نام کاربری به عنوان session ID
        return res;
    }
    else {
        Response* res = Response::redirect("/login?error=" + loginResult);
        return res;
    }
}
