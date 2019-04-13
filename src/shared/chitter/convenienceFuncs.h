//
// Created by zeke on 11/4/18.
//

#ifndef CHIT_CPP_CONVENIENCEFUNCS_H
#define CHIT_CPP_CONVENIENCEFUNCS_H

#include "utils.h"
#include "userFuncs.h"
#include <User.h>
#include "serverFuncs.h"
#include "channelFuncs.h"

namespace chitter {

    // *******************
    // common.h
    // *******************
    inline std::string getCurrentDatetime() {
        pqxx::connection connection = initiate();
        return getCurrentDatetime(connection);
    }

    // *******************
    // userFuncs
    // *******************
    inline bool checkUserExists(const std::string userID) {
        pqxx::connection connection = initiate();
        return checkUserExists(userID, connection);
    }

    inline bool verifyPassword(const std::string userID, const std::string password) {
        pqxx::connection connection = initiate();
        return verifyPassword(userID, password, connection);
    }

    inline void updatePassword(const std::string userID, const std::string newPassword) {
        pqxx::connection connection = initiate();
        return updatePassword(userID, newPassword, connection);
    }

    inline void insertUser(const User &user) {
        pqxx::connection connection = initiate();
        return insertUser(user, connection);
    }

    inline bool handleUser(const User& user) {
        pqxx::connection connection = initiate();
        return handleUser(user, connection);
    }

    inline std::string getBio(const std::string userID) {
        pqxx::connection connection = initiate();
        return getBio(userID, connection);
    }

    inline void updateBio(const std::string userID, const std::string bio) {
        pqxx::connection connection = initiate();
        return updateBio(userID, bio, connection);
    }

    inline void insertLogin(const std::string userID, const tcp::endpoint& endpoint) {
        pqxx::connection connection = initiate();
        return insertLogin(userID, endpoint, connection);
    }

    inline bool checkAlreadyFriends(const std::string friend1ID, const std::string friend2ID) {
        pqxx::connection connection = initiate();
        return checkAlreadyFriends(friend1ID, friend2ID, connection);
    }

    inline void insertFriend(const std::string userID, const std::string friendUserID) {
        pqxx::connection connection = initiate();
        return insertFriend(userID, friendUserID, connection);
    }

    inline std::vector<std::string> fetchFriends(const std::string userID) {
        pqxx::connection connection = initiate();
        return fetchFriends(userID, connection);
    }

    // *******************
    // serverFuncs
    // *******************
    inline bool checkServerExists(const std::string serverName) {
        pqxx::connection connection = initiate();
        return checkServerExists(serverName, connection);
    }

    inline void insertServer(const std::string serverName) {
        pqxx::connection connection = initiate();
        return insertServer(serverName, connection);
    }

    inline void insertServerMetadata(const std::string serverName, const tcp::endpoint &endpoint) {
        pqxx::connection connection = initiate();
        return insertServerMetadata(serverName, endpoint, connection);
    }

    inline void handleServer(const std::string serverName,
                      const tcp::endpoint& endpoint) {
        pqxx::connection connection = initiate();
        return handleServer(serverName, endpoint, connection);
    }

    inline std::tuple<Status, std::string> getServerRoles(const std::string userID, const std::string serverName) {
        pqxx::connection connection = initiate();
        return getServerRoles(userID, serverName, connection);
    }

    inline void insertServerRoles(const std::string userID, const std::string serverName,
                           const Status statusEnum, 
                           optional<std::string> displayName) {
        pqxx::connection connection = initiate();
        insertServerRoles(userID, serverName, statusEnum, displayName, connection);
    }

    // *******************
    // channelFuncs
    // *******************
    inline bool checkChannelExists(const std::string channelName, const std::string serverName) {
        pqxx::connection connection = initiate();
        return checkChannelExists(channelName, serverName, connection);
    }

    inline void insertChannel(const std::string channelName, const std::string channelTopic, const std::string serverName) {
        pqxx::connection connection = initiate();
        return insertChannel(channelName, channelTopic, serverName, connection);
    }

    inline void updateChannelTopic(const std::string channelName, const std::string channelTopic,
                            const std::string serverName) {
        pqxx::connection connection = initiate();
        return updateChannelTopic(channelName, channelTopic, serverName, connection);
    }

    inline std::string getChannelTopic(const std::string channelName, const std::string serverName) {
        pqxx::connection connection = initiate();
        return getChannelTopic(channelName, serverName, connection);
    }

    inline void insertMsg(const std::string channelName, const std::string userID, const std::string msg,
                   const std::string serverName) {
        pqxx::connection connection = initiate();
        return insertMsg(channelName, userID, msg, serverName, connection);
    }

    inline Status getChannelRoles(const std::string userID, const std::string channelName,
                           const std::string serverName) {
        pqxx::connection connection = initiate();
        return getChannelRoles(userID, channelName, serverName, connection);
    }

    inline void insertChannelRoles(const std::string userID, const std::string channelName, const std::string serverName,
                            const Status statusEnum) {
        pqxx::connection connection = initiate();
        return insertChannelRoles(userID, channelName, serverName, statusEnum, connection);
    }
}
#endif //CHIT_CPP_CONVENIENCEFUNCS_H
