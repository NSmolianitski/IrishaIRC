#pragma once
#include <map>
#include <vector>

#include <string>
#include "User.hpp"
class Channel
{
private:
    std::map<char, int> mode_;
    char                type_;
    std::string         topic_;
    std::string         name_;
    std::string         key_;
    std::vector<User*>  users_;
    std::vector<User*>  operators_;
public:
    Channel(const std::string &name);

    void setTopic(const std::string &topic_msg);
    void setMode(const std::string &mode_msg);
    void setKey(const std::string &key_msg);
    void setType(const char type);
    void addUser(User* user);
    void delUser(User* user);
    void addOperators(User* oper);
    void delOperators(User* oper);

    const std::string &getTopic() const;
    const std::map<char, int> &getMode() const;
    const std::vector<User*> &getUsers() const;
    const std::vector<User*> &getOperators() const;
    std::string getListUsers();
    ~Channel();
};