//
// Created by zeke on 11/4/18.
//

#ifndef CHIT_CPP_CHANNELFUNCS_H
#define CHIT_CPP_CHANNELFUNCS_H

#include <string>
#include <User.h>
#include <pqxx/pqxx>

namespace chitter {

    bool checkChannelExists(const std::string channelName, const std::string serverName,
                            pqxx::connection& connection);

    void insertChannel(const std::string channelName, const std::string channelTopic, const std::string serverName,
                       pqxx::connection& connection);

    void updateChannelTopic(const std::string channelName, const std::string channelTopic, const std::string serverName,
                            pqxx::connection& connection);

    std::string getChannelTopic(const std::string channelName, const std::string serverName, pqxx::connection& connection);

    void insertMsg(const std::string channelName, const std::string userID, const std::string msg,
                   const std::string serverName, pqxx::connection& connection);

    //@TODO: displace to serverFuncs and remove channelID arg
    void insertConnection(const std::string channelID, const User& user, const Status status,
                          const std::string serverName, pqxx::connection& connection);

    Status getChannelRoles(const std::string userID, const std::string channelName, const std::string serverName,
                           pqxx::connection& connection);

    void insertChannelRoles(const std::string userID, const std::string channelName, const std::string serverName,
                            const Status statusEnum, pqxx::connection& connection);

}

#endif //CHIT_CPP_CHANNELFUNCS_H
