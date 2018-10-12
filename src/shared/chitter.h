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
//#include <optional>
#include <experimental/optional>
//@TODO: figure out why experimental is required for file system and optional.
//@TODO: make 'Status' more descriptive

using StringOpt = std::experimental::optional<std::string>;

namespace chitter {
    using tcp = asio::ip::tcp;

    struct DbConfig {
        std::string type;
        std::string username;
        std::string password;
        std::string ip;
        std::string port;
        std::string name;
    };

    enum class Status {
        Admin,
        User,
        Banned,
        Nonexistent
    };

    template <class T>
    T& operator << (T& stream, const Status statusEnum);

    std::string getStatusString(const Status statusEnum);

    Status getStatusEnum(const std::string statusString);

    DbConfig loadConfig(const std::string fileName = "./config");

    pqxx::connection initiate(const std::string fileName = "./config");

    pqxx::connection initiate(const DbConfig db);

    bool checkUserExists(const std::string userID, pqxx::connection& connection);

    bool checkUserExists(const std::string userID);

    bool verifyPassword(const std::string userID, const std::string password, pqxx::connection& connection);

    bool verifyPassword(const std::string userID, const std::string password);

    void updatePassword(const std::string userID, const std::string newPassword,
                        pqxx::connection& connection);

    void updatePassword(const std::string userID, const std::string newPassword);

    void insertUser(const User& user, pqxx::connection& connection);

    void insertUser(const User& user);

    // does overall 'handling' of a user object.
    // if exists, checks password. else, inserts user. returns false if password is incorrect for existing user,
    // else returns true
    bool handleUser(const User& user, pqxx::connection& connection);

    bool handleUser(const User& user);

    std::string getBio(const std::string userID, pqxx::connection& connection);

    std::string getBio(const std::string userID);

    void updateBio(const std::string userID, const std::string bio, pqxx::connection& connection);

    void updateBio(const std::string userID, const std::string bio);

    std::string getCurrentDatetime(pqxx::connection& connection);

    std::string getCurrentDatetime();

    void insertLogin(const std::string userID, const tcp::endpoint& endpoint,
                        pqxx::connection& connection);

    void insertLogin(const std::string userID, const tcp::endpoint& endpoint);

    bool checkServerExists(const std::string serverID, pqxx::connection& connection);

    bool checkServerExists(const std::string serverID);

    void insertServer(const std::string serverID, pqxx::connection& connection);

    void insertServer(const std::string serverID);

    void insertServerMetadata(const std::string serverID, const tcp::endpoint& endpoint,
                        pqxx::connection& connection);

    void insertServerMetadata(const std::string serverID, const tcp::endpoint& endpoint);

    void handleServer(const std::string serverID, const tcp::endpoint& endpoint,
                        pqxx::connection& connection);

    void handleServer(const std::string serverID, const tcp::endpoint& endpoint);

    bool checkChannelExists(const std::string channelName, pqxx::connection& connection);

    bool checkChannelExists(const std::string channelName);

    void insertChannel(const std::string channelName, const std::string channelTopic, const std::string serverName,
                        pqxx::connection& connection);

    void insertChannel(const std::string channelName, const std::string channelTopic, const std::string serverName);

    void insertMsg(const std::string channelName, const std::string userID, const std::string msg,
                    const std::string serverName, pqxx::connection& connection);

    void insertMsg(const std::string channelName, const std::string userID, const std::string msg,
                   const std::string serverName);

    void insertConnection(const std::string channelID, const User& user, const Status status,
                          const std::string serverName, pqxx::connection& connection);

    void insertConnection(const std::string channelID, const User& user, const Status status,
                          const std::string serverName);
//    void insertConnection(const std::string channelID, const User& user, const Status status,
//                          const std::string serverName, const pqxx::connection& connection = initiate());

    std::tuple<Status, std::string> getServerRoles(const std::string userId, const std::string serverName,
                          pqxx::connection& connection);

    std::tuple<Status, std::string> getServerRoles(const std::string userID, const std::string serverName);

    void insertServerRoles(const std::string userID, const std::string serverName, const Status statusEnum,
                           StringOpt displayName, pqxx::connection& connection);

    void insertServerRoles(const std::string userID, const std::string serverName, const Status statusEnum,
                            StringOpt displayName);

    Status getChannelRoles(const std::string userID, const std::string channelName, const std::string serverName,
                            pqxx::connection& connection);

    Status getChannelRoles(const std::string userID, const std::string channelName, const std::string serverName);

    void insertChannelRoles(const std::string userID, const std::string channelName, const std::string serverName,
                            const Status statusEnum, pqxx::connection& connection);

    void insertChannelRoles(const std::string userID, const std::string channelName, const std::string serverName,
                            const Status statusEnum);

    bool checkAlreadyFriends(const std::string friend1ID, const std::string friend2ID,
                            pqxx::connection& connection);

    bool checkAlreadyFriends(const std::string friend1ID, const std::string friend2ID);

    void insertFriend(const std::string userID, const std::string friendUserID,
                                    pqxx::connection& connection);

    void insertFriend(const std::string userID, const std::string friendUserID);

    std::vector<std::string> fetchFriends(const std::string userID, pqxx::connection& connection);

    std::vector<std::string> fetchFriends(const std::string userID);
};

#endif //CHIT_CPP_CHITTER_H
