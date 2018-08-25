//
// Created by zeke on 8/22/18.
//

#ifndef CHIT_CPP_CHITTER_H
#define CHIT_CPP_CHITTER_H

// this file should be defining common methods to interact with psql database
#include <pqxx/pqxx>
#include <string>
#include <sstream>

namespace chitter {

    namespace db {
        std::string type;
        std::string username;
        std::string password;
        std::string ip;
        std::string port;
        std::string name;
    };

    void load_config();
    pqxx::connection initiate();

    bool checkUserExists(const std::string userID);

};

#endif //CHIT_CPP_CHITTER_H
