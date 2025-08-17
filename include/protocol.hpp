#pragma once

#include "database.hpp"
class Protocol
{
public:
    Protocol(Database &database) : m_database(database) {};
    std::string handle(const std::string &request);

private:
    Database &m_database;
};