#include "UserManager.hpp"
#define RESPONSE_OK "OK"
#define RESPONSE_BAD_REQUEST "Bad Request"
#define RESPONSE_PERMISSION_DENIED "Permission Denied"
#define RESPONSE_NOT_FOUND "Not Found"

std::string UserManager::signup(const std::string &username, const std::string &password)
{
    if (currentUser.has_value())
        return RESPONSE_PERMISSION_DENIED;

    if (username.empty() || password.empty())
        return RESPONSE_BAD_REQUEST;

    if (users.count(username))
        return RESPONSE_BAD_REQUEST;

    User newUser;
    newUser.userId = nextUserId++;
    newUser.username = username;
    newUser.password = password;

    users[username] = newUser;
    return RESPONSE_OK;
}

std::string UserManager::login(const std::string &username, const std::string &password)
{
    if (currentUser.has_value())
        return RESPONSE_PERMISSION_DENIED;

    auto it = users.find(username);
    if (it == users.end())
        return RESPONSE_NOT_FOUND;

    if (it->second.password != password)
        return RESPONSE_PERMISSION_DENIED;

    currentUser = it->second;
    return RESPONSE_OK;
}

std::string UserManager::logout()
{
    if (!currentUser.has_value())
        return RESPONSE_PERMISSION_DENIED;

    currentUser.reset();
    return RESPONSE_OK;
}

bool UserManager::isLoggedIn() const
{
    return currentUser.has_value();
}

std::string UserManager::getCurrentUsername() const
{
    return currentUser ? currentUser->username : "";
}

int UserManager::getCurrentUserId() const
{
    return currentUser ? currentUser->userId : -1;
}

bool UserManager::userExists(const std::string &username) const
{
    return users.find(username) != users.end();
}

int UserManager::getUserIdByUsername(const std::string &username) const
{
    auto it = users.find(username);
    if (it != users.end())
    {
        return it->second.userId;
    }
    return -1;
}
