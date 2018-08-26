//
// Created by zeke on 8/22/18.
//

#ifndef CHIT_CPP_CHITTER_H
#define CHIT_CPP_CHITTER_H

// this file should be defining common methods to interact with psql database
#include <pqxx/pqxx>
#include <string>
#include <sstream>
#include "User.h"
#include <asio.hpp>

namespace chitter {
    using tcp = asio::ip::tcp;

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

    bool checkUserExists(const std::string userID, pqxx::connection& connection);

    bool checkUserExists(const std::string userID);

    bool verifyPassword(const std::string userID, const std::string password, pqxx::connection& connection);

    bool verifyPassword(const std::string userID, const std::string password);

    void updatePassword(const std::string userID, const std::string newPassword,
                        pqxx::connection connection = initiate());

    void insertUser(const User& user, pqxx::connection connection = initiate());

    std::string getBio(const std::string userID, pqxx::connection connection = initiate());

    void updateBio(const std::string userID, const std::string bio, pqxx::connection connection = initiate());

    std::string getCurrentDatetime(pqxx::connection connection = initiate());

    void insertLogin(const std::string userID, const tcp::endpoint& endpoint,
                        pqxx::connection connection = initiate());

    bool checkServerExists(const std::string serverID, pqxx::connection connection = initiate());

    void insertServer(const std::string serverID, pqxx::connection connection = initiate());

    void insertServerMetadata(const std::string serverID, const tcp::endpoint& endpoint,
                        pqxx::connection connection = initiate());

    bool checkChannelExists(const std::string channelName, pqxx::connection connection = initiate());

    void insertChannel(const std::string channelName, const std::string channelTopic, const std::string serverName,
                        pqxx::connection connection = initiate());

    void insertMsg(const std::string channelName, const std::string userID, const std::string msg,
                    const std::string serverName, pqxx::connection connection = initiate());

};

#endif //CHIT_CPP_CHITTER_H
