//
// Created by zeke on 11/4/18.
//

#ifndef CHIT_CPP_INITIATE_H
#define CHIT_CPP_INITIATE_H

#include <string>
#include <pqxx/connection>
#include <pqxx/pqxx>

namespace chitter {
    struct DbConfig {
        std::string type;
        std::string username;
        std::string password;
        std::string ip;
        std::string port;
        std::string name;
    };

    DbConfig loadConfig(const std::string fileName = "./config");

    pqxx::connection initiate(const std::string fileName = "./config");

    pqxx::connection initiate(const DbConfig dbConfig);

    std::string getCurrentDatetime(pqxx::connection& connection);

    void passValues(std::stringstream& ss, pqxx::work& work,
            std::vector<std::string> values);

    void printResult(const pqxx::result result);
}

#endif //CHIT_CPP_INITIATE_H
