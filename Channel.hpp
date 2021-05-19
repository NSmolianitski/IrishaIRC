#pragma once
#include <map>
#include <vector>
#include "Irisha.hpp"
#include <string>
#include "User.hpp"

#define ITERATOR std::vector<User*>::iterator
#define CITERATOR std::vector<User*>::const_iterator
class Channel
{
private:
    std::map<char, int> mode_;
    char                type_;
    int                 max_users_;
    std::string         topic_;
    std::string         name_;
    std::string         key_;
    std::vector<User*>  users_;
    std::vector<User*>  operators_;
    std::vector<User*>  ban_users_;
    std::vector<User*>  moderator_users_;
    std::vector<User*>  invite_users_;
public:
    Channel(const std::string &name);

    void setTopic(const std::string &topic_msg);
    void setMode(const std::string &mode_msg);
    void setKey(const std::string &key_msg);
    void setType(const char type);
    void addBanUser(User* user);
    void delBanUser(User* user);
    void addUser(User* user);
    void delUser(User* user);
    void addOperators(User* oper);
    void delOperators(User* oper);
    void addInviteUser(User* user);
    void delInviteUser(User* user);
    void addModeratorUser(User* user);
    void delModeratorUser(User* user);

    const std::string &getTopic() const;
    const std::string &getKey() const;
    const std::map<char, int> &getMode() const;
    const std::vector<User*> &getUsers() const;
    const std::vector<User*> &getOperators() const;
    const std::vector<User*> &getBanUsers() const;
    const std::vector<User*> &getInviteUsers() const;
    const std::vector<User*> &getModerators() const;
    const int &getMaxUsers() const;
    std::string getListUsers();
    ~Channel();
};