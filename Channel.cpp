#include "Channel.hpp"

Channel::Channel(const std::string &name) : name_(name), arr_mode_("opsitnbv"){

}

Channel::~Channel() {

}

const std::string &Channel::getTopic() const {
    return topic_;
}

void Channel::setTopic(const std::string &topic_msg) {
    topic_ = topic_msg;
}

const std::vector<char> &Channel::getMode() const {
    return mode_;
}

void Channel::setMode(const std::string &mode_msg) {
    int flag = 0;
    std::vector<char>::iterator itr;

    for (size_t i = 0; i < mode_msg.size(); ++i) {
        if (mode_msg[i] == '+'){
            flag = 1;
            continue;
        }
        if (mode_msg[i] == '-'){
            flag = 2;
            continue;
        }
        if (flag == 1){
            if (arr_mode_.find(mode_msg[i]) != std::string::npos){
                for (itr = mode_.begin(); itr != mode_.end() ; ++itr) {
                    if (*itr == mode_msg[i])
                        break;
                }
                if (itr == mode_.end())
                    mode_.push_back(mode_msg[i]);
            }
        }
        if (flag == 2){
            if (arr_mode_.find(mode_msg[i]) != std::string::npos)
                for (itr = mode_.begin(); itr != mode_.end() ; ++itr) {
                    if (*itr == mode_msg[i])
                        mode_.erase(itr);
                }
        }
    }
}

void Channel::setKey(const std::string &key_msg) {
    key_ = key_msg;
}

const std::set<std::string> &Channel::getUsers() const {
    return users_;
}

void Channel::addUser(const std::string &nick_msg) {
    users_.insert(nick_msg);
}

void Channel::delUser(const std::string &nick_msg) {
    users_.erase(nick_msg);
}
