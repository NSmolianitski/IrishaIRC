#include "Channel.hpp"

Channel::Channel(const std::string &name) : name_(name), max_users_(0){
//    mode_.insert(std::pair<char, int>('O', 0)); //give "channel creator" status
    mode_.insert(std::pair<char, int>('o', 0)); //give/take channel operator privileges
    mode_.insert(std::pair<char, int>('v', 0)); //give/take the voice privilege
//    mode_.insert(std::pair<char, int>('a', 0)); //toggle the anonymous channel flag
    mode_.insert(std::pair<char, int>('i', 0)); //toggle the invite-only channel flag
    mode_.insert(std::pair<char, int>('m', 0)); //toggle the moderated channel
    mode_.insert(std::pair<char, int>('n', 0)); //toggle the no messages to channel from clients on the outside
    mode_.insert(std::pair<char, int>('q', 0)); //toggle the quiet channel flag
//    mode_.insert(std::pair<char, int>('p', 0)); //toggle the private channel flag
    mode_.insert(std::pair<char, int>('s', 0)); //toggle the secret channel flag
    mode_.insert(std::pair<char, int>('r', 0)); //toggle the server reop channel flag
    mode_.insert(std::pair<char, int>('t', 1)); //toggle the topic settable by channel operator only flag
    mode_.insert(std::pair<char, int>('k', 0)); //set/remove the channel key (password)
    mode_.insert(std::pair<char, int>('l', 0)); //set/remove the user limit to channel
    mode_.insert(std::pair<char, int>('b', 0)); //set/remove ban mask to keep users out
    mode_.insert(std::pair<char, int>('e', 0)); //set/remove an exception mask to override a ban mask
    mode_.insert(std::pair<char, int>('I', 0)); //set/remove an invitation mask to automatically override the invite-only flag
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

void Channel::setMode(const char c, int mode) {
    mode_.find(c)->second = mode;
}

void Channel::setKey(const std::string &key_msg) {
    key_ = key_msg;
}

const std::vector<User*> &Channel::getUsers() const {
    return users_;
}

void Channel::addUser(User* user) {
    ITERATOR itr = users_.begin();
    ITERATOR ite = users_.end();

    while (itr != ite){
        if (*itr == user){
            return;
        }
        itr++;
    }
    users_.push_back(user);
}

void Channel::delUser(User* user) {
    ITERATOR itr = users_.begin();
    ITERATOR ite = users_.end();

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
    ITERATOR itr = operators_.begin();
    ITERATOR ite = operators_.end();

    while (itr != ite){
        if (*itr == oper){
            return;
        }
        itr++;
    }
    operators_.push_back(oper);
}

void Channel::delOperators(User* oper) {
    ITERATOR itr = operators_.begin();
    ITERATOR ite = operators_.end();

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
    ITERATOR itr = operators_.begin();
    ITERATOR ite = operators_.end();
    while (itr != ite){
        list_users.append("@" + (*itr)->nick() + " ");
        itr++;
    }
    itr = moderator_users_.begin();
    ite = moderator_users_.end();
    while (itr != ite){
        list_users.append("+" + (*itr)->nick() + " ");
        itr++;
    }
    ITERATOR itr_u = users_.begin();
    ITERATOR ite_u = users_.end();
    while (itr_u != ite_u){
        itr = operators_.begin();
        ite = operators_.end();
        while (itr != ite){
            if ((*itr) == (*itr_u))
                break;
            itr++;
        }
        if (itr == ite)
            list_users.append((*itr_u)->nick() + " ");
        itr_u++;
    }
    list_users.erase(list_users.size() - 1);
    return list_users;
}

const std::string &Channel::getKey() const {
    return key_;
}

void Channel::addBanUser(User *user) {
    ITERATOR itr = ban_users_.begin();
    ITERATOR ite = ban_users_.end();

    while (itr != ite){
        if (*itr == user){
            return;
        }
        itr++;
    }
    itr = users_.begin();
    ite = users_.end();
    while (itr != ite){
        if (*itr == user){
            users_.erase(itr);
        }
        itr++;
    }
    ban_users_.push_back(user);
}

void Channel::delBanUser(User *user) {
    ITERATOR itr = ban_users_.begin();
    ITERATOR ite = ban_users_.end();

    while (itr != ite){
        if (*itr == user){
            users_.erase(itr);
            break;
        }
        itr++;
    }
}

const std::vector<User *> &Channel::getBanUsers() const {
    return ban_users_;
}

void Channel::addInviteUser(User *user) {
    ITERATOR itr = invite_users_.begin();
    ITERATOR ite = invite_users_.end();

    while (itr != ite){
        if (*itr == user){
            return;
        }
        itr++;
    }
    invite_users_.push_back(user);
}

void Channel::delInviteUser(User *user) {
    ITERATOR itr = invite_users_.begin();
    ITERATOR ite = invite_users_.end();

    while (itr != ite){
        if (*itr == user){
            invite_users_.erase(itr);
            break;
        }
        itr++;
    }
}

const std::vector<User *> &Channel::getInviteUsers() const {
    return invite_users_;
}

const int &Channel::getMaxUsers() const {
    return max_users_;
}

void Channel::addModeratorUser(User *user) {
    ITERATOR itr = moderator_users_.begin();
    ITERATOR ite = moderator_users_.end();

    while (itr != ite){
        if (*itr == user){
            return;
        }
        itr++;
    }
    moderator_users_.push_back(user);
}

void Channel::delModeratorUser(User *user) {
    ITERATOR itr = moderator_users_.begin();
    ITERATOR ite = moderator_users_.end();

    while (itr != ite){
        if (*itr == user){
            moderator_users_.erase(itr);
            break;
        }
        itr++;
    }
}

const std::vector<User *> &Channel::getModerators() const {
    return moderator_users_;
}

std::string Channel::getListMode() {
    std::string list_mode;
    std::map<char, int>::iterator itr = mode_.begin();
    std::map<char, int>::iterator ite = mode_.end();

    while (itr != ite){
        if (itr->second == 1)
            list_mode.push_back(itr->first);
        itr++;
    }
    if (mode_.find('l')->second == 1)
        list_mode.append(" " + int_to_str(max_users_));
    return list_mode;
}

bool Channel::isOperator(User *user) {
    ITERATOR itr = operators_.begin();
    ITERATOR ite = operators_.end();

    while (itr != ite){
        if (*itr == user)
            return true;
        itr++;
    }
    return false;
}

bool Channel::isUser(User *user) {
    ITERATOR itr = users_.begin();
    ITERATOR ite = users_.end();

    while (itr != ite){
        if (*itr == user)
            return true;
        itr++;
    }
    return false;
}
