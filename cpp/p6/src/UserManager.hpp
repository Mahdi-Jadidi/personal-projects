#ifndef USER_MANAGER_HPP
#define USER_MANAGER_HPP

#include <string>
#include <map>
#include <optional>

struct User
{
    int userId;
    std::string username;
    std::string password;
};

class UserManager
{
private:
    std::map<std::string, User> users;
    std::optional<User> currentUser;
    int nextUserId = 1;
    std::map<std::string, int> usernameToId;

public:
    bool userExists(const std::string &username) const;
    int getUserIdByUsername(const std::string &username) const;

    std::string signup(const std::string &username, const std::string &password);
    std::string login(const std::string &username, const std::string &password);
    std::string logout();
    bool isLoggedIn() const;
    std::string getCurrentUsername() const;
    int getCurrentUserId() const;
};

#endif
