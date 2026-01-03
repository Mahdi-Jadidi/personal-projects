#include <iostream>
#include <string>

#include "../server/server.hpp"
#include "../src/CalendarManager.hpp"
#include "../src/HolidayManager.hpp"
#include "../src/UserManager.hpp"
#include "handlers.hpp"

void mapServerPaths(Server& server, UserManager& userManager, CalendarManager& calendarManager, HolidayManager& holidayManager) {
    server.get("/report", new ShowPage("static/report.html"));
    server.get("/report.css", new ShowFile("static/report.css", "text/css"));
    server.get("/report.js", new ShowFile("static/report.js", "application/javascript"));
    server.post("/report", new GetReportHandler(calendarManager));
    server.get("/invitations", new ShowPage("static/invitations.html"));
    server.get("/invitations.css", new ShowFile("static/invitations.css", "text/css"));
    server.get("/invitations.js", new ShowFile("static/invitations.js", "application/javascript"));
    server.post("/invitations", new InvitationsHandler(calendarManager));

    server.get("/addevent", new ShowPage("static/addevent.html"));
    server.get("/addevent.css", new ShowFile("static/addevent.css", "text/css"));
    server.get("/addevent.js", new ShowFile("static/addevent.js", "application/javascript"));
    server.post("/addevent", new AddEventHandler(calendarManager));
    server.get("/addtask", new ShowPage("static/addtask.html"));
    server.get("/edittask", new ShowPage("static/edittask.html"));
    server.get("/edittask.css", new ShowFile("static/edittask.css", "text/css"));
    server.post("/edittask", new EditTaskHandler(calendarManager));
    server.setNotFoundErrPage("static/404.html");
    server.get("/signup", new ShowPage("static/signup.html"));
    server.post("/signup", new SignupHandler(&userManager));
    server.get("/addtask", new ShowPage("static/addtask.html"));
    server.get("/addtask.css", new ShowFile("static/addtask.css", "text/css"));
    server.post("/removetask", new RemoveTaskHandler(calendarManager));
    server.get("/removetask", new ShowPage("static/removetask.html"));
    server.get("/removetask.css", new ShowFile("static/removetask.css", "text/css"));
    server.post("/addtask", new AddTaskHandler(calendarManager));
    server.get("/logout", new LogoutHandler(&userManager));
    server.get("/", new ShowPage("static/home.html"));
    server.get("/home.png", new ShowImage("static/home.png"));
    server.get("/login", new ShowPage("static/login.html"));
    server.post("/login", new LoginHandler(&userManager));
}

int main(int argc, char** argv) {
    try {
        if (argc < 3) {
            std::cerr << "Usage: ./UTrello <holiday_file>\n";
            return 1;
        }
        HolidayManager holidayManager;
        holidayManager.loadFromFile(argv[2]);
        int port = argc > 1 ? std::stoi(argv[1]) : 5000;
        UserManager userManager;
        CalendarManager calendarManager(holidayManager, userManager);
        Server server(port);
        mapServerPaths(server, userManager, calendarManager, holidayManager);
        std::cout << "Server running on port: " << port << std::endl;
        server.run();
    }
    catch (const std::invalid_argument& e) {
        std::cerr << e.what() << std::endl;
    }
    catch (const Server::Exception& e) {
        std::cerr << e.getMessage() << std::endl;
    }
    return 0;
}
