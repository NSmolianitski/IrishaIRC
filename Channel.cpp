#include "Channel.hpp"

Channel::Channel(const std::string &name) : name_(name){
    mode_.insert(std::pair<char, int>('o', 0)); //give/take channel operator privileges
    mode_.insert(std::pair<char, int>('p', 0)); // private channel flag
    mode_.insert(std::pair<char, int>('s', 0)); // secret channel flag
    mode_.insert(std::pair<char, int>('i', 0)); // invite-only channel flag
    mode_.insert(std::pair<char, int>('t', 0)); // topic settable by channel operator only flag
    mode_.insert(std::pair<char, int>('n', 0)); // no messages to channel from clients on the outside
    mode_.insert(std::pair<char, int>('m', 0)); // moderated channel
    mode_.insert(std::pair<char, int>('l', 0)); // set the user limit to channel
    mode_.insert(std::pair<char, int>('b', 0)); // set a ban mask to keep users out
    mode_.insert(std::pair<char, int>('v', 0)); // give/take the ability to speak on a moderated channel
    mode_.insert(std::pair<char, int>('k', 0)); // set a channel key (password)
}

Channel::~Channel() {

}

const std::string &Channel::getTopic() const {
    return topic_;
}

void Channel::setTopic(const std::string &topic_msg) {
    topic_ = topic_msg;
}

const std::map<char, int> &Channel::getMode() const {
    return mode_;
}

void Channel::setMode(const std::string &mode_msg) {

}

void Channel::setKey(const std::string &key_msg) {
    key_ = key_msg;
}

const std::vector<User*> &Channel::getUsers() const {
    return users_;
}

void Channel::addUser(User* user) {
    std::vector<User*>::iterator itr = users_.begin();
    std::vector<User*>::iterator ite = users_.begin();

    while (itr != ite){
        if (*itr == user){
            return;
        }
        itr++;
    }
    itr = operators_.begin();
    ite = operators_.end();
    while (itr != ite){
        if (*itr == user){
            return;
        }
        itr++;
    }
    users_.push_back(user);
}

void Channel::delUser(User* user) {
    std::vector<User*>::iterator itr = users_.begin();
    std::vector<User*>::iterator ite = users_.begin();

    while (itr != ite){
        if (*itr == user){
            users_.erase(itr);
            break;
        }
        itr++;
    }
}

void Channel::setType(const char type) {
    type_ = type;
}

void Channel::addOperators(User* oper) {
    std::vector<User*>::iterator itr = users_.begin();
    std::vector<User*>::iterator ite = users_.begin();

    while (itr != ite){
        if (*itr == oper){
            return;
        }
        itr++;
    }
    itr = operators_.begin();
    ite = operators_.end();
    while (itr != ite){
        if (*itr == oper){
            return;
        }
        itr++;
    }
    operators_.push_back(oper);
}

void Channel::delOperators(User* oper) {
    std::vector<User*>::iterator itr = operators_.begin();
    std::vector<User*>::iterator ite = operators_.begin();

    while (itr != ite){
        if (*itr == oper){
            operators_.erase(itr);
            break;
        }
        itr++;
    }
}

const std::vector<User *> &Channel::getOperators() const {
    return operators_;
}

std::string Channel::getListUsers() {
    std::string list_users;
    std::vector<User*>::iterator itr = operators_.begin();
    std::vector<User*>::iterator ite = operators_.end();
    while (itr != ite){
        list_users.append("@" + (*itr)->nick() + " ");
        itr++;
    }
    itr = users_.begin();
    ite = users_.end();
    while (itr != ite){
        list_users.append((*itr)->nick() + " ");
        itr++;
    }
    list_users.erase(list_users.size() - 1);
    return list_users;
}

void Channel::sendChannel(const int sock) {
    std::vector<User*>::iterator itr = operators_.begin();
    std::vector<User*>::iterator ite = operators_.end();

    while (itr != ite){

    }
}
