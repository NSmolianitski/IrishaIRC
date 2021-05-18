//
// Created by Parfait Kentaurus on 5/12/21.
//

#include "AConnection.hpp"

AConnection::AConnection() {}
AConnection::AConnection(int socket, eType type, int hopcount) : socket_(socket), type_(type), hopcount_(hopcount) {}
AConnection::~AConnection() {}

int				AConnection::socket		() const {	return socket_; }
eType			AConnection::type		() const {	return type_; }
int				AConnection::hopcount	() const { return hopcount_; }
std::string&	AConnection::buff		() { return buff_; }
