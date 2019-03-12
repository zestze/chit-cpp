//
// Created by zeke on 11/4/18.
//

#ifndef CHIT_CPP_USERFUNCS_H
#define CHIT_CPP_USERFUNCS_H

#include "utils.h"
#include <User.h>

namespace chitter {

    ///
    /// \brief
    /// \param userID
    /// \param connection
    /// \return
    bool checkUserExists(const std::string userID, pqxx::connection& connection);

    bool verifyPassword(const std::string userID, const std::string password,
            pqxx::connection& connection);

    void updatePassword(const std::string userID, const std::string newPassword,
            pqxx::connection& connection);

    void insertUser(const User& user, pqxx::connection& connection);

    // does overall 'handling' of a user object.
    // if exists, checks password. else, inserts user. returns false if password is incorrect
    // for existing user, else returns true.
    bool handleUser(const User& user, pqxx::connection& connection);

    std::string getBio(const std::string userID, pqxx::connection& connection);

    void updateBio(const std::string userID, const std::string bio,
            pqxx::connection& connection);

    void insertLogin(const std::string userID, const tcp::endpoint& endpoint,
            pqxx::connection& connection);

    bool checkAlreadyFriends(const std::string friend1ID, const std::string friend2ID,
                             pqxx::connection& connection);

    void insertFriend(const std::string userID, const std::string friendUserID,
                      pqxx::connection& connection);

    std::vector<std::string> fetchFriends(const std::string userID, pqxx::connection& connection);
}

#endif //CHIT_CPP_USERFUNCS_H
