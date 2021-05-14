#pragma once
#include <map>
#include <set>
#include <string>
class Channel
{
private:
    std::map<char, int>         mode_;
    std::string                 topic_;
    std::string                 name_;
    std::string                 key_;
    std::set<std::string>    users_;
public:
    Channel(const std::string &name);
    const std::string &getTopic() const;
    void setTopic(const std::string &topic_msg);
    const std::map<char, int> &getMode() const;
    void setMode(const std::string &mode_msg);
    void setKey(const std::string &key_msg);
    const std::set<std::string> &getUsers() const;
    void addUser(const std::string &nick_msg);
    void delUser(const std::string &nick_msg);
    ~Channel();
};