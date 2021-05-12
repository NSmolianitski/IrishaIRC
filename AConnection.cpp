//
// Created by Parfait Kentaurus on 5/12/21.
//

#include "AConnection.hpp"

AConnection::AConnection() {}
AConnection::AConnection(int socket, Type type) : socket_(socket), type_(type) {}
AConnection::~AConnection() {}

int AConnection::socket() const {	return socket_; }
int AConnection::type() const {	return type_; }

